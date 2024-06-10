DWORD* SetManager;
class MsgRemakeAllSetObject : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x0167EE98);
};

BB_ASSERT_SIZEOF(MsgRemakeAllSetObject, 0x10);
class ActivateLayer : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x0167F0C0);
	Hedgehog::base::CSharedString m_SetLayerName;
	bool m_Field18;
	ActivateLayer(const Hedgehog::base::CSharedString& in_SetLayerName, const bool in_Field18) : m_SetLayerName(in_SetLayerName), m_Field18(in_Field18) {}
};
BB_ASSERT_OFFSETOF(ActivateLayer, m_SetLayerName, 0x10);
BB_ASSERT_OFFSETOF(ActivateLayer, m_Field18, 0x14);
BB_ASSERT_SIZEOF(ActivateLayer, 0x18);
class MsgDeactivatelayer : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x0167F0D4);
	Hedgehog::base::CSharedString m_SetLayerName;
};

BB_ASSERT_OFFSETOF(MsgDeactivatelayer, m_SetLayerName, 0x10);
BB_ASSERT_SIZEOF(MsgDeactivatelayer, 0x14);


inline FUNCTION_PTR(char, __thiscall, SetObjectManager_ProcessMessageF, 0x00EB3BE0, DWORD* This, Hedgehog::Universe::Message&, int flag);
//uint32_t __thiscall sub_EB2A80(DWORD *this, int a2, int a3, int a4)
HOOK(char, __fastcall, SetObjectManager_ProcessMessage, 0x00EB3BE0, DWORD* This, void* Edx, Hedgehog::Universe::Message& message, int flag)
{
	SetManager = This;
	return originalSetObjectManager_ProcessMessage(This, Edx, message, flag);
}
void SetEditorTest::remakeAllObjects()
{	
	MsgRemakeAllSetObject msg = MsgRemakeAllSetObject();
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
void SetEditorTest::activateLayer(const char* m_SetLayerName)
{
	ActivateLayer msg = ActivateLayer("base", true);
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
void SetEditorTest::deactivateLayer(const char* m_SetLayerName)
{
	MsgDeactivatelayer msg = MsgDeactivatelayer();
	msg.m_SetLayerName = "base";
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
bool showSetTest = false;
std::string nameLayer;
void SetEditorTest::draw()
{
	ImGui::SetNextWindowBgAlpha(1);
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;

	if (GetAsyncKeyState(VK_F2) & 1)
		showSetTest ^= true;
	if (SetManager == nullptr)
		showSetTest = false;
	if (ImGui::Begin("SetEditorTest", &showSetTest, flags))
	{
		if (ImGui::Button("MsgRemakeAllSetObject"))
		{
			SetEditorTest::remakeAllObjects();
		}
		if (ImGui::Button("MsgActivateLayer (base)"))
		{
			SetEditorTest::activateLayer("");
		}
		if (ImGui::Button("MsgDeactivateLayer (base)"))
		{
			SetEditorTest::deactivateLayer("");
		}
	}
	ImGui::End();
}
int numb;
struct CSetLayerManager
{
public:
	Hedgehog::Base::CSharedString* hedgehog__base__csharedstring0;
	byte gap4[12];
	Hedgehog::Base::CSharedString* phedgehog__base__csharedstring10;
};

HOOK(void*, __fastcall, SetUpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	
	if (inputPtr->IsTapped(Sonic::eKeyState_B))
	{
		MsgDeactivatelayer msg = MsgDeactivatelayer();
		msg.m_SetLayerName = "base";
		SetObjectManager_ProcessMessageF(SetManager, msg, 0);
	}
	if (GetAsyncKeyState(VK_LEFT) & 1)
	{
		numb--;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 1)
	{
		auto member =  Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember;
		auto member1 =  Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager;
		auto paramBank = (uint32_t*)Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spParameterBankManager.get();
		auto a1 = ((uint32_t*)Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember) + 0x38;
		auto fo = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spUserIDGroupCategoryManager;
		auto one =  Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spParameterBankManager;
		auto tw = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager;
		auto th = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetObjectEventManager;

		/*FUNCTION_PTR(void, __stdcall, sub_4ECB60, 0x4ECB60, int a1P);
		auto v1 = a1 + 3;
		sub_4ECB60(*(DWORD*)(a1[4] + 4));
		*(DWORD*)(v1[1] + 4) = v1[1];
		auto v2 = (DWORD*)a1[4];
		a1[5] = 0;
		*(DWORD*)(v1[1] + 8) = v1[1];
		auto result = (int*)a1[1];
		auto v4 = (int*)*result;
		auto a2 = (int)v4;
		auto fff = *(DWORD*)(v4[4] + 20);

		void* ff2 = *(void**)fff;*/
		

	}
	if (SetManager)
	{
		



		//int* setManagerIntP = 
		/*auto t = SetManager - 40;
		auto v2 = *((DWORD*)t + 43);
		auto v4 = *(DWORD*)(v2 + 80);
		auto v22 = (uint32_t*)*((DWORD*)v4 + 1);
		auto v23 = *(uint32_t**)v22;
		const char* n = (*(Hedgehog::base::CSharedString*)(v23 + 4 + numb)).c_str();
		DebugDrawText::log(std::format("SETTEST\n\n{0}", n).c_str(), 0);*/

	}
	return originalSetUpdateApplication(This, Edx, elapsedTime, a3);
}
void SetEditorTest::applyPatches()
{
	INSTALL_HOOK(SetUpdateApplication);
	INSTALL_HOOK(SetObjectManager_ProcessMessage);
}