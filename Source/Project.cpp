#include "Project.h"
#include "SetObject/SetObjectHookContainer.h"
#include "UI/UIHookContainer.h"
#include "System/SystemHookContainer.h"
#include "System/tinyxml2.h"

namespace SUC
{
	//---------------Gameplay---------------
	bool							Project::m_DoQSS;
	int								Project::s_LogoType;
	Project::ETitleType				Project::menuType;
	std::string						Project::s_ModPath;
	std::string						Project::s_ModInfoPath;
	ModInfo_t* Project::s_ModInfo;
	bool							Project::s_IgnoreWarnings;
	Project::WorldData				Project::s_WorldData;
	bool							Project::s_LargeAddressAware;
	Project::DebugStageTree			Project::s_DebugStageTree;
	bool							Project::s_CpkRedirCompatibilityMode;
	Project::SequenceData			Project::s_SequenceDataQueue;
	Hedgehog::Math::CVector			Project::s_TempArmswingNode;
	std::vector<std::string>		Project::s_GenerationsStages;
	Project::ArchiveTreeDefinitions Project::s_AdditionalArchiveTree;
	float							Project::s_DeltaTo60;


	float Project::ms_DeltaTime = 0.0f;
	float Project::ms_HudDeltaTime = 0.0f;
	int Project::ms_FrameDeltaTime = 0.0f;

