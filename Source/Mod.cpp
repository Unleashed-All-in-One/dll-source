
#include "Accuracy/AccuracyHookContainer.h"
#include "Gameplay/GameplayHookContainer.h"
#include "Player/PlayerHookContainer.h"
#include "SetObject/SetObjectHookContainer.h"
#include "System/DiscordStatus.h"
#include "System/SystemHookContainer.h"
#include "Testing/TestingCode.h"
#include "UI/EventViewer.h"
#include "UI/UIHookContainer.h"

//void __thiscall InAppTracker::StartTracking(_BYTE *this, const char *a2, const char *a3, const char *a4)
HOOK(void, __fastcall, StartTracking, 0x00A5C930, void* This, void* Edx, const char* a2, const char* a3, const char* a4)
{
	DebugDrawText::log(SUC::Format("TRACK: %s | %s | %s", a2,a3,a4));
	originalStartTracking(This, Edx, a2, a3, a4);
}
extern "C" __declspec(dllexport) void PreInit(ModInfo_t * modInfo)
{
	// Load configuration
	SUC::Project::Load(modInfo);
	//Make Gens support 4gb+
	if(SUC::Project::s_LargeAddressAware)
		modInfo->API->SendMessageToLoader(ML_MSG_REQ_LARGE_ADDRESS_AWARE, nullptr);

	std::srand(static_cast<unsigned int>(std::time(nullptr)));
}
extern "C" __declspec(dllexport) void Init(ModInfo_t * modInfo)
{
#if _DEBUG
	MessageBox(nullptr, TEXT("Attach Debugger to see hooks."), TEXT("Unleashed Conversion"), MB_ICONINFORMATION);
#endif
	SUC::Project::ParseGlobalFile();
	SUC::Project::RegisterGlobalHooks();
	SUC::Hooks::InstallGameplayHooks();
	SUC::Hooks::InstallAccuracyHooks();
	SUC::Hooks::InstallSetObjectHooks();
	SUC::Hooks::InstallSystemHooks();
	SUC::Hooks::InstallUIHooks();
	SUC::Hooks::InstallPlayerHooks();
	TestingCode::applyPatches();
}
extern "C" __declspec(dllexport) void PostInit()
{
	SUC::Project::CheckIncompatibleMods();
}
extern "C" void __declspec(dllexport) OnFrame()
{	
	//---------------System---------------
	SUC::ImGuiMenu::ImGuiContextManager::Update();
	SUC::System::Discord::DiscordStatus::Update();
	SUC::System::StageManager::Update();
	SUC::UI::Event::EventViewer::Update();
	SUC::System::SequenceHelpers::Update();
}