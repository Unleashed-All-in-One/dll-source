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
std::string replace(const std::string& original, const std::string& from, const std::string& to) {
	if (from.empty()) {
		return original; // Avoid infinite loop when `from` is empty.
	}

	std::string result = original;
	size_t start_pos = 0;

	// Iterate through the string and replace all occurrences of `from` with `to`.
	while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
		result.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Move past the last replaced substring.
	}

	return result;
}
std::string splitStr2(std::string const& s, char separator)
{
	std::string::size_type pos = s.find(separator);
	if (pos != std::string::npos)
	{
		return s.substr(0, pos);
	}
	else
	{
		return s;
	}
}
const char* GetNewName(const char* existing)
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	std::string stageIDName = *(char**)stageTerrainAddress;
	std::string name = std::string(existing);
	stageIDName = splitStr2(stageIDName, '_');

	name = replace(name, "ghz", stageIDName);
	name = replace(name, "cpz", stageIDName);
	name = replace(name, "ssz", stageIDName);
	name = replace(name, "sph", stageIDName);
	name = replace(name, "cte", stageIDName);
	name = replace(name, "ssh", stageIDName);
	name = replace(name, "csc", stageIDName);
	name = replace(name, "euc", stageIDName);
	name = replace(name, "pla", stageIDName);
	DebugDrawText::logRegardless(name.c_str(), 10);
	return _strdup(name.c_str());
}

//void __thiscall sub_B1F080(DWORD *this, int *a2, Hedgehog::Base::SSymbolNode *a3, int a4)
//HOOK(void, __fastcall, moviedisp, 0xB1F080, void* This, void* Edx, void* a2, Hedgehog::Base::SSymbolNode* a3, int a4)
//{
//	return;
//}
// uint32_t __thiscall sub_EB2A80(DWORD *this, int a2, int a3, int a4)

void __declspec(naked) Floor()
{
	static uint32_t pAddr = 0x01008C9D;
	static uint32_t Csharedstring = 0x006621A0;
	__asm{
		push [edi]
		call GetNewName
		mov [edi], eax
		lea     ecx, [esp + 544h -530h]; Load Effective Address
		call    [Csharedstring]
		jmp[pAddr]
	}
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
	//INSTALL_HOOK(sub_1008C00);
	WRITE_JUMP(0x01008C91, Floor);
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