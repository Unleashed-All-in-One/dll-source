#pragma once
struct IntStuffPlayEvent
{
	BYTE gap0[4];
	int value;
};

struct __declspec(align(4)) LuaParamPlayEventEntry
{
	BYTE gap0[4];
	const char* eventName;
	IntStuffPlayEvent eventStageType;
};

struct LuaParamPlayEvent
{
	BYTE gap0[4];
	LuaParamPlayEventEntry* entry;
	LuaParamPlayEventEntry* entry1;
};
class StageManager
{

public:
	static void triggerSequenceEvents(int type, bool dontSetPlayerType);
	static std::string nextStageID;
	static std::string nextEvsID;
	static void setGameParameters(std::string stageID, std::string evsID);
	static const char* NextLevelLoad;
	static const char* CheckForID;
	static bool enteredStageFromETF;
	static bool ReplacingNext;	
	static bool ActiveReplacement;
	static bool WhiteWorldEnabled;
	static bool InStory;
	static int LastSavedQueueIndex;
	static void forcePlayCutscene(std::string in_EventName, std::string in_StageName, bool flag, int playerType);
	static void update();
	static void setETFInfo(std::string etfHubStageName);
	static void initialize();
	static std::string getEventID();
	static const char* getStageToLoad();
	static void setCorrectStage();
};
