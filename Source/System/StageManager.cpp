
const char* StageManager::NextLevelLoad;
bool StageManager::ActiveReplacement;
bool StageManager::ReplacingNext;
const char* lastStageID = "";
const char* lastStageIDGate = "";
bool StageManager::WhiteWorldEnabled = false;
int StageManager::LastSavedQueueIndex = -1;
uint32_t nextPlayerType = 0;
std::string StageManager::nextStageID;
std::string StageManager::nextEvsID;
const char* temp_nextStageIDCutscene = "ev011";
const char* temp_nextEVSIDCutscene = "ghz100";
bool isMovieCutscene;
bool StageManager::enteredStageFromETF;
std::string etfHubName = "ghz205";
bool StageManager::InStory;
bool inStoryBefore;
bool stageEnded;
bool skipCurrentQueueEvent;
DWORD* storySequence;
bool eventTransitionFinished;

void __declspec(naked) sub_00D40070(int skipped, DWORD* a2, Hedgehog::Base::CSharedString* eventName)
{
	static uint32_t func = 0x00D40070;
	__asm
	{
		push eventName
		push a2
		mov eax, skipped
		call [func]
		retn
	}
}

void StageManager::setGameParameters(std::string stageID, std::string evsID)
{
	nextStageID = stageID;
	nextEvsID = evsID;
	uint32_t* appdocMember = (uint32_t*)Sonic::CApplicationDocument::GetInstance()->m_pMember;
	auto gameParameters = *((DWORD*)appdocMember + 109);
	Hedgehog::Base::CSharedString* evsIDLoc = (Hedgehog::Base::CSharedString*)(*((DWORD*)gameParameters + 32) + 44);
	Hedgehog::Base::CSharedString* stageIDLoc = (Hedgehog::Base::CSharedString*)(*((DWORD*)gameParameters + 32) + 48);
	*evsIDLoc = evsID.c_str();
	*stageIDLoc = stageID.c_str();
}
std::string StageManager::getEventID()
{
	return nextEvsID;
}
void StageManager::setETFInfo(std::string etfHubStageName)
{
	StageManager::enteredStageFromETF = true;
	skipCurrentQueueEvent = false;
	etfHubName = etfHubStageName;
}
void loadCapital(std::string stageName, bool isWerehog)
{
	SequenceHelpers::loadStage(stageName.c_str(), 0, false);
	TitleWorldMap::LoadingReplacementEnabled = true;
	StageManager::WhiteWorldEnabled = true;
	//SequenceHelpers::changeModule(ModuleFlow::PlayableMenu);
	SequenceHelpers::setPlayerType(isWerehog ? PlayerType::CLASSIC_SONIC : PlayerType::GENERIC_SONIC);
	DebugDrawText::log(std::format("[LLM] Loading Capital (Day) \"{}\" as {}", stageName, isWerehog ? "Werehog" : "Modern").c_str(), 5);
}
const char* StageManager::getStageToLoad()
{
	const char* stageToLoad = "ghz200";

	int latestFlag = TitleWorldMap::LastValidFlagSelected;
	int stageSelected = TitleWorldMap::StageSelectedWindow;

	if (TitleWorldMap::CapitalWindowOpen)
		stageToLoad = Project::worldData.data[latestFlag].data[Project::getCapital(latestFlag)].levelID.c_str();
	else
	{
		if (Project::worldData.data[latestFlag].dataNight.size() != 0 && TitleWorldMap::Flag[latestFlag].night)
		{
			stageToLoad = Project::worldData.data[latestFlag].dataNight[stageSelected].levelID.c_str();
			SequenceHelpers::setPlayerType(1);
		}
		else
		{
			stageToLoad = Project::worldData.data[latestFlag].data[stageSelected].levelID.c_str();
			SequenceHelpers::setPlayerType(0);
		}
	}
	return stageToLoad;
}
void StageManager::forcePlayCutscene(std::string in_EventName, std::string in_StageName, bool flag, int playerType)
{
	auto playEventRequest = new LuaParamPlayEvent();
	playEventRequest->entry = new LuaParamPlayEventEntry();
	playEventRequest->entry->eventName = in_EventName.c_str();
	playEventRequest->entry->eventStageType = IntStuffPlayEvent();
	playEventRequest->entry->eventStageType.value = flag ? 1 : 0;
	FUNCTION_PTR(void, __thiscall, PlayEventLuanne, 0x00D72520, DWORD * StorySeq, int a2, LuaParamPlayEvent * a3);
	PlayEventLuanne(storySequence, 0, playEventRequest);

	nextEvsID = in_EventName.c_str();
	nextStageID = in_StageName.c_str();
	isMovieCutscene = true;
	SequenceHelpers::setPlayerType(playerType);
	auto message1 = Sonic::Message::MsgSequenceEvent(0, 7);
	Sonic::Sequence::Main::ProcessMessage(&message1);

	//uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	//char** h = (char**)stageTerrainAddress;
	//const char* stageToLoad = in_StageName.c_str();
	//strcpy(*(char**)stageTerrainAddress, stageToLoad);
}
void StageManager::triggerSequenceEvents(int type, bool dontSetPlayerType)
{
	switch (type)
	{
	case 0:
	{
		//Play stage (day)
		SequenceHelpers::loadStage(StageManager::nextStageID.c_str(), 0, false);
		if(!dontSetPlayerType)
			SequenceHelpers::setPlayerType(PlayerType::GENERIC_SONIC);
		StageManager::WhiteWorldEnabled = 0;
		DebugDrawText::log(std::format("[LLM] Loading Stage (Day) \"{}\" as {}",StageManager::nextStageID, (int)PlayerType::GENERIC_SONIC).c_str(), 5);
		break;
	}
	case 1:
	{
		//Play stage (night)
		SequenceHelpers::loadStage(StageManager::nextStageID.c_str(), 0, false);
		if (!dontSetPlayerType)
		SequenceHelpers::setPlayerType(PlayerType::CLASSIC_SONIC);
		StageManager::WhiteWorldEnabled = 0;
		DebugDrawText::log(std::format("[LLM] Loading Stage (Night) \"{}\" as {}", StageManager::nextStageID, (int)PlayerType::CLASSIC_SONIC).c_str(), 5);
		break;
	}
	case 2:
	{
		//Go to capital (day)
		loadCapital(StageManager::nextStageID, 0);
		if (!dontSetPlayerType)
		SequenceHelpers::setPlayerType(PlayerType::GENERIC_SONIC);
		break;
	}
	case 3:
	{
		//Go to capital (day)
		loadCapital(StageManager::nextStageID, 1);
		if (!dontSetPlayerType)
		SequenceHelpers::setPlayerType(PlayerType::CLASSIC_SONIC);
		break;
	}
	case 4:
	{
		SequenceHelpers::playEvent(StageManager::nextEvsID.c_str(), ModuleFlow::Event);
		if (!dontSetPlayerType)
		SequenceHelpers::setPlayerType(PlayerType::GENERIC_SONIC);
		auto message = Sonic::Message::MsgSequenceEvent(5, 0);
		Sonic::Sequence::Main::ProcessMessage(&message);
		isMovieCutscene = false;
		StageManager::nextStageID = "ghz100";
		break;
	}
	case 5:
	{
		//Play event (realtime)
		
		auto playEventRequest = new LuaParamPlayEvent();
		playEventRequest->entry = new LuaParamPlayEventEntry();
		playEventRequest->entry->eventName = StageManager::nextEvsID.c_str();
		playEventRequest->entry->eventStageType = IntStuffPlayEvent();
		playEventRequest->entry->eventStageType.value = 0;
		FUNCTION_PTR(void, __thiscall, PlayEventLuanne, 0x00D72520, Sonic::Sequence::Story * StorySeq, int a2, LuaParamPlayEvent* a3);
		PlayEventLuanne(SequenceHelpers::storySequenceInstance, 0, playEventRequest);

		isMovieCutscene = true;
		//SequenceHelpers::resetStorySequence();
		auto message1 = Sonic::Message::MsgSequenceEvent(0, 7);
		SequenceHelpers::changeModule(ModuleFlow::Event);
		Sonic::Sequence::Main::ProcessMessage(&message1);
		//SequenceHelpers::setPlayerType(0);
		uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
		char** h = (char**)stageTerrainAddress;
		const char* stageToLoad = StageManager::nextStageID.c_str();
		strcpy(*(char**)stageTerrainAddress, stageToLoad);
		break;
	}
	}
}
void ExecuteSequenceData(std::vector<QueueData> dataList)
{
	int index = StageManager::LastSavedQueueIndex;
	QueueData data = dataList[index];
	//if (data.type == 4 && data.immediate)
	//{
	//	//EventQueue will play videos in order
	//	TriggerSequenceEvents(data);
	//	if(data.playerTypeOverride != -1)
	//		SequenceHelpers::setPlayerType(data.playerTypeOverride);
	//	for (size_t i = index + 1; i < dataList.size(); i++)
	//	{
	//		if (dataList[i].type == 4 && dataList[i].immediate == false)
	//		{
	//			StageManager::LastSavedQueueIndex++;
	//			DebugDrawText::log(std::format("[LLM] Queueing event \"{}\"", dataList[i].dataName).c_str(), 5);
	//			SequenceHelpers::queueEvent(dataList[i].dataName.c_str());
	//		}
	//		else
	//			break;
	//	}
	//}	
	//else
	//{
		//TriggerSequenceEvents(data);
	//}
}

