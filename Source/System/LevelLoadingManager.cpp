
const char* LevelLoadingManager::NextLevelLoad;
bool LevelLoadingManager::ActiveReplacement;
bool LevelLoadingManager::ReplacingNext;
const char* lastStageID = "";
const char* lastStageIDGate = "";
bool LevelLoadingManager::WhiteWorldEnabled = false;
int LevelLoadingManager::LastSavedQueueIndex = -1;
uint32_t nextPlayerType = 1;

bool LevelLoadingManager::InStory;
bool inStoryBefore;
//capitalWindowOpen
//lastValidFlagSelected
//stageSelectedWindow
//flag
//camInitialized
//lastStageID
DWORD* storySequence;
bool eventTransitionFinished;


const char* LevelLoadingManager::getStageToLoad()
{
	const char* stageToLoad = "ghz200";

	int latestFlag = TitleWorldMap::LastValidFlagSelected;
	int stageSelected = TitleWorldMap::StageSelectedWindow;

	if (TitleWorldMap::CapitalWindowOpen)
		stageToLoad = Configuration::worldData.data[latestFlag].data[Configuration::getCapital(latestFlag)].levelID.c_str();
	else
	{
		if (Configuration::worldData.data[latestFlag].dataNight.size() != 0 && TitleWorldMap::Flag[latestFlag].night)
		{
			stageToLoad = Configuration::worldData.data[latestFlag].dataNight[stageSelected].levelID.c_str();
			SequenceHelpers::setPlayerType(1);
		}
		else
		{
			stageToLoad = Configuration::worldData.data[latestFlag].data[stageSelected].levelID.c_str();
			SequenceHelpers::setPlayerType(0);
		}
	}
	return stageToLoad;
}
void ExecuteSequenceData(QueueData data)
{
	switch (data.type)
	{
	case 0:
	{
		//Play stage (day)
		SequenceHelpers::loadStage(data.dataName.c_str());
		SequenceHelpers::setPlayerType(PlayerType::GENERIC_SONIC);
		LevelLoadingManager::WhiteWorldEnabled = 0;
		break;
	}
	case 1:
	{
		//Play stage (night)
		SequenceHelpers::loadStage(data.dataName.c_str());
		SequenceHelpers::setPlayerType(PlayerType::CLASSIC_SONIC);
		LevelLoadingManager::WhiteWorldEnabled = 0;
		break;
	}
	case 2:
	{
		//Go to capital (day)
		SequenceHelpers::loadStage(data.dataName.c_str());
		SequenceHelpers::setPlayerType(PlayerType::GENERIC_SONIC);
		LevelLoadingManager::WhiteWorldEnabled = 1;
		break;
	}
	case 3:
	{
		//Go to capital (day)
		SequenceHelpers::loadStage(data.dataName.c_str());
		SequenceHelpers::setPlayerType(PlayerType::CLASSIC_SONIC);
		LevelLoadingManager::WhiteWorldEnabled = 1;
		break;
	}
	case 4:
	{
		//Play event (pre-rendered)
		SequenceHelpers::playEvent(data.dataName.c_str(), ModuleFlow::Event);
		break;
	}
	case 5:
	{
		//Play event (realtime)
		SequenceHelpers::playEvent(data.dataName.c_str(), ModuleFlow::StageEvent);
		break;
	}
	}
}
void SetCorrectStageFromFlag()
{
	TitleWorldMap::CamInitialized = false;
	if (!LevelLoadingManager::InStory)
	{
		if (!TitleWorldMap::LoadingReplacementEnabled)
		{
			if (Title::inInstall)
			{
				uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
				char** h = (char**)stageTerrainAddress;
				const char* terr = *h;

				if (terr != lastStageID && lastStageID[0] != '\0')
					strcpy(*(char**)stageTerrainAddress, lastStageID);
			}
			return;
		}
	}
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	const char* stageToLoad = "ghz200";
	if (!LevelLoadingManager::InStory)
	{
		if (Configuration::worldData.data.size() < TitleWorldMap::LastValidFlagSelected)
		{
			//if only cpp had the same ${} system as c#
			std::string message = "This country has an invalid configuration. Loading " + std::string(stageToLoad) + " instead.";
			MessageBoxA(NULL, message.c_str(), "Unleashed Title Screen", 0);
			printf("\n[WorldMap] Missing config for FlagID %d", TitleWorldMap::LastValidFlagSelected);
		}
		else
		{
			stageToLoad = LevelLoadingManager::getStageToLoad();
		}
		TitleWorldMap::Active = false;
		if (LevelLoadingManager::NextLevelLoad != nullptr)
		{
			stageToLoad = LevelLoadingManager::NextLevelLoad;
			LevelLoadingManager::NextLevelLoad = nullptr;
		}
		strcpy(*(char**)stageTerrainAddress, stageToLoad);
	}
	else
	{
		if (LevelLoadingManager::LastSavedQueueIndex == -1)
		{
			printf("[LevelLoadingManager] No saves have been found.");
			LevelLoadingManager::LastSavedQueueIndex = 0;
		}
		if (inStoryBefore == LevelLoadingManager::InStory)
			LevelLoadingManager::LastSavedQueueIndex++;
		ExecuteSequenceData(Configuration::queueData.data[LevelLoadingManager::LastSavedQueueIndex]);
	}

	DebugDrawText::log((std::string("Stage Loading: ") + std::string(stageToLoad)).c_str());
	inStoryBefore = LevelLoadingManager::InStory;
}
void __declspec(naked) SetCorrectTerrainForMission_ASM()
{
	static uint32_t sub_662010 = 0x662010;
	static uint32_t returnAddress = 0xD56CCF;
	__asm
	{
		call[sub_662010]
		push    esi
		call    SetCorrectStageFromFlag
		pop     esi
		jmp[returnAddress]
	}
}

