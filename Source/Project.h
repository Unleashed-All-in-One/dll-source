#pragma once

namespace SUC
{
	class Project
	{
	private:
		static float ms_DeltaTime;
		static int ms_FrameDeltaTime;
		static float ms_HudDeltaTime;
	public:
		struct DebugStageTree
		{
			struct DebugStageTreeNode
			{
				struct DebugStageTreeNodeEntry
				{
					std::string stage;
					std::string cutsceneID;
					std::string displayName;
				};
				std::string name;
				std::vector<DebugStageTreeNodeEntry> treeEntries;
				std::vector<DebugStageTreeNode> children;
			};
			std::vector<DebugStageTreeNode> treeNodes;
		};
		struct ArchiveTreeDefinitions
		{
			std::vector<ArchiveDependency> data;
		};
		struct SequenceData
		{
			struct QueueData
			{
				int type;
				std::string dataName;
				std::string stageEventName;
				bool immediate;
				int playerTypeOverride;
				const char* comment;
			};
			std::vector<QueueData> data;
		};
		struct WorldData
		{
			struct FlagData
			{
				struct LevelData
				{
					std::string levelID;
					std::string optionName;
					bool isWhiteWorld;
					bool isCapital;
				};
				std::string description;
				std::vector<LevelData> data;
				std::vector<LevelData> dataNight;
			};
			std::vector<FlagData> data;
		};

		enum ETitleType
		{
			Retail,
			Preview,
			E3
		};
		//-------------Mod Initialization--------
		static void Load(ModInfo_t* in_ModInfo);
		static void RegisterGlobalHooks();
		static void CheckIncompatibleMods();

		//---------------Functions---------------
		static void GetStageList();
		static void GetDebugTree();
		static void GetLevelQueue();
		static void GetTempCustomArchiveTree();
		static int GetFlagFromStage(const char* in_Stage);
		static int GetCapital(int in_FlagID, bool in_IsNight);
		static std::vector<std::string> GetAllWhiteWorld();
		static float GetDeltaTime() { return ms_DeltaTime; }
		static void SetDeltaTime(float dt) { ms_DeltaTime = dt; }
		static std::string GetDirectoryPath(const std::string& in_Path);
		static int GetFrameDeltaTime() { return ms_FrameDeltaTime; }
		static void SetHudDeltaTime(float dt) { ms_HudDeltaTime = dt; }
		static std::vector<std::string> GetAllLevelIDs(bool onlyCustom);
		static void SetFrameDeltaTime(int dt) { ms_FrameDeltaTime = dt; }
		static float GetHudDeltaTime() { return ms_HudDeltaTime == 0.0f ? 0.0f : ms_DeltaTime; }

		//---------------Parameters---------------
		static bool m_DoQSS;
		static int s_LogoType;
		static ETitleType menuType;
		static std::string s_ModPath;
		static std::string s_ModInfoPath;
		static ModInfo_t* s_ModInfo;
		static bool s_IgnoreWarnings;
		static WorldData s_WorldData;
		static bool s_LargeAddressAware;
		static DebugStageTree s_DebugStageTree;
		static bool s_CpkRedirCompatibilityMode;
		static SequenceData s_SequenceDataQueue;
		static Hedgehog::Math::CVector s_TempArmswingNode;
		static std::vector<std::string> s_GenerationsStages;
		static ArchiveTreeDefinitions s_AdditionalArchiveTree;

		static std::map<const char*, const char*> s_IncompatibleMods;
	};
	inline char* Format(const char* fmt, ...)
	{
		thread_local char buffer[1024];

		va_list args;
		va_start(args, fmt);
		(void)vsprintf(buffer, fmt, args);
		va_end(args);

		return buffer;
	}

}