void SetCorrectStageFromPAM(uint32_t stageIDPam)
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	const char* stageToLoad = "ghz200";
	//get current flag based on the pam stage loaded
	int flag = Project::getFlagFromStage(*h);
	stageToLoad = Project::worldData.data[flag].data[stageIDPam].levelID.c_str();
	lastStageID = stageToLoad;
	SequenceHelpers::loadStage(stageToLoad);
	strcpy(*(char**)stageTerrainAddress, stageToLoad);
}
void SetCorrectStageFromFlag()
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	TitleWorldMap::CamInitialized = false;

	//strcpy(*(char**)stageTerrainAddress, StageManager::NextLevelLoad);
	if (!StageManager::InStory)
	{		
		const char* stageToLoad = "ghz200";
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
		if (Project::worldData.data.size() < TitleWorldMap::LastValidFlagSelected)
		{
			//if only cpp had the same ${} system as c#
			std::string message = "This country has an invalid configuration. Loading " + std::string(stageToLoad) + " instead.";
			MessageBoxA(NULL, message.c_str(), "Unleashed Title Screen", 0);
			printf("\n[WorldMap] Missing config for FlagID %d", TitleWorldMap::LastValidFlagSelected);
		}
		else
		{
			stageToLoad = StageManager::getStageToLoad();
		}
		TitleWorldMap::Active = false;
		if (StageManager::NextLevelLoad != nullptr)
		{
			stageToLoad = StageManager::NextLevelLoad;
			StageManager::NextLevelLoad = nullptr;
		}

		strcpy(*(char**)stageTerrainAddress, stageToLoad);
	}
	else
	{
		if (StageManager::enteredStageFromETF)
		{
			strcpy(*(char**)stageTerrainAddress, StageManager::nextStageID.c_str());
			//StageManager::enteredStageFromETF = false;
		}
		if (!skipCurrentQueueEvent)
		{
			LuaManager::onStageLoad();
		}
		else
		{
			StageManager::enteredStageFromETF = false;
			skipCurrentQueueEvent = false;
		}
		//if (StageManager::LastSavedQueueIndex == -1)
		//{
		//	printf("[StageManager] No saves have been found.");
		//	StageManager::LastSavedQueueIndex = 0;
		//}
		//if (!skipCurrentQueueEvent)
		//{
		//	if (inStoryBefore == StageManager::InStory)
		//		StageManager::LastSavedQueueIndex++;
		//	ExecuteSequenceData(Project::queueData.data);
		//}
		//if(Project::queueData.data[StageManager::LastSavedQueueIndex].type != 5)
		//strcpy(*(char**)stageTerrainAddress, Project::queueData.data[StageManager::LastSavedQueueIndex].dataName.c_str());
		//skipCurrentQueueEvent = false;
	}

	DebugDrawText::log((std::string("Stage Loading: ") + std::string(*h)).c_str());
	inStoryBefore = StageManager::InStory;
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
void __declspec(naked) Test(int in)
{
	printf("in");
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
			
		cmp ecx, 6
		je ChangeToWhiteWorld
		cmp ecx, 5
		je ChangeModuleDefault

		//Change module only if its white world
		ChangeModuleDefault:
		je ChangeToAct
		jmp[loc_D0E170]

		ChangeToAct :
		cmp StageManager::WhiteWorldEnabled, 1
			je FunctionFinish
			mov ecx, 6
			jmp[loc_D0E170]


			ChangeToTitle :
			cmp ecx, 5
			jne FunctionFinish
			mov ecx, 3
			jmp[loc_D0E170]

			ChangeToWhiteWorld :
			cmp StageManager::WhiteWorldEnabled, 1
			jne ChangeModuleDefault
			mov ecx, 5
			jmp[loc_D0E170]

			FunctionFinish :
			jmp[loc_D0E170]
	}
}
void StageManager::update()
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	const char* terr = *h;
	DebugDrawText::log(terr, 0);
	lastStageID = terr;
	/*if (StageManager::ActiveReplacement && StageManager::NextLevelLoad != nullptr)
	{
		if (lastStageIDGate == nullptr)
			lastStageIDGate = terr;
		if (terr != StageManager::NextLevelLoad)
		{
			strcpy(*(char**)stageTerrainAddress, StageManager::NextLevelLoad);
		}
	}	*/
}
HOOK(int32_t*, __fastcall, HudLoading_CHudLoadingCStateOutroBegin, 0x1093410, hh::fnd::CStateMachineBase::CStateBase* This)
{
	StageManager::ActiveReplacement = false;
	StageManager::NextLevelLoad = nullptr;
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
void StageManager::setCorrectStage()
{
	SetCorrectStageFromFlag();
}
//int __thiscall CalledOnResultEnd(CTempState *this)
HOOK(int, __fastcall, CalledOnResultEnd, 0x00CFB300, void* This, void* Edx)
{
	if (StageManager::enteredStageFromETF)
	{
		std::string stageBef = StageManager::nextStageID.c_str();
		std::string evsBef = StageManager::nextEvsID.c_str();
		LuaManager::onStageEnd();
		if (StageManager::nextStageID == stageBef && StageManager::nextEvsID == evsBef)
		{
			loadCapital(etfHubName, 0);
			skipCurrentQueueEvent = false;
		}
		else
		{
			StageManager::enteredStageFromETF = false;
		}
	}
	else
	{
		SetCorrectStageFromFlag();
		skipCurrentQueueEvent = true;
	}
	return 0;
}
HOOK(void, __fastcall, HudResult_CHudResultAdvance, 0x10B96D0, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{	
	if (*(uint32_t*)0x10B96E6 != 0xFFD285E8)
	{
		if (!stageEnded)
		{
			
			//SequenceHelpers::resetStorySequence();
		}
		stageEnded = true;
		return;
	}
	else
	{
		stageEnded = false;
	}
	originalHudResult_CHudResultAdvance(This, Edx, in_rUpdateInfo);
}
void __declspec(naked) ForceGoToPlayableMenu()
{
	static uint32_t whiteWorld = 0x00D0DE30;
	static uint32_t stage = 0x00D0DEB3;
	__asm
	{
		cmp StageManager::WhiteWorldEnabled, 1
		je WhiteWorld
		mov     ebp, esp
		jmp[stage]
		WhiteWorld:
		jmp[whiteWorld]

	}
}
HOOK(void, __fastcall, CHudGateMenuMainCStateOutroBegin, 0x107B770, hh::fnd::CStateMachineBase::CStateBase* This)
{
	uint32_t contextBase = (uint32_t)This->GetContextBase();
	int* contextBaseIntP = (int*)This->GetContextBase();
	auto stageID = (*(uint8_t*)((int)contextBase + 336));
	bool classic;
	if ((int)contextBaseIntP[83] == 0)
	{
		classic = true;
	}
	else if ((int)contextBaseIntP[83] == 4)
	{
		classic = false;
	}
	if (stageID % 2 == 0 && !classic) stageID += 1;
	if (!*(bool*)(contextBase + 512)) // Not customize skill
	{
		uint32_t v8 = *(uint32_t*)(contextBase + 440);
		if (v8 != 1)
		{
			if (v8 == 2)
			{
				StageManager::WhiteWorldEnabled = false;
				SetCorrectStageFromPAM(stageID);
			}
		}
		else
		{
			// Based on sub_107AC50
			uint32_t v9 = *(uint32_t*)(contextBase + 520);
			if (v9 != 7 && v9 != 11 && v9 != 13)
			{
				StageManager::WhiteWorldEnabled = false;
				SetCorrectStageFromPAM(stageID);
			}
		}
	}
	originalCHudGateMenuMainCStateOutroBegin(This);
}
//int __cdecl GetEventIndex(Hedgehog::Base::CSharedString *in_EventName)
HOOK(int, __cdecl, GetEventIndex, 0x00B266B0, Hedgehog::Base::CSharedString* in_EventName)
{
	return 4;
}
void SetStuffTest()
{
	std::string stageIdCopy = std::string(StageManager::nextStageID);
	std::string evsNameCopy = std::string(StageManager::nextEvsID);
	DebugDrawText::log(std::format("[LLM] Loading Event ID \"{}\" as {}", evsNameCopy, stageIdCopy).c_str(), 5);
	StageManager::setGameParameters(stageIdCopy, evsNameCopy);
}
void MovieStuff()
{

}
void __declspec(naked) SetCorrectStageForCutscene()
{
	static uint32_t sub_662010 = 0x662010;
	static uint32_t returnAddress = 0x00B26861;
	__asm
	{
		cmp isMovieCutscene, 1
		je PlayStuff
		push edi
		call MovieStuff
		pop edi
		mov al, 0
		retn

		PlayStuff:
		push edi
		call SetStuffTest
		pop edi
		mov al, 1
		retn
	}
}
HOOK(void, __fastcall, CEventScene_OnEnd, 0xB1EA80, int* This, void* Edx, int a2)
{
	skipCurrentQueueEvent = false;
	//StageManager::LastSavedQueueIndex++;
	SetCorrectStageFromFlag();
	return originalCEventScene_OnEnd(This, Edx, a2);
}
//LONG __thiscall sub_B1ECF0(int this, int a2, int a3, int _38)
HOOK(void*, __fastcall, sub_B1ECF0, 0xB1ECF0, int* This, void* Edx, int a2, int a3, int a4)
{
	//uint32_t* appdocMember = (uint32_t*)Sonic::CApplicationDocument::GetInstance()->m_pMember;
	//auto v4 = *((DWORD*)appdocMember + 109);
	//auto e = (Hedgehog::Base::CSharedString*)(*((DWORD*)v4 + 32) + 44);
	//auto e2 = (Hedgehog::Base::CSharedString*)(*((DWORD*)v4 + 32) + 48);
	//*e2 = std::string(Project::queueData.data[StageManager::LastSavedQueueIndex + 1].dataName).c_str();
	//
	//uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	//char** h = (char**)stageTerrainAddress;
	//
	//strcpy(*(char**)stageTerrainAddress, std::string(Project::queueData.data[StageManager::LastSavedQueueIndex + 1].dataName).c_str());
	skipCurrentQueueEvent = true;
	return originalsub_B1ECF0(This, Edx, a2, a3, a4);
}
extern "C" __declspec(dllexport) bool API_IsEvent()
{
	return false;
}
extern "C" __declspec(dllexport) int API_GetLoadingScreenMotionIndex()
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	std::string stageIDName = *h;
	std::unordered_map<std::string, int> stageMap = {
		{"ghz", 0}, {"cpz", 2}, {"ssz", 4}, {"sph", 6},
		{"cte", 8}, {"ssh", 10}, {"csc", 12}, {"euc", 14},
		{"pla", 16}
	};

	// if the stage id is not any of the stages, return the mono pic
	if (stageMap.find(stageIDName.substr(0, 3)) == stageMap.end()) {
		return 18;
	}

	std::string prefix = stageIDName.substr(0, 3);
	int indexOffset = (stageIDName[3] == '2') ? 1 : 0; // add 1 if its werehog
	std::string numberPart = stageIDName.substr(3 + indexOffset, 3);

	return stageMap[prefix] + indexOffset;
}
HOOK(void, __fastcall, Sonic_Mission_CScriptImpl_SendMissionType, 0x011041E0, float a1, void* Edx, int a2, int a3, int a4)
{
	return;
}
void StageManager::initialize()
{
	WRITE_JUMP(0xD56CCA, SetCorrectTerrainForMission_ASM);
	WRITE_JUMP(0x00D0E164, InterceptGameplayFlowLoading);
	
	//WRITE_JUMP(0x00D0DEB1, ForceGoToPlayableMenu);
	//Blocks Gate UI options and switch
	WRITE_JUMP(0x01080F02, 0x01080FB7);

	WRITE_JUMP(0x00B267D0, SetCorrectStageForCutscene)
	////btn skip
	//WRITE_JUMP(0x00B21D38, SkipEvent);
	////dont spawn button ui
	//WRITE_JUMP(0x00B21CF5, 0x00B21CF3);
	//INSTALL_HOOK(GetEventIndex);


	//Patch out reading MissionScript to avoid crashes when loading stages without the stgXXX archive name format
	INSTALL_HOOK(Sonic_Mission_CScriptImpl_SendMissionType);
	INSTALL_HOOK(CalledOnResultEnd);
	INSTALL_HOOK(sub_B1ECF0);
	INSTALL_HOOK(CEventScene_OnEnd);
	INSTALL_HOOK(HudLoading_CHudLoadingCStateOutroBegin);
	INSTALL_HOOK(CHudGateMenuMainCStateOutroBegin);
	//INSTALL_HOOK(HudResult_CHudResultAdvance);
	INSTALL_HOOK(LoadPamSettings);
	INSTALL_HOOK(ConstructStorySequence);
}