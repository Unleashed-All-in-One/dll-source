#pragma once
enum PlayerType : int
{
	GENERIC_SONIC = 0,
	CLASSIC_SONIC = 1,
	SUPER_SONIC = 2
};
enum ModuleFlow
{
	Unknown,
	Boot,
	Logo, //SEGA logo
	Title,
	DebugMenu,//WILL crash, missing constructor?
	PlayableMenu, //White World
	StageAct,
	StageEvent,
	Event, //Cutscenes, movie playback
	StaffRoll,
	Genesis,
	OnlineStage,
	TrialMenu,
	NoticeBoard, //post-SEGA logo notices
	Statue, //Collection Room?
	DebugInit, //Used at game launch, if you draw debug text, it'll display the "Thank you for patience" message
	Save
};
namespace SUC::System
{
	class LuaStringEntry {
		BYTE gap0[4];
	public:
		const char* content;
		//thanks skyth
		char* allocateStr(const char* value)
		{
			char* allocatedStr = (char*)__HH_ALLOC(strlen(value) + 1);
			strcpy(allocatedStr, value);
			return allocatedStr;
		}
		LuaStringEntry(const char* integer, bool allocate = true)
		{
			content = allocate ? allocateStr(integer) : integer;
		}
	};

	class LuaStringEntryContainer
	{
	public:
		void* unknown;
		LuaStringEntry* entry;
		LuaStringEntryContainer(const char* content, bool allocate = true)
		{
			LuaStringEntry* newEntry = new LuaStringEntry(content, allocate);
			entry = newEntry;
		}
	};
	class LuaStringEntryContainer2
	{
	public:
		class LuaStringEntry2 {
			BYTE gap0[4];
		public:
			Hedgehog::Base::CSharedString content;			
		};
		void* unknown;
		LuaStringEntry2* entry;
	};


	class LuaIntegerEntry {
		BYTE gap0[4];
	public:
		uint32_t content;
		LuaIntegerEntry(uint32_t integer)
		{
			content = integer;
		}
	};
	class LuaIntegerEntryContainer
	{
	public:
		void* unknown;
		LuaIntegerEntry* entry;

		LuaIntegerEntryContainer(uint32_t integerContent)
		{
			LuaIntegerEntry newEntry = LuaIntegerEntry(integerContent);
			entry = &newEntry;
		}
	};
	class LuaStringIntegerEntryContainer
	{
	public:
		class LuaStringIntegerEntry
		{
			BYTE gap0[4];
		public:
			Hedgehog::Base::CSharedString content;
			LuaIntegerEntry* content2;
			//thanks skyth
			/*char* allocateStr(const char* value)
			{
				char* allocatedStr = (char*)__HH_ALLOC(strlen(value) + 1);
				strcpy(allocatedStr, value);
				return allocatedStr;
			}*/
			LuaStringIntegerEntry(Hedgehog::Base::CSharedString const& integer, int integer2)
			{
				content = integer;
				content2 = new LuaIntegerEntry(integer2);
			}
		};
		BYTE gap0[4];
		LuaStringIntegerEntry* entry;

		LuaStringIntegerEntryContainer(const char* stringContent, int integerContent)
		{
			LuaStringIntegerEntry newEntry1 = LuaStringIntegerEntry(stringContent, integerContent);
			entry = &newEntry1;
		}
	};
	class SequenceHelpers
	{
	public:
		static void ResetStorySequence();
		static void ChangeModule(ModuleFlow in_Flow);
		static void PlayEvent(const char* in_EventName, ModuleFlow in_EventModule);
		static void QueueEvent(const char* in_EventName);
		static void LoadStage(const char* in_StageName, int sequenceEventExtra = 0, bool resetStorySequence = true);
		static void SetPlayerType(int in_PlayerType, bool forced = true);
		static void RegisterHooks();
		static void Update();
		static std::string GetCurrentStageName(bool withoutNumber);
		static Sonic::Sequence::Story* storySequenceInstance;
	};
}
