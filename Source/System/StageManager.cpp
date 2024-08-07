#include "..\UI\TitleWorldMap.h"
const char* StageManager::NextLevelLoad;
bool StageManager::ActiveReplacement;
bool StageManager::ReplacingNext;
const char* lastStageID = "";
const char* lastStageIDGate = "";
bool StageManager::WhiteWorldEnabled = false;
bool StageManager::LoadingReplacementEnabled = false;
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
	StageManager::LoadingReplacementEnabled = true;
	StageManager::WhiteWorldEnabled = true;
	//SequenceHelpers::changeModule(ModuleFlow::PlayableMenu);
	SequenceHelpers::setPlayerType(isWerehog ? PlayerType::CLASSIC_SONIC : PlayerType::GENERIC_SONIC);
	DebugDrawText::log(std::format("[LLM] Loading Capital (Day) \"{}\" as {}", stageName, isWerehog ? "Werehog" : "Modern").c_str(), 5);
}
const char* StageManager::getStageToLoad()
{
	const char* stageToLoad = "ghz200";

	int latestFlag = TitleWorldMap::m_lastFlagSelected;
	int stageSelected = TitleWorldMap::m_stageSelectWindowSelection;

	if (TitleWorldMap::m_isCapitalWindowOpened)
		stageToLoad = SUC::Project::s_WorldData.data[latestFlag].data[SUC::Project::GetCapital(latestFlag, TitleWorldMap::m_flags[latestFlag].night)].levelID.c_str();
	else
	{
		if (SUC::Project::s_WorldData.data[latestFlag].dataNight.size() != 0 && TitleWorldMap::m_flags[latestFlag].night)
		{
			stageToLoad = SUC::Project::s_WorldData.data[latestFlag].dataNight[stageSelected].levelID.c_str();
			SequenceHelpers::setPlayerType(1);
		}
		else
		{
			stageToLoad = SUC::Project::s_WorldData.data[latestFlag].data[stageSelected].levelID.c_str();
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
void executeSequenceData(std::vector <SUC::Project::SequenceData::QueueData> dataList)
{
	int index = StageManager::LastSavedQueueIndex;
	SUC::Project::SequenceData::QueueData data = dataList[index];
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

void calculateNextStageFromHub(uint32_t stageIDPam)
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	const char* stageToLoad = "ghz200";
	//get current flag based on the pam stage loaded
	int flag = SUC::Project::GetFlagFromStage(*h);
	stageToLoad = SUC::Project::s_WorldData.data[flag].data[stageIDPam].levelID.c_str();
	lastStageID = stageToLoad;
	SequenceHelpers::loadStage(stageToLoad);
	strcpy(*(char**)stageTerrainAddress, stageToLoad);
}
void calculateNextStage()
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	TitleWorldMap::m_isWorldMapCameraInit = false;

	//strcpy(*(char**)stageTerrainAddress, StageManager::NextLevelLoad);
	if (!StageManager::InStory)
	{		
		const char* stageToLoad = "ghz200";
		if (!StageManager::LoadingReplacementEnabled)
		{
			//if (Title::inInstall)
			//{
			//	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
			//	char** h = (char**)stageTerrainAddress;
			//	const char* terr = *h;
			//
			//	if (terr != lastStageID && lastStageID[0] != '\0')
			//		strcpy(*(char**)stageTerrainAddress, lastStageID);
			//}
			return;
		}
		if (SUC::Project::s_WorldData.data.size() < TitleWorldMap::m_lastFlagSelected)
		{
			//if only cpp had the same ${} system as c#
			std::string message = "This country has an invalid configuration. Loading " + std::string(stageToLoad) + " instead.";
			MessageBoxA(NULL, message.c_str(), "Unleashed Title Screen", 0);
			printf("\n[WorldMap] Missing config for FlagID %d", TitleWorldMap::m_lastFlagSelected);
		}
		else
		{
			stageToLoad = StageManager::getStageToLoad();
		}
		TitleWorldMap::m_isActive = false;
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
		//	executeSequenceData(SUC::Project::s_SequenceDataQueue.data);
		//}
		//if(SUC::Project::s_SequenceDataQueue.data[StageManager::LastSavedQueueIndex].type != 5)
		//strcpy(*(char**)stageTerrainAddress, SUC::Project::s_SequenceDataQueue.data[StageManager::LastSavedQueueIndex].dataName.c_str());
		//skipCurrentQueueEvent = false;
	}

	DebugDrawText::log((std::string("Stage Loading: ") + std::string(*h)).c_str());
	inStoryBefore = StageManager::InStory;
}
void __declspec(naked) ASM_OverrideStageIDLoading()
{
	static uint32_t sub_662010 = 0x662010;
	static uint32_t returnAddress = 0xD56CCF;
	__asm
	{
		call[sub_662010]
		push    esi
		call    calculateNextStage
		pop     esi
		jmp[returnAddress]
	}
}
void __declspec(naked) ASM_InterceptGameplayFlowLoading()
{
	static uint32_t normal = 0x00D0E166;
	static uint32_t loc_D0E170 = 0xD0E170;
	__asm
	{
		//Original function
		mov     esi, eax
		xor eax, eax
		cmp StageManager::LoadingReplacementEnabled, 1
		jne FunctionFinish


		cmp TitleWorldMap::m_forceLoadToTitle, 1
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
void StageManager::Update()
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
HOOK(DWORD*, __fastcall, CStoryImplConstructor, 0xD76930)
{
	storySequence = originalCStoryImplConstructor();
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
	calculateNextStage();
}
HOOK(int, __fastcall, CGameplayFlowStage_CStateWaitEnd, 0x00CFB300, void* This, void* Edx)
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
		calculateNextStage();
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
				calculateNextStageFromHub(stageID);
			}
		}
		else
		{
			// Based on sub_107AC50
			uint32_t v9 = *(uint32_t*)(contextBase + 520);
			if (v9 != 7 && v9 != 11 && v9 != 13)
			{
				StageManager::WhiteWorldEnabled = false;
				calculateNextStageFromHub(stageID);
			}
		}
	}
	originalCHudGateMenuMainCStateOutroBegin(This);
}

