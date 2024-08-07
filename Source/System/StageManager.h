#pragma once
namespace SUC::System
{
	class StageManager
	{
	public:
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


		static void TriggerSequenceEvents(int type, bool dontSetPlayerType);
		static void SetGameParameters(std::string stageID, std::string evsID);
		static void ForcePlayCutscene(std::string in_EventName, std::string in_StageName, bool flag, int playerType);
		static void Update();
		static void SetETFInfo(std::string etfHubStageName);
		static void Initialize();
		static std::string GetEventID();
		static const char* GetStageToLoad();
		static void SetCorrectStage();

		static std::string s_NextStage;
		static std::string s_NextEventScene;
		//static const char* s_NextLevelLoad;
		static const char* s_CheckForID;
		static bool s_ETFEntered;
		static bool s_OverrideNextStage;
		static bool s_HubModeEnabled;
		static bool s_InStoryMode;
		static bool s_LoadingReplacementEnabled;
		static int s_LastSavedQueueIndex;
	};
}