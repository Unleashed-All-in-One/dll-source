#pragma once
struct LevelData
{
	std::string levelID;
	std::string optionName;
	bool isWhiteWorld;
	bool isCapital;
};
struct FlagData
{
	std::string description;
	std::vector<LevelData> data;
	std::vector<LevelData> dataNight;
};
struct WorldData
{
	std::vector<FlagData> data;
};

class Configuration
{
public:
	static void load();

	//---------------Gameplay---------------
	static bool m_bQSS;

	//---------------UI---------------
	static void getStageList();
	enum TitleType {
		Retail,
		Preview,
		E3
	};	

	static std::vector<std::string> getAllLevelIDs(bool onlyCustom);
	static std::vector<std::string> getAllWhiteWorld();
	static std::vector<std::string> gensStages;
	static int getCapital(int flagID);
	static int logoType;
	static bool ignoreWarnings;
	static bool compatibilityMode;
	static WorldData worldData;
	static TitleType menuType;
};