void setCorrectStageForCutscene_OnRealtime()
{
	std::string stageIdCopy = std::string(StageManager::nextStageID);
	std::string evsNameCopy = std::string(StageManager::nextEvsID);
	DebugDrawText::log(std::format("[LLM] Loading Event ID \"{}\" as {}", evsNameCopy, stageIdCopy).c_str(), 5);
	StageManager::setGameParameters(stageIdCopy, evsNameCopy);
}
void setCorrectStageForCutscene_OnMovie()
{
	// -shrug-
}
void __declspec(naked) ASM_SetCorrectStageForCutscene()
{
	static uint32_t sub_662010 = 0x662010;
	static uint32_t returnAddress = 0x00B26861;
	__asm
	{
		cmp isMovieCutscene, 1
		je PlayStuff
		push edi
		call setCorrectStageForCutscene_OnMovie
		pop edi
		mov al, 0
		retn

		PlayStuff:
		push edi
		call setCorrectStageForCutscene_OnRealtime
		pop edi
		mov al, 1
		retn
	}
}
HOOK(void, __fastcall, CEventSceneEnd, 0xB1EA80, int* This, void* Edx, int a2)
{
	skipCurrentQueueEvent = false;
	//StageManager::LastSavedQueueIndex++;
	calculateNextStage();
	return originalCEventSceneEnd(This, Edx, a2);
}

