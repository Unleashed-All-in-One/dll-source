DWORD* SetManager;

inline FUNCTION_PTR(char, __thiscall, SetObjectManager_ProcessMessageF, 0x00EB3BE0, DWORD* This, Hedgehog::Universe::Message&, int flag);
//uint32_t __thiscall sub_EB2A80(DWORD *this, int a2, int a3, int a4)
HOOK(char, __fastcall, SetObjectManager_ProcessMessage, 0x00EB3BE0, DWORD* This, void* Edx, Hedgehog::Universe::Message& message, int flag)
{
	SetManager = This;
	return originalSetObjectManager_ProcessMessage(This, Edx, message, flag);
}
void SetEditorTest::remakeAllObjects()
{	
	Sonic::Message::MsgRemakeAllSetObject msg = Sonic::Message::MsgRemakeAllSetObject();
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
void SetEditorTest::activateLayer(const char* m_SetLayerName)
{
	Sonic::Message::MsgActivateLayer msg = Sonic::Message::MsgActivateLayer("base", true);
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
void SetEditorTest::deactivateLayer(const char* m_SetLayerName)
{
	Sonic::Message::MsgDeactivateLayer msg = Sonic::Message::MsgDeactivateLayer("base");
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
	if(showSetTest)
	{
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
	
	
	return originalSetUpdateApplication(This, Edx, elapsedTime, a3);
}
void SetEditorTest::applyPatches()
{
	//INSTALL_HOOK(SetUpdateApplication);
	INSTALL_HOOK(SetObjectManager_ProcessMessage);
}