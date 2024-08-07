#pragma once
#define INI_FILE "UnleashedConversion.ini"
#define STAGE_LIST_FILE "stage_list.json"
#define ARCHIVE_LIST_FILE "archivelist.json"
#define QUEUE_LIST_FILE "sequence.json"
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

		//---------------Functions---------------
		static void Load(const char* path);
		static void GetStageList();
		static void GetDebugTree();
		static void RegisterGlobalHooks();
		static void GetLevelQueue();
		static void GetTempCustomArchiveTree();
		static int GetFlagFromStage(const char* stage);
		static int GetCapital(int flagID, bool isNight);
		static std::vector<std::string> GetAllWhiteWorld();
		static float GetDeltaTime() { return ms_DeltaTime; }
		static void SetDeltaTime(float dt) { ms_DeltaTime = dt; }
		static std::string GetDirectoryPath(const std::string& path);
		static float GetFrameDeltaTime() { return ms_FrameDeltaTime; }
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
		static bool s_IgnoreWarnings;
		static WorldData s_WorldData;
		static bool s_LargeAddressAware;
		static DebugStageTree s_DebugStageTree;
		static bool s_CpkRedirCompatibilityMode;
		static SequenceData s_SequenceDataQueue;
		static Hedgehog::Math::CVector s_TempArmswingNode;
		static std::vector<std::string> s_GenerationsStages;
		static ArchiveTreeDefinitions s_AdditionalArchiveTree;
	};
}