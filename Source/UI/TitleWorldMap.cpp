//	TODO:
//	Make sun gameobject to spawn the particle that Qution made
//	Add proper white-world support
//  Make all things related to title custom renderables instead of title renderables

#include "TitleWorldMap.h"

#include <Hedgehog/Universe/Engine/hhStateMachineBase.h>

#include "../System/MiniAudioHelper.h"
#include "../System/UpdateDirector.h"
using namespace hh::math;


namespace SUC::UI::TitleScreen
{
	/// ADD TO PARAMETERS
	constexpr float m_MagneticSpeed = 2.5f; //2.5f
	float multiplierRotationLight = 0.1f;
	constexpr float rotationPitchRate = 1.25f;
	constexpr float rotationYawRate = 1.25f;
	constexpr float deadzone = 0.2f;
	constexpr float dotThreshold = 0.95f;

	/// TEMPORARY - MOVE TO CLASSES ONCE CAMERA IS ADDED
	static float timePan;
	float camHeight = -20;
	float cameraDistance = 7.0f;
	float s_RotationAngle;
	float s_RotationAngleSpace;
	boost::shared_ptr<Sonic::CLightManager> light;
	class CWorldCountry;
	
	
	
	///This was prob one of the hardest things ive had to figure out
///ParticleManager is responsible for all Glitter functionality
///It is only spawned in the Act gameplayflows and thus in the titlescreen it didnt exist
///This spawns it.
	


	void CWorldCountry::Update(const hh::math::CVector& in_LightPos, const hh::math::CQuaternion& in_WorldRotation,
		const Sonic::CCamera* in_Cam)
	{
		m_Rotation = in_WorldRotation;
		auto m_RotatedPos = m_Rotation * m_Position;
		visibility = fmax(
			0.0f, -(in_Cam->m_MyCamera.m_Direction.dot(
				(m_RotatedPos - SUC::UI::TitleScreen::TitleWorldMap::s_PivotPosition).normalized()))) * 100;
		//CHECK FOR REMOVAL (missing offset aspect)
		auto pos = Utility::WorldToUIPosition(in_WorldRotation * m_Position, in_Cam, Hedgehog::Math::CVector2(0, 0));
		m_ScreenPosition = hh::math::CVector2(pos.x(), pos.y());
		float shadeAmount = fmax(
				0.0f, -(in_LightPos.
					dot((m_RotatedPos).normalized()))) *
			100;
		bool isDark = shadeAmount > 50;
		if (m_IsChangingDaylight)
		{
			if (m_rcDaylightIndicator->m_MotionFrame == 0 
				|| m_rcDaylightIndicator->m_MotionDisableFlag)
				m_IsChangingDaylight = false;
		}
		//			&& 
		//			)
		//			m_Flag->m_IsChangingDaylight = false;


		m_IsInShade = !isDark;
			
		if (!m_IsChangingDaylight)
		{
			if (m_IsInShade == m_LastState)
			{

				CSDCommon::PlayAnimation(m_rcDaylightIndicator, "Fade_Anim",
				                         Chao::CSD::eMotionRepeatType_PlayOnce, 0, visibility, visibility);
			}

			if (m_IsInShade != m_LastState)
			{
				m_IsChangingDaylight = true;
				CSDCommon::PlayAnimation(m_rcDaylightIndicator, "Switch_Anim",
				                         Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0, 0, !m_IsInShade, !m_IsInShade);
			}
		}
		CSDCommon::PlayAnimation(m_rcFlag, "Fade_Anim",
		                         Chao::CSD::eMotionRepeatType_PlayOnce, 0, visibility, visibility);

		m_rcFlag->SetPosition(m_ScreenPosition.x(), m_ScreenPosition.y());
		m_rcDaylightIndicator->SetPosition(m_ScreenPosition.x() + 36, m_ScreenPosition.y() - 23);

		DebugDrawText::log(SUC::Format("[FLAG]\n LAST: %s | CURRENT: %s", m_LastState ? "true" : "false", m_IsInShade ? "true" : "false"), 0);
		if (m_IsSelected)
		{
			if (m_IsInShade && !m_LastState)
				Common::PlaySoundStaticCueName(m_SoundHandle, "sys_worldmap_sunset");
			else if (!m_IsInShade && m_LastState)
				Common::PlaySoundStaticCueName(m_SoundHandle, "sys_worldmap_sunrise");
		}
		m_LastState = m_IsInShade;

			
		
	}

	hh::math::CVector CWorldCountry::GetPositionAdjusted() const
	{
		return m_Rotation * m_Position;
	}


	void CSUCTitleCompanion::CWorldMapCanvas::ShowTextAct(bool visible)
	{
		for (size_t i = 0; i <= WM_STAGELIST_MAXLISTVISIBLE; i++)
		{
			rcTextElement[i]->SetHideFlag(!visible);
		}
	}

