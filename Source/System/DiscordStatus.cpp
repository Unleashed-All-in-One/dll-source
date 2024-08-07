namespace SUC::System::Discord
{
	std::vector<DiscordStatus::EDiscordPresenceData> DiscordStatus::presenceData;
	discord::Activity currentActivity = {};
	discord::Core* core{};
	bool m_isEvent = false;


	HOOK(void, __fastcall, HudLoading_CHudLoadingCStateIntroBegin, 0x10938F0, hh::fnd::CStateMachineBase::CStateBase* This)
	{
		char const* eventName = *(char**)Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x2C });
		m_isEvent = !std::string(eventName).empty();

		if (!m_isEvent)
		{
			DiscordStatus::ChangeInformationFromStageInfo(true);
		}

		originalHudLoading_CHudLoadingCStateIntroBegin(This);
	}
	void Log(discord::LogLevel level, const char* message)
	{
		printf("[DiscordStatus] Severity: %d | %s\n", level, message);
	}	
	void DiscordStatus::ChangeInformationFromStageInfo(bool timestamp)
	{
		uint8_t stageID = Common::GetCurrentStageID() & 0xFF;
		static char* stageName = *(char**)(4 * stageID + 0x1E66B48);
		stageName[5] = '0' + ((Common::GetCurrentStageID() & 0xFF00) >> 8);
		s_UseTimestamp = timestamp;
		for (EDiscordPresenceData data : presenceData)
		{
			if (std::string(stageName) == data.id)
			{
				ChangeInformation(data);
			}
		}
	}
	void DiscordStatus::ChangeInformation(EDiscordPresenceData data)
	{
		switch (data.stageType)
		{
		case EDiscordPresenceData::EStageType::Day:
			s_StageTypeText = "Day";
			s_ThumbnailSmall = "day";
			break;
		case EDiscordPresenceData::EStageType::Night:
			s_StageTypeText = "Night";
			s_ThumbnailSmall = "night";
			break;
		}

		s_ActivityTopText = data.action;
		char buffer[64];
		snprintf(buffer, 64, "%s %s, Act %d", data.stage.c_str(), s_StageTypeText.c_str(), data.act);
		s_ActivityBottomText = buffer;




		UpdateActivityInformation();
	}
	void DiscordStatus::Update()
	{
		core->RunCallbacks();
	}
	void DiscordStatus::UpdateActivityInformation()
	{
		//if (useTimestamp)
		//{
		//	currentActivity.GetTimestamps().SetStart(time(0));
		//}
		//currentActivity.SetType(discord::ActivityType::Playing);
		//currentActivity.SetDetails(TopText.c_str());
		//currentActivity.SetState(BottomText.c_str());
		//
		//currentActivity.GetAssets().SetLargeImage(Thumbnail.c_str());
		//currentActivity.GetAssets().SetSmallImage(ThumbnailSmall.c_str());
		//currentActivity.GetAssets().SetSmallText(StageTypeText.c_str());
		//core->ActivityManager().UpdateActivity(currentActivity, [](discord::Result result) {
		//
		//result == discord::Result::Ok
		//	? std::cout << "[DiscordStatus] Activity Update Success!\n"
		//	: std::cout << "[DiscordStatus] Activity Update Failed!\n";
		//	});

	}
	void DiscordStatus::Initialize()
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
}