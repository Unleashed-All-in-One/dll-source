#include "System/DiscordStatus.h"
#include "UI/EventViewer.h"
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
	SUC::Project::RegisterGlobalHooks();
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