void __declspec(naked) InterceptGameplayFlowLoading()
{
	static uint32_t normal = 0x00D0E166;
	static uint32_t loc_D0E170 = 0xD0E170;
	__asm
	{
		//Original function
		mov     esi, eax
		xor eax, eax
		cmp TitleWorldMap::LoadingReplacementEnabled, 1
		jne FunctionFinish


		cmp TitleWorldMap::ForceLoadToFlowTitle, 1
		je ChangeToTitle

		cmp ecx, 5
		//Change module only if its white world
		je ChangeToAct
		jmp[loc_D0E170]

		ChangeToAct :
		cmp LevelLoadingManager::WhiteWorldEnabled, 1
			je FunctionFinish
			mov ecx, 6
			jmp[loc_D0E170]

			ChangeToTitle :
			cmp ecx, 5
			jne FunctionFinish
			mov ecx, 3
			jmp[loc_D0E170]

			FunctionFinish :
			jmp[loc_D0E170]
	}
}
void LevelLoadingManager::update()
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	const char* terr = *h;
	DebugDrawText::log(terr, 0);
	lastStageID = terr;
	/*if (LevelLoadingManager::ActiveReplacement && LevelLoadingManager::NextLevelLoad != nullptr)
	{
		if (lastStageIDGate == nullptr)
			lastStageIDGate = terr;
		if (terr != LevelLoadingManager::NextLevelLoad)
		{
			strcpy(*(char**)stageTerrainAddress, LevelLoadingManager::NextLevelLoad);
		}
	}	*/
}
HOOK(int32_t*, __fastcall, HudLoading_CHudLoadingCStateOutroBegin, 0x1093410, hh::fnd::CStateMachineBase::CStateBase* This)
{
	LevelLoadingManager::ActiveReplacement = false;
	LevelLoadingManager::NextLevelLoad = nullptr;
	return originalHudLoading_CHudLoadingCStateOutroBegin(This);
}

HOOK(void, __fastcall, LoadPamSettings, 0x00D0A850, DWORD* This)
{
	static boost::shared_ptr<hh::db::CRawData> rawData;
	auto database = hh::db::CDatabase::CreateDatabase();
	auto& databaseLoader = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spDatabaseLoader;
	rawData = database->GetRawData("pamSetting.lua");
	if (!rawData)
	{
		MessageBox(nullptr, TEXT("There is no pamSetting file attached to this capital.\nThe game will crash if you touch a StageGate object."), TEXT("WARNING"), MB_ICONERROR);
	}
	originalLoadPamSettings(This);

}
HOOK(DWORD*, __fastcall, ConstructStorySequence, 0xD76930)
{
	storySequence = originalConstructStorySequence();
	return storySequence;
}
void __declspec(naked) SkipEvent()
{
	static uint32_t pAddr = 0x00B21D42;
	static uint32_t pAddrNothing = 0x00B21CF3;
	__asm
	{
		cmp eventTransitionFinished, 1
		jne DoNothing
		jmp[pAddr]

		DoNothing:
		jmp[pAddrNothing]
	}
}
void LevelLoadingManager::initialize()
{
	WRITE_JUMP(0xD56CCA, SetCorrectTerrainForMission_ASM);
	WRITE_JUMP(0x00D0E164, InterceptGameplayFlowLoading);

	//Blocks Gate UI options and switch
	WRITE_JUMP(0x01080F02, 0x01080FB7);
	////btn skip
	//WRITE_JUMP(0x00B21D38, SkipEvent);
	////dont spawn button ui
	//WRITE_JUMP(0x00B21CF5, 0x00B21CF3);
	INSTALL_HOOK(HudLoading_CHudLoadingCStateOutroBegin);
	INSTALL_HOOK(LoadPamSettings);
	INSTALL_HOOK(ConstructStorySequence);
}