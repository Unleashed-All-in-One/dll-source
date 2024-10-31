#include "../UI/TitleWorldMap.h"
namespace SUC::System
{
	std::string StageManager::s_NextStage;
	std::string StageManager::s_NextEventScene;
	const char* StageManager::s_CheckForID;
	bool		StageManager::s_ETFEntered;
	bool		StageManager::s_OverrideNextStage;
	bool		StageManager::s_HubModeEnabled = false;
	bool		StageManager::s_InStoryMode;
	bool		StageManager::s_LoadingReplacementEnabled = true;
	int			StageManager::s_LastSavedQueueIndex = -1;

	const char* lastStageID = "";
	const char* lastStageIDGate = "";
	uint32_t nextPlayerType = 0;
	const char* temp_nextStageIDCutscene = "ev011";
	const char* temp_nextEVSIDCutscene = "ghz100";
	bool isMovieCutscene;
	std::string etfHubName = "ghz205";
	bool inStoryBefore;
	bool stageEnded;
	bool skipCurrentQueueEvent;
	DWORD* storySequence;
	bool eventTransitionFinished;
	std::function<std::string()> overrideStageIDEvent;
	bool m_TriggerStageLoadNextFrame;
	bool expectingLoad; //mega pause

	
	SLoadInfo* m_CurrentLoadInfo;

