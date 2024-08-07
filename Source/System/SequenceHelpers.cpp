#include <Hedgehog/Universe/Engine/hhStateMachineBase.h>
#include "..\BlueBlurCustom\Sonic\System\ApplicationDocument.h"
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
class MsgRequestEndModule : public Hedgehog::Universe::MessageTypeGet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x01681224);
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
class CModeCreaterList
{
	BB_INSERT_PADDING(12);

};
class CGameplayFlow : public Hedgehog::Universe::CStateMachineBase::CStateBase
{
public:
	int m_Field60;
	int m_Field64;
	int m_Field68;
	int m_Field6C;
	int m_Field70;
};
BB_ASSERT_OFFSETOF(CGameplayFlow, m_Field60, 0x60);
BB_ASSERT_OFFSETOF(CGameplayFlow, m_Field64, 0x64);
BB_ASSERT_OFFSETOF(CGameplayFlow, m_Field68, 0x68);
BB_ASSERT_OFFSETOF(CGameplayFlow, m_Field6C, 0x6C);
BB_ASSERT_OFFSETOF(CGameplayFlow, m_Field70, 0x70);
class CGameplayFlowStage : public CGameplayFlow
{
public:
	//Some of this shit may be IUpdateCoordinator
	void* m_UpdateCoordinator;
	void* m_Field78;
	void* m_Field7C;
	void* m_Field80;
	void* m_Field84;
	void* m_Field88;
	void* m_Field8C;
	BB_INSERT_PADDING(0x20);
	float m_FieldB0;
	char m_FieldB4;
	char m_FieldB5;
	char m_FieldB6;
	char m_FieldB7;
	char m_FieldB8;
	Hedgehog::Universe::TTinyStateMachine<CGameplayFlowStage>* m_FieldBC;
	char gapC0[44];
	void* m_FieldEC;
	int m_IsPaused;
	int m_ModeID;
	char gapF8;
	char m_FieldF9;
	char m_FieldFA;
	char m_FieldFB;
	char gapFC;
	char m_FieldFD;
	__declspec(align(4)) void* m_Field100;
	void* m_Field104;
	void* m_Field108;
	void* m_Field10C;
	void* m_Field110;
	void* m_Field114;
	char m_Field118;
	__declspec(align(4)) void* m_Field11C;
	int m_Field120;
	char m_Field124;
	char m_Field125;
	__declspec(align(4)) void* m_Field128;
	char gap12C[8];
	char m_Field134;
	char gap135[11];
	float m_Field140;
	float m_Field144;
	void* m_Field148;
	float m_Field14C;
	Hedgehog::math::CQuaternion m_Field150;
	void* m_Field160; //some kind of list
	boost::shared_ptr<void>* m_spSoundDataCueControl;
	void* m_Field168;
	void* m_Field16C;
	void* m_Field170;
	void* m_Field174;
	void* m_Field178;
	void* m_Field17C;
	BB_INSERT_PADDING(0x10);
};
BB_ASSERT_OFFSETOF(CGameplayFlowStage, m_UpdateCoordinator, 0x74);
BB_ASSERT_OFFSETOF(CGameplayFlowStage, m_spSoundDataCueControl, 0x164);
BB_ASSERT_SIZEOF(CGameplayFlowStage, 0x180 + 0x10);//???
Sonic::Message::SRequestChangeModuleInfo* info;
//224

