#pragma once
#include "System/ArchiveTreePatcher.h"
#include "System/tinyxml2.h"

//namespace mod = SUC;
//namespace SUC
//{
//	namespace acc = Accuracy;
//	namespace ui = UI;
//	namespace set = SetObject;
//	namespace sys = System;
//	namespace game = Gameplay;
//	namespace ply = Player;
//	namespace Player
//	{
//		namespace ev = Evil;
//	}
//}
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
			struct STreeNode
			{
				struct SNodeEntry
				{
					enum ENodeType
					{
						eNodeType_Stage,
						eNodeType_StageNight,
						eNodeType_Cutscene
					};
					std::string Stage;
					std::string EventID;
					std::string DisplayName;
					ENodeType Type;
					bool IsNight;
				};
				std::string Name;
				std::vector<SNodeEntry> TreeEntry;
				std::vector<STreeNode> Children;
			};
			std::vector<STreeNode> TreeNodes;
		};
		struct ArchiveTreeDefinitions
		{
			std::vector<System::ArchiveTreePatcher::ArchiveDependency> Dependencies;
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
		static void ParseGlobalFile();
		static void RegisterGlobalHooks();
		static void CheckIncompatibleMods();

		//---------------Functions---------------
		static void GetStageList(tinyxml2::XMLDocument& in_XmlDocument);
		static void GetDebugTree(tinyxml2::XMLDocument& in_XmlDocument);
		static void GetLevelQueue();
		static void GetTempCustomArchiveTree(tinyxml2::XMLDocument& in_XmlDocument);
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
		static float s_DeltaTo60;

		static std::map<const char*, const char*> s_IncompatibleMods;
	};
	inline char* Format(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		const int size = snprintf(NULL, 0, fmt, args);
		thread_local char* buffer = (char*)malloc(size + 1);
		(void)vsprintf(buffer, fmt, args);
		va_end(args);

		return buffer;
	}
	inline void LogMessage(const char* in_String)
	{
		printf(Format("\n[%s] %s"), MOD_NAME, in_String);
	}

}