	void CSUCTitleCompanion::CWorldMapCanvas::Configure(const boost::shared_ptr<Sonic::CCsdProject>& in_CsdProj)
	{
		spCsdProject = in_CsdProj;
		rcWorldMap = spCsdProject->m_rcProject;

		//auto e = DecorationTextManager::InitializeText("const char* text", 1, 0, Hedgehog::math::CVector(0, 0, 0));
		rcInfoBg1 = rcWorldMap->CreateScene("info_bg_1");
		rcInfoImg1 = rcWorldMap->CreateScene("info_img_1");
		rcInfoImg2 = rcWorldMap->CreateScene("info_img_2");
		rcInfoImg3 = rcWorldMap->CreateScene("info_img_3");
		rcInfoImg4 = rcWorldMap->CreateScene("info_img_4");

		rcHeaderBgW = rcWorldMap->CreateScene("worldmap_header_bg");
		rcFooterBgW = rcWorldMap->CreateScene("worldmap_footer_bg");
		rcFooterImgW = rcWorldMap->CreateScene("worldmap_footer_img");
		rcHeaderImgW = rcWorldMap->CreateScene("worldmap_header_img");
		rcWorldmapFooterImgA = rcWorldMap->CreateScene("worldmap_footer_img_A");
		rcCtsGuideWindow = rcWorldMap->CreateScene("cts_guide_window");
		rcCtsGuideScreenshot = rcWorldMap->CreateScene("cts_guide_ss");
		rcCtsStageScreenshot = rcWorldMap->CreateScene("cts_guide_ss");
		rcCtsGuideText = rcWorldMap->CreateScene("cts_guide_txt");
		rcCtsName = rcWorldMap->CreateScene("cts_name");
		rcCtsCursorEffect = rcWorldMap->CreateScene("cts_cursor_effect");
		rcCtsGuide4Mission = rcWorldMap->CreateScene("cts_guide_4_misson");
		rcCtsGuide5Medal = rcWorldMap->CreateScene("cts_guide_5_medal");

		rcCtsGuide1Hiscore = rcWorldMap->CreateScene("cts_guide_1_hiscore");
		rcCtsGuide2BestTime = rcWorldMap->CreateScene("cts_guide_2_besttime");
		rcCtsGuide3Rank = rcWorldMap->CreateScene("cts_guide_3_rank");
		rcCtsStage4Mission = rcWorldMap->CreateScene("cts_guide_4_misson");
		rcCtsStage5Medal = rcWorldMap->CreateScene("cts_guide_5_medal");


		rcCtsStageWindow = rcWorldMap->CreateScene("cts_stage_window");
		rcCtsStageInfoWindow = rcWorldMap->CreateScene("cts_guide_window");
		rcCtsStageSelect = rcWorldMap->CreateScene("cts_stage_select");
		rcCtsName_2 = rcWorldMap->CreateScene("cts_name_2");
		rcStageSelectFlag = rcWorldMap->CreateScene("cts_stage_flag");

		rcCtsChoicesBg = rcWorldMap->CreateScene("cts_choices_bg");
		rcCtsChoicesWindow = rcWorldMap->CreateScene("cts_choices_window");
		rcCtsChoicesSelect = rcWorldMap->CreateScene("cts_choices_select");
		rcCursorLeft = rcWorldMap->CreateScene("cts_cursor");
		rcCursorTop = rcWorldMap->CreateScene("cts_cursor");
		rcCursorBottom = rcWorldMap->CreateScene("cts_cursor");
		rcCursorRight = rcWorldMap->CreateScene("cts_cursor");
		CSDCommon::PlayAnimation(rcInfoBg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcInfoImg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcInfoImg2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcInfoImg3, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcInfoImg4, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcHeaderBgW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcFooterBgW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcFooterBgW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcHeaderImgW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcWorldmapFooterImgA, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcCtsCursorEffect, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcCtsName, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);


		for (size_t i = 0; i <= WM_STAGELIST_MAXLISTVISIBLE; i++)
		{
			//Set text nodes to the exact positions they should be at according to the animation
			rcTextElement[i] = rcWorldMap->CreateScene("deco_text");
			CSDCommon::PlayAnimation(rcTextElement[i], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

			CSDCommon::PlayAnimation(rcCtsStageSelect, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, (i) * 10,
			                         i * 10);
			rcTextElement[i]->SetPosition(rcCtsStageSelect->GetNode("select_img")->GetPosition().x(),
			                              rcCtsStageSelect->GetNode("select_img")->GetPosition().y());

			//Set text
			char actname[6];
			sprintf(actname, "Act %d", i + 1);
			rcTextElement[i]->GetNode("Text_blue")->SetText(actname);
		}
		rcTextElement[7] = rcWorldMap->CreateScene("deco_text_centered");
		rcTextElement[8] = rcWorldMap->CreateScene("deco_text_centered");

		rcTextElement[7]->GetNode("Text_blue")->SetText("Go to capital");
		rcTextElement[8]->GetNode("Text_blue")->SetText("Select stage");

		rcTextElement[7]->SetPosition(System::AspectRatioHelper::ScreenHalfPoint->x(), System::AspectRatioHelper::ScreenHalfPoint->y() - 45);
		rcTextElement[8]->SetPosition(System::AspectRatioHelper::ScreenHalfPoint->x(), System::AspectRatioHelper::ScreenHalfPoint->y() + 25);
		ShowTextAct(false);
		rcTextElement[7]->SetHideFlag(true);
		rcTextElement[8]->SetHideFlag(true);
	}

	void CSUCTitleCompanion::CWorldMapCanvas::SetInactive(bool in_Hide)
	{
		rcInfoBg1->SetHideFlag(in_Hide);
		rcWorldmapFooterImgA->SetHideFlag(in_Hide);
		rcInfoImg1->SetHideFlag(in_Hide);
		rcInfoImg2->SetHideFlag(in_Hide);
		rcInfoImg3->SetHideFlag(in_Hide);
		rcInfoImg4->SetHideFlag(in_Hide);
		rcHeaderBgW->SetHideFlag(in_Hide);
		rcHeaderImgW->SetHideFlag(in_Hide);
		rcFooterBgW->SetHideFlag(in_Hide);
		if (rcFooterImgW) rcFooterImgW->SetHideFlag(in_Hide);
		rcCursorLeft->SetHideFlag(in_Hide);
		rcCursorTop->SetHideFlag(in_Hide);
		rcCursorBottom->SetHideFlag(in_Hide);
		rcCursorRight->SetHideFlag(in_Hide);
		rcCtsName->SetHideFlag(in_Hide);
		rcCtsCursorEffect->SetHideFlag(in_Hide);

		rcCtsGuideScreenshot->SetHideFlag(in_Hide);
		rcCtsGuideWindow->SetHideFlag(in_Hide);
		rcCtsGuideText->SetHideFlag(in_Hide);
	}

	void CSUCTitleCompanion::DestroyCSDScene(Chao::CSD::RCPtr<Chao::CSD::CScene>& in_CsdScene)
	{
		Chao::CSD::CProject::DestroyScene(UI->rcWorldMap.Get(), in_CsdScene);
	}

	void CSUCTitleCompanion::KillCallback()
	{
		using Chao::CSD::CProject;
		KillScreen();
		DestroyCSDScene(UI->rcInfoBg1);
		DestroyCSDScene(UI->rcInfoImg2);
		DestroyCSDScene(UI->rcInfoImg3);
		DestroyCSDScene(UI->rcInfoImg4);
		DestroyCSDScene(UI->rcHeaderBgW);
		DestroyCSDScene(UI->rcHeaderImgW);
		DestroyCSDScene(UI->rcFooterBgW);
		DestroyCSDScene(UI->rcFooterImgW);
		DestroyCSDScene(UI->rcWorldmapFooterImgA);
		DestroyCSDScene(UI->rcCursorLeft);
		DestroyCSDScene(UI->rcCursorTop);
		DestroyCSDScene(UI->rcCursorBottom);
		DestroyCSDScene(UI->rcCursorRight);
		DestroyCSDScene(UI->rcCtsName);
		DestroyCSDScene(UI->rcCtsCursorEffect);
		DestroyCSDScene(UI->rcCtsGuideScreenshot);
		DestroyCSDScene(UI->rcCtsStageScreenshot);
		DestroyCSDScene(UI->rcCtsGuideWindow);
		DestroyCSDScene(UI->rcCtsGuideText);
		DestroyCSDScene(UI->rcTextWorldDescription);
		DestroyCSDScene(UI->rcCtsGuide4Mission);
		DestroyCSDScene(UI->rcCtsGuide5Medal);
		DestroyCSDScene(UI->rcCtsStageWindow);
		DestroyCSDScene(UI->rcCtsStageSelect);
		DestroyCSDScene(UI->rcCtsStageInfoWindow);
		DestroyCSDScene(UI->rcCtsGuide1Hiscore);
		DestroyCSDScene(UI->rcCtsGuide2BestTime);
		DestroyCSDScene(UI->rcCtsGuide3Rank);
		DestroyCSDScene(UI->rcCtsStage4Mission);
		DestroyCSDScene(UI->rcCtsStage5Medal);
		DestroyCSDScene(UI->rcCtsChoicesBg);
		DestroyCSDScene(UI->rcCtsChoicesWindow);
		DestroyCSDScene(UI->rcCtsChoicesSelect);

		for (size_t i = 0; i < m_Country.size(); i++)
		{
			Chao::CSD::CProject::DestroyScene(UI->rcWorldMap.Get(), m_Country[i]->m_rcFlag);
			Chao::CSD::CProject::DestroyScene(UI->rcWorldMap.Get(), m_Country[i]->m_rcDaylightIndicator);
		}
		for (size_t i = 0; i < 9; i++) Chao::CSD::CProject::DestroyScene(UI->rcWorldMap.Get(), UI->rcTextElement[i]);

		UI->rcWorldMap = nullptr;

		timePan = 0;
		camHeight = -20;
		introPlayed = false;
		TitleWorldMap::s_WorldmapCursorDisabled = true;
		//rotationPitch = -0.4f;
		//FOV = 0.84906584f;
		cameraDistance = 5.0f;
		//rotationYaw = 0.55f;
		TitleWorldMap::s_TargetDisabled = true;
		cursorSelected = false;
		TitleWorldMap::s_StageSelectWindowSelection = 0;
		isStageWindowOpen = false;
		timeStageSelectDelay = 0;
		lastflagSelectionAmount = 0;
	}

	void CSUCTitleCompanion::ShowSaveIcon()
	{
		FUNCTION_PTR(void, __thiscall, FSave, 0xD22A70,
		             boost::shared_ptr<SaveLoadTestStruct>&spSave, void* a2);
		FSave(m_spSave, nullptr);
	}

	void CSUCTitleCompanion::AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,
		Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
	{
		UI = new CWorldMapCanvas();
		Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
		//pGameDocument->AddUpdateUnit("0", this);
		//pGameDocument->AddUpdateUnit("1", this);
		SetContextBase(this);
		if (Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spUpdateManager)
			Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spUpdateManager->AddUpdateUnit("1", this);
	}

	void CSUCTitleCompanion::UpdateSerial(const Hedgehog::Universe::SUpdateInfo& in_rUpdateInfo)
	{
		this->UpdateStateMachine(in_rUpdateInfo);
		DebugDrawText::log("Hallo!", 0);
	}

	void CSUCTitleCompanion::SetUIVisibility(bool in_Hide)
	{
		UI->SetInactive(in_Hide);
		for (const auto& m_ContinentUI : m_Country)
		{
			m_ContinentUI->m_rcFlag->SetHideFlag(in_Hide);
			m_ContinentUI->m_rcDaylightIndicator->SetHideFlag(in_Hide);
		}
	}

	void CSUCTitleCompanion::PlayCursorAnim(const char* name, bool reverse)
	{
		float reversePoint = reverse ? 80 : 0;
		CSDCommon::PlayAnimation(UI->rcCursorTop, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, reversePoint, false,
		                         reverse);
		CSDCommon::PlayAnimation(UI->rcCursorBottom, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, reversePoint, false,
		                         reverse);
		CSDCommon::PlayAnimation(UI->rcCursorLeft, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, reversePoint, false,
		                         reverse);
		CSDCommon::PlayAnimation(UI->rcCursorRight, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, reversePoint, false,
		                         reverse);
	}

	void CSUCTitleCompanion::CheckCursorAnimStatus()
	{
		if (UI->rcCtsGuideWindow)
		{
			if (UI->rcCtsGuideWindow->m_MotionFrame >= 85 && UI->rcCtsGuideWindow->m_MotionFrame <= 90)
			{
				UI->rcCtsGuide4Mission->SetHideFlag(false);
				UI->rcCtsGuide5Medal->SetHideFlag(false);
			}
		}
	}

	void CSUCTitleCompanion::SetCursorPos(const CVector2& pos)
	{
		if (!UI->rcCursorLeft || !TitleWorldMap::s_IsActive)
			return;
		if (((pos.x() + pos.y()) != 0) && !playingPointerMove)
		{
			Common::PlaySoundStaticCueName(cursorMoveHandle, "sys_worldmap_pointermove");
			//Common::MiniAudioHelper::playSoundStatic(cursorMoveHandle, 10);
			playingPointerMove = true;
		}
		else if (((pos.x() + pos.y()) == 0) && playingPointerMove)
		{
			playingPointerMove = false;
			cursorMoveHandle.reset();
			//cursorMoveHandle.reset();
		}
		posCursorCenter = pos + *System::AspectRatioHelper::ScreenHalfPoint;
		float sizeBox = 125;
		auto posL = CVector2(pos.x() - (sizeBox / 2), pos.y());
		auto posR = CVector2(pos.x() + sizeBox / 2, pos.y());
		auto posT = CVector2(pos.x(), pos.y() + sizeBox / 2);
		auto posB = CVector2(pos.x(), pos.y() - (sizeBox / 2));

		UI->rcCursorLeft->SetPosition(posL.x() + System::AspectRatioHelper::ScreenHalfPoint->x(),
		                              posL.y() + System::AspectRatioHelper::ScreenHalfPoint->y());
		UI->rcCursorRight->SetPosition(posR.x() + System::AspectRatioHelper::ScreenHalfPoint->x(),
		                               posR.y() + System::AspectRatioHelper::ScreenHalfPoint->y());
		UI->rcCursorTop->SetPosition(posT.x() + System::AspectRatioHelper::ScreenHalfPoint->x(),
		                             posT.y() + System::AspectRatioHelper::ScreenHalfPoint->y());
		UI->rcCursorBottom->SetPosition(posB.x() + System::AspectRatioHelper::ScreenHalfPoint->x(),
		                                posB.y() + System::AspectRatioHelper::ScreenHalfPoint->y());
		UI->rcCursorRight->GetNode("arrow_position_9")->SetRotation(180);
		UI->rcCursorTop->GetNode("arrow_position_9")->SetRotation(90);
		UI->rcCursorBottom->GetNode("arrow_position_9")->SetRotation(-90);
	}

	bool CSUCTitleCompanion::IsInsideCursorRange(const CVector2& input, float visibility, int flagID)
	{
		if (!UI->rcCursorLeft || TitleWorldMap::s_WorldmapCursorDisabled ||
			TitleWorldMapPause::s_IsPaused)
			return false;
		bool result = false;
		float range = 100;

		// Check if the point p is on the same side of each edge as the other three corners
		if (abs(Common::GetVector2Distance(input, posCursorCenter)) <= 50)
		{
			if (visibility >= 80 && flagID != lastFlagIndex)
			{
				lastFlagIndex = flagID;
				//for eventual night versions, add 9
				UI->rcCtsGuideScreenshot->GetNode("town_ss_img")->SetPatternIndex(flagID);
				//rcCtsGuideText->GetNode("text_size")->SetPatternIndex(flagID);
				UI->rcCtsName->GetNode("img")->SetPatternIndex(flagID);

				result = true;
			}
		}
		else
		{
			lastFlagIndex = -1;
		}
		return result;
	}

	void CSUCTitleCompanion::SetLevelTextCast(const char* text)
	{
		auto input = std::string(text);
		std::stringstream ss(input);
		std::string line;
		std::vector<std::string> lines;
		while (getline(ss, line, '\n'))
		{
			lines.push_back(line);
		}
		if (lines.size() == 0)
			lines.push_back("");
		for (size_t i = 0; i < 6; i++)
		{
			char buff[32];
			sprintf(buff, "text_line_%d", i);
			if (i <= lines.size() - 1)
			{
				UI->rcCtsGuideText->GetNode(buff)->SetText(lines.at(i).c_str());
			}
			else
			{
				UI->rcCtsGuideText->GetNode(buff)->SetText(" ");
			}
		}
	}

	void CSUCTitleCompanion::PopulateStageSelect(int id, int offset)
	{
		if (SUC::Project::s_WorldData.data.size() < id)
		{
			printf("\n[WorldMap] Missing config for FlagID %d", id);
			return;
		}
		stageSelectedWindowMax = m_Country[id]->m_IsInShade && SUC::Project::s_WorldData.data[id].dataNight.size() != 0
			                         ? SUC::Project::s_WorldData.data[id].dataNight.size() - 1
			                         : SUC::Project::s_WorldData.data[id].data.size() - 1;
		//Common::ClampInt(stageSelectedWindowMax, 0, 6);
		for (size_t i = 0; i <= WM_STAGELIST_MAXLISTVISIBLE; i++)
		{
			UI->rcTextElement[i]->SetHideFlag(true);
		}

		for (size_t i = 0; i <= WM_STAGELIST_MAXLISTVISIBLE; i++)
		{
			if (stageSelectedWindowMax < i + offset)
				break;

			const char* optionName;
			if (m_Country[id]->m_IsInShade && SUC::Project::s_WorldData.data[id].dataNight.size() != 0)
				optionName = SUC::Project::s_WorldData.data[id].dataNight[i + offset].optionName.c_str();
			else
				optionName = SUC::Project::s_WorldData.data[id].data[i + offset].optionName.c_str();

			UI->rcTextElement[i]->SetHideFlag(false);
			UI->rcTextElement[i]->GetNode("Text_blue")->SetText(optionName);
		}
	}

	SaveStageInfo CSUCTitleCompanion::GetInfoForStage(std::string id)
	{
		auto it = std::find(SUC::Project::s_GenerationsStages.begin(), SUC::Project::s_GenerationsStages.end(), id);
		auto returnI = SaveStageInfo();
		returnI.stageID_string = id;
		//Return an empty Info struct if the stage isnt a native gens stage. This should be expanded to have custom stage slot save files in the future.
		if (it == SUC::Project::s_GenerationsStages.end())
			return returnI;

		returnI.stageIDForGens = std::distance(SUC::Project::s_GenerationsStages.begin(), it);
		returnI.isStageCompleted = Common::IsStageCompleted(returnI.stageIDForGens);
		Common::GetStageData(returnI.stageIDForGens, returnI.bestScore, returnI.bestTime, returnI.bestTime2,
		                     returnI.bestTime3, returnI.bestRank, returnI.bestRing, returnI.redRingCount);
		return returnI;
	}

	void CSUCTitleCompanion::PopulateCountryPreviewInfo(int flag)
	{
		int redRingMax = 5 * SUC::Project::s_WorldData.data[flag].data.size();
		int redRingCurrent = 0;
		int stageCompletedAmount = 0;
		for (size_t i = 0; i < SUC::Project::s_WorldData.data[flag].data.size(); i++)
		{
			auto e = GetInfoForStage(SUC::Project::s_WorldData.data[flag].data[i].levelID);
			redRingCurrent += e.redRingCount;
			stageCompletedAmount += e.isStageCompleted;
		}
		auto redRingsCurrent = new char[8];
		auto redRingsMax = new char[8];
		auto stageMax = new char[8];
		auto stageCount = new char[8];
		sprintf(redRingsCurrent, "%02d", redRingCurrent);
		sprintf(redRingsMax, "%02d", redRingMax);
		sprintf(stageMax, "%02d", SUC::Project::s_WorldData.data[flag].data.size());
		sprintf(stageCount, "%02d", stageCompletedAmount);

		UI->rcCtsGuide4Mission->GetNode("num_nume")->SetText(stageCount);
		UI->rcCtsGuide4Mission->GetNode("num_deno")->SetText(stageMax);

		UI->rcCtsGuide5Medal->GetNode("s_num_nume")->SetText(redRingsCurrent);
		UI->rcCtsGuide5Medal->GetNode("s_num_deno")->SetText(redRingsMax);

		UI->rcCtsGuide5Medal->GetNode("m_num_nume")->SetText(redRingsCurrent);
		UI->rcCtsGuide5Medal->GetNode("m_num_deno")->SetText(redRingsMax);
	}

	void CSUCTitleCompanion::PopulateStageWindowInfo(std::string id)
	{
		auto info = GetInfoForStage(id);

		// Time
		uint32_t minutes, seconds, milliseconds;
		uint32_t totalMilliseconds = info.bestTime * 1000.0f;
		minutes = totalMilliseconds / 60000;
		if (info.bestTime > 0.0f && minutes <= 99)
		{
			seconds = (totalMilliseconds % 60000) / 1000;
			milliseconds = (totalMilliseconds % 60000) % 1000;
		}
		else
		{
			minutes = 00;
			seconds = 00;
			milliseconds = 000;
		}
		auto scoreCount = new char[8];
		auto redCount = new char[2];
		auto bestTimeC = new char[16];
		sprintf(bestTimeC, "%02d:%02d:%02d", minutes, seconds, milliseconds);
		sprintf(scoreCount, "%08d", info.bestScore);
		sprintf(redCount, "%02d", info.redRingCount);

		UI->rcCtsGuide2BestTime->GetNode("num")->SetText(bestTimeC);
		UI->rcCtsGuide1Hiscore->GetNode("num")->SetText(scoreCount);
		UI->rcCtsGuide3Rank->GetNode("rank_shade")->SetPatternIndex(info.bestRank);
		UI->rcCtsGuide3Rank->GetNode("rank_img")->SetPatternIndex(info.bestRank);

		UI->rcCtsStage5Medal->GetNode("s_num_nume")->SetText(redCount);
		UI->rcCtsStage5Medal->GetNode("s_num_deno")->SetText("05");

		UI->rcCtsStage5Medal->GetNode("m_num_nume")->SetText(redCount);
		UI->rcCtsStage5Medal->GetNode("m_num_deno")->SetText("05");
	}

	void CSUCTitleCompanion::SetVisibilityPlayerInfo(bool visible)
	{
		UI->rcInfoBg1->SetHideFlag(!visible);
		UI->rcInfoImg1->SetHideFlag(!visible);
		UI->rcInfoImg2->SetHideFlag(!visible);
		UI->rcInfoImg3->SetHideFlag(!visible);
		UI->rcInfoImg4->SetHideFlag(!visible);
		UI->rcCtsGuide4Mission->SetHideFlag(!visible);
		UI->rcCtsGuide5Medal->SetHideFlag(!visible);
		CSDCommon::FreezeMotion(UI->rcCtsGuideScreenshot, 0);
		CSDCommon::FreezeMotion(UI->rcCtsGuideText, 0);
		CSDCommon::FreezeMotion(UI->rcCtsGuideWindow, 0);
		CSDCommon::PlayAnimation(UI->rcCtsGuideScreenshot, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
		                         !visible);
		CSDCommon::PlayAnimation(UI->rcCtsGuideText, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
		                         !visible);
		CSDCommon::PlayAnimation(UI->rcCtsGuideWindow, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
		                         !visible);
	}

	void CSUCTitleCompanion::SetVisibilityCapitalWindow(bool visible)
	{
		selectedCapital = 0;
		if (visible) //im only doing this so that it doesnt disable when disabling and plays anims instead
		{
			UI->rcCtsChoicesBg->SetHideFlag(!visible);
			UI->rcCtsChoicesWindow->SetHideFlag(!visible);
		}
		UI->rcCtsChoicesSelect->SetHideFlag(!visible);
		CSDCommon::PlayAnimation(UI->rcCtsChoicesBg, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
		                         !visible);
		CSDCommon::PlayAnimation(UI->rcCtsChoicesWindow, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
		                         !visible);

		UI->rcTextElement[7]->SetHideFlag(!visible);
		UI->rcTextElement[8]->SetHideFlag(!visible);

		CSDCommon::PlayAnimation(UI->rcTextElement[7], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(UI->rcTextElement[8], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(UI->rcCtsChoicesSelect, "Scroll_down_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 100);
	}

	void CSUCTitleCompanion::SetVisibilityStageWindow(bool visible)
	{
		CSDCommon::PlayAnimation(UI->rcCtsStageWindow, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
		                         !visible);
		CSDCommon::PlayAnimation(UI->rcCtsName_2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
		                         !visible);
		CSDCommon::PlayAnimation(UI->rcCtsStageSelect, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
		                         !visible);
		CSDCommon::PlayAnimation(UI->rcStageSelectFlag, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
		                         !visible);
		CSDCommon::PlayAnimation(UI->rcCtsStageScreenshot, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
		                         !visible);
		CSDCommon::PlayAnimation(UI->rcCtsStageInfoWindow, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0,
		                         true, !visible);
		CSDCommon::FreezeMotion(UI->rcCtsStageSelect, 0);

		PopulateStageSelect(TitleWorldMap::s_LastFlagSelected);
		UI->rcCtsStageSelect->SetHideFlag(!visible);
		UI->rcCtsStage5Medal->SetHideFlag(!visible);
		UI->rcCtsGuide3Rank->SetHideFlag(!visible);
		UI->rcCtsGuide1Hiscore->SetHideFlag(!visible);
		UI->rcCtsGuide2BestTime->SetHideFlag(!visible);
		if (visible) //im only doing this so that it doesnt disable when disabling and plays anims instead
		{
			CSDCommon::PlayAnimation(UI->rcCtsGuideText, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0);
			SetLevelTextCast("Head for the goal!");
			UI->rcCtsGuideText->SetPosition(0, 40);
			UI->rcCtsStageWindow->SetHideFlag(false);
			UI->rcCtsStageScreenshot->SetHideFlag(false);
			UI->rcCtsName_2->SetHideFlag(false);
			UI->rcStageSelectFlag->SetHideFlag(false);
			UI->rcCtsStageInfoWindow->SetHideFlag(false);
		}
		else

			UI->rcCtsGuideText->SetPosition(140, 40);
	}

	void CSUCTitleCompanion::CreateScreen()
	{
		if (UI->rcWorldMap && !UI->spWorldMap)
		{
			this->m_pMember->m_pGameDocument->AddGameObject(
				UI->spWorldMap = boost::make_shared<Sonic::CGameObjectCSD>(UI->rcWorldMap, 0.5f, "HUD", false), "main",
				this);
		}
	}

	void CSUCTitleCompanion::KillScreen()
	{
		if (UI->spWorldMap)
		{
			UI->spWorldMap->SendMessage(UI->spWorldMap->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
			UI->spWorldMap = nullptr;
		}
	}

	void CSUCTitleCompanion::ToggleScreen(const bool visible)
	{
		if (visible)
			CreateScreen();
		else
			KillScreen();
	}

	void CSUCTitleCompanion::CapitalWindow_Update()
	{
		if (timeStageSelectDelay < 5)
			timeStageSelectDelay += 1;
		auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->
			m_CurrentPadStateIndex];
		//CHECK FOR REMOVAL (fix unnecessary repeat
		//Selection increase
		if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickDown) && selectedCapital == 0)
		{
			selectedCapital += 1;

			Common::PlaySoundStaticCueName(stageSelectHandle, "sys_worldmap_cursor");
			CSDCommon::PlayAnimation(UI->rcCtsChoicesSelect, "Scroll_up_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		}
		//Selection decrease
		if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickUp) && selectedCapital == 1)
		{
			selectedCapital -= 1;
			Common::PlaySoundStaticCueName(stageSelectHandle, "sys_worldmap_cursor");
			CSDCommon::PlayAnimation(UI->rcCtsChoicesSelect, "Scroll_down_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		}
		Common::ClampInt(selectedCapital, 0, 1);

		if (inputPtr->IsTapped(Sonic::eKeyState_A) && timeStageSelectDelay >= 5)
		{
			if (selectedCapital == 0)
			{
				Common::PlaySoundStaticCueName(stageSelectHandle, "sys_worldmap_finaldecide");
				System::StageManager::s_HubModeEnabled = SUC::Project::s_WorldData.data[TitleWorldMap::s_LastFlagSelected].data[
					SUC::Project::GetCapital(TitleWorldMap::s_LastFlagSelected,
					                         m_Country[TitleWorldMap::s_LastFlagSelected]->m_IsInShade)].isWhiteWorld;

				/*if (StageManager::WhiteWorldEnabled)
					{
						WRITE_STRING(0x015B499C, GetStageToLoad());
						WRITE_STRING(0x015C789C, GetStageToLoad());
						WRITE_STRING(0x015C8148, GetStageToLoad());
						WRITE_STRING(0x0169A304, GetStageToLoad());

					}*/
				Title::ShowLoadingTransition(true);
			}
			else
			{
				Common::PlaySoundStaticCueName(stageSelectHandle, "sys_worldmap_decide");
				SetVisibilityStageWindow(true);
				SetVisibilityCapitalWindow(false);
				//ShowTextAct(true);
				TitleWorldMap::s_IsCapitalWindowOpened = false;
				timeStageSelectDelay = 0;
			}
		}
	}

	void CSUCTitleCompanion::SetStageSelectionScreenshot()
	{
		/// 
			/// Rewrite all of this and make it use the new blueblur image replace stuff
			/// 
		if (!UI->rcCtsStageScreenshot)
			return;
		char nameCast[16];
		for (size_t i = 0; i < 9; i++)
		{
			sprintf(nameCast, "ss_%02dd", i + 1);
			UI->rcCtsStageScreenshot->GetNode(nameCast)->SetHideFlag(true);
		}
		sprintf(nameCast, "ss_%02dd", TitleWorldMap::s_LastFlagSelected + 1);

		UI->rcCtsStageScreenshot->GetNode(nameCast)->SetHideFlag(false);
	}

	void CSUCTitleCompanion::StageWindow_Update(Sonic::CGameObject* This)
	{
		auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->
			m_CurrentPadStateIndex];
		//Pressing A requires a delay because otherwise even just tapping it will press A twice when this is called and it'll launch the stage immediately
		if (timeStageSelectDelay < 5)
			timeStageSelectDelay += 1;
		//Enables loading to the stage
		if (inputPtr->IsTapped(Sonic::eKeyState_A) && timeStageSelectDelay >= 5)
		{
			Common::PlaySoundStaticCueName(stageSelectHandle, "sys_worldmap_finaldecide");
			System::StageManager::s_HubModeEnabled = SUC::Project::s_WorldData.data[TitleWorldMap::s_LastFlagSelected].data[
				TitleWorldMap::s_StageSelectWindowSelection].isWhiteWorld;

			Title::ShowLoadingTransition(true);
		}
		DebugDrawText::log(std::format("SELINDEX: {0}\nSTAGESEL: {1}", m_StageListSelection,
		                               TitleWorldMap::s_StageSelectWindowSelection).c_str(), 0);
		//Selection decrease
		if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickUp) && TitleWorldMap::s_StageSelectWindowSelection > 0)
		{
			if (m_StageListSelection - 1 < 7 && m_StageListSelection - 1 >= 0)
				m_StageListSelection--;
			else
			{
				if (m_StageListOverflow > 0)
					m_StageListOverflow--;
			}
			if (TitleWorldMap::s_StageSelectWindowSelection >= 0)
				TitleWorldMap::s_StageSelectWindowSelection--;
			PopulateStageSelect(TitleWorldMap::s_LastFlagSelected, m_StageListOverflow);

			Common::PlaySoundStaticCueName(stageSelectHandle, "sys_worldmap_cursor");
			CSDCommon::PlayAnimation(UI->rcCtsStageSelect, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,
			                         130 - ((m_StageListSelection + 1) * 10), 130 - (m_StageListSelection) * 10);
		}

		//Selection increase
		if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickDown) && TitleWorldMap::s_StageSelectWindowSelection !=
			stageSelectedWindowMax)
		{
			bool doAnim = true;
			if (m_StageListSelection != stageSelectedWindowMax)
			{
				if (m_StageListSelection <= 5)
				{
					m_StageListSelection++;
				}
				else
				{
					doAnim = false;
					if (m_StageListOverflow < 6)
						m_StageListOverflow++;
				}
			}

			if (TitleWorldMap::s_StageSelectWindowSelection <= stageSelectedWindowMax)
				TitleWorldMap::s_StageSelectWindowSelection++;
			PopulateStageSelect(TitleWorldMap::s_LastFlagSelected, m_StageListOverflow);
			Common::PlaySoundStaticCueName(stageSelectHandle, "sys_worldmap_cursor");
			if (TitleWorldMap::s_StageSelectWindowSelection > 0 && doAnim) //technically not needed
				CSDCommon::PlayAnimation(UI->rcCtsStageSelect, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,
				                         (m_StageListSelection - 1) * 10, m_StageListSelection * 10);
		}

