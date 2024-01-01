#include "Configuration.h"

#define INI_FILE "UnleashedConversion.ini"
#define STAGE_LIST_FILE "stage_list.json"
#define QUEUE_LIST_FILE "sequence.json"

//---------------Gameplay---------------
bool Configuration::m_bQSS = true;

//---------------UI---------------
std::vector<std::string> Configuration::gensStages;
bool Configuration::ignoreWarnings = false;
bool Configuration::compatibilityMode = false;
int Configuration::logoType = 0;
Configuration::TitleType Configuration::menuType = (TitleType)0;
WorldData Configuration::worldData;
SequenceData Configuration::queueData;

void Configuration::load()
{
	INIReader reader(INI_FILE);
	if (reader.ParseError() != 0)
	{
		MessageBox(NULL, L"Failed to parse mod configuration", NULL, MB_ICONERROR);
		exit(-1);
		return;
	}

	//---------------Gameplay---------------
	m_bQSS = reader.GetBoolean("Gameplay", "bQSS", m_bQSS);


	//---------------UI---------------
	logoType = reader.GetInteger("Appearance", "LogoType", logoType);
	menuType = (TitleType)reader.GetInteger("Appearance", "MenuType", menuType);
	ignoreWarnings = reader.GetBoolean("Main", "IgnoreWarnings", ignoreWarnings);
	compatibilityMode = reader.Get("Main", "IncludeDir1", "disk_sounds") == "";
	gensStages = { "ghz100","ghz200","cpz100","cpz200","ssz100","ssz200","sph100","sph200","cte100", "cte200","ssh100","ssh200","csc100","csc200","euc100","euc200","pla100","pla200" };
	getStageList();
	getLevelQueue();
}
void Configuration::getLevelQueue()
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
			queueData.data[i].dataName = std::string(arrayFlag[i]["dataName"].asCString());
		}
		break;
	}
}
void Configuration::getStageList()
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
std::vector<std::string> Configuration::getAllLevelIDs(bool onlyCustom)
{
	std::vector<std::string> returned;
	for (size_t i = 0; i < Configuration::worldData.data.size(); i++)
	{
		for (size_t x = 0; x < Configuration::worldData.data[i].data.size(); x++)
		{
			if (onlyCustom)
			{
				if (std::find(gensStages.begin(), gensStages.end(), Configuration::worldData.data[i].data[x].levelID) == gensStages.end())
				{
					returned.push_back(Configuration::worldData.data[i].data[x].levelID);
					continue;
				}
			}
			else
				returned.push_back(Configuration::worldData.data[i].data[x].levelID);
		}
	}
	return returned;
}
std::vector<std::string> Configuration::getAllWhiteWorld()
{
	std::vector<std::string> returned;
	for (size_t i = 0; i < Configuration::worldData.data.size(); i++)
	{
		for (size_t x = 0; x < Configuration::worldData.data[i].data.size(); x++)
		{
			if (Configuration::worldData.data[i].data[x].isWhiteWorld)
			{
				returned.push_back(Configuration::worldData.data[i].data[x].levelID);
				continue;
			}
		}
	}
	return returned;
}
int Configuration::getCapital(int flagID)
{
	int returned = -1;
	for (size_t i = 0; i < Configuration::worldData.data[flagID].data.size(); i++)
	{
		if (Configuration::worldData.data[flagID].data[i].isCapital)
		{
			returned = i;
			return returned;
		}
	}
	return returned;
}
