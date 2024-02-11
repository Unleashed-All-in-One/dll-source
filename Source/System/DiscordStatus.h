#pragma once
#define CLIENT_ID 1092192076260704345

enum class StageType {
	Day,
	Night
};

struct DiscordPresenceData {
	std::string id;
	std::string stage; 
	int act; 
	int maxAct; 
	std::string action; 
	StageType stageType; 
	std::string thumbnail = "icon";
};

static std::vector<DiscordPresenceData> presenceData = {
	{
		"ghz100", 
		"Windmill Isle",
		1,
		6,
		"Playing a stage",
		StageType::Day
	},
	{
		"ghz101",
		"Windmill Isle",
		2,
		6,
		"Playing a stage",
		StageType::Day
	},
	{
		"ghz102",
		"Windmill Isle",
		3,
		6,
		"Playing a stage",
		StageType::Day
	},
	{
		"ghz103",
		"Windmill Isle",
		4,
		6,
		"Playing a stage",
		StageType::Day
	},
	{
		"ghz104",
		"Windmill Isle [1-2]",
		5,
		6,
		"Playing a stage",
		StageType::Day
	},
	{
		"ghz105",
		"Windmill Isle [2-2]",
		6,
		6,
		"Playing a stage",
		StageType::Day
	},
	{
		"ghz200",
		"Windmill Isle",
		1,
		6,
		"Playing a stage",
		StageType::Night
	}
};

class DiscordStatus {
public:
	static void initialize();
	static void ChangeInformationFromStageInfo(bool timestamp) {
		uint8_t stageID = Common::GetCurrentStageID() & 0xFF;
		static char* stageName = *(char**)(4 * stageID + 0x1E66B48);
		stageName[5] = '0' + ((Common::GetCurrentStageID() & 0xFF00) >> 8);
		useTimestamp = timestamp;
		for (DiscordPresenceData data : presenceData) {
			if (std::string(stageName) == data.id) {
				ChangeInformation(data);
			}
		}
	}
	static void ChangeInformation(DiscordPresenceData data) {
		switch (data.stageType)
		{
		case StageType::Day:
			StageTypeText = "Day";
			ThumbnailSmall = "day";
			break;
		case StageType::Night:
			StageTypeText = "Night";
			ThumbnailSmall = "night";
			break;
		}

		TopText = data.action;
		char buffer[64];
		snprintf(buffer,64, "%s %s, Act %d", data.stage.c_str(), StageTypeText.c_str(), data.act);
		BottomText = buffer;


		

		UpdateActivityInformation();
	}
	static void UpdateActivityInformation();
	static void update();

	static inline std::string StageTypeText = "Day";
	static inline std::string Thumbnail = "icon";
	static inline std::string ThumbnailSmall = "day";
	static inline bool useTimestamp = false;
	static inline int partySize = 1;
	static inline int partyMax = 1;

	static inline std::string TopText;
	static inline std::string BottomText;
};