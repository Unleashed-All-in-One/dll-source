#pragma once
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
	Hedgehog::Base::CSharedString m_StageXmlData;
	int m_Field1B0; //m_Field08 of SRequestModuleInfo
	BB_INSERT_PADDING(0x8); // ? ? ?	
};
BB_ASSERT_OFFSETOF(CGameplayFlowStageAct, m_pStageLoaderXML, 400);
BB_ASSERT_OFFSETOF(CGameplayFlowStageAct, m_Field1A4, 0x1A4);
BB_ASSERT_OFFSETOF(CGameplayFlowStageAct, m_Field1A8, 0x1A8);
BB_ASSERT_OFFSETOF(CGameplayFlowStageAct, m_StageXmlData, 0x1AC);
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