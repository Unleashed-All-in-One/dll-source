DWORD* SetManager;
class MsgRemakeAllSetObject : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x0167EE98);
};
class ActivateLayer : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x0167F0C0);
	Hedgehog::base::CSharedString layerName;
	bool unk;
};
class Deactivatelayer : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x0167F0D4);
	Hedgehog::base::CSharedString layerName;
};


inline FUNCTION_PTR(char, __thiscall, SetObjectManager_ProcessMessageF, 0x00EB3BE0, DWORD* This, Hedgehog::Universe::Message&, int flag);
//uint32_t __thiscall sub_EB2A80(_DWORD *this, int a2, int a3, int a4)
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
void SetEditorTest::activateLayer(const char* layerName)
{
	ActivateLayer msg = ActivateLayer();
	msg.layerName = "base";
	msg.unk = true;
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
void SetEditorTest::deactivateLayer(const char* layerName)
{
	Deactivatelayer msg = Deactivatelayer();
	msg.layerName = "base";
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
HOOK(void*, __fastcall, SetUpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (inputPtr->IsTapped(Sonic::eKeyState_Y))
	{
		ActivateLayer msg = ActivateLayer();
		msg.layerName = "base";
		msg.unk = true;
		SetObjectManager_ProcessMessageF(SetManager, msg, 0);
	}
	if (inputPtr->IsTapped(Sonic::eKeyState_B))
	{
		Deactivatelayer msg = Deactivatelayer();
		msg.layerName = "base";
		SetObjectManager_ProcessMessageF(SetManager, msg, 0);
	}
	if (GetAsyncKeyState(VK_LEFT) & 1)
	{
		numb--;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 1)
	{
		numb++;
	}
	if (SetManager)
	{
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
	//INSTALL_HOOK(SetUpdateApplication);
	INSTALL_HOOK(SetObjectManager_ProcessMessage);
}