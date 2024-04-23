
#define HINTSFILE_NAME_FORMAT "{0}\\disk\\Hints\\hints_{1}.json"
HintDataList* Hintring::hintData;
HelpCaptionUIContainer* Hintring::m_HintUI;
HOOK(void*, __fastcall, InitializePlayer, 0x00D96110, void* This)
{
    uint32_t stageIDAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
    char** stageName = (char**)stageIDAddress;
	std::string path = std::format(HINTSFILE_NAME_FORMAT,  Configuration::modPath,std::string(*stageName)).c_str();
    std::ifstream jsonFile;
	try {
		jsonFile.open(path);
	}
	catch (std::ios_base::failure& e) {
		std::cerr << e.what() << '\n';
	}
	HintDataList* data = new HintDataList();

	if (!jsonFile)	
		return originalInitializePlayer(This);

	Json::Value root;
	jsonFile >> root;
    const Json::Value hintDataList = root["HintDataList"];
	if (!hintDataList.isNull()) 
	{
		const Json::Value hintData = hintDataList["HintData"];
		if (!hintData.isNull()) 
		{
			for (const auto& hint : hintData) 
			{
				data->hintData.push_back(new HintData());
				int latestData = data->hintData.size() - 1;
				const Json::Value hintScreen = hint["HintScreen"];
				if (!hintScreen.isNull()) 
				{
					for (const auto& screen : hintScreen) 
					{
						data->hintData[latestData]->hintScreens.push_back(new HintScreen());
						int latestScreen = data->hintData[latestData]->hintScreens.size() - 1;
						data->hintData[latestData]->hintScreens[latestScreen]->emotionIndex = screen["EmotionIndex"].asInt();
						data->hintData[latestData]->hintScreens[latestScreen]->characterIndex = screen["CharacterIndex"].asInt();
						if(!screen["ImageIndex"].isNull())
							data->hintData[latestData]->hintScreens[latestScreen]->imageIndex = screen["ImageIndex"].asInt();
						else
							data->hintData[latestData]->hintScreens[latestScreen]->imageIndex = -1;


						const Json::Value hintStrings = screen["HintString"];
						if (!hintStrings.isNull()) 
						{
							for (const auto& hintString : hintStrings) 
							{
								//todo: match only if locale is the same as ingame
								int locale = hintString["locale"].asInt();
								if(locale == (int)Common::GetUILanguageType())
									data->hintData[latestData]->hintScreens[latestScreen]->text = hintString["string"].asString();
							}
						}
					}
				}
			}
		}
	}
	Hintring::hintData = data;
	return originalInitializePlayer(This);
}
BB_SET_OBJECT_MAKE_HOOK(Hintring);
void Hintring::registerObject()
{
	INSTALL_HOOK(InitializePlayer);
    BB_INSTALL_SET_OBJECT_MAKE_HOOK(Hintring)
}