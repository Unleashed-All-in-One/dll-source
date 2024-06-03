Sonic::Sequence::Story* SequenceHelpers::storySequenceInstance;
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
	/*Sonic::Message::MsgRequestChangeModule* message = new Sonic::Message::MsgRequestChangeModule(in_Flow);
	Sonic::Sequence::Main::ProcessMessage(message)*/;
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

std::string SequenceHelpers::getCurrentStageName(bool withoutNumber = false)
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	std::string returnVal = *h;
	if (withoutNumber)
	{
		returnVal.erase(returnVal.length() - 3);
	}
	return returnVal;
}
void SequenceHelpers::resetStorySequence()
{
	//you have to run this twice. dont ask me why, what works works.
	FUNCTION_PTR(void, __thiscall, StorySeqProcessStorySequenceEvent, 0x00D76980, Sonic::Sequence::Story * StorySeq, Sonic::Message::MsgStorySequenceEvent * a2);

	auto message2 = Sonic::Message::MsgStorySequenceEvent(0, 0);
	StorySeqProcessStorySequenceEvent(storySequenceInstance, &message2);
	StorySeqProcessStorySequenceEvent(storySequenceInstance, &message2);
}
/// <summary>
/// Sends a message to CMainSequence to trigger a stage load.
/// </summary>
/// <param name="in_EventName">Stage archive name</param>
void SequenceHelpers::loadStage(const char* in_StageName, int sequenceEventExtra, bool resetStorySequence) 
{
	nextStage = allocateStr(in_StageName);
	//int __thiscall Sonic::Sequence::CStoryImpl::LuanneFunctions::BeginStory(int this, int a2, int a3)
	//void __thiscall Sonic::Sequence::CStoryImpl::LuanneFunctions::StoryLabel(int this, int a2, int stringComparison)
	auto message = Sonic::Message::MsgSequenceEvent(2, sequenceEventExtra);
	auto message2 = Sonic::Message::MsgStorySequenceEvent(0, 0);
	auto test = Sonic::Sequence::Main::GetInstance();
	//void __thiscall StorySeqProcessStorySequenceEvent(int storySequence, CMsgStorySequenceEvent *storySequenceEvent)
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	const char* terr = *h;
	strcpy(*(char**)stageTerrainAddress, in_StageName);
	if (resetStorySequence)
	{
		SequenceHelpers::resetStorySequence();
	}
	FUNCTION_PTR(int, __thiscall, StartModuleF, 0x00D77020, Sonic::Sequence::Story * StorySequence, int a2, LuaStringEntryContainer * a3);
	StartModuleF(storySequenceInstance, 0, new LuaStringEntryContainer(_strdup("Stage")));
	//Sonic::Sequence::Main::ProcessMessage(&message);
	uint32_t stageIDAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x0 });
	uint32_t* test2 = &stageIDAddress;
	*test2 = 0;
}
/// <summary>
/// Uses the SetupPlayer and SetupPlayerForce functions from CStorySequence to change player classes
/// NOTE: This does not switch players in realtime.
/// </summary>
/// <param name="in_PlayerType"></param>
void SequenceHelpers::setPlayerType(int in_PlayerType, bool forced)
{
	nextPlayerType = in_PlayerType;
	if (forced)
	{
		FUNCTION_PTR(char, __thiscall, SetupPlayerForce, 0xD72270, Sonic::Sequence::Story * This, int a2, LuaIntegerEntryContainer * a3);
		LuaIntegerEntryContainer newLuaMessage = LuaIntegerEntryContainer(in_PlayerType);
		SetupPlayerForce(storySequenceInstance, 0, &newLuaMessage);
	}
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
	return originalCStoryLua_SetupPlayerForce(SequenceHelpers::storySequenceInstance, Edx, a2, a3);
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

	DebugDrawText::log(std::format("[LUA_STORYSEQUENCE] SetupStage received \"{}\"", stage->entry->content).c_str(), 100);
	nextStage = nullptr;
	originalCStoryLua_SetupStage(StorySequence, Edx, a2, stage);
}
HOOK(Sonic::Sequence::Story*, __cdecl, ConstructStorySequence, 0xD76930)
{
	SequenceHelpers::storySequenceInstance = originalConstructStorySequence();
	return SequenceHelpers::storySequenceInstance;
}
void SequenceHelpers::applyPatches()
{
	INSTALL_HOOK(ConstructStorySequence);
	INSTALL_HOOK(CStoryLua_SetupPlayer);
	INSTALL_HOOK(CStoryLua_SetupPlayerForce);
	INSTALL_HOOK(CStoryLua_SetupStage);
}
#pragma endregion