HOOK(void*, __fastcall, CEventSceneStart, 0xB1ECF0, int* This, void* Edx, int a2, int a3, int a4)
{
	skipCurrentQueueEvent = true;
	return originalCEventSceneStart(This, Edx, a2, a3, a4);
}
extern "C" __declspec(dllexport) bool API_IsEvent()
{
	return false;
}
std::string splitStr(std::string const& s, char separator)
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
extern "C" __declspec(dllexport) int API_GetLoadingScreenMotionIndex()
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	std::string stageIDName = *h;
	std::unordered_map<std::string, int> stageMap = {
		{"Mykonos", 0}, {"Africa", 2}, {"Europe", 4}, {"China", 6},
		{"Snow", 8}, {"Petra", 10}, {"NewYork", 12}, {"Beach", 14},
		{"EggmanLand", 16}
	};
	std::string splitName = splitStr(stageIDName, '_').erase(0, 1);
	
	// if the stage id is not any of the stages, return the mono pic
	if (stageMap.find(splitName) == stageMap.end()) {
		return 18;
	}

	int indexOffset = (stageIDName[0] == 'N') ? 1 : 0; // add 1 if its werehog
	std::string numberPart = stageIDName.substr(3 + indexOffset, 3);

	return stageMap[splitName] + indexOffset;
}
HOOK(void, __fastcall, Sonic_Mission_CScriptImpl_SendMissionType, 0x011041E0, float a1, void* Edx, int a2, int a3, int a4)
{
	return;
}
std::string ModeStrings[4] = { "Stage", "Event", "PlayableMenu", "Title" };
//figure out a way to call this as a function
HOOK(int*, __fastcall, StartModule, 0x00D77020, DWORD* StorySequence,void* Edx, DWORD* a2, LuaStringEntryContainer* a3)
{
	auto exampleRead = a3->entry->content;
	if (!StageManager::WhiteWorldEnabled)
	{
		if(std::string(exampleRead) == "PlayableMenu")
		{
			a3 = new LuaStringEntryContainer(_strdup(ModeStrings[0].c_str()));
		}
	}
	else
	{
		if (std::string(exampleRead) == "Stage" || std::string(exampleRead) == "Event" || std::string(exampleRead) == "StageEvent")
		{
			a3 = new LuaStringEntryContainer(_strdup(ModeStrings[2].c_str()));
		}
	}
	if (TitleWorldMap::m_forceLoadToTitle)
	{
		if (std::string(exampleRead) != "Title")
		{
			a3 = new LuaStringEntryContainer(_strdup(ModeStrings[3].c_str()));
		}
	}
	DebugDrawText::log(std::format("[LUASTORYSEQUENCE] Loading module \"{0}\"", a3->entry->content).c_str());
	return originalStartModule(StorySequence, Edx, a2, a3);
}

//void __stdcall sub_D6BE60(int a1, int a2)
HOOK(void, __fastcall, ModuleStuffTest, 0xD6BE60, int a1, void* Edx, int a2)
{
	auto v2 = a1;
	auto v6 = *(DWORD*)(v2 + 4);
	auto v7 = *(DWORD*)(v6 + 52);
	
	return originalModuleStuffTest(a1, Edx, a2);
}
void StageManager::initialize()
{
	//Blocks Gate UI options and switch
	WRITE_JUMP(0x01080F02, 0x01080FB7);
	WRITE_JUMP(0xD56CCA, ASM_OverrideStageIDLoading);
	WRITE_JUMP(0x00B267D0, ASM_SetCorrectStageForCutscene);
	WRITE_JUMP(0x00D0E164, ASM_InterceptGameplayFlowLoading);
	//INSTALL_HOOK(ModuleStuffTest);
	INSTALL_HOOK(CStoryImplConstructor);
	INSTALL_HOOK(CEventSceneStart);
	INSTALL_HOOK(CEventSceneEnd);
	INSTALL_HOOK(CGameplayFlowStage_CStateWaitEnd);
	INSTALL_HOOK(CHudGateMenuMainCStateOutroBegin);
	INSTALL_HOOK(HudLoading_CHudLoadingCStateOutroBegin);
	//INSTALL_HOOK(StartModule);

	//Patch out reading MissionScript to avoid crashes when loading stages without the stgXXX archive name format
	INSTALL_HOOK(Sonic_Mission_CScriptImpl_SendMissionType);
}