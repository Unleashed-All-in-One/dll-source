#pragma once
class ETFStageGateUIContainer
{
public:
	Chao::CSD::RCPtr<Chao::CSD::CProject> projGate;
	boost::shared_ptr<Sonic::CGameObjectCSD> spGate;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcBg, rcArea_Tag, rcInfo_1, rcInfo_2, rcInfo_4, rcInfo_5, rcInfo_6, rcStageTitle, rcStageSS, rcActNumber, rcArrow, rcStatus_footer;
	int textIndex;
	bool isComplete;
	bool werehogMode;
	static ETFStageGateUIContainer* Generate(Sonic::CGameObject* Parent, bool isWerehog, bool isBoss = false)
	{
		ETFStageGateUIContainer* dataOut = new ETFStageGateUIContainer();
		Sonic::CCsdDatabaseWrapper wrapper(Parent->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
		auto spCsdProject = wrapper.GetCsdProject("ui_gate_suc");
		dataOut->projGate = spCsdProject->m_rcProject;
		dataOut->rcBg = dataOut->projGate->CreateScene("bg");
		dataOut->rcStageTitle = dataOut->projGate->CreateScene("status_title");
		dataOut->rcStageSS = dataOut->projGate->CreateScene("stage_ss");

		dataOut->rcArea_Tag = dataOut->projGate->CreateScene("area_tag");
		dataOut->rcInfo_1 = dataOut->projGate->CreateScene("info_1");
		dataOut->rcInfo_2 = dataOut->projGate->CreateScene("info_2");
		dataOut->rcInfo_4 = dataOut->projGate->CreateScene("info_4");
		dataOut->rcInfo_5 = dataOut->projGate->CreateScene("info_5");
		dataOut->rcInfo_6 = dataOut->projGate->CreateScene("info_6");
		dataOut->rcArrow = dataOut->projGate->CreateScene("arrow");
		dataOut->rcActNumber = dataOut->projGate->CreateScene("stage_name");
		dataOut->rcStatus_footer = dataOut->projGate->CreateScene("status_footer");
		dataOut->rcStatus_footer->SetHideFlag(true);
		dataOut->werehogMode = isWerehog;
		//Window on the left and right
		dataOut->rcArrow->SetHideFlag(true);
		dataOut->rcBg->GetNode("position_deco_1")->SetHideFlag(true);
		dataOut->rcBg->GetNode("position_2")->SetHideFlag(true);
		if (isBoss)
			dataOut->rcArea_Tag->GetNode("act_name")->SetPatternIndex(9);		

		Parent->m_pMember->m_pGameDocument->AddGameObject(dataOut->spGate = boost::make_shared<Sonic::CGameObjectCSD>(dataOut->projGate, 0.5f, "HUD", false), "main", Parent);
		dataOut->Show();
		return dataOut;
	}

	void UpdateState(std::string in_StageID, int actNumber, int subIndex, int worldIndex)
	{
		auto saveObj = SaveManager::getCurrentSave();
		int index = saveObj->getStageDataIndexFromID(in_StageID);
		StageSaveData* stageData;
		if (index > saveObj->stageData.size() || index == -1)
		{
			stageData = new StageSaveData();			
			stageData->rank = (StageRank)(-1);
		}
		else
		{
			stageData = saveObj->stageData[index];
		}

		uint32_t minutes, seconds, milliseconds, bestScore, bestMoonMedal, bestSunMedal;

		bestScore = stageData->score;
		bestMoonMedal = stageData->moonMedalCount;
		bestSunMedal = stageData->sunMedalCount;
		uint32_t totalMilliseconds = stageData->timeInSeconds * 1000.0f;
		minutes = totalMilliseconds / 60000;
		if (stageData->timeInSeconds > 0.0f && minutes <= 99)
		{
			seconds = (totalMilliseconds % 60000) / 1000;
			milliseconds = (totalMilliseconds % 60000) % 1000;
		}
		else
		{
			minutes = 0;
			seconds = 0;
			milliseconds = 0;
		}
		char* scoreCount = new char[8];
		char* secCount = new char[8];
		char* minCount = new char[8];
		char* msecCount = new char[8];
		char* sunCount = new char[3];
		char* moonCount = new char[3];
		sprintf(scoreCount, "%08d", bestScore);
		sprintf(secCount, "%02d", seconds);
		sprintf(minCount, "%02d", minutes);
		sprintf(msecCount, "%02d", milliseconds);
		sprintf(sunCount, "%d", bestSunMedal);
		sprintf(moonCount, "%d", bestMoonMedal);
		//i have no idea why but sprintf doesn't like the format for the milliseconds
		//and will print 3 instead of 2.
		if (strlen(msecCount) >= 3)
			msecCount[strlen(msecCount) - 1] = '\0';

		//for crash, it crashes here.
		if (rcInfo_1)
			rcInfo_1->GetNode("num")->SetText(scoreCount);
		if (rcInfo_2)
		{
			rcInfo_2->GetNode("num_msec")->SetText(msecCount);
			rcInfo_2->GetNode("num_sec")->SetText(secCount);
			rcInfo_2->GetNode("num_min")->SetText(minCount);
		}
		if (rcInfo_4)
		{
			rcInfo_4->GetNode("num_nume")->SetText(sunCount);
			rcInfo_4->GetNode("num_deno")->SetText("05");
		}
		if (rcInfo_5)
		{
			rcInfo_5->GetNode("num_nume")->SetText(moonCount);
			rcInfo_5->GetNode("num_deno")->SetText("05");
		}
		if (rcInfo_6)
		{
			int indexMotion = 5;
			switch (stageData->rank)
			{
			case Rank_S:
			{
				indexMotion = 0;
				break;
			}
			case Rank_A:
			{
				indexMotion = 1;
				break;
			}
			case Rank_B:
			{
				indexMotion = 2;
				break;
			}
			case Rank_C:
			{
				indexMotion = 3;
				break;
			}
			case Rank_D:
			{
				indexMotion = 4;
				break;
			}
			case Rank_E:
			{
				indexMotion = 5;
				break;
			}
			}
			rcInfo_6->GetNode("rank")->SetPatternIndex(indexMotion);
			rcInfo_6->GetNode("rank_shade")->SetPatternIndex(indexMotion);
		}
		std::string actText = std::format("Act {0}", actNumber);
		if (subIndex != -1)
		{
			actText += std::format("-{0}", subIndex);
		}
		if (rcActNumber)
			rcActNumber->GetNode("txt_box")->SetText(actText.c_str());
		if (rcArea_Tag)
			rcArea_Tag->GetNode("img_en")->SetPatternIndex(worldIndex);
	}
	void Step()
	{

	}
	void AddText(std::vector<std::string> in_String)
	{
	}
	void Reset()
	{
		textIndex = 0;
	}
	void Show()
	{
		CSDCommon::PlayAnimation(*rcActNumber, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		CSDCommon::PlayAnimation(*rcInfo_1, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		CSDCommon::PlayAnimation(*rcStageTitle, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(*rcBg, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(*rcArea_Tag, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		rcInfo_1->SetHideFlag(false);
		rcInfo_2->SetHideFlag(false);
		rcInfo_4->SetHideFlag(false);
		rcInfo_5->SetHideFlag(false);
		rcInfo_6->SetHideFlag(false);
		rcStageSS->SetHideFlag(false);
		rcActNumber->SetHideFlag(false);
		rcStatus_footer->SetHideFlag(false);
	}
	void Hide()
	{
		CSDCommon::PlayAnimation(*rcActNumber, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0, 0, false, true);
		CSDCommon::PlayAnimation(*rcInfo_1, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop,1, 0, 0, false, true);
		CSDCommon::PlayAnimation(*rcStageTitle, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,1, 0, 0, false, true);
		CSDCommon::PlayAnimation(*rcBg, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,1, 0, 0, false, true);
		CSDCommon::PlayAnimation(*rcArea_Tag, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,1, 0, 0, false, true);
		rcInfo_1->SetHideFlag(true);
		rcInfo_2->SetHideFlag(true);
		rcInfo_4->SetHideFlag(true);
		rcInfo_5->SetHideFlag(true);
		rcInfo_6->SetHideFlag(true);
		rcStageSS->SetHideFlag(true);
		rcActNumber->SetHideFlag(true);
		rcStatus_footer->SetHideFlag(true);
	}
	void Kill()
	{
		spGate->SendMessage(spGate->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spGate = nullptr;
	}
};
class HudGate
{
public:
	static void Install();
	static void CreateScreen(Sonic::CGameObject* pParentGameObject);
	static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
	static void KillScreen();
	static void PlayAnimation(Chao::CSD::CScene* pScene, const char* name, Chao::CSD::EMotionRepeatType repeatType, float motionSpeed, float startFrame, float endFrame = 0.0f, bool reEnable = false, bool reverse = false);
	static void FreezeMotion(Chao::CSD::CScene* pScene, float frame = -1);
	static void StopMotion(Chao::CSD::CScene* pScene, bool end);
};