	void StageManager::ConfigureNextStage(std::string in_Stage, SLoadInfo::SSonicType in_Type, bool in_Hub)
	{
		DebugDrawText::log("Deleted old SLoadInfo", 10, 100, TEXT_RED);
		m_CurrentLoadInfo = nullptr;

		m_CurrentLoadInfo = new SLoadInfo();
		m_CurrentLoadInfo->IsHub = in_Hub;
		m_CurrentLoadInfo->PlayerType = in_Type;
		m_CurrentLoadInfo->StageArchiveName = _strdup(in_Stage.c_str());
	}
	void StageManager::SetOverrideStageIDProcessor(std::function<std::string()> in_Function, bool in_TriggerOnNextTick, const char* in_FileNameCode)
	{
		m_TriggerStageLoadNextFrame = in_TriggerOnNextTick;
		overrideStageIDEvent = in_Function;
	}
	std::string SplitString(std::string const& s, char separator)
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
	void StageManager::SetGameParameters(std::string stageID, std::string evsID)
	{
		s_NextStage = stageID;
		s_NextEventScene = evsID;
		auto gameParameter = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter;
		gameParameter->m_pStageParameter->TerrainArchiveName = s_NextStage.c_str();
		if (!evsID.empty())
			gameParameter->m_pStageParameter->ModeName = s_NextStage.c_str();
	}
	std::string StageManager::GetEventID()
	{
		return s_NextEventScene;
	}
	void StageManager::SetETFInfo(std::string etfHubStageName)
	{
		s_HubModeEnabled = true;
		skipCurrentQueueEvent = false;
		etfHubName = etfHubStageName;
	}
	void loadCapital(std::string stageName, bool isWerehog)
	{
		SequenceHelpers::LoadStage(stageName.c_str(), 0, false);
		StageManager::s_LoadingReplacementEnabled = true;
		StageManager::s_HubModeEnabled = true;
		//SequenceHelpers::ChangeModule(ModuleFlow::PlayableMenu);
		SequenceHelpers::SetPlayerType(isWerehog ? PlayerType::CLASSIC_SONIC : PlayerType::GENERIC_SONIC);
		DebugDrawText::log(std::format("[LLM] Loading Capital (Day) \"{}\" as {}", stageName, isWerehog ? "Werehog" : "Modern").c_str(), 5);
	}
	const char* StageManager::GetStageToLoad()
	{
		const char* stageToLoad = "ghz200";

		//int latestFlag = SUC::UI::TitleScreen::TitleWorldMap::s_LastFlagSelected;
		//int stageSelected = SUC::UI::TitleScreen::TitleWorldMap::s_StageSelectWindowSelection;
		///CHECK FOR REMOVAL
		//if (SUC::UI::TitleScreen::TitleWorldMap::s_IsCapitalWindowOpened)
		//	stageToLoad = SUC::Project::s_WorldData.data[latestFlag].data[SUC::Project::GetCapital(latestFlag, SUC::UI::TitleScreen::TitleWorldMap::s_Flags[latestFlag].night)].levelID.c_str();
		//else
		//{
		//	if (SUC::Project::s_WorldData.data[latestFlag].dataNight.size() != 0 && SUC::UI::TitleScreen::TitleWorldMap::s_Flags[latestFlag].night)
		//	{
		//		stageToLoad = SUC::Project::s_WorldData.data[latestFlag].dataNight[stageSelected].levelID.c_str();
		//		SequenceHelpers::SetPlayerType(1);
		//	}
		//	else
		//	{
		//		stageToLoad = SUC::Project::s_WorldData.data[latestFlag].data[stageSelected].levelID.c_str();
		//		SequenceHelpers::SetPlayerType(0);
		//	}
		//}
		return stageToLoad;
	}
	void StageManager::ForcePlayCutscene(std::string in_EventName, std::string in_StageName, bool flag, int playerType)
	{
		auto playEventRequest = new LuaParamPlayEvent();
		playEventRequest->entry = new LuaParamPlayEventEntry();
		playEventRequest->entry->eventName = in_EventName.c_str();
		playEventRequest->entry->eventStageType = IntStuffPlayEvent();
		playEventRequest->entry->eventStageType.value = flag ? 1 : 0;
		FUNCTION_PTR(void, __thiscall, PlayEventLuanne, 0x00D72520, DWORD * StorySeq, int a2, LuaParamPlayEvent * a3);
		PlayEventLuanne(storySequence, 0, playEventRequest);

		s_NextEventScene = in_EventName.c_str();
		s_NextStage = in_StageName.c_str();
		isMovieCutscene = true;
		SequenceHelpers::SetPlayerType(playerType);
		auto message1 = Sonic::Message::MsgSequenceEvent(0, 7);
		Sonic::Sequence::Main::ProcessMessage(&message1);
	}
	void StageManager::TriggerSequenceEvents(int type, bool dontSetPlayerType)
	{
		switch (type)
		{
		case 0:
		{
			//Play stage (day)
			SequenceHelpers::LoadStage(s_NextStage.c_str(), 0, false);
			if (!dontSetPlayerType)
				SequenceHelpers::SetPlayerType(PlayerType::GENERIC_SONIC);
			s_HubModeEnabled = 0;
			DebugDrawText::log(std::format("[LLM] Loading Stage (Day) \"{}\" as {}", s_NextStage, (int)PlayerType::GENERIC_SONIC).c_str(), 5);
			break;
		}
		case 1:
		{
			//Play stage (night)
			SequenceHelpers::LoadStage(s_NextStage.c_str(), 0, false);
			if (!dontSetPlayerType)
				SequenceHelpers::SetPlayerType(PlayerType::CLASSIC_SONIC);
			s_HubModeEnabled = 0;
			DebugDrawText::log(std::format("[LLM] Loading Stage (Night) \"{}\" as {}", s_NextStage, (int)PlayerType::CLASSIC_SONIC).c_str(), 5);
			break;
		}
		case 2:
		{
			//Go to capital (day)
			loadCapital(s_NextStage, 0);
			if (!dontSetPlayerType)
				SequenceHelpers::SetPlayerType(PlayerType::GENERIC_SONIC);
			break;
		}
		case 3:
		{
			//Go to capital (day)
			loadCapital(s_NextStage, 1);
			if (!dontSetPlayerType)
				SequenceHelpers::SetPlayerType(PlayerType::CLASSIC_SONIC);
			break;
		}
		case 4:
		{
			SequenceHelpers::PlayEvent(s_NextEventScene.c_str(), ModuleFlow::Event);
			if (!dontSetPlayerType)
				SequenceHelpers::SetPlayerType(PlayerType::GENERIC_SONIC);
			auto message = Sonic::Message::MsgSequenceEvent(5, 0);
			Sonic::Sequence::Main::ProcessMessage(&message);
			isMovieCutscene = false;
			s_NextStage = "ghz100";
			break;
		}
		case 5:
		{
			//Play event (realtime)

			//auto playEventRequest = new LuaParamPlayEvent();
			//playEventRequest->entry = new LuaParamPlayEventEntry();
			//playEventRequest->entry->eventName = s_NextEventScene.c_str();
			//playEventRequest->entry->eventStageType = IntStuffPlayEvent();
			//playEventRequest->entry->eventStageType.value = 0;
			//FUNCTION_PTR(void, __thiscall, PlayEventLuanne, 0x00D72520, Sonic::Sequence::Story * StorySeq, int a2, LuaParamPlayEvent * a3);
			//PlayEventLuanne(SequenceHelpers::storySequenceInstance, 0, playEventRequest);
			SequenceHelpers::QueueEvent(s_NextEventScene.c_str());
			isMovieCutscene = true;
			//SequenceHelpers::ResetStorySequence();
			SequenceHelpers::ChangeModule(ModuleFlow::Event);
			//SequenceHelpers::SetPlayerType(0);
			SetGameParameters(s_NextStage, s_NextEventScene);
			break;
		}
		}
	}
	void executeSequenceData(std::vector <SUC::Project::SequenceData::QueueData> dataList)
	{
		int index = StageManager::s_LastSavedQueueIndex;
		SUC::Project::SequenceData::QueueData data = dataList[index];
		//if (data.type == 4 && data.immediate)
		//{
		//	//EventQueue will play videos in order
		//	TriggerSequenceEvents(data);
		//	if(data.playerTypeOverride != -1)
		//		SequenceHelpers::SetPlayerType(data.playerTypeOverride);
		//	for (size_t i = index + 1; i < dataList.size(); i++)
		//	{
		//		if (dataList[i].type == 4 && dataList[i].immediate == false)
		//		{
		//			StageManager::LastSavedQueueIndex++;
		//			DebugDrawText::log(std::format("[LLM] Queueing event \"{}\"", dataList[i].dataName).c_str(), 5);
		//			SequenceHelpers::QueueEvent(dataList[i].dataName.c_str());
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

	void CalculateNextStageFromHub(uint32_t stageIDPam)
	{
		uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
		char** h = (char**)stageTerrainAddress;
		const char* stageToLoad = "ghz200";
		//get current flag based on the pam stage loaded
		int flag = SUC::Project::GetFlagFromStage(*h);
		stageToLoad = SUC::Project::s_WorldData.data[flag].data[stageIDPam].levelID.c_str();
		lastStageID = stageToLoad;
		SequenceHelpers::LoadStage(stageToLoad);
		strcpy(*(char**)stageTerrainAddress, stageToLoad);
	}
	//void CalculateNextStage()
	//{
	//	SUC::UI::TitleScreen::TitleWorldMap::s_IsWorldMapCameraInitialized = false;
	//	if (!StageManager::s_LoadingReplacementEnabled)
	//		return;
	//
	//	if (!StageManager::s_InStoryMode)
	//	{
	//		const char* stageToLoad = "ghz200";
	//		if (!StageManager::s_LoadingReplacementEnabled)
	//		{
	//			return;
	//		}
	//		if (Project::s_WorldData.data.size() < SUC::UI::TitleScreen::TitleWorldMap::s_LastFlagSelected)
	//		{
	//			MessageBoxA(NULL, SUC::Format("The country you selected (%d) has an invalid configuration!", UI::TitleScreen::TitleWorldMap::s_LastFlagSelected), MOD_NAME, 0);
	//			LogMessage("Missing configuration for flag!");
	//		}
	//		else
	//		{
	//			stageToLoad = StageManager::GetStageToLoad();
	//		}
	//		SUC::UI::TitleScreen::TitleWorldMap::s_IsActive = false;
	//		/*if (!StageManager::s_NextStage.empty())
	//		{
	//			stageToLoad = _strdup(StageManager::s_NextStage.c_str());
	//		}*/
	//		StageManager::SetGameParameters(stageToLoad, "Stage");
	//	}
	//	else
	//	{
	//		if (StageManager::s_ETFEntered)
	//		{
	//			StageManager::SetGameParameters(StageManager::s_NextStage.c_str(), "Stage");
	//			//StageManager::enteredStageFromETF = false;
	//		}
	//		if (!skipCurrentQueueEvent)
	//		{
	//			LuaManager::OnStageLoad();
	//		}
	//		else
	//		{
	//			StageManager::s_ETFEntered = false;
	//			skipCurrentQueueEvent = false;
	//		}
	//		//if (StageManager::LastSavedQueueIndex == -1)
	//		//{
	//		//	printf("[StageManager] No saves have been found.");
	//		//	StageManager::LastSavedQueueIndex = 0;
	//		//}
	//		//if (!skipCurrentQueueEvent)
	//		//{
	//		//	if (inStoryBefore == StageManager::InStory)
	//		//		StageManager::LastSavedQueueIndex++;
	//		//	executeSequenceData(SUC::Project::s_SequenceDataQueue.data);
	//		//}
	//		//if(SUC::Project::s_SequenceDataQueue.data[StageManager::LastSavedQueueIndex].type != 5)
	//		//strcpy(*(char**)stageTerrainAddress, SUC::Project::s_SequenceDataQueue.data[StageManager::LastSavedQueueIndex].dataName.c_str());
	//		//skipCurrentQueueEvent = false;
	//	}
	//
	//	DebugDrawText::log(SUC::Format("[StageManager] Forcing TerrainArchiveName: %s", StageManager::s_NextStage.c_str()), 10, 0, TEXT_YELLOW);
	//	inStoryBefore = StageManager::s_InStoryMode;
	//}
	void setCorrectStageForCutscene_OnRealtime()
	{
		std::string stageIdCopy = std::string(StageManager::s_NextStage);
		std::string evsNameCopy = std::string(StageManager::s_NextEventScene);
		DebugDrawText::log(std::format("[LLM] Loading Event ID \"{}\" as {}", evsNameCopy, stageIdCopy).c_str(), 5);
		StageManager::SetGameParameters(stageIdCopy, evsNameCopy);
	}
	void setCorrectStageForCutscene_OnMovie()
	{
		// -shrug-
	}
	void StageManager::Update()
	{
		DebugDrawText::log(SUC::Format("TerrainArchiveName: %s", Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter->m_pStageParameter->TerrainArchiveName.c_str()), 0, 0, TEXT_GRAY);

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
	void StageManager::SetCorrectStage()
	{
		//CalculateNextStage();
	}
	//void __declspec(naked) ASM_OverrideStageIDLoading()
	//{
	//	static uint32_t sub_662010 = 0x662010;
	//	static uint32_t returnAddress = 0xD56CCF;
	//	__asm
	//	{
	//		call[sub_662010]
	//		push    esi
	//		call    CalculateNextStage
	//		pop     esi
	//		jmp[returnAddress]
	//	}
	//}
	void __declspec(naked) ASM_InterceptGameplayFlowLoading()
	{
		static uint32_t normal = 0x00D0E166;
		static uint32_t loc_D0E170 = 0xD0E170;
		__asm
		{
			//Original function
			mov     esi, eax
			xor eax, eax
			cmp StageManager::s_LoadingReplacementEnabled, 1
			jne FunctionFinish


			cmp SUC::UI::TitleScreen::TitleWorldMap::s_ForceTitleFlow, 1
			je ChangeToTitle

			cmp ecx, 6
			je ChangeToWhiteWorld
			cmp ecx, 5
			je ChangeModuleDefault

			//Change module only if its white world
			ChangeModuleDefault :
			je ChangeToAct
				jmp[loc_D0E170]

				ChangeToAct :
				cmp StageManager::s_HubModeEnabled, 1
				je FunctionFinish
				mov ecx, 6
				jmp[loc_D0E170]


				ChangeToTitle :
				cmp ecx, 5
				jne FunctionFinish
				mov ecx, 3
				jmp[loc_D0E170]

				ChangeToWhiteWorld :
				cmp StageManager::s_HubModeEnabled, 1
				jne ChangeModuleDefault
				mov ecx, 5
				jmp[loc_D0E170]

				FunctionFinish :
				jmp[loc_D0E170]
		}
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

			PlayStuff :
			push edi
				call setCorrectStageForCutscene_OnRealtime
				pop edi
				mov al, 1
				retn
		}
	}
	HOOK(int32_t*, __fastcall, HudLoading_CHudLoadingCStateOutroBegin, 0x1093410, hh::fnd::CStateMachineBase::CStateBase* This)
	{
		return originalHudLoading_CHudLoadingCStateOutroBegin(This);
	}
	HOOK(DWORD*, __fastcall, CStoryImplConstructor, 0xD76930)
	{
		storySequence = originalCStoryImplConstructor();
		return storySequence;
	}
	HOOK(int, __fastcall, CGameplayFlowStage_CStateWaitEnd, 0x00CFB300, void* This, void* Edx)
	{
		if (StageManager::s_ETFEntered)
		{
			std::string stageBef = StageManager::s_NextStage.c_str();
			std::string evsBef = StageManager::s_NextEventScene.c_str();
			LuaManager::OnStageEnd();
			if (StageManager::s_NextStage == stageBef && StageManager::s_NextEventScene == evsBef)
			{
				loadCapital(etfHubName, 0);
				skipCurrentQueueEvent = false;
			}
			else
			{
				StageManager::s_ETFEntered = false;
			}
		}
		else
		{
			//CalculateNextStage();
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
				//SequenceHelpers::ResetStorySequence();
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
					StageManager::s_HubModeEnabled = false;
					CalculateNextStageFromHub(stageID);
				}
			}
			else
			{
				// Based on sub_107AC50
				uint32_t v9 = *(uint32_t*)(contextBase + 520);
				if (v9 != 7 && v9 != 11 && v9 != 13)
				{
					StageManager::s_HubModeEnabled = false;
					CalculateNextStageFromHub(stageID);
				}
			}
		}
		originalCHudGateMenuMainCStateOutroBegin(This);
	}
	HOOK(void, __fastcall, CEventSceneEnd, 0xB1EA80, int* This, void* Edx, int a2)
	{
		skipCurrentQueueEvent = false;
		//StageManager::LastSavedQueueIndex++;
		//CalculateNextStage();
		return originalCEventSceneEnd(This, Edx, a2);
	}
	HOOK(void*, __fastcall, CEventSceneStart, 0xB1ECF0, int* This, void* Edx, int a2, int a3, int a4)
	{
		skipCurrentQueueEvent = true;
		return originalCEventSceneStart(This, Edx, a2, a3, a4);
	}
	HOOK(void, __fastcall, Sonic_Mission_CScriptImpl_SendMissionType, 0x011041E0, float a1, void* Edx, int a2, int a3, int a4)
	{
		return;
	}
	//figure out a way to call this as a function
	


	HOOK(void*, __fastcall, SM_UpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
	{
		if(m_TriggerStageLoadNextFrame)
		{
			m_TriggerStageLoadNextFrame = false;
			std::string m_NextStage = "ghz100";
			if (overrideStageIDEvent)
				m_NextStage = overrideStageIDEvent();
			SequenceHelpers::LoadStage(_strdup(m_NextStage.c_str()));
			SequenceHelpers::SetPlayerType(0);
		}
		return originalSM_UpdateApplication(This, Edx, elapsedTime, a3);
	}
	struct SConversionLoadingInfo
	{
		int LoadingType;
		int MotionIndex;
	};
	extern "C" __declspec(dllexport) SConversionLoadingInfo API_GetLoadingScreenInfo()
	{
		SConversionLoadingInfo* returnedInfo = new SConversionLoadingInfo();
		std::string stageIDName = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter->m_pStageParameter->TerrainArchiveName.c_str();
		std::unordered_map<std::string, int> stageMap = {
			{"Mykonos", 0}, {"Africa", 2}, {"Europe", 4}, {"China", 6},
			{"Snow", 8}, {"Petra", 10}, {"NewYork", 12}, {"Beach", 14},
			{"EggmanLand", 16}
		};
		std::string splitName = SplitString(stageIDName, '_').erase(0, 1);

		// if the stage id is not any of the stages, return the mono pic
		if (stageMap.find(splitName) == stageMap.end()) 
		{
			returnedInfo->LoadingType = 2;
			returnedInfo->MotionIndex = 18;
		}
		else
		{
			int indexOffset = (stageIDName[0] == 'N') ? 1 : 0; // add 1 if its werehog
			std::string numberPart = stageIDName.substr(3 + indexOffset, 3);

			//Change with sequence
			returnedInfo->LoadingType = 2;
			returnedInfo->MotionIndex = stageMap[splitName] + indexOffset;			
		}		
		return *returnedInfo;
	}
	extern "C" __declspec(dllexport) bool API_IsEvent()
	{
		return false;
	}
	void StageManager::TriggerStageLoad()
	{
		expectingLoad = true;
		SequenceHelpers::SetStageInfo(m_CurrentLoadInfo);
		SequenceHelpers::ChangeModule(ModuleFlow::StageAct);
	}
	//char __thiscall Sonic::Sequence::CSequenceMainImpl::ProcessMessage(MainSequenceActor *this, int a1, int a2)
	void StageManager::Initialize()
	{
		//Blocks Gate UI options and switch
		WRITE_JUMP(0x01080F02, 0x01080FB7);
		WRITE_JUMP(0x00B267D0, ASM_SetCorrectStageForCutscene);

		//NOTE: can be replaced with latest BB
		INSTALL_HOOK(CStoryImplConstructor);
		INSTALL_HOOK(CEventSceneStart);
		INSTALL_HOOK(CEventSceneEnd);
		INSTALL_HOOK(CGameplayFlowStage_CStateWaitEnd);
		INSTALL_HOOK(CHudGateMenuMainCStateOutroBegin);
		//INSTALL_HOOK(HudLoading_CHudLoadingCStateOutroBegin);
		//INSTALL_HOOK(SM_UpdateApplication);

		//Patch out reading MissionScript to avoid crashes when loading stages without the stgXXX archive name format
		INSTALL_HOOK(Sonic_Mission_CScriptImpl_SendMissionType);
	}

}