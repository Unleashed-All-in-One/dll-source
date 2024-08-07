#include "UI/EventViewer.h"

extern "C" __declspec(dllexport) void PreInit(ModInfo_t * modInfo)
{
	// Load configuration
	SUC::Project::Load(modInfo->CurrentMod->Path);
	//Make Gens support 4gb+
	if(SUC::Project::s_LargeAddressAware)
		modInfo->API->SendMessageToLoader(ML_MSG_REQ_LARGE_ADDRESS_AWARE, nullptr);

	std::srand(static_cast<unsigned int>(std::time(nullptr)));
}
extern "C" __declspec(dllexport) void Init(ModInfo_t * modInfo)
{
#if _DEBUG
	MessageBox(nullptr, TEXT("Attach Debugger and press OK."), TEXT("Unleashed Conversion"), MB_ICONINFORMATION);
#endif

	SUC::Project::RegisterGlobalHooks();
}
extern "C" __declspec(dllexport) void PostInit()
{
	if (GetModuleHandle(TEXT("EnemyTrigger.dll")))
	{
		MessageBox(nullptr, TEXT("\"Enemy Trigger\" mod should not be enabled with this mod."), TEXT("Unleashed Conversion"), MB_ICONERROR);
		exit(-1);
	}
	
	if (GetModuleHandle(TEXT("QSSRestoration.dll")))
	{
		MessageBox(nullptr, TEXT("\"QSS Restoration\" mod should not be enabled with this mod."), TEXT("Unleashed Conversion"), MB_ICONERROR);
		exit(-1);
	}
	if (GetModuleHandle(TEXT("UnleashedTitlescreen.dll")))
	{
		MessageBox(nullptr, TEXT("\"SWA Titlescreen\" mod should not be enabled with this mod."), TEXT("Unleashed Conversion"), MB_ICONERROR);
		exit(-1);
	}
}
extern "C" void __declspec(dllexport) OnFrame()
{	
	//---------------System---------------
	ImguiInitializer::update();
	CSDCommon::update();
	DiscordStatus::update();
	StageManager::update();
	EventViewer::update();
	SequenceHelpers::update();
}