struct StageLoaderXML
{
	void* dword0;
	void* Field4;
	void* Field8;
	char FieldC;
	__declspec(align(4)) Hedgehog::Base::CSharedString* phedgehog__base__csharedstring10;
	char Field14;
	char Field15;
	char Field16;
	__declspec(align(2)) void* Field18;
	__declspec(align(8)) char Field20;
	char gap21[15];
	BB_INSERT_PADDING(0x10);
	char gap40[4];
	void* Field44;
	void* Field48;
	__declspec(align(8)) void* Field50;
	void* Field54;
	char Field58;
	__declspec(align(4)) void* Field5C;
	void* Field60;
	void* Field64;
	char gap68[40];
	char Field90;
	char Field91;
	char gap92[14];
	Hedgehog::Math::CQuaternion FieldA0;
	Hedgehog::Math::CQuaternion FieldB0;
	char gapC0;
	char FieldC1;
	Hedgehog::base::CSharedString FieldC4;
	void* FieldC8;
	void* FieldCC;
	Hedgehog::base::CSharedString FieldD0;
	float floatD4;
};
BB_ASSERT_OFFSETOF(StageLoaderXML, floatD4, 0xD4);
class CGameplayFlowStageAct : public CGameplayFlowStage
{
public:
	StageLoaderXML* m_pStageLoaderXML;
	boost::shared_ptr<StageLoaderXML>* m_spStageLoaderXML;
	int m_Field198;
	int m_Field19C;
	Sonic::CGameActParameter* m_pGameActParameter;
	Hedgehog::Base::CSharedString m_Field1A4;
	Hedgehog::Base::CSharedString m_Field1A8;
	Hedgehog::Base::CSharedString m_Field1AC;
	int m_Field1B0; //m_Field08 of SRequestModuleInfo
	BB_INSERT_PADDING(0x8); // ? ? ?	
};
BB_ASSERT_OFFSETOF(CGameplayFlowStageAct, m_pStageLoaderXML, 400);
BB_ASSERT_OFFSETOF(CGameplayFlowStageAct, m_Field1A4, 0x1A4);
BB_ASSERT_OFFSETOF(CGameplayFlowStageAct, m_Field1A8, 0x1A8);
BB_ASSERT_OFFSETOF(CGameplayFlowStageAct, m_Field1AC, 0x1AC);
BB_ASSERT_SIZEOF(CGameplayFlowStageAct, 0x1C0);
class CGameplayFlowStagePlayableMenu : public CGameplayFlowStageAct
{
public:
	int m_Field1C0; /// SERIOUSLY?!
};
BB_ASSERT_OFFSETOF(CGameplayFlowStagePlayableMenu, m_Field1C0, 0x1C0);
class CGameplayFlowTrialMenu : public CGameplayFlow
{
	BB_INSERT_PADDING(144 - sizeof(CGameplayFlow));
public:
	

};
BB_ASSERT_SIZEOF(CGameplayFlowTrialMenu, 144);
struct MsgReplyChangeModule : public hh::fnd::MessageTypeSet
{
	HH_FND_MSG_MAKE_TYPE(0x01681238);
};
struct SRequestChangeModuleInfo2
{
public:
	void* t;
	Sonic::Message::SRequestChangeModuleInfo* two;
};
struct struct_a2_5
{
	Hedgehog::Base::CSharedString hedgehog__base__csharedstring0;
	Hedgehog::Base::CSharedString hedgehog__base__csharedstring4;
	int dword8;
	Hedgehog::Base::CSharedString hedgehog__base__csharedstringC;
	Hedgehog::Base::CSharedString hedgehog__base__csharedstring10;
	char char14;
	void* pdword18;
	__declspec(align(8)) void* dword20;
	void* dword24;
	Hedgehog::Base::CSharedString hedgehog__base__csharedstring28;
	Hedgehog::Base::CSharedString hedgehog__base__csharedstring2C;
	float float30;
	float float34;
};


