#include "Project.h"

#define INI_FILE "UnleashedConversion.ini"
#define STAGE_LIST_FILE "stage_list.json"
#define ARCHIVE_LIST_FILE "archivelist.json"
#define QUEUE_LIST_FILE "sequence.json"

//---------------Gameplay---------------
bool Project::m_bQSS = true;

//---------------UI---------------
std::vector<std::string> Project::gensStages;
bool Project::ignoreWarnings = false;
bool Project::use4gbMode = false;
bool Project::compatibilityMode = false;
int Project::logoType = 0;
Project::TitleType Project::menuType = (TitleType)0;
WorldData Project::worldData;
ArchiveTreeDefinitions Project::archiveTree;
SequenceData Project::queueData;
std::string Project::modPath;
float Project::m_deltaTime = 0.0f;
float Project::m_hudDeltaTime = 0.0f;
int Project::m_frameDeltaTime = 0;
boost::shared_ptr<Sonic::CMatrixNodeTransform> Project::nodeForArmswing;

//didnt know where else to put this
extern "C" __declspec(dllexport) boost::shared_ptr<Sonic::CMatrixNodeTransform> API_GetClosestSetObjectForArmswing()
{
	return Project::nodeForArmswing;
}
HOOK(void*, __fastcall, UpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
	Project::setDeltaTime(elapsedTime);
	return originalUpdateApplication(This, Edx, elapsedTime, a3);
}

