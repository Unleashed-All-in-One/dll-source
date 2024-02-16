extern "C" __declspec(dllexport) void PreInit(ModInfo_t * modInfo)
{
	//Make Gens support 4gb+
	modInfo->API->SendMessageToLoader(ML_MSG_REQ_LARGE_ADDRESS_AWARE, nullptr);
}

extern "C" __declspec(dllexport) void Init(ModInfo_t * modInfo)
{
	MessageBox(nullptr, TEXT("Attach Debugger and press OK."), TEXT("Unleashed Conversion"), MB_ICONINFORMATION);
	// Load configuration
	Configuration::load(modInfo->CurrentMod->Path);

	// Set random seed
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	//---------------System---------------
	LevelLoadingManager::initialize();
	ArchiveTreePatcher::applyPatches();
	AnimationSetPatcher::applyPatches();
	EnemyTrigger::applyPatches();
	LetterboxHelper::initialize(1280, 720);
	MiniAudioHelper::initialize(modInfo->CurrentMod->Path);
	DiscordStatus::initialize();
	SequenceHelpers::applyPatches();

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

	//---------------UI---------------
	Title::applyPatches();
	TitleWorldMap::applyPatches();
	TitleWorldMapPause::applyPatches();
	EventViewer::applyPatches();
	// NOTE: Because we're essentially remaking the title experience to be the exact same, we might want to make the options change in the
	// same way unleashed does it (via an small options sub-tab-thing and remove TitleOption entirely.
	// Right now, exiting from Options will cause a crash due to the saving/loading in the WorldMap.
	TitleOption::applyPatches();

	TestingCode::applyPatches();
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
	CSDCommon::update();
	//DiscordStatus::update();
	LevelLoadingManager::update();
	EventViewer::update();
	SequenceHelpers::update();
}