bool init2 = false;
static constexpr int STAGEACT_CONSTRUCTOR = 0x00D0DEB0;
static constexpr int STAGEPLAYABLEMENU_CONSTRUCTOR = 0x00D0DE30;
void ModuleChangeProcessor(const Sonic::Message::SRequestChangeModuleInfo& in_Info, int in_Address, int moduleIndex)
{
	FUNCTION_PTR(int, __thiscall, ChangeStateWithoutRegistering, 0x76EE60, void* in_StateMachine, int* out_Data, const boost::shared_ptr<Hedgehog::Universe::CStateMachineBase::CStateBase>&in_StateBase, int a4, float a5, int a6);
	FUNCTION_PTR(boost::shared_ptr<CGameplayFlowStageAct>, __cdecl, InitCGameplayFlowStageAct, in_Address, const Sonic::Message::SRequestChangeModuleInfo & in_ModuleRequestInfo, void* in_Manager);
	auto appDocMember = (Sonic2::CApplicationDocument::CMember*)Sonic::CApplicationDocument::GetInstance()->m_pMember;
	auto manager = appDocMember->m_pGameplayFlowManager;
	boost::shared_ptr<CGameplayFlowStageAct> m_StateThatHasToBeAdded = boost::make_shared<CGameplayFlowStageAct>();

	m_StateThatHasToBeAdded = InitCGameplayFlowStageAct(in_Info, appDocMember->m_pGameplayFlowManager);
	auto m_ReplyChangeModule = MsgReplyChangeModule(); //not sure if this is necessary
	auto m_EndChangeModule = MsgRequestEndModule();
	auto g = appDocMember->m_pGameplayFlowManager;
	manager->ProcessMessageInStateMachine(m_EndChangeModule, 0);
	manager->ProcessMessageInStateMachine(m_ReplyChangeModule, 0);
	if(m_StateThatHasToBeAdded->m_Name == manager->GetCurrentState()->m_Name)
	{
		manager->test->m_CurrentModuleIndex = moduleIndex;
		manager->test->m_pModuleManager->m_ModuleConstructorAddress = (Sonic2::CModuleManager::SModuleConstructor*)(8 * moduleIndex + 0x15B8528);
		manager->test->m_BitmaskWeird44 = 2;
	}
	else
	{
		int m_Unused[3];
		ChangeStateWithoutRegistering(appDocMember->m_pGameplayFlowManager, m_Unused, m_StateThatHasToBeAdded, 0, 0, 0);
		m_StateThatHasToBeAdded.reset();		
	}
	//Force the game to end the current mode prematurely
	//
	//
	//
	//8 * m_ModuleIndex + 0x15B8528
}
void SequenceHelpers::changeModule(ModuleFlow in_Flow)
{
	FUNCTION_PTR(void, __stdcall, ChangeModuleTest, 0x01107D50, Hedgehog::Universe::CMessageActor * Th, int a2);
	
	//ChangeModuleTest(Sonic::Sequence::Main::GetInstance(), (int)in_Flow);
	Sonic::Message::MsgRequestChangeModule* message2 = new Sonic::Message::MsgRequestChangeModule();
	message2->m_ModuleIndex = (int)in_Flow;
	message2->moduleInfo = Sonic::Message::SRequestChangeModuleInfo();
	if(in_Flow == StageAct)
	{
		//message2->moduleInfo.m_Field00 = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter->m_pStageParameter->TerrainArchiveName;
		//message2->moduleInfo.m_Field04 = "Stage";
	}
	message2->moduleInfo.m_Field00 = "DMykonos_2";
	message2->moduleInfo.m_Field04 = "Stage";
	message2->moduleInfo.m_Field20 = hh::list<Hedgehog::base::CSharedString>();
	
	init2 = true;
	ModuleChangeProcessor(message2->moduleInfo, STAGEACT_CONSTRUCTOR, (int)in_Flow);
	//Sonic::CApplicationDocument::GetInstance()->m_pMember->m_pGameplayFlowManager->RegisterStateFactory();


	//Sonic::CApplicationDocument::GetInstance()->m_pMember->m_pGameplayFlowManager->ChangeState(m_StateThatHasToBeAdded->m_Name);
	
	//actor->ProcessMessage(*message2, 0);
	//Sonic::Sequence::Main::ProcessMessage(message);
	//Sonic::Sequence::Main::ProcessMessage(message2);
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
	std::string returnVal = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter->m_pStageParameter->TerrainArchiveName.c_str();
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
	changeModule(ModuleFlow::StageAct);
	nextStage = allocateStr(in_StageName);	
	auto message = Sonic::Message::MsgSequenceEvent(2, sequenceEventExtra);	
	Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter->m_pStageParameter->TerrainArchiveName = _strdup(in_StageName);
	Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter->m_pStageParameter->ModeName = _strdup("Stage");
	auto actor = (hh::fnd::CMessageActor*)(Sonic::CApplicationDocument::GetInstance()->m_pMember + 0x34);
	//auto msg = Sonic::Message::MsgRequestChangeModule();
	
	//actor->ProcessMessage(msg)
	if (resetStorySequence)
	{
		SequenceHelpers::resetStorySequence();
	}
	//Sonic::Sequence::Main::ProcessMessage(&message);
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
void SequenceHelpers::Update()
{
	if (GetAsyncKeyState(VK_F12))
	{
		auto appDocMember = (Sonic2::CApplicationDocument::CMember*)Sonic::CApplicationDocument::GetInstance()->m_pMember;
		CGameplayFlowStageAct* act = (CGameplayFlowStageAct*)appDocMember->m_pGameplayFlowManager->GetCurrentState().get();
		printf("");
		 
	}
}
void SequenceHelpers::applyPatches()
{
	INSTALL_HOOK(ConstructStorySequence);
	INSTALL_HOOK(CStoryLua_SetupPlayer);
	INSTALL_HOOK(CStoryLua_SetupPlayerForce);
	INSTALL_HOOK(CStoryLua_SetupStage);
}
#pragma endregion