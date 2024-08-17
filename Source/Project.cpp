#include "Project.h"
#include "Gameplay/GameplayHookContainer.h"
#include "Accuracy/AccuracyHookContainer.h"
#include "Player/PlayerHookContainer.h"
#include "SetObject/SetObjectHookContainer.h"
#include "UI/UIHookContainer.h"
#include "System/SystemHookContainer.h"
#include "Testing/TestingCode.h"
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
		s_GenerationsStages = { "ghz100","ghz200","cpz100","cpz200","ssz100","ssz200","sph100","sph200","cte100", "cte200","ssh100","ssh200","csc100","csc200","euc100","euc200","pla100","pla200" };
		GetStageList();
		GetLevelQueue();
		GetTempCustomArchiveTree();
	}
	void Project::RegisterGlobalHooks()
	{
#if _DEBUG
		MessageBox(nullptr, TEXT("Attach Debugger to see hooks."), TEXT("Unleashed Conversion"), MB_ICONINFORMATION);
#endif

		Hooks::InstallGameplayHooks();
		Hooks::InstallAccuracyHooks();
		Hooks::InstallSetObjectHooks();
		Hooks::InstallSystemHooks();
		Hooks::InstallUIHooks();
		Hooks::InstallPlayerHooks();
		TestingCode::applyPatches();
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
				;
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
	void SUC::Project::GetDebugTree()
	{
		std::string path = SUC::Project::s_ModPath + "\\debuglist.json";
		std::ifstream jsonFile(path.c_str());

		s_DebugStageTree = DebugStageTree();

		if (!jsonFile)
		{
			return;
		}

		Json::Value root;
		jsonFile >> root;

		const Json::Value& stageTreeT = root["StageTree"];
		if (!stageTreeT.isArray())
		{
			printf("StageTree is not a valid array.\n");
			return;
		}

		std::vector<SUC::Project::DebugStageTree::DebugStageTreeNode> stageTrees;
		for (const auto& tree : stageTreeT)
		{
			SUC::Project::DebugStageTree::DebugStageTreeNode stageTree;
			ParseStageTree(tree, stageTree);
			stageTrees.push_back(stageTree);
		}

		/*for (const auto& tree : stageTreeT)
		{
			DebugStageTreeNode stageTreeObj;
			stageTreeObj.name = tree["name"].asString();

			const Json::Value& treeEntries = tree["TreeEntry"];
			if (!treeEntries.isArray())
			{
				printf("StageTreeEntry is not a valid array.\n");
				return;
			}

			for (const auto& entry : treeEntries)
			{
				DebugStageTreeNodeEntry treeEntry;
				treeEntry.stage = entry["stage"].asString();
				if(entry["cutsceneID"])
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
				stageTreeObj.treeEntries.push_back(treeEntry);
			}

			stageTrees.push_back(stageTreeObj);
		}*/
		s_DebugStageTree.treeNodes = stageTrees;

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
	void SUC::Project::GetTempCustomArchiveTree()
	{
		std::ifstream jsonFile(ARCHIVE_LIST_FILE);

		s_AdditionalArchiveTree = ArchiveTreeDefinitions();

		if (!jsonFile)
		{
			MessageBox(NULL, L"Failed to parse ArchiveTree", NULL, MB_ICONERROR);
			exit(-1);
			return;
		}

		Json::Value root;
		jsonFile >> root;
		Json::Value arrayFlag = root["ArchiveTreeDefinitions"];
		for (int i = 0; i < arrayFlag.size(); i++)
		{
			System::ArchiveTreePatcher::ArchiveDependency dependency;
			dependency.m_archive = arrayFlag[i]["archiveName"].asCString();
			Json::Value flagDep = arrayFlag[i]["dependencies"];
			for (size_t d = 0; d < flagDep.size(); d++)
			{
				dependency.m_dependencies.push_back(flagDep[d]["archiveName"].asCString());
			}
			s_AdditionalArchiveTree.data.push_back(dependency);
		}

	}
	void SUC::Project::GetStageList()
	{
		std::ifstream jsonFile(STAGE_LIST_FILE);

		s_WorldData = WorldData();

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
			std::ifstream jsonFile(modList.at(a) + STAGE_LIST_FILE);

			if (!jsonFile)
				continue;

			Json::Value root;
			jsonFile >> root;
			auto wd = root["WorldData"];
			Json::Value arrayFlag = wd["FlagData"];
			for (int i = 0; i < arrayFlag.size(); i++)
			{
				s_WorldData.data.push_back(SUC::Project::WorldData::FlagData());
				s_WorldData.data[i].description = std::string(arrayFlag[i]["Description"].asCString());
				Json::Value element = arrayFlag[i]["LevelData"];
				for (int x = 0; x < element.size(); x++)
				{
					s_WorldData.data[i].data.push_back(SUC::Project::WorldData::FlagData::LevelData());
					s_WorldData.data[i].data[x].levelID = std::string(element[x]["levelID"].asCString());
					s_WorldData.data[i].data[x].optionName = std::string(element[x]["optionName"].asCString());
					s_WorldData.data[i].data[x].isWhiteWorld = element[x]["isWhiteWorld"].asBool();
					s_WorldData.data[i].data[x].isCapital = element[x]["isCapital"].asBool();
				}
				if (arrayFlag[i]["NightLevelData"] != NULL)
				{
					element = arrayFlag[i]["NightLevelData"];
					for (int x = 0; x < element.size(); x++)
					{
						s_WorldData.data[i].dataNight.push_back(SUC::Project::WorldData::FlagData::LevelData());
						s_WorldData.data[i].dataNight[x].levelID = std::string(element[x]["levelID"].asCString());
						s_WorldData.data[i].dataNight[x].optionName = std::string(element[x]["optionName"].asCString());
						s_WorldData.data[i].dataNight[x].isWhiteWorld = element[x]["isWhiteWorld"].asBool();
						s_WorldData.data[i].dataNight[x].isCapital = element[x]["isCapital"].asBool();
					}
				}
			}
			break;
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
