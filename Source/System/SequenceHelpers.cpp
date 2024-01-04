Sonic::Sequence::Story* storySequenceInstance;
int nextPlayerType = 0;
const char* nextStage = nullptr;
bool test = false;

class MsgGetEventQueueState : public Hedgehog::Universe::MessageTypeGet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x01681C80);

	int m_pPosition;

	MsgGetEventQueueState(int in_pPosition) : m_pPosition(in_pPosition) {}
};
char* allocateStr(const char* value)
{
	char* allocatedStr = (char*)__HH_ALLOC(strlen(value) + 1);
	strcpy(allocatedStr, value);
	return allocatedStr;
}
/// <summary>
/// Sends a message to CMainSequence to trigger a module change.
/// Modules are like "Scenes".
/// </summary>
/// <param name="in_Flow">Module to switch to</param>
void SequenceHelpers::changeModule(ModuleFlow in_Flow)
{
	FUNCTION_PTR(void, __stdcall, ChangeModuleTest, 0x01107D50, Hedgehog::Universe::CMessageActor * Th, int a2);
	ChangeModuleTest(Sonic::Sequence::Main::GetInstance(), (int)in_Flow);
}
void SequenceHelpers::queueEvent(const char* in_EventName)
{
	test = true;
	auto message = Sonic::Message::MsgEventRequest(in_EventName, 0);
	Sonic::Event::EventQueue::ProcessMessage(&message);
}
void getEventState()
{
	if (!test)
		return;
	auto msgno = MsgGetEventQueueState(0);
	MsgGetEventQueueState* message = &msgno;
	Sonic::Event::EventQueue::ProcessMessage(message);
	DebugDrawText::log(std::to_string(message->m_pPosition).c_str(), 0);
}
/// <summary>
/// Sends a message to CEventQueue to queue an event.
/// </summary>
/// <param name="in_EventName">Event archive name</param>
/// <param name="in_EventModule">Event module, if it's prerendered use "Event"
/// If it's realtime, use "StageEvent"</param>
void SequenceHelpers::playEvent(const char* in_EventName, ModuleFlow in_EventModule)
{
	auto message = Sonic::Message::MsgEventRequest(in_EventName, 0);
	Sonic::Event::EventQueue::ProcessMessage(&message);
	SequenceHelpers::changeModule(in_EventModule);
}
/// <summary>
/// Sends a message to CMainSequence to trigger a stage load.
/// </summary>
/// <param name="in_EventName">Stage archive name</param>
void SequenceHelpers::loadStage(const char* in_StageName) 
{
	nextStage = allocateStr(in_StageName);
	auto message = Sonic::Message::MsgSequenceEvent(2, 0);
	Sonic::Sequence::Main::ProcessMessage(&message);
}
/// <summary>
/// Uses the SetupPlayer and SetupPlayerForce functions from CStorySequence to change player classes
/// NOTE: This does not switch players in realtime.
/// </summary>
/// <param name="in_PlayerType"></param>
void SequenceHelpers::setPlayerType(int in_PlayerType)
{
	FUNCTION_PTR(char, __thiscall, SetupPlayerForce, 0xD72270, Sonic::Sequence::Story* This, int a2, LuaIntegerEntryContainer * a3);
	
	nextPlayerType = in_PlayerType;
	LuaIntegerEntryContainer newLuaMessage = LuaIntegerEntryContainer(in_PlayerType);
	SetupPlayerForce(storySequenceInstance, 0, &newLuaMessage);
}


#pragma region Compromises
HOOK(char, __fastcall, CStoryLua_SetupPlayerForce, 0xD72270, Sonic::Sequence::Story* StorySequence, void* Edx, int a2, LuaIntegerEntryContainer* a3)
{
	if (nextPlayerType != -1)
	{
		if (nextPlayerType == 0)
			DebugDrawText::log(std::format("[SH] Set player type to Modern").c_str(), 5);
		else if (nextPlayerType == 1)
			DebugDrawText::log(std::format("[SH] Set player type to Classic").c_str(), 5);
		a3->entry->content = nextPlayerType;
		nextPlayerType = -1;
	}
	return originalCStoryLua_SetupPlayerForce(storySequenceInstance, Edx, a2, a3);
}
HOOK(void, __fastcall, CStoryLua_SetupPlayer, 0xD72360, Sonic::Sequence::Story* StorySequence, void* Edx, int a2, LuaIntegerEntryContainer* a3)
{
	if (nextPlayerType != -1)
	{
		if (nextPlayerType == 0)
			DebugDrawText::log(std::format("[SH] Set player type to Modern").c_str(), 5);
		else if(nextPlayerType == 1)
			DebugDrawText::log(std::format("[SH] Set player type to Classic").c_str(), 5);
		a3->entry->content = nextPlayerType;
		nextPlayerType = -1;
	}
	
	return originalCStoryLua_SetupPlayer(StorySequence, Edx, a2, a3);
}
HOOK(void, __fastcall, CStoryLua_SetupStage, 0x00D71A90, Sonic::Sequence::Story* StorySequence, void* Edx, DWORD* a2, LuaStringEntryContainer* stage)
{
	if (nextStage != nullptr)
		stage->entry->content = nextStage;
	nextStage = nullptr;
	originalCStoryLua_SetupStage(StorySequence, Edx, a2, stage);
}
HOOK(Sonic::Sequence::Story*, __fastcall, ConstructStorySequence, 0xD76930)
{
	storySequenceInstance = originalConstructStorySequence();
	return storySequenceInstance;
}
void SequenceHelpers::update()
{
}
void SequenceHelpers::applyPatches()
{
	INSTALL_HOOK(ConstructStorySequence);
	INSTALL_HOOK(CStoryLua_SetupPlayer);
	INSTALL_HOOK(CStoryLua_SetupPlayerForce);
	INSTALL_HOOK(CStoryLua_SetupStage);
}
#pragma endregion