HOOK(void, __fastcall, CHudSonicStageUpdate, 0x1098A50, void* This, void* Edx, float* dt)
{
	Project::setHudDeltaTime(*dt);
	originalCHudSonicStageUpdate(This, Edx, dt);
}
void Project::load(const char* path)
{
	INIReader reader(INI_FILE);
	if (reader.ParseError() != 0)
	{
		MessageBox(NULL, L"Failed to parse mod configuration", NULL, MB_ICONERROR);
		exit(-1);
		return;
	}

	INSTALL_HOOK(CHudSonicStageUpdate);
	INSTALL_HOOK(UpdateApplication);
	std::filesystem::path modP = path;
	Project::modPath = modP.parent_path().string();

	//---------------Gameplay---------------
	m_bQSS = reader.GetBoolean("Gameplay", "bQSS", m_bQSS);


	//---------------UI---------------
	logoType = reader.GetInteger("Appearance", "LogoType", logoType);
	menuType = (TitleType)reader.GetInteger("Appearance", "MenuType", menuType);
	use4gbMode = reader.GetBoolean("Main", "Use4GB", use4gbMode);
	compatibilityMode = reader.Get("Main", "IncludeDir1", "disk_sounds") == "";
	gensStages = { "ghz100","ghz200","cpz100","cpz200","ssz100","ssz200","sph100","sph200","cte100", "cte200","ssh100","ssh200","csc100","csc200","euc100","euc200","pla100","pla200" };
	getStageList();
	getLevelQueue();
	getTempCustomArchiveTree();
}
void Project::getLevelQueue()
{
	std::ifstream jsonFile(QUEUE_LIST_FILE);

	queueData = SequenceData();

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
			queueData.data.push_back(QueueData());
			queueData.data[i].type = arrayFlag[i]["type"].asInt();
			queueData.data[i].immediate = arrayFlag[i]["immediate"].asInt();
			if (arrayFlag[i]["playerTypeOverride"])
				queueData.data[i].playerTypeOverride = arrayFlag[i]["playerTypeOverride"].asInt();
			else
				queueData.data[i].playerTypeOverride = -1;
			queueData.data[i].dataName = std::string(arrayFlag[i]["dataName"].asCString());

			if (arrayFlag[i]["eventStageName"])
			{
				queueData.data[i].stageEventName = std::string(arrayFlag[i]["eventStageName"].asCString());
			}
		}
		break;
	}
}
int Project::getFlagFromStage(const char* stage)
{
	std::string stageString = stage;
	//if only c++ had linq
	for (size_t i = 0; i < worldData.data.size(); i++)
	{
		for (size_t a = 0; a < worldData.data[i].data.size(); a++)
		{
			if (worldData.data[i].data[a].levelID == stageString)
			{
				return i;
			}
		}
		for (size_t a = 0; a < worldData.data[i].dataNight.size(); a++)
		{
			if (worldData.data[i].dataNight[a].levelID == stageString)
			{
				return i;
			}
		}
		
	}
	return -1;
}
void Project::getTempCustomArchiveTree()
{
	std::ifstream jsonFile(ARCHIVE_LIST_FILE);

	archiveTree = ArchiveTreeDefinitions();

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
		ArchiveDependency dependency;
		dependency.m_archive = arrayFlag[i]["archiveName"].asCString();
		Json::Value flagDep = arrayFlag[i]["dependencies"];
		for (size_t d = 0; d < flagDep.size(); d++)
		{
			dependency.m_dependencies.push_back(flagDep[d]["archiveName"].asCString());
		}
		archiveTree.data.push_back(dependency);
	}

}
void Project::getStageList()
{
	std::ifstream jsonFile(STAGE_LIST_FILE);

	worldData = WorldData();

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
			worldData.data.push_back(FlagData());
			worldData.data[i].description = std::string(arrayFlag[i]["Description"].asCString());
			Json::Value element = arrayFlag[i]["LevelData"];
			for (int x = 0; x < element.size(); x++)
			{
				worldData.data[i].data.push_back(LevelData());
				worldData.data[i].data[x].levelID = std::string(element[x]["levelID"].asCString());
				worldData.data[i].data[x].optionName = std::string(element[x]["optionName"].asCString());
				worldData.data[i].data[x].isWhiteWorld = element[x]["isWhiteWorld"].asBool();
				worldData.data[i].data[x].isCapital = element[x]["isCapital"].asBool();
			}
			if (arrayFlag[i]["NightLevelData"] != NULL)
			{
				element = arrayFlag[i]["NightLevelData"];
				for (int x = 0; x < element.size(); x++)
				{
					worldData.data[i].dataNight.push_back(LevelData());
					worldData.data[i].dataNight[x].levelID = std::string(element[x]["levelID"].asCString());
					worldData.data[i].dataNight[x].optionName = std::string(element[x]["optionName"].asCString());
					worldData.data[i].dataNight[x].isWhiteWorld = element[x]["isWhiteWorld"].asBool();
					worldData.data[i].dataNight[x].isCapital = element[x]["isCapital"].asBool();
				}
			}
		}
		break;
	}
}
std::vector<std::string> Project::getAllLevelIDs(bool onlyCustom)
{
	std::vector<std::string> returned;
	for (size_t i = 0; i < Project::worldData.data.size(); i++)
	{
		for (size_t x = 0; x < Project::worldData.data[i].data.size(); x++)
		{
			if (onlyCustom)
			{
				if (std::find(gensStages.begin(), gensStages.end(), Project::worldData.data[i].data[x].levelID) == gensStages.end())
				{
					returned.push_back(Project::worldData.data[i].data[x].levelID);
					continue;
				}
			}
			else
				returned.push_back(Project::worldData.data[i].data[x].levelID);
		}
	}
	return returned;
}
std::vector<std::string> Project::getAllWhiteWorld()
{
	std::vector<std::string> returned;
	for (size_t i = 0; i < Project::worldData.data.size(); i++)
	{
		for (size_t x = 0; x < Project::worldData.data[i].data.size(); x++)
		{
			if (Project::worldData.data[i].data[x].isWhiteWorld)
			{
				returned.push_back(Project::worldData.data[i].data[x].levelID);
				continue;
			}
		}
	}
	return returned;
}
int Project::getCapital(int flagID)
{
	int returned = -1;
	for (size_t i = 0; i < Project::worldData.data[flagID].data.size(); i++)
	{
		if (Project::worldData.data[flagID].data[i].isCapital)
		{
			returned = i;
			return returned;
		}
	}
	return returned;
}