		SetStageSelectionScreenshot();
		Common::ClampInt(TitleWorldMap::s_StageSelectWindowSelection, 0, stageSelectedWindowMax);
		PopulateStageWindowInfo(
			SUC::Project::s_WorldData.data[TitleWorldMap::s_LastFlagSelected].data[TitleWorldMap::s_StageSelectWindowSelection].levelID);
	}

	class CSUCTitleState : public Hedgehog::Universe::CStateMachineBase::CStateBase
	{
	public:
		virtual void Start(){};
		virtual void Update(){};
		virtual void OnDestroy(){};
		CSUCTitleCompanion* GetBase() const
		{
			return (CSUCTitleCompanion*)GetContextBase();
		}
		void EnterState() override
		{
			DebugDrawText::log(SUC::Format("Starting State %s", m_Name));
			Start();
		}
		void UpdateState() override
		{
			Update();
		}
		void LeaveState()override
		{
			DebugDrawText::log(SUC::Format("Leaving State %s", m_Name));
			OnDestroy();
		}
	};
	hh::math::CVector2 CalculateAspectOffsets()
	{
		if (*(size_t*)0x6F23C6 != 0x75D8C0D9) // Widescreen Support
		{
			const float aspect = static_cast<float>(*(size_t*)0x1DFDDDC) / static_cast<float>(*(size_t*)0x1DFDDE0);

			
			if (aspect * 9.0f > 16.0f)
			{
				return CVector2(720.0f * aspect - 1280.0f, 0.0f);
			}
			else
			{
				return CVector2(0, 1280.0f / aspect - 720.0f);
			}
		}
		else
		{
			return CVector2(0, 0);
		}
	}
	hh::math::CVector2 CalculateAspectResolution()
	{
		return CVector2(static_cast<float>(*(size_t*)0x01804F8C), static_cast<float>(*(size_t*)0x01804F90));
	}
	class CTitleStateInitialize : public CSUCTitleState
	{
	public:
		BB_STATE_NAME("Initialize");
		void SetupWorldmap()
		{
			const auto& m_Title = GetBase();
			CalculateAspectOffsets();
			auto lightColor = (Eigen::Vector3f*)0x01A42308;
			lightColor->x() = 0.5976471f;
			lightColor->y() = 0.5835295f;
			lightColor->z() = 0.5364707f;
			if (m_Title->light == nullptr)
				m_Title->light = Sonic::CGameDocument::GetInstance()->m_pMember->m_spLightManager;
			m_Title->light->m_GlobalLightDirection = CVector(1, 1, 1);

			//Set light properties
			m_Title->light->m_GlobalLightDiffuse = CVector(0.02f, 0.02f, 0.02f);
			//light->m_GlobalLightDirection = lightPosition;
			m_Title->light->m_GlobalLightSpecular = CVector(0.02f, 0.02f, 0.02f);
			Sonic::CCsdDatabaseWrapper wrapper(m_Title->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
			boost::shared_ptr<Sonic::CCsdProject> spCsdProject = wrapper.GetCsdProject("ui_worldmap");

			m_Title->UI->Configure(spCsdProject);


			constexpr float earthRadius = 5.25f; //Used to normalize flag positions to the globe's curvature
			//By default the cursor in the worldmap is set to the left anchor 
			
			TitleWorldMap::s_IsWorldMapCameraInitialized = false;
			m_Title->m_Country.push_back(new CWorldCountry(Util::Spherize(CVector(0.31f, 0.36f, 2.28f), earthRadius)));
			m_Title->m_Country.push_back(new CWorldCountry(Util::Spherize(CVector(2.310000f, 2.360000f, 1.111371f),		earthRadius)));
			m_Title->m_Country.push_back(new CWorldCountry(Util::Spherize(CVector(2.810000f, -0.140000f, -6.649425f),	earthRadius)));
			m_Title->m_Country.push_back(new CWorldCountry(Util::Spherize(CVector(2.810000f, -1.890000f, 1.742745f),		earthRadius)));
			m_Title->m_Country.push_back(new CWorldCountry(Util::Spherize(CVector(-0.190000f, 4.610000f, -3.543527f),	earthRadius)));
			m_Title->m_Country.push_back(new CWorldCountry(Util::Spherize(CVector(-5.190000f, 0.110000f, -3.363136f),	earthRadius)));
			m_Title->m_Country.push_back(new CWorldCountry(Util::Spherize(CVector(0.060000f, -2.639999f, -6.829812f),	earthRadius)));
			m_Title->m_Country.push_back(new CWorldCountry(Util::Spherize(CVector(-4.440000f, -2.390000f, -0.798426f),	earthRadius)));
			m_Title->m_Country.push_back(new CWorldCountry(Util::Spherize(CVector(-3.600000f, 3.00000f, -1.160f),		earthRadius)));

			int continentIndex = 0;
			for (auto& m_Continent : m_Title->m_Country)
			{
				m_Continent->m_rcFlag = m_Title->UI->rcWorldMap->CreateScene("cts_parts_flag");
				m_Continent->m_rcDaylightIndicator = m_Title->UI->rcWorldMap->CreateScene("cts_parts_sun_moon");

				m_Continent->m_rcFlag->GetNode("img")->SetPatternIndex(continentIndex);
				CSDCommon::PlayAnimation(m_Continent->m_rcFlag, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				auto cameraPos = CVector(0, camHeight, cameraDistance);
				float shadeAmount = fmax(
					0.0f, -(m_Title->lightPosition.dot((m_Continent->m_Position - TitleWorldMap::s_PivotPosition).normalized())))
					* 100;
				float visibility = fmax(
					0.0f, -(cameraPos.dot((m_Continent->m_Position - TitleWorldMap::s_PivotPosition).normalized()))) * 100;

				CSDCommon::PlayAnimation(m_Continent->m_rcDaylightIndicator, "Switch_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,
					0, (shadeAmount > 50) ? 0 : 100); //Set image to either sun or moon
				CSDCommon::PlayAnimation(m_Continent->m_rcDaylightIndicator, "Fade_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,
					0, visibility, visibility);
				m_Continent->m_IsInShade = shadeAmount > 50;
				continentIndex++;
			}
			m_Title->CreateScreen();
			for (auto e : m_Title->m_Country) e->m_rcFlag->SetHideFlag(true);

			m_Title->UI->SetInactive(true);
			//CHECK FOR REMOVAL
			m_Title->UI->rcCtsGuide4Mission->SetHideFlag(true);
			m_Title->UI->rcCtsGuide5Medal->SetHideFlag(true);
			m_Title->UI->rcCtsStageWindow->SetHideFlag(true);
			m_Title->UI->rcCtsStageSelect->SetHideFlag(true);
			m_Title->UI->rcCtsName_2->SetHideFlag(true);
			m_Title->UI->rcCtsCursorEffect->SetHideFlag(true);
			m_Title->UI->rcStageSelectFlag->SetHideFlag(true);
			m_Title->UI->rcCtsStageInfoWindow->SetHideFlag(true);
			m_Title->UI->rcCtsStage4Mission->SetHideFlag(true);
			m_Title->UI->rcCtsStage5Medal->SetHideFlag(true);
			m_Title->UI->rcCtsGuide1Hiscore->SetHideFlag(true);
			m_Title->UI->rcCtsGuide2BestTime->SetHideFlag(true);
			m_Title->UI->rcCtsGuide3Rank->SetHideFlag(true);
			m_Title->UI->rcCtsChoicesBg->SetHideFlag(true);
			m_Title->UI->rcCtsChoicesWindow->SetHideFlag(true);
			m_Title->UI->rcCtsChoicesSelect->SetHideFlag(true);
			m_Title->UI->rcCtsStageScreenshot->SetHideFlag(true);

			float sceneFixPosX = 140;
			//Set up scenes to make it work in 16:9, because for some reason unleashed does it in code
			m_Title->UI->rcHeaderImgW->SetPosition(-sceneFixPosX, 0);
			m_Title->UI->rcInfoBg1->SetPosition(-sceneFixPosX, 0);
			m_Title->UI->rcInfoImg1->SetPosition(-sceneFixPosX, 0);
			m_Title->UI->rcInfoImg2->SetPosition(-sceneFixPosX, 0);
			m_Title->UI->rcInfoImg3->SetPosition(-sceneFixPosX, 0);
			m_Title->UI->rcInfoImg4->SetPosition(-sceneFixPosX, 0);
			m_Title->UI->rcCtsGuideWindow->SetPosition(sceneFixPosX, 0);
			m_Title->UI->rcCtsGuideScreenshot->SetPosition(sceneFixPosX, 0);
			m_Title->UI->rcCtsGuideText->SetPosition(sceneFixPosX, 40);
			m_Title->UI->rcCtsGuide4Mission->SetPosition(sceneFixPosX, 0);
			m_Title->UI->rcCtsGuide5Medal->SetPosition(sceneFixPosX, 0);

			m_Title->m_spSkySpace = boost::make_shared<CTitleWorldMapSky>(m_Title->lightPosition);
			m_Title->m_spSun = boost::make_shared<CTitleWorldMapSun>();
			// TitleWorldMap::s_PivotPosition
			m_Title->m_GlobeModel = boost::make_shared<CTitleWorldMapGlobe>();
			Sonic::CGameDocument::GetInstance()->AddGameObject(m_Title->m_GlobeModel);
			Sonic::CGameDocument::GetInstance()->AddGameObject(m_Title->m_spSkySpace);
			Sonic::CGameDocument::GetInstance()->AddGameObject(m_Title->m_spSun);

		}

		void Start() override
		{

			SetupWorldmap();
			const auto& m_Title = GetBase();
			//Init Anims
			TitleWorldMap::s_IsActive = true;
			System::StageManager::s_LoadingReplacementEnabled = true;
			TitleWorldMap::s_ForceTitleFlow = false;
			m_Title->ShowSaveIcon();
			//Set lives text
			m_Title->UI->rcInfoImg1->GetNode("num")->SetText(Common::IntToString(Common::GetLivesCount(), "%02d"));
			
			m_Title->SetUIVisibility(false);
			m_Title->PlayCursorAnim("Intro_Anim");
			CSDCommon::PlayAnimation(m_Title->UI->rcInfoBg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(m_Title->UI->rcInfoImg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(m_Title->UI->rcInfoImg2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(m_Title->UI->rcInfoImg3, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(m_Title->UI->rcInfoImg4, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(m_Title->UI->rcHeaderBgW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60);
			//60 so it doesnt fade in from the previous header and footer of HudTitle
			CSDCommon::PlayAnimation(m_Title->UI->rcFooterBgW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60);
			//60 so it doesnt fade in from the previous header and footer of HudTitle
			CSDCommon::PlayAnimation(m_Title->UI->rcHeaderImgW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(m_Title->UI->rcCtsGuideScreenshot, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(m_Title->UI->rcCtsGuideText, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(m_Title->UI->rcCtsGuideWindow, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(m_Title->UI->rcWorldmapFooterImgA, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

			m_Title->PlayCursorAnim("Intro_Anim");
			for (size_t i = 0; i < 9; i++) CSDCommon::PlayAnimation(m_Title->m_Country[i]->m_rcFlag, "Intro_Anim",
				Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			m_Title->UI->SetInactive(true);
		}
		void Update() override
		{
			
		}
		void LeaveState()override
		{
			
		}
	};
	class CTitleStateWorldMap : public CSUCTitleState
	{
	public:

		BB_STATE_NAME("WorldMapUpdate");
		void Start() override
		{
			TitleWorldMap::s_WorldmapCursorDisabled = false;
			auto m_Title = GetBase();
			m_Title->UI->SetInactive(false);
		}
		void Update() override
		{
			auto m_Title = GetBase();
			if (m_Title->m_spSkySpace)
			{
				//m_spSkySpace->SetRotation(lightRotation);
				DebugDrawText::log(std::format("ROTATION0: {0}, {1}, {2}", m_Title->m_spSkySpace->m_spMatrixNodeTransform->m_Transform.m_Rotation.x(), m_Title->m_spSkySpace->m_spMatrixNodeTransform->m_Transform.m_Rotation.y(), m_Title->m_spSkySpace->m_spMatrixNodeTransform->m_Transform.m_Rotation.z()).c_str(), 0);
				//DebugDrawText::log(std::format("ROTATION1: {0}, {1}, {2}", m_spWorldmapObject->m_spModelButtonTransform->m_Transform.m_Rotation.x(), m_spWorldmapObject->m_spModelButtonTransform->m_Transform.m_Rotation.y(), m_spWorldmapObject->m_spModelButtonTransform->m_Transform.m_Rotation.z()).c_str(), 0);
			}
			auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->
				m_CurrentPadStateIndex];
			//Allow keyboard users to use the world map
			inputPtr->LeftStickHorizontal = inputPtr->IsDown(Sonic::eKeyState_DpadLeft) ? -1.0f : (inputPtr->IsDown(Sonic::eKeyState_DpadRight) ? 1.0f : inputPtr->LeftStickHorizontal);
			inputPtr->LeftStickVertical = inputPtr->IsDown(Sonic::eKeyState_DpadDown) ? -1.0f : (inputPtr->IsDown(Sonic::eKeyState_DpadUp) ? 1.0f : inputPtr->LeftStickVertical);

			if (TitleWorldMap::s_IsActive)
			{
				//Pretty much just "if titleworldmap exists"
				if (m_Title->m_Country[0]->m_rcFlag && m_Title->UI->rcCtsGuide4Mission && m_Title->UI->rcCtsGuide5Medal)
				{
					if (m_Title->UI->rcInfoImg2->m_MotionDisableFlag) CSDCommon::PlayAnimation(
						m_Title->UI->rcInfoImg2, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 0, 0);
					if (m_Title->UI->rcInfoImg3->m_MotionDisableFlag)CSDCommon::PlayAnimation(
						m_Title->UI->rcInfoImg3, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 0, 0);
					if (m_Title->UI->rcInfoImg4->m_MotionDisableFlag)CSDCommon::PlayAnimation(
						m_Title->UI->rcInfoImg4, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 0, 0);

					multiplierRotationLight = inputPtr->IsDown(Sonic::eKeyState_X) ? 10 : 0.005;
					if(inputPtr->IsDown(Sonic::eKeyState_X))
					{
						if(!m_Title->stageSelectHandle)
							Common::PlaySoundStaticCueName(m_Title->stageSelectHandle, "sys_worldmap_sun");
					}
					if(inputPtr->IsUp(Sonic::eKeyState_X))
					{
						if(m_Title->stageSelectHandle)
						m_Title->stageSelectHandle.reset();
					}
					if (!m_Title->isStageWindowOpen && !TitleWorldMapPause::s_IsPaused)
						m_Title->SetCursorPos(CVector2(inputPtr->LeftStickHorizontal * m_Title->cursorMultiplier,
							-inputPtr->LeftStickVertical * m_Title->cursorMultiplier));
					else
					{
						m_Title->cursorMoveHandle.reset();
					}
					if(!m_Title->isStageWindowOpen)
					m_Title->flagSelectionAmount = 0;
					if (TitleWorldMap::s_WorldMapCamera)
					{
						if (m_Title->m_Country[0]->m_rcFlag->m_MotionDisableFlag && !m_Title->introPlayed)
							m_Title->introPlayed = true;

						//Update all flags
						for (int i = 0; i < m_Title->m_Country.size(); ++i)
						{
							auto worldCountry = m_Title->m_Country[i];
							if (m_Title->introPlayed)
								worldCountry->Update(m_Title->light->m_GlobalLightDirection, m_Title->m_GlobeModel->m_Rotation, TitleWorldMap::s_WorldMapCamera);

							bool inrange = m_Title->IsInsideCursorRange(worldCountry->m_ScreenPosition, worldCountry->visibility, i);
							worldCountry->m_IsSelected = inrange;
							m_Title->currentFlagSelected = inrange ? i : -1;
							if (!m_Title->isStageWindowOpen)
							m_Title->flagSelectionAmount += inrange;
						}

						if (m_Title->flagSelectionAmount != m_Title->lastflagSelectionAmount && m_Title->currentFlagSelected == m_Title->lastFlagIndex)
						{
							if (m_Title->flagSelectionAmount > 0)
							{
								//Show country info window, etc
								m_Title->cursorSelected = true;
								CSDCommon::PlayAnimation(m_Title->UI->rcCtsName, "Intro_1_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,
									0);
								CSDCommon::PlayAnimation(m_Title->UI->rcCtsCursorEffect, "Select_Anim", Chao::CSD::eMotionRepeatType_Loop,
									1, 0);
								m_Title->PlayCursorAnim("Select_Anim");
								m_Title->UI->rcCtsGuide4Mission->SetHideFlag(false);
								m_Title->UI->rcCtsGuide5Medal->SetHideFlag(false);
								CSDCommon::FreezeMotion(m_Title->UI->rcCtsGuideScreenshot, 0);
								CSDCommon::FreezeMotion(m_Title->UI->rcCtsGuideText, 0);
								CSDCommon::FreezeMotion(m_Title->UI->rcCtsGuideWindow, 0);
								CSDCommon::PlayAnimation(m_Title->UI->rcCtsGuideScreenshot, "Intro_Anim",
									Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
								CSDCommon::PlayAnimation(m_Title->UI->rcCtsGuideText, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,
									1, 0);
								CSDCommon::PlayAnimation(m_Title->UI->rcCtsGuideWindow, "Intro_Anim_2",
									Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

								Common::PlaySoundStaticCueName(m_Title->cursorSelectHandle, "sys_worldmap_window");
								Common::PlaySoundStaticCueName(m_Title->stageSelectHandle, "sys_worldmap_selectflag");

								m_Title->PopulateCountryPreviewInfo(TitleWorldMap::s_LastFlagSelected);
								m_Title->SetLevelTextCast(
									SUC::Project::s_WorldData.data[TitleWorldMap::s_LastFlagSelected].description.c_str());
							}
							else
							{
								//Hide country info
								m_Title->cursorSelected = false;
								m_Title->PlayCursorAnim("Select_Anim", true);
								m_Title->UI->rcCtsGuide4Mission->SetHideFlag(true);
								m_Title->UI->rcCtsGuide5Medal->SetHideFlag(true);
								CSDCommon::FreezeMotion(m_Title->UI->rcCtsGuideScreenshot, 0);
								CSDCommon::FreezeMotion(m_Title->UI->rcCtsGuideText, 0);
								CSDCommon::FreezeMotion(m_Title->UI->rcCtsGuideWindow, 0);
								CSDCommon::PlayAnimation(m_Title->UI->rcCtsGuideScreenshot, "Intro_Anim",
									Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, true);
								CSDCommon::PlayAnimation(m_Title->UI->rcCtsGuideText, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,
									1, 0, 0, true, true);
								CSDCommon::PlayAnimation(m_Title->UI->rcCtsGuideWindow, "Intro_Anim_2",
									Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, true);
							}
						}
						if (m_Title->flagSelectionAmount > 0 && m_Title->introPlayed)
						{
							//Open Stage Selection
							if (inputPtr->IsTapped(Sonic::eKeyState_A) && !m_Title->isStageWindowOpen)
							{
								m_Title->isStageWindowOpen = true;
								TitleWorldMap::s_WorldmapCursorDisabled = true;
								TitleWorldMap::s_StageSelectWindowSelection = 0;
								m_Title->m_StageListSelection = 0;
								m_Title->PopulateStageSelect(TitleWorldMap::s_LastFlagSelected);
								m_Title->SetVisibilityPlayerInfo(false);
								Common::PlaySoundStaticCueName(m_Title->stageSelectHandle, "sys_worldmap_decide");
								m_Title->UI->rcCtsName_2->GetNode("img_1")->SetPatternIndex(TitleWorldMap::s_LastFlagSelected);
								m_Title->UI->rcStageSelectFlag->GetNode("img")->SetPatternIndex(TitleWorldMap::s_LastFlagSelected);

								if (SUC::Project::GetCapital(TitleWorldMap::s_LastFlagSelected,
									m_Title->m_Country[TitleWorldMap::s_LastFlagSelected]->m_IsInShade) != -1)
								{
									TitleWorldMap::s_IsCapitalWindowOpened = true;
									m_Title->UI->ShowTextAct(false);
									m_Title->SetVisibilityCapitalWindow(true);
								}
								else
								{
									m_Title->SetVisibilityStageWindow(true);
								}
							}
							if (inputPtr->IsTapped(Sonic::eKeyState_B) && m_Title->isStageWindowOpen)
							{
								m_Title->isStageWindowOpen = false;
								TitleWorldMap::s_WorldmapCursorDisabled = false;

								m_Title->SetVisibilityPlayerInfo(true);

								m_Title->timeStageSelectDelay = 0;
								Common::PlaySoundStaticCueName(m_Title->stageSelectHandle, "sys_worldmap_cansel");

								if (TitleWorldMap::s_IsCapitalWindowOpened)
									m_Title->SetVisibilityCapitalWindow(false);
								else
									m_Title->SetVisibilityStageWindow(false);
								m_Title->UI->ShowTextAct(false);
								TitleWorldMap::s_IsCapitalWindowOpened = false;
							}
							if (m_Title->isStageWindowOpen && !TitleWorldMap::s_IsCapitalWindowOpened)
							{
								m_Title->StageWindow_Update(m_Title);
							}
							if (TitleWorldMap::s_IsCapitalWindowOpened)
							{
								m_Title->CapitalWindow_Update();
							}
						}
						m_Title->UI->rcCtsName->SetHideFlag(!m_Title->cursorSelected);
						m_Title->UI->rcCtsCursorEffect->SetHideFlag(!m_Title->cursorSelected);
						m_Title->lastflagSelectionAmount = m_Title->flagSelectionAmount;
					}
					m_Title->CheckCursorAnimStatus();
				}
			}
			
		}
	};
	bool TitleWorldMap::s_ForceTitleFlow = false;
	bool TitleWorldMap::s_IsWorldMapCameraInitialized = false;
	const CVector TitleWorldMap::s_PivotPosition = CVector(0, 0, 0);
	const Sonic::CCamera* TitleWorldMap::s_WorldMapCamera;
	bool TitleWorldMap::s_WorldmapCursorDisabled = true;
	bool TitleWorldMap::s_TargetDisabled = true;
	bool TitleWorldMap::s_IsCapitalWindowOpened = false;
	int TitleWorldMap::s_LastFlagSelected = 0;
	int TitleWorldMap::s_StageSelectWindowSelection = 0;
	bool TitleWorldMap::s_IsActive = false;
	CVector2* offsetAspect;
	CVector2* offsetRes;	
	static float m_CameraPitch = 20.0f;
	static float FOV = 0.84906584f;
	static float m_CameraYaw = 0.0f;
	bool playingPan = false;
	void TitleWorldMap::Start()
	{
		m_spTitle->ChangeState("WorldMapUpdate");
	}
	void TitleWorldMap::PlayPanningAnim()
	{
		s_TargetDisabled = true;
		s_WorldmapCursorDisabled = true;
		playingPan = true;
	}
	void TitleWorldMap::EnableInput()
	{
		s_WorldmapCursorDisabled = false;
	}	
	HOOK(void, __fastcall, TitleWorldMap_CTitleMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3,
		void** a4)
	{
		originalTitleWorldMap_CTitleMain(This, Edx, a2, a3, a4);
		TitleWorldMap::m_spTitle = boost::make_shared<CSUCTitleCompanion>();
		Sonic::CGameDocument::GetInstance()->AddGameObject(TitleWorldMap::m_spTitle);
		TitleWorldMap::m_spTitle->RegisterStateFactory<CTitleStateInitialize>();
		TitleWorldMap::m_spTitle->RegisterStateFactory<CTitleStateWorldMap>();
		TitleWorldMap::m_spTitle->ChangeState<CTitleStateInitialize>();
	}
	HOOK(void*, __fastcall, TitleWorldMap_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx,
		float elapsedTime, uint8_t a3)
	{
		//for (auto mUpdateUnit : m_UpdateUnits)
		//{
		//	mUpdateUnit->UpdateParallel(Hedgehog::Universe::SUpdateInfo(elapsedTime, 0, "0"));
		//	mUpdateUnit->UpdateSerial(Hedgehog::Universe::SUpdateInfo(elapsedTime, 0, "0"));
		//}
		return originalTitleWorldMap_UpdateApplication(This, Edx, elapsedTime, a3);
	}
#pragma region BBExts/Ahremic Title Camera
	

	class TransitionTitleCamera : public Sonic::CGameObject3D
	{
	public:
		int m_Unk00;
		boost::shared_ptr<Sonic::CCamera> m_spCamera;
	};

	inline void __cdecl ApplyCameraStuff(TransitionTitleCamera* CameraImpl, Sonic::CCamera* camera)
	{
		static uint32_t func = 0x10FA1D0;
		__asm
		{
			mov edi, camera
			mov esi, CameraImpl
			call func
		}
	}

	

	

	float LerpEaseInOut(float start, float end, float time, bool inEnabled, bool outEnabled)
	{
		Common::ClampFloat(time, 0, 1);
		time = (time < 0.5f)
			? (inEnabled ? (2.0f * time * time) : time)
			: (outEnabled ? (-1.0f + (4.0f - 2.0f * time) * time) : time); // Ease in/out
		return start + (end - start) * time; // Interpolate and return result
	}

	void PlayPan(Sonic::CCamera* camera, const Hedgehog::Universe::SUpdateInfo& updateInfo)
	{
		if (timePan >= 2.5f)
		{
			playingPan = false;
			TitleWorldMap::s_TargetDisabled = false;
		}
		timePan += updateInfo.DeltaTime;
		camHeight = LerpEaseInOut(-20, 0, timePan / 2.15f, true, false);
		m_CameraPitch = LerpEaseInOut(-0.4f, -0.5f, timePan / 2.15f, true, true);
		cameraDistance = LerpEaseInOut(5.0f, 20.0f, timePan / 2.2f, true, true);
		//FOV = LerpEaseInOut(0.84906584f, 0.44906584f, timePan / 2.15f, false, true);
	}

	

	inline float lerpUnclampedf(const float a, const float b, const float t)
	{
		return a + (b - a) * t;
	}

	inline float lerpf(float a, float b, float t)
	{
		const float min = fmin(a, b);
		const float max = fmax(a, b);
		return fmin(min, fmax(max, lerpUnclampedf(a, b, t)));
	}

	///Lerp camera to position around the globe.
	void MagnetizeToFlag(const CVector& flagPosition, float deltaTime, float powerMultipler = 1)
	{
		if (playingPan || TitleWorldMap::s_TargetDisabled || !TitleWorldMap::s_IsActive)
			return;
		//Comments are from Ceramic
		constexpr float halfway = (180.0f * DEG_TO_RAD);
		// First, we need to convert our flag position to target radians.
		// Reminder: Yaw increases when rotating to the right, Pitch is negative facing down & positive facing up.
		// Pitch is easy. Get the angle in radians, then subtract by half max.
		const float rPitch = Util::VectorAngle(flagPosition, CVector(0, 1, 0)) - (90.0f * DEG_TO_RAD);
		// Yaw is tricky due to the wraparound, and because it's a planar rotation.
		// This means we actually need 0 -> 360, which involves a few steps.
		// We can't just do an angle check with forward. We need a PLANAR vector to compare with.
		// TODO: These math operations are getting expensive, so we need to really pre-compute the radians we want to use.
		const CVector planarPosition = CVector(flagPosition.x(), 0, flagPosition.z()).normalized();
		const float rInitialYaw = Util::VectorAngle(planarPosition, CVector(0, 0, 1));
		// Our angle is an unsigned angle from 0 -> 180. We need to convert that to 0 -> 360, and... well this is how you do that lol
		const float rCorrectedYaw = flagPosition.dot(CVector(-1, 0, 0)) > 0.0f
			? (halfway - rInitialYaw) + halfway
			: rInitialYaw;
		// Now, we want to do some bullshit to make the radian lerp-towards work.
		const bool isOverfill = fabs(m_CameraYaw - rCorrectedYaw) > halfway;
		const bool isFlagLeft = m_CameraYaw > halfway;
		float rYaw = rCorrectedYaw;
		if (isOverfill)
		{
			const float compensation = (isFlagLeft ? 360.0f : -360.0f) * DEG_TO_RAD;
			rYaw += compensation;
		}
		// Time to lerp!
		m_CameraPitch = lerpUnclampedf(m_CameraPitch, rPitch, deltaTime * (m_MagneticSpeed * powerMultipler));
		m_CameraYaw = lerpUnclampedf(m_CameraYaw, rYaw, deltaTime * (m_MagneticSpeed * powerMultipler));
	}

	
	HOOK(void, __fastcall, TitleWorldMap_CameraUpdate, 0x0058CDA0, TransitionTitleCamera* This, void* Edx,
		const Hedgehog::Universe::SUpdateInfo& updateInfo)
	{
		using namespace hh::math;
		auto* const camera = This->m_spCamera.get();
		TitleWorldMap::s_WorldMapCamera = camera;
		if (!camera)
			originalTitleWorldMap_CameraUpdate(This, Edx, updateInfo);

		//Move to constructor
		if (!TitleWorldMap::s_IsWorldMapCameraInitialized)
		{
			TitleWorldMap::s_IsWorldMapCameraInitialized = true;
			camera->m_Position = CVector(0, 0, cameraDistance);
			camera->m_TargetPosition = CVector(0, 0, 0);
			light = Sonic::CGameDocument::GetInstance()->m_pMember->m_spLightManager;
		}

		Sonic::SPadState input = Sonic::CInputState::GetInstance()->GetPadState();
		auto m_Title = TitleWorldMap::m_spTitle;

		CVector cameraTargetPosition = CVector(0, camHeight, 0);
		CVector cameraPosition = CVector(0, camHeight, cameraDistance);
		CVector2 m_Input(input.LeftStickHorizontal, input.LeftStickVertical);

		//Keyboard/Dpad support
		m_Input.x() = input.IsDown(Sonic::eKeyState_DpadLeft) ? -1.0f : (input.IsDown(Sonic::eKeyState_DpadRight) ? 1.0f : m_Input.x());
		m_Input.y() = input.IsDown(Sonic::eKeyState_DpadDown) ? -1.0f : (input.IsDown(Sonic::eKeyState_DpadUp) ? 1.0f : m_Input.y());
		
		const bool hasInput = m_Input.squaredNorm() > deadzone * deadzone;
		//CHECK FOR REMOVAL - && !isStageWindowOpen
		if (!TitleWorldMap::s_WorldmapCursorDisabled && !TitleWorldMapPause::s_IsPaused && hasInput&& !m_Title->isStageWindowOpen)
		{
			m_CameraPitch -= m_Input.y() * rotationPitchRate * updateInfo.DeltaTime;
			m_CameraYaw += m_Input.x() * rotationYawRate * updateInfo.DeltaTime;
		}
		// Do the thing where we magnetize our input.
		// TODO: Handle the HUD update here too I guess, because this is where a flag will be "selected"
		// Otherwise, make that handled somewhere else. I know you do an overlap check already, but best to do this once.

		for (int i = 0; i < m_Title->m_Country.size(); ++i)
		{
			auto& m_Continent = m_Title->m_Country[i];
			const CVector direction = m_Continent->GetPositionAdjusted().normalized();
			if (-direction.dot(camera->m_MyCamera.m_Direction) < dotThreshold)
			{
				continue;
			}
			if (m_Title->introPlayed)
			{
				MagnetizeToFlag(direction, updateInfo.DeltaTime, 1 - (m_Input.squaredNorm() / 2));
			}
		}

		// Min and max extents configured differently so we aren't aimlessly rotating around the south pole (TBD)
		// and so we can actually select Holoska correctly.
		constexpr float m_BottomPitch = 50.0f * DEG_TO_RAD;
		constexpr float m_TopPitch = 75.0f * DEG_TO_RAD;

		// Now limit
		m_CameraPitch = fmax(-m_TopPitch, fmin(m_CameraPitch, m_BottomPitch));
		// Cycle yaw so it doesn't go over 360, so we don't approach Very Large Numbers.
		m_CameraYaw = Util::WrapAroundFloat(m_CameraYaw, 360.0 * DEG_TO_RAD);

		const CQuaternion pitch = Util::QuaternionFromAngleAxis(m_CameraPitch, CVector(1, 0, 0));
		const CQuaternion yaw = Util::QuaternionFromAngleAxis(m_CameraYaw, CVector(0, 1, 0));

		const CQuaternion rotation = yaw * pitch;

		if (playingPan)
			PlayPan(camera, updateInfo);

		// Now we rotate everything.
		camera->m_Position = rotation * (cameraPosition - TitleWorldMap::s_PivotPosition) + TitleWorldMap::s_PivotPosition;
		camera->m_TargetPosition = rotation * (cameraTargetPosition - TitleWorldMap::s_PivotPosition) +
			TitleWorldMap::s_PivotPosition;

		// This happens after the fact for some reason. Wonder why...

		camera->m_FieldOfView = FOV;
		camera->m_FieldOfViewTarget = FOV;


		constexpr float rotationhRateLight = 0.1f;

		
		//Set up light stuff
		const float rotationForce = rotationhRateLight * multiplierRotationLight * updateInfo.DeltaTime;
		s_RotationAngle += rotationForce;
		// Wrap this around 360 degrees if you feel so inclined
		s_RotationAngle = Util::WrapAroundFloat(s_RotationAngle, 360.0 * DEG_TO_RAD);

		const CQuaternion lightRotation = Util::QuaternionFromAngleAxis(-s_RotationAngle, CVector(0, 1, 0));
		
		light->m_GlobalLightDirection = lightRotation * (CSUCTitleCompanion::lightPosition - TitleWorldMap::s_PivotPosition) +
			TitleWorldMap::s_PivotPosition;
		if(m_Title != nullptr)
		{
			if (m_Title->m_spSkySpace)
				if (m_Title->m_spSkySpace->spSun)
				{
					m_Title->m_spSkySpace->spSun->m_Transform.SetPosition(Eigen::AngleAxisf(180 * DEG_TO_RAD, CVector(0, 1, 0)) * light->m_GlobalLightDirection);
					m_Title->m_spSkySpace->spSun->NotifyChanged();
				}
			//if (m_Title->m_GlobeModel)
			//{
			//	if(m_Title->m_GlobeModel->m_spModelButtonTransform)
			//	{
			//		m_Title->m_GlobeModel->m_spModelButtonTransform->m_Transform.SetPosition(Eigen::AngleAxisf(180 * DEG_TO_RAD, CVector(0, 1, 0)) * light->m_GlobalLightDirection);
			//		m_Title->m_GlobeModel->m_spModelButtonTransform->NotifyChanged();					
			//	}
			//	
			//}
		}
		
		//Set light properties
		light->m_GlobalLightDiffuse = CVector(0.02f, 0.02f, 0.02f);
		//light->m_GlobalLightDirection = lightPosition;

		light->m_GlobalLightSpecular = CVector(4, 4, 4);

		ApplyCameraStuff(This, camera);
		camera->UpdateParallel(updateInfo);
	}
#pragma endregion
	//void __thiscall Sonic::TransitionTitle::CTitleTransitionImpl::UpdateParallel(Sonic::TransitionTitle::CTitleTransitionImpl *this, int a2)
	HOOK(void, __fastcall, UpdatePar, 0x0058C710, void* This, void* Edx, int a2)
	{
		return;
	}
	//int __thiscall sub_58C800(int this, int a2)
	HOOK(int, __fastcall, sub_58C800, 0x0058C800, void* This, void* Edx, int a2)
	{
		return 0;
	}
	//Hedgehog::Mirage::CRenderable *__thiscall TitleAddRenderables(_DWORD *this, int a2, int a3, int a4)
	HOOK(void*, __fastcall, TitleAddRenderables, 0x0058E650, void* This, void* Edx, int a2, int a3, int a4)
	{
		return nullptr;
	}
	//void __thiscall Sonic::CGameplayFlowTitle::PlayMusic(CTempState *this)
	//void __thiscall sub_CF8F40(void *this)
	HOOK(void, __fastcall, sub_CF8F40, 0x00CF8FE0, void* This, void* Edx)
	{
		originalsub_CF8F40(This, Edx);
		if(!Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager)
			Util::SpawnParticleManager();
	}
	void* updateDirectorCustom(CUpdateDirector* context, float elapsedTime)
	{
		//const bool loadObjectsInRange = config->loadObjectsInRange && strcmp(STAGE_ID, "blb") != 0;

		context->AddUpdateCommand(elapsedTime, "0");
		context->AddUpdateCommand(elapsedTime, "1");
		context->AddUpdateCommand(elapsedTime, "2");
		context->AddUpdateCommand(elapsedTime, "3");
		context->AddUpdateCommand(elapsedTime, "b");

		context->CallUnknownFunction(elapsedTime);

		context->AddRenderCommand(elapsedTime, "0");
		context->AddRenderCommand(elapsedTime, "1");
		context->AddRenderCommand(elapsedTime, "2");
		context->AddRenderCommand(elapsedTime, "3");
		context->AddRenderCommand(elapsedTime, "b");

		context->FinishRenderer();
		return context->FinishUpdater();
	}
	HOOK(void*, __fastcall, UpdateDirectorSimple, 0x1105A60, void* This, void* Edx, CUpdateDirector* context, float elapsedTime)
	{
		//if (TitleWorldMap::m_spTitle)
		//	return updateDirectorCustom(context, elapsedTime);

		return originalUpdateDirectorSimple(This, Edx, context, elapsedTime);
	}
	void TitleWorldMap::RegisterHooks()
	{
		//CSUCTitleCompanion title = CSUCTitleCompanion();
		//title.RegisterStateFactory<CTitleStateInitialize>();
		//WRITE_JUMP(0x00584CEE, (void*)0x00588820);
		//WRITE_JUMP(0x015B8188, (void*)0x015B8198);
		WRITE_JUMP(0x0058D41F, (void*)0x0058D7D8); //Skip setting light properties every second
		
		//INSTALL_HOOK(UpdateDirectorSimple);
		INSTALL_HOOK(TitleWorldMap_CameraUpdate);
		INSTALL_HOOK(TitleWorldMap_CTitleMain);

		INSTALL_HOOK(UpdatePar);
		INSTALL_HOOK(sub_58C800);
		INSTALL_HOOK(sub_CF8F40);
		//Fix a crash related to the world map
		WRITE_JUMP(0x0058C481, 0x0058C651);

		//INSTALL_HOOK(TitleAddRenderables);
		INSTALL_HOOK(TitleWorldMap_UpdateApplication);

		//From Brianuu's 06 Title, makes saving not crash
		WRITE_JUMP(0xD22A83, (void*)0xD22B84);
		WRITE_MEMORY(0xD22CE8, uint8_t, 0);
		//WRITE_JUMP(0x00A51B66, 0x00A51BCE);
	}

}