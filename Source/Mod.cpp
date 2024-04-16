

extern "C" __declspec(dllexport) void PreInit(ModInfo_t * modInfo)
{
	// Load configuration
	Configuration::load(modInfo->CurrentMod->Path);
	//Make Gens support 4gb+
	if(Configuration::use4gbMode)
	modInfo->API->SendMessageToLoader(ML_MSG_REQ_LARGE_ADDRESS_AWARE, nullptr);
}

std::string getDirectoryPath(const std::string& path)
{
	const size_t pos = path.find_last_of("\\/");
	return path.substr(0, pos != std::string::npos ? pos : 0);
}
//00E62450
//int __thiscall Sonic::Player::CPlayerSpeedContext::UpdateFunc(CSonicContext *this, float *deltaTime)

extern "C" __declspec(dllexport) void Init(ModInfo_t * modInfo)
{
	MessageBox(nullptr, TEXT("Attach Debugger and press OK."), TEXT("Unleashed Conversion"), MB_ICONINFORMATION);

	// Set random seed
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	std::string path =  std::string(modInfo->CurrentMod->Path);
	path = path.substr(0, path.find_last_of("/\\"));

	// Functionality patches that don't necessarily need their own file
	Patches::applyPatches();
	//---------------System---------------
	LevelLoadingManager::initialize();
	ArchiveTreePatcher::applyPatches();
	AnimationSetPatcher::applyPatches();
	EnemyTrigger::applyPatches();
	LetterboxHelper::initialize(1280, 720);
	MiniAudioHelper::initialize(path.c_str());
	DiscordStatus::initialize();
	SequenceHelpers::applyPatches();
	ImguiInitializer::initialize();
	Context::setModDirectoryPath(getDirectoryPath(modInfo->CurrentMod->Path));
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
	ImguiInitializer::update();
	CSDCommon::update();
	//DiscordStatus::update();
	LevelLoadingManager::update();
	EventViewer::update();
	SequenceHelpers::update();
}