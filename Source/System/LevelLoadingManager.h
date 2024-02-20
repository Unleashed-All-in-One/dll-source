#pragma once

class LevelLoadingManager
{

public:
	static const char* NextLevelLoad;
	static const char* CheckForID;
	static bool ReplacingNext;	
	static bool ActiveReplacement;
	static bool WhiteWorldEnabled;
	static bool InStory;
	static int LastSavedQueueIndex;
	static void forcePlayCutscene(std::string in_EventName, std::string in_StageName, bool flag, int playerType);
	static void update();
	static void initialize();
	static const char* getStageToLoad();
	static void setCorrectStage();
};
