
//Set objects
#include "SetObject/Hintring.h"
#include "SetObject/DebugObject.h"
#include "SetObject/ETFTimeSwitch.h"
#include "SetObject/Paraloop.h"
#include "SetObject/SpeedDownCollision.h"
#include "SetObject/ETFStageGate.h"
#include "SetObject/MoonMedal.h"
#include "SetObject/EvilEnemyReckless.h"
#include "SetObject/EvilLiftDoor.h"
#include "SetObject/Holoska/Icicle.h"
#include "SetObject/Holoska/StompingSwitch.h"
#include "SetObject/Mykonos/Pelican.h"
#include "SetObject/Mykonos/IrremovableMobMykonos.h"
#include "SetObject/CObjCamera2D.h"

//Rest
#include "SetObject/Werehog/CObjHangOn.h"
#include "Testing/TestingCode.h"
#include "UI/TitleWorldMap.h"

extern "C" __declspec(dllexport) void PreInit(ModInfo_t * modInfo)
{
	// Load configuration
	SUC::Project::Load(modInfo->CurrentMod->Path);
	//Make Gens support 4gb+
	if(SUC::Project::s_LargeAddressAware)
		modInfo->API->SendMessageToLoader(ML_MSG_REQ_LARGE_ADDRESS_AWARE, nullptr);
}



//DWORD *__thiscall ProcMsgRequestChangeModule(void *this, int a2)
HOOK(DWORD*, __fastcall, ProcMsg, 0xD0B2E0, void* This, void* Edx, Sonic::Message::MsgRequestChangeModule& msg)
{
	auto msg2 = msg;
	return originalProcMsg(This, Edx, msg);
}
extern "C" __declspec(dllexport) void Init(ModInfo_t * modInfo)
{
#if _DEBUG
	MessageBox(nullptr, TEXT("Attach Debugger and press OK."), TEXT("Unleashed Conversion"), MB_ICONINFORMATION);
#endif

	// Set random seed
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	std::string path =  std::string(modInfo->CurrentMod->Path);
	path = path.substr(0, path.find_last_of("/\\"));
	// Functionality patches that don't necessarily need their own file
	Patches::applyPatches();
	//---------------System---------------
	StageManager::initialize();
	ArchiveTreePatcher::applyPatches();
	AnimationSetPatcher::applyPatches();
	EnemyTrigger::applyPatches();
	LetterboxHelper::initialize(1280, 720);
	MiniAudioHelper::initialize(path.c_str());
	DiscordStatus::initialize();
	SequenceHelpers::applyPatches();
	ImguiInitializer::initialize();
	Context::setModDirectoryPath(SUC::Project::GetDirectoryPath(modInfo->CurrentMod->Path));
	SaveManager::applyPatches();
	LuaManager::initialize();

	//---------------Gameplay---------------
	QSSRestore::applyPatches();
	Sweepkick::applyPatches();
	Ramp::applyPatches();
	ShortJump::applyPatches();
	Drift::applyPatches();
	Pole::applyPatches();

	//-----------Functionality (accuracy)-----------
	PauseBgm::applyPatches();
	FallCam::applyPatches();
	RingEnergy::applyPatches();
	EXPCollect::applyPatches();

	//-----------Register Set Objects-----------
	WerehogPole::registerObject();
	ETFTimeSwitch::registerObject();
	Hintring::registerObject();
	Paraloop::registerObject();
	SpeedDownCollision::registerObject();
	ETFStageGate::registerObject();
	MoonMedal::registerObject();
	EvilEnemyReckless::registerObject();
	EvilLiftDoor::registerObject();
	Icicle::registerObject();
	Pelican::registerObject();
	IrremovableMobMykonos::registerObject();
	StompingSwitch::registerObject();
	CObjHangOn::registerObject();
	BB_INSTALL_SET_OBJECT_MAKE_HOOK(CObjCamera2D);

	//---------------UI---------------
	Title::applyPatches();
	TitleWorldMap::applyPatches();
	TitleWorldMapPause::applyPatches();
	EventViewer::applyPatches();
	SubtitleUI::applyPatches();
	SubtitleUI::m_captionData.init();
	HudButtonGuide::applyPatches();
	// NOTE: Because we're essentially remaking the title experience to be the exact same, we might want to make the options change in the
	// same way unleashed does it (via an small options sub-tab-thing and remove TitleOption entirely.
	// Right now, exiting from Options will cause a crash due to the saving/loading in the WorldMap.
	TitleOption::applyPatches();
	TestingCode::applyPatches();
	INSTALL_HOOK(ProcMsg);
	
	WRITE_MEMORY(0x1AD99D0, char*, "shader_debug.ar");
	WRITE_MEMORY(0x1AD99D4, char*, "shader_debug_add.ar");
	WRITE_MEMORY(0x1AD99E8, char*, "shader_debug.arl");
	WRITE_MEMORY(0x1AD99EC, char*, "shader_debug_add.arl");
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
bool draw;


//BB_FUNCTION_PTR(void, __cdecl, InitCGameplayFlowStageAct, 0x00D0DEB0, boost::shared_ptr<CGameplayFlowAct> a1, Sonic::Message::SRequestChangeModuleInfo* a2);

//char __thiscall Sonic::CSetObjectManager::ProcessMessage(char *this, int a1, int a2
extern "C" void __declspec(dllexport) OnFrame()
{
	if(GetAsyncKeyState('K'))
	{
		auto main = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spSequenceMain;	
		main.reset();
	}	
	*(bool*)0x1E75518 = 1;
	//WRITE_MEMORY(0x1E5E438, int, 1);
	//---------------System---------------
	ImguiInitializer::update();
	CSDCommon::update();
	DiscordStatus::update();
	StageManager::update();
	EventViewer::update();
	SequenceHelpers::update();
}