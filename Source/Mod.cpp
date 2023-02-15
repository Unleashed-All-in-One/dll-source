extern "C" __declspec(dllexport) void Init(ModInfo * modInfo)
{
	MessageBoxA(NULL, "", "", 0);
	// Load configuration
	Configuration::load();

	// Set random seed
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	//---------------System---------------
	ArchiveTreePatcher::applyPatches();
	EnemyTrigger::applyPatches();
	LetterboxHelper::initialize(1280, 720);
	MiniAudioHelper::initialize(modInfo->CurrentMod->Path);

	//---------------Gameplay---------------
	QSSRestore::applyPatches();

	//---------------UI---------------
	Title::applyPatches();
	TitleWorldMap::applyPatches();
	TitleWorldMapPause::applyPatches();
	// NOTE: Because we're essentially remaking the title experience to be the exact same, we might want to make the options change in the
	// same way unleashed does it (via an small options sub-tab-thing and remove TitleOption entirely.
	// Right now, exiting from Options will cause a crash due to the saving/loading in the WorldMap.
	TitleOption::applyPatches();
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
}