	std::map<const char*, const char*> Project::s_IncompatibleMods =
	{
		{"EnemyTrigger", "EnemyTrigger.dll"},
		{"QSS Restoration", "QSSRestoration.dll" },
		{"SWA Titlescreen", "UnleashedTitlescreen.dll"}
	};
	HOOK(void*, __fastcall, Project_UpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
	{
		Project::s_DeltaTo60 = elapsedTime / (1.0f / 60.0f);
		CSDCommon::Update();
		void* returnVal = originalProject_UpdateApplication(This, Edx, elapsedTime, a3);
		auto time = std::chrono::high_resolution_clock::now();
		CSDCommon::Update();
		double elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(std::chrono::high_resolution_clock::now() - time).count();
		
		DebugDrawText::log(Format("CSDCOMMON TOOK %.3fms", elapsed),0, 0, TEXT_GREEN);
		Project::SetDeltaTime(elapsedTime);
		return returnVal;
	}
	HOOK(void, __fastcall, Project_CHudSonicStage_Update, 0x1098A50, void* This, void* Edx, float* dt)
	{
		Project::SetHudDeltaTime(*dt);
		originalProject_CHudSonicStage_Update(This, Edx, dt);
	}	
	void Project::Load(ModInfo_t* in_ModInfo)
	{
		INIReader reader(INI_FILE);
		if (reader.ParseError() != 0)
		{
			MessageBox(NULL, L"Failed to parse mod configuration", NULL, MB_ICONERROR);
			exit(-1);
		}

		std::filesystem::path modP = in_ModInfo->CurrentMod->Path;
		s_ModPath = modP.parent_path().string();
		s_ModInfoPath = std::string(in_ModInfo->CurrentMod->Path);
		s_ModInfo = in_ModInfo;
		//---------------Gameplay---------------
		m_DoQSS = reader.GetBoolean("Gameplay", "bQSS", m_DoQSS);


		//---------------UI---------------
		s_LogoType = reader.GetInteger("Appearance", "s_LogoType", s_LogoType);
		menuType = (ETitleType)reader.GetInteger("Appearance", "MenuType", menuType);
		s_LargeAddressAware = reader.GetBoolean("Main", "Use4GB", s_LargeAddressAware);
		s_CpkRedirCompatibilityMode = reader.Get("Main", "IncludeDir1", "disk_sounds") == "";
		
	}
	void Project::ParseGlobalFile()
	{
		tinyxml2::XMLDocument m_Document;
		m_Document.LoadFile(GLOBAL_CONFIG_FILE);
		GetLevelQueue();
		GetStageList(m_Document);
		GetTempCustomArchiveTree(m_Document);
		GetDebugTree(m_Document);
	}
	void Project::RegisterGlobalHooks()
	{
		INSTALL_HOOK(Project_UpdateApplication);
		INSTALL_HOOK(Project_CHudSonicStage_Update);

		WRITE_MEMORY(0x1AD99D0, char*, "shader_debug.ar");
		WRITE_MEMORY(0x1AD99D4, char*, "shader_debug_add.ar");
		WRITE_MEMORY(0x1AD99E8, char*, "shader_debug.arl");
		WRITE_MEMORY(0x1AD99EC, char*, "shader_debug_add.arl");
	}
	void Project::CheckIncompatibleMods()
	{
		for (auto const& incompatibleModAssembly : s_IncompatibleMods)
		{
			if (GetModuleHandleA(incompatibleModAssembly.second))
			{
				MessageBoxA(nullptr, SUC::Format("\"%s\" should not be enabled with %s.", incompatibleModAssembly.first, MOD_NAME), MOD_NAME, MB_ICONERROR);
				exit(-1);
			}
		}
	}
	void ParseStageTree(const Json::Value& in_JsonValue, SUC::Project::DebugStageTree::DebugStageTreeNode& in_StageTree)
	{
		in_StageTree.name = in_JsonValue["name"].asString();
		in_StageTree.children = std::vector<SUC::Project::DebugStageTree::DebugStageTreeNode>();
		const Json::Value& treeEntries = in_JsonValue["TreeEntry"];
		if (!treeEntries.isArray())
		{
			printf("StageTreeEntry is not a valid array.\n");
			return;
		}
		if (in_JsonValue["children"])
		{
			for (const auto& tree : in_JsonValue["children"])
			{
				SUC::Project::DebugStageTree::DebugStageTreeNode stageTreeC;
				ParseStageTree(tree, stageTreeC);
				in_StageTree.children.push_back(stageTreeC);
			}
		}
		std::vector<SUC::Project::DebugStageTree::DebugStageTreeNode::DebugStageTreeNodeEntry> entries;
		for (const auto& entry : treeEntries)
		{
			SUC::Project::DebugStageTree::DebugStageTreeNode::DebugStageTreeNodeEntry treeEntry;
			treeEntry.stage = entry["stage"].asString();
			if (entry["cutsceneID"])
				treeEntry.cutsceneID = entry["cutsceneID"].asString();

			if (entry["displayName"])
			{
				treeEntry.displayName = entry["displayName"].asString();
			}
			else
			{
				if (!treeEntry.cutsceneID.empty())
					treeEntry.displayName = std::format("{0} @{1}", treeEntry.cutsceneID, treeEntry.stage);
				else
					treeEntry.displayName = treeEntry.stage;
			}
			entries.push_back(treeEntry);
		}
		in_StageTree.treeEntries = entries;
	}
	void SUC::Project::GetDebugTree(tinyxml2::XMLDocument& in_XmlDocument)
	{
		Project::s_DebugStageTree = Project::DebugStageTree();
		std::vector<SUC::Project::DebugStageTree::DebugStageTreeNode> stageTrees;

		tinyxml2::XMLElement* m_XmlStageListRoot = in_XmlDocument.FirstChildElement("ModSchema")->FirstChildElement("StageSelectRoot");
		for (tinyxml2::XMLElement* stageTreeNodeElem = m_XmlStageListRoot->FirstChildElement("StageTreeNode"); stageTreeNodeElem != nullptr; stageTreeNodeElem = stageTreeNodeElem->NextSiblingElement("StageTreeNode"))
		{
			SUC::Project::DebugStageTree::DebugStageTreeNode stageNode = SUC::Project::DebugStageTree::DebugStageTreeNode();
			stageNode.name = stageTreeNodeElem->FirstChildElement("Name")->GetText() ? stageTreeNodeElem->FirstChildElement("Name")->GetText() : "";

			for (tinyxml2::XMLElement* nodeElem = stageTreeNodeElem->FirstChildElement("Node"); nodeElem != nullptr; nodeElem = nodeElem->NextSiblingElement("Node"))
			{
				auto node = SUC::Project::DebugStageTree::DebugStageTreeNode::DebugStageTreeNode();
				node.name = nodeElem->FirstChildElement("Name")->GetText() ? nodeElem->FirstChildElement("Name")->GetText() : "";

				for (tinyxml2::XMLElement* treeEntryElem = nodeElem->FirstChildElement("TreeEntry"); treeEntryElem != nullptr; treeEntryElem = treeEntryElem->NextSiblingElement("TreeEntry"))
				{
					SUC::Project::DebugStageTree::DebugStageTreeNode::DebugStageTreeNodeEntry entry = SUC::Project::DebugStageTree::DebugStageTreeNode::DebugStageTreeNodeEntry();
					entry.stage = treeEntryElem->FirstChildElement("Archive")->GetText() ? treeEntryElem->FirstChildElement("Archive")->GetText() : "";
					entry.cutsceneID = treeEntryElem->FirstChildElement("EventID")->GetText() ? treeEntryElem->FirstChildElement("EventID")->GetText() : "";
					entry.displayName = treeEntryElem->FirstChildElement("DisplayName") ? treeEntryElem->FirstChildElement("DisplayName")->GetText() : entry.stage;
					entry.night = treeEntryElem->FirstChildElement("Night") ? strcmp(treeEntryElem->FirstChildElement("Night")->GetText(), "true") == 0 : false;
					node.treeEntries.push_back(entry);
				}

				stageNode.children.push_back(node);
			}
			stageTrees.push_back(stageNode);
		}
		Project::s_DebugStageTree.treeNodes = stageTrees;

	}
	void SUC::Project::GetLevelQueue()
	{
		std::ifstream jsonFile(QUEUE_LIST_FILE);

		s_SequenceDataQueue = SequenceData();

		std::vector<std::string> modList;
		Common::GetModIniList(modList);
		modList.insert(modList.end(), ""); //Unleashed title's own stage_list

		//Parse stage_list
		for (size_t a = 0; a < modList.size(); a++)
		{
			size_t pos = modList.at(a).find_last_of("\\/");
			if (pos != std::string::npos)
			{
				modList.at(a).erase(pos + 1);
			}
			std::ifstream jsonFile(modList.at(a) + QUEUE_LIST_FILE);

			if (!jsonFile)
				continue;

			Json::Value root;
			jsonFile >> root;
			auto wd = root["CustomStorySequence"];
			Json::Value arrayFlag = wd["SequenceData"];
			for (int i = 0; i < arrayFlag.size(); i++)
			{
				s_SequenceDataQueue.data.push_back(SUC::Project::SequenceData::QueueData());
				s_SequenceDataQueue.data[i].type = arrayFlag[i]["type"].asInt();
				s_SequenceDataQueue.data[i].immediate = arrayFlag[i]["immediate"].asInt();
				if (arrayFlag[i]["playerTypeOverride"])
					s_SequenceDataQueue.data[i].playerTypeOverride = arrayFlag[i]["playerTypeOverride"].asInt();
				else
					s_SequenceDataQueue.data[i].playerTypeOverride = -1;
				s_SequenceDataQueue.data[i].dataName = std::string(arrayFlag[i]["dataName"].asCString());

				if (arrayFlag[i]["eventStageName"])
				{
					s_SequenceDataQueue.data[i].stageEventName = std::string(arrayFlag[i]["eventStageName"].asCString());
				}
			}
			break;
		}
	}
	int SUC::Project::GetFlagFromStage(const char* in_Stage)
	{
		std::string stageString = in_Stage;
		//if only c++ had linq
		for (size_t i = 0; i < s_WorldData.data.size(); i++)
		{
			for (size_t a = 0; a < s_WorldData.data[i].data.size(); a++)
			{
				if (s_WorldData.data[i].data[a].levelID == stageString)
				{
					return i;
				}
			}
			for (size_t a = 0; a < s_WorldData.data[i].dataNight.size(); a++)
			{
				if (s_WorldData.data[i].dataNight[a].levelID == stageString)
				{
					return i;
				}
			}

		}
		return -1;
	}
	void ParseArchiveTreeXMLRoot(const std::string& in_Path)
	{
		
	}
	void SUC::Project::GetTempCustomArchiveTree(tinyxml2::XMLDocument& in_XmlDocument)
	{
		Project::s_AdditionalArchiveTree = Project::ArchiveTreeDefinitions();

		tinyxml2::XMLElement* m_XmlStageListRoot = in_XmlDocument.FirstChildElement("ModSchema")->FirstChildElement("ArchiveRoot");
		for (tinyxml2::XMLElement* nodeElem = m_XmlStageListRoot->FirstChildElement("Node"); nodeElem != nullptr; nodeElem = nodeElem->NextSiblingElement("Node"))
		{
			System::ArchiveTreePatcher::ArchiveDependency m_NewArchive = System::ArchiveTreePatcher::ArchiveDependency();
			m_NewArchive.m_archive = nodeElem->FirstChildElement("Name")->GetText() ? nodeElem->FirstChildElement("Name")->GetText() : "";

			for (tinyxml2::XMLElement* usedByElem = nodeElem->FirstChildElement("UsedBy")->FirstChildElement("Name"); usedByElem != nullptr; usedByElem = usedByElem->NextSiblingElement("Name"))
			{
				if (const char* usedByName = usedByElem->GetText())
				{
					m_NewArchive.m_dependencies.push_back(usedByName);
				}
			}

			Project::s_AdditionalArchiveTree.data.push_back(m_NewArchive);
		}
	}
	
	void ParseStageListXMLRoot(const std::string& in_Path)
	{
		
	}
	void SUC::Project::GetStageList(tinyxml2::XMLDocument& in_XmlDocument)
	{
		Project::s_WorldData = Project::WorldData();

		tinyxml2::XMLElement* m_XmlStageListRoot = in_XmlDocument.FirstChildElement("ModSchema")->FirstChildElement("StageListRoot");
		for (tinyxml2::XMLElement* m_XmlCountry = m_XmlStageListRoot->FirstChildElement("Country"); m_XmlCountry != nullptr; m_XmlCountry = m_XmlCountry->NextSiblingElement("Country"))
		{
			Project::WorldData::FlagData country = Project::WorldData::FlagData();
			tinyxml2::XMLElement* descElem = m_XmlCountry->FirstChildElement("Description");

			if (descElem && descElem->GetText()) country.description = descElem->GetText();

			tinyxml2::XMLElement* m_XmlStageList = m_XmlCountry->FirstChildElement("StageList");

			// Loop through each <Stage> within <StageList>
			for (tinyxml2::XMLElement* m_XmlStage = m_XmlStageList->FirstChildElement("StageDay"); m_XmlStage != nullptr; m_XmlStage = m_XmlStage->NextSiblingElement("StageDay"))
			{
				Project::WorldData::FlagData::LevelData stage = Project::WorldData::FlagData::LevelData();
				tinyxml2::XMLElement* archiveElem = m_XmlStage->FirstChildElement("Archive");
				tinyxml2::XMLElement* displayNameElem = m_XmlStage->FirstChildElement("DisplayName");
				tinyxml2::XMLElement* isTownElem = m_XmlStage->FirstChildElement("IsTown");
				tinyxml2::XMLElement* isCapitalElem = m_XmlStage->FirstChildElement("IsCapital");

				if (archiveElem && archiveElem->GetText()) stage.levelID = archiveElem->GetText();
				if (displayNameElem && displayNameElem->GetText()) stage.optionName = displayNameElem->GetText();
				stage.isWhiteWorld = (isTownElem && strcmp(isTownElem->GetText(), "true") == 0);
				stage.isCapital = (isCapitalElem && strcmp(isCapitalElem->GetText(), "true") == 0);

				country.data.push_back(stage);
			}
			for (tinyxml2::XMLElement* m_XmlStage = m_XmlStageList->FirstChildElement("StageNight"); m_XmlStage != nullptr; m_XmlStage = m_XmlStage->NextSiblingElement("StageNight"))
			{
				Project::WorldData::FlagData::LevelData stage = Project::WorldData::FlagData::LevelData();
				tinyxml2::XMLElement* archiveElem = m_XmlStage->FirstChildElement("Archive");
				tinyxml2::XMLElement* displayNameElem = m_XmlStage->FirstChildElement("DisplayName");
				tinyxml2::XMLElement* isTownElem = m_XmlStage->FirstChildElement("IsTown");
				tinyxml2::XMLElement* isCapitalElem = m_XmlStage->FirstChildElement("IsCapital");

				if (archiveElem && archiveElem->GetText()) stage.levelID = archiveElem->GetText();
				if (displayNameElem && displayNameElem->GetText()) stage.optionName = displayNameElem->GetText();
				stage.isWhiteWorld = (isTownElem && strcmp(isTownElem->GetText(), "true") == 0);
				stage.isCapital = (isCapitalElem && strcmp(isCapitalElem->GetText(), "true") == 0);

				country.dataNight.push_back(stage);
			}

			Project::s_WorldData.data.push_back(country);
		}
	}
	std::vector<std::string> SUC::Project::GetAllLevelIDs(bool onlyCustom)
	{
		std::vector<std::string> returned;
		for (size_t i = 0; i < SUC::Project::s_WorldData.data.size(); i++)
		{
			for (size_t x = 0; x < SUC::Project::s_WorldData.data[i].data.size(); x++)
			{
				if (onlyCustom)
				{
					if (std::find(s_GenerationsStages.begin(), s_GenerationsStages.end(), SUC::Project::s_WorldData.data[i].data[x].levelID) == s_GenerationsStages.end())
					{
						returned.push_back(SUC::Project::s_WorldData.data[i].data[x].levelID);
						continue;
					}
				}
				else
					returned.push_back(SUC::Project::s_WorldData.data[i].data[x].levelID);
			}
		}
		return returned;
	}
	std::vector<std::string> SUC::Project::GetAllWhiteWorld()
	{
		std::vector<std::string> returned;
		for (size_t i = 0; i < SUC::Project::s_WorldData.data.size(); i++)
		{
			for (size_t x = 0; x < SUC::Project::s_WorldData.data[i].data.size(); x++)
			{
				if (SUC::Project::s_WorldData.data[i].data[x].isWhiteWorld)
				{
					returned.push_back(SUC::Project::s_WorldData.data[i].data[x].levelID);
					continue;
				}
			}
		}
		return returned;
	}
	int SUC::Project::GetCapital(int in_FlagID, bool in_IsNight)
	{
		int returned = -1;
		std::vector<SUC::Project::WorldData::FlagData::LevelData> levels = in_IsNight ? SUC::Project::s_WorldData.data[in_FlagID].dataNight : SUC::Project::s_WorldData.data[in_FlagID].data;
		for (size_t i = 0; i < levels.size(); i++)
		{
			if (levels[i].isCapital)
			{
				returned = i;
				return returned;
			}
		}
		return returned;
	}
	std::string Project::GetDirectoryPath(const std::string& in_Path)
	{
		const size_t pos = in_Path.find_last_of("\\/");
		return in_Path.substr(0, pos != std::string::npos ? pos : 0);
	}
}
