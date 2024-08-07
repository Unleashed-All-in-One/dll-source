#pragma once
namespace SUC::System::Discord
{
	//Please overhaul this with the newer stage system at some point (07/08/2024)
	class DiscordStatus
	{
	public:
		struct EDiscordPresenceData
		{
			enum class EStageType
			{
				Day,
				Night
			};
			std::string id;
			std::string stage;
			int act;
			int maxAct;
			std::string action;
			EStageType stageType;
			std::string thumbnail = "icon";
		};
		static void Initialize();
		static void ChangeInformationFromStageInfo(bool timestamp);
		static void ChangeInformation(EDiscordPresenceData data);
		static void UpdateActivityInformation();
		static void Update();
		static std::vector<EDiscordPresenceData> presenceData;
		static inline std::string s_StageTypeText = "Day";
		static inline std::string s_Thumbnail = "icon";
		static inline std::string s_ThumbnailSmall = "day";
		static inline bool s_UseTimestamp = false;

		static inline std::string s_ActivityTopText;
		static inline std::string s_ActivityBottomText;
	};
}