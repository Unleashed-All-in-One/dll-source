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
struct QueueData
{
	int type;
	std::string dataName;
	std::string stageEventName;
	bool immediate;
	int playerTypeOverride;
	const char* comment;
};
struct SequenceData
{
	std::vector<QueueData> data;
};
struct ArchiveTreeDefinitions
{
	std::vector<ArchiveDependency> data;
};

class Project
{
private:
	static float m_deltaTime;
	static int m_frameDeltaTime;
	static float m_hudDeltaTime;
public:
	static void load(const char* path);

	//---------------Gameplay---------------
	static bool m_bQSS;

	//---------------UI---------------
	static void getStageList();
	static void getLevelQueue();
	static void getTempCustomArchiveTree();
	enum TitleType {
		Retail,
		Preview,
		E3
	};
	static boost::shared_ptr<Sonic::CMatrixNodeTransform> nodeForArmswing;
	static std::string modPath;
	static std::vector<std::string> getAllLevelIDs(bool onlyCustom);
	static std::vector<std::string> getAllWhiteWorld();
	static void setDeltaTime(float dt) { m_deltaTime = dt; }
	static float getDeltaTime() { return m_deltaTime; }
	static float getFrameDeltaTime() { return m_frameDeltaTime; }
	static void setHudDeltaTime(float dt) { m_hudDeltaTime = dt; }
	static void setFrameDeltaTime(int dt) { m_frameDeltaTime = dt; }
	static float getHudDeltaTime() { return m_hudDeltaTime == 0.0f ? 0.0f : m_deltaTime; }
	static int getFlagFromStage(const char* stage);
	static std::vector<std::string> gensStages;
	static int getCapital(int flagID);
	static int logoType;
	static bool ignoreWarnings;
	static bool use4gbMode;
	static bool compatibilityMode;
	static WorldData worldData;
	static ArchiveTreeDefinitions archiveTree;
	static SequenceData queueData;
	static TitleType menuType;
};

