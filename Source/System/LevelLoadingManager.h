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
	static void update();
	static void initialize();
	static const char* getStageToLoad();
	static void setCorrectStage();
};
