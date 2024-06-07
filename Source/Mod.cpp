extern "C" __declspec(dllexport) void PreInit(ModInfo_t * modInfo)
{
	// Load configuration
	Project::load(modInfo->CurrentMod->Path);
	//Make Gens support 4gb+
	if(Project::use4gbMode)
	modInfo->API->SendMessageToLoader(ML_MSG_REQ_LARGE_ADDRESS_AWARE, nullptr);
}

std::string getDirectoryPath(const std::string& path)
{
	const size_t pos = path.find_last_of("\\/");
	return path.substr(0, pos != std::string::npos ? pos : 0);
}
void __declspec(naked) ASMTest()
{
	static uint32_t retn = 0x011031E4;
	__asm
	{
		mov[esp + 34h + -1Ch], 0
		jmp[retn]
	}
}

//void __thiscall sub_B1F080(_DWORD *this, int *a2, Hedgehog::Base::SSymbolNode *a3, int a4)
//HOOK(void, __fastcall, moviedisp, 0xB1F080, void* This, void* Edx, void* a2, Hedgehog::Base::SSymbolNode* a3, int a4)
//{
//	return;
//}
// uint32_t __thiscall sub_EB2A80(_DWORD *this, int a2, int a3, int a4)
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
	//DiscordStatus::initialize();
	SequenceHelpers::applyPatches();
	ImguiInitializer::initialize();
	Context::setModDirectoryPath(getDirectoryPath(modInfo->CurrentMod->Path));
	SaveManager::applyPatches();
	LuaManager::initialize();
	SetEditorTest::applyPatches();

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
	WRITE_JUMP(0x011031DC, ASMTest);
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
const char* a3 = "Stage";
//char __thiscall Sonic::CSetObjectManager::ProcessMessage(char *this, int a1, int a2
extern "C" void __declspec(dllexport) OnFrame()
{
	//---------------System---------------
	ImguiInitializer::update();
	CSDCommon::update();
	//DiscordStatus::update();
	StageManager::update();
	EventViewer::update();
}