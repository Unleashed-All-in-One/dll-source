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
	LuaStringEntry(const char* integer)
	{
		content = allocateStr(integer);
	}
};

class LuaStringEntryContainer
{
public:
	void* unknown;
	LuaStringEntry* entry;
	LuaStringEntryContainer(const char* content)
	{
		LuaStringEntry newEntry = LuaStringEntry(content);
		entry = &newEntry;
	}
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
class SequenceHelpers
{
public:
	static void changeModule(ModuleFlow in_Flow);
	static void playEvent(const char* in_EventName, ModuleFlow in_EventModule);
	static void loadStage(const char* in_StageName);
	static void setPlayerType(int in_PlayerType);
	static void applyPatches();
};
