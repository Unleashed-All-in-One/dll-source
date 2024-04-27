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
		DiscordStatus::ChangeInformationFromStageInfo(true);
	}

	originalHudLoading_CHudLoadingCStateIntroBegin(This);
}

void DiscordStatus::initialize()
{
	INSTALL_HOOK(HudLoading_CHudLoadingCStateIntroBegin);
	//i hate discord's game sdk -Nextin
	discord::Result coreResult;

	coreResult = discord::Core::Create(CLIENT_ID, DiscordCreateFlags_NoRequireDiscord, &core);
	coreResult == discord::Result::Ok && core
		? printf("[DiscordStatus] Core Initialized Successfully!\n")
		: printf("[DiscordStatus] Core Initialization failed!\n");
	core->SetLogHook(discord::LogLevel::Debug, Log);
	core->SetLogHook(discord::LogLevel::Info, Log);
	core->SetLogHook(discord::LogLevel::Warn, Log);
	core->SetLogHook(discord::LogLevel::Error, Log);
}
void DiscordStatus::update()
{
	core->RunCallbacks();
}

void DiscordStatus::UpdateActivityInformation()
{
	if (useTimestamp)
	{
		currentActivity.GetTimestamps().SetStart(time(0));
	}
	currentActivity.SetType(discord::ActivityType::Playing);
	currentActivity.SetDetails(TopText.c_str());
	currentActivity.SetState(BottomText.c_str());

	currentActivity.GetAssets().SetLargeImage(Thumbnail.c_str());
	currentActivity.GetAssets().SetSmallImage(ThumbnailSmall.c_str());
	currentActivity.GetAssets().SetSmallText(StageTypeText.c_str());
	core->ActivityManager().UpdateActivity(currentActivity, [](discord::Result result) {

	result == discord::Result::Ok
		? std::cout << "[DiscordStatus] Activity Update Success!\n"
		: std::cout << "[DiscordStatus] Activity Update Failed!\n";
		});

}