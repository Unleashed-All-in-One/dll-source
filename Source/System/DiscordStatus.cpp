discord::Activity currentActivity = {};
discord::Core* core{};

bool m_isEvent = false;
void Log(discord::LogLevel level, const char* message) {
	printf("[DiscordStatus] Severity: %d | %s\n", level, message);
}
HOOK(void, __fastcall, HudLoading_CHudLoadingCStateIntroBegin, 0x10938F0, hh::fnd::CStateMachineBase::CStateBase* This)
{
	char const* eventName = *(char**)Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x2C });
	m_isEvent = !std::string(eventName).empty();

	if (!m_isEvent) {
		DiscordStatus::ChangeInformationFromStageInfo();
	}

	originalHudLoading_CHudLoadingCStateIntroBegin(This);
}

void DiscordStatus::Initialize()
{
	INSTALL_HOOK(HudLoading_CHudLoadingCStateIntroBegin);
	//i hate discord's game sdk -Nextin
	discord::Result coreResult;

	coreResult = discord::Core::Create(CLIENT_ID, DiscordCreateFlags_Default, &core);
	coreResult == discord::Result::Ok && core
		? printf("[DiscordStatus] Core Initialized Successfully!\n")
		: printf("[DiscordStatus] Core Initialization failed!\n");
	currentActivity.GetTimestamps().SetStart(time(0));
}
void DiscordStatus::Update()
{
	core->RunCallbacks();
}

void DiscordStatus::UpdateActivityInformation()
{
	currentActivity.SetType(discord::ActivityType::Playing);
	currentActivity.SetDetails(TopText.c_str());
	currentActivity.SetState(BottomText.c_str());

	currentActivity.GetAssets().SetLargeImage(Thumbnail.c_str());
	//currentActivity.largeImageText = Country;

  /*  currentActivity.GetParty(). = partySize;
	currentActivity.partyMax = partyMax;*/

	currentActivity.GetAssets().SetSmallImage(ThumbnailSmall.c_str());
	currentActivity.GetAssets().SetSmallText(StageTypeText.c_str());
	core->ActivityManager().UpdateActivity(currentActivity, [](discord::Result result) {
		std::cout << "Activity callback begin\n";

	result == discord::Result::Ok
		? std::cout << "Activity Callback Ok\n"
		: std::cout << "Activity Callback Not Ok\n";
		});

}