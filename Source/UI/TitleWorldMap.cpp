//	TODO:
//	Make sun gameobject to spawn the particle that Qution made
//	Add proper white-world support
//  Make all things related to title custom renderables instead of title renderables

#include "TitleWorldMap.h"
#include "../System/AspectRatioHelper.h"
#include "../System/MiniAudioHelper.h"
using namespace hh::math;

namespace SUC::UI::TitleScreen
{
	bool TitleWorldMap::s_ForceTitleFlow = false;
	bool TitleWorldMap::s_IsWorldMapCameraInitialized = false;
	const CVector TitleWorldMap::s_PivotPosition = CVector(0, 0, -2.34f);
	const TitleWorldMapCamera* TitleWorldMap::s_WorldMapCamera;
	TitleWorldMap::SFlagUIInformation TitleWorldMap::s_Flags[9];
	bool TitleWorldMap::s_WorldmapCursorDisabled = true;
	bool TitleWorldMap::s_TargetDisabled = true;
	bool TitleWorldMap::s_IsCapitalWindowOpened = false;
	int TitleWorldMap::s_LastFlagSelected = 0;
	int TitleWorldMap::s_StageSelectWindowSelection = 0;
	bool TitleWorldMap::s_IsActive = false;
	int WM_STAGELIST_MAXLISTVISIBLE = 6;

	//vs shits itself if these are in pch, no idea why
	constexpr double RAD2DEG = 57.29578018188477;
	constexpr double DEG2RAD = 0.01745329238474369;
	Chao::CSD::RCPtr<Chao::CSD::CProject> rcWorldMap;
	boost::shared_ptr<Sonic::CGameObjectCSD> spWorldMap;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcInfoBg1, rcWorldmapFooterImgA, rcInfoImg1, rcInfoImg2, rcInfoImg3, rcInfoImg4,
		rcHeaderBgW, rcHeaderImgW, rcFooterBgW, rcFooterImgW;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcCursorLeft, rcCursorTop, rcCursorBottom, rcCursorRight;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcCtsName, rcCtsCursorEffect, rcCtsGuideWindow, rcCtsGuideScreenshot, rcCtsGuideText
		, rcCtsGuide4Mission, rcCtsGuide5Medal;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcCtsStageWindow, rcCtsStageSelect, rcCtsName_2, rcStageSelectFlag,
		rcCtsStageInfoWindow, rcCtsGuide1Hiscore, rcCtsGuide2BestTime, rcCtsGuide3Rank,
		rcCtsStage4Mission, rcCtsStage5Medal, rcCtsStageScreenshot;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcCtsChoicesBg, rcCtsChoicesWindow, rcCtsChoicesSelect;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcTextWorldDescription, rcTextStageDescription;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcTextElement[9];

	bool isStageWindowOpen = false;
	//	theres defo a better way to handle this
	bool m_isInShade[9];
	bool m_SunMoonTransitionArray[9];
	//

	std::vector<CVector> m_flagPositions;
	static AudioHandle cursorMoveHandle, cursorSelectHandle, stageSelectHandle, worldMapMusicHandle;
	CVector2 posCursorCenter;
	CVector2* offsetAspect;
	CVector2* offsetRes;

	static float cameraDistance = 5.0f;
	static float rotationPitch = 20.0f;
	static float FOV = 0.84906584f;
	static float rotationYaw = 0.0f;
	float cursorMultiplier = 24;
	int stageSelectedWindowMax = 6;
	int flagSelectionAmount = 0;

	int selectedCapital = 0;
	bool playingPointerMove;
	bool introPlayed = false;
	bool cursorSelected = false;
	bool playingPan = false;
	float timePan, timeStageSelectDelay = 0;
	float camHeight = -20;
	float lastflagSelectionAmount = 0;
	float editorMulti = 1;
	int currentFlagSelected, lastFlagIn = 0;
	hh::fnd::CStateMachineBase::CStateBase* testState;
	boost::shared_ptr<Sonic::CLightManager> light;
	//boost::shared_ptr<Sonic::CGameObject3D> earth;
	boost::shared_ptr<CTitleWorldMapGlobe> m_GlobeModel;

	boost::shared_ptr<CTitleWorldMapSky> m_spWorldmapObject;
	float multiplierRotationLight = 0.1f;

	hh::math::CQuaternion rotationEarth;


	void CreateParticleController(boost::shared_ptr<Sonic::CGameObject>& a1)
	{
		uint32_t func = 0xE8FE50;
		__asm
		{
			mov eax, a1
			call func

		}
	};





	//From Brianuu's 06 Title
	boost::shared_ptr<SaveLoadTestStruct> m_spSave;
	FUNCTION_PTR(void, __thiscall, TitleWorldMap_CTitleOptionCStateOutroSaving, 0xD22A70,
		boost::shared_ptr<SaveLoadTestStruct>& spSave, void* a2);

	void fpAddParticle(DWORD* manager, void* handle, const boost::shared_ptr<Sonic::CMatrixNodeTransform>& node,
		const hh::base::CSharedString& name, uint32_t flag)
	{
		uint32_t func = 0x00E8F8A0;
		__asm
		{
			push flag
			push name
			push node
			push handle
			mov eax, manager
			call func
		}
	};

	Sonic::CGameObject* PauseInitTest(Sonic::CGameObject* objectt, int PAUSEtype)
	{
		uint32_t func = 0x010A1A30;
		__asm
		{
			mov eax, objectt;
			mov ecx, PAUSEtype;
			push ecx;
			call func;
			retn
		}
	};

	void TitleWorldMap::SetHideEverything(bool hide)
	{
		rcInfoBg1->SetHideFlag(hide);
		rcWorldmapFooterImgA->SetHideFlag(hide);
		rcInfoImg1->SetHideFlag(hide);
		rcInfoImg2->SetHideFlag(hide);
		rcInfoImg3->SetHideFlag(hide);
		rcInfoImg4->SetHideFlag(hide);
		rcHeaderBgW->SetHideFlag(hide);
		rcHeaderImgW->SetHideFlag(hide);
		rcFooterBgW->SetHideFlag(hide);
		if (rcFooterImgW)
			rcFooterImgW->SetHideFlag(hide);
		rcCursorLeft->SetHideFlag(hide);
		rcCursorTop->SetHideFlag(hide);
		rcCursorBottom->SetHideFlag(hide);
		rcCursorRight->SetHideFlag(hide);
		rcCtsName->SetHideFlag(hide);
		rcCtsCursorEffect->SetHideFlag(hide);

		rcCtsGuideScreenshot->SetHideFlag(hide);
		rcCtsGuideWindow->SetHideFlag(hide);
		rcCtsGuideText->SetHideFlag(hide);

		for (size_t i = 0; i < 9; i++)
		{
			s_Flags[i].flag->SetHideFlag(hide);
			s_Flags[i].sun_moon->SetHideFlag(hide);
		}
	}

	void CalculateAspectOffsets()
	{
		if (*(size_t*)0x6F23C6 != 0x75D8C0D9) // Widescreen Support
		{
			const float aspect = static_cast<float>(*(size_t*)0x1DFDDDC) / static_cast<float>(*(size_t*)0x1DFDDE0);

			offsetRes = new CVector2(static_cast<float>(*(size_t*)0x01804F8C), static_cast<float>(*(size_t*)0x01804F90));

			if (aspect * 9.0f > 16.0f)
			{
				offsetAspect = new CVector2(720.0f * aspect - 1280.0f, 0.0f);
			}
			else
			{
				offsetAspect = new CVector2(0, 1280.0f / aspect - 720.0f);
			}
		}
		else
		{
			offsetAspect = new CVector2(0, 0);
		}
	}

	void PlayCursorAnim(const char* name, bool reverse = false)
	{
		float reversePoint = reverse ? 80 : 0;
		CSDCommon::PlayAnimation(rcCursorTop, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, reversePoint, false,
			reverse);
		CSDCommon::PlayAnimation(rcCursorBottom, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, reversePoint, false,
			reverse);
		CSDCommon::PlayAnimation(rcCursorLeft, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, reversePoint, false,
			reverse);
		CSDCommon::PlayAnimation(rcCursorRight, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, reversePoint, false,
			reverse);
	}

	void CheckCursorAnimStatus()
	{
		if (rcCtsGuideWindow)
		{
			if (rcCtsGuideWindow->m_MotionFrame >= 85 && rcCtsGuideWindow->m_MotionFrame <= 90)
			{
				rcCtsGuide4Mission->SetHideFlag(false);
				rcCtsGuide5Medal->SetHideFlag(false);
			}
		}
		/*if (cursorSelected)
			return;
		if (!rcCursorLeft->m_MotionDisableFlag && rcCursorLeft->m_MotionFrame <= 0)
			CSDCommon::FreezeMotion(rcCursorLeft);

		if (!rcCursorRight->m_MotionDisableFlag && rcCursorRight->m_MotionFrame <= 0 )
			CSDCommon::FreezeMotion(rcCursorRight);

		if (!rcCursorTop->m_MotionDisableFlag && rcCursorTop->m_MotionFrame <= 0 )
			CSDCommon::FreezeMotion(rcCursorTop);

		if (!rcCursorBottom->m_MotionDisableFlag && rcCursorBottom->m_MotionFrame <= 0)
			CSDCommon::FreezeMotion(rcCursorBottom);*/
	}

	void SetCursorPos(const CVector2& pos)
	{
		if (!rcCursorLeft || !TitleWorldMap::s_IsActive)
			return;
		if (((pos.x() + pos.y()) != 0) && !playingPointerMove)
		{
			MiniAudioHelper::playSound(cursorMoveHandle, 10, "PointerMove", true);
			//Common::MiniAudioHelper::playSoundStatic(cursorMoveHandle, 10);
			playingPointerMove = true;
		}
		else if (((pos.x() + pos.y()) == 0) && playingPointerMove)
		{
			playingPointerMove = false;
			MiniAudioHelper::stopSound(cursorMoveHandle);
			//cursorMoveHandle.reset();
		}
		posCursorCenter = pos + *System::AspectRatioHelper::ScreenHalfPoint;
		float sizeBox = 125;
		auto posL = CVector2(pos.x() - (sizeBox / 2), pos.y());
		auto posR = CVector2(pos.x() + sizeBox / 2, pos.y());
		auto posT = CVector2(pos.x(), pos.y() + sizeBox / 2);
		auto posB = CVector2(pos.x(), pos.y() - (sizeBox / 2));

		rcCursorLeft->SetPosition(posL.x() + System::AspectRatioHelper::ScreenHalfPoint->x(),
			posL.y() + System::AspectRatioHelper::ScreenHalfPoint->y());
		rcCursorRight->SetPosition(posR.x() + System::AspectRatioHelper::ScreenHalfPoint->x(),
			posR.y() + System::AspectRatioHelper::ScreenHalfPoint->y());
		rcCursorTop->SetPosition(posT.x() + System::AspectRatioHelper::ScreenHalfPoint->x(),
			posT.y() + System::AspectRatioHelper::ScreenHalfPoint->y());
		rcCursorBottom->SetPosition(posB.x() + System::AspectRatioHelper::ScreenHalfPoint->x(),
			posB.y() + System::AspectRatioHelper::ScreenHalfPoint->y());
		rcCursorRight->GetNode("arrow_position_9")->SetRotation(180);
		rcCursorTop->GetNode("arrow_position_9")->SetRotation(90);
		rcCursorBottom->GetNode("arrow_position_9")->SetRotation(-90);
	}

	bool IsInsideCursorRange(const CVector2& input, float visibility, int flagID)
	{
		if (!rcCursorLeft || TitleWorldMap::s_WorldmapCursorDisabled ||
			TitleWorldMapPause::s_IsPaused)
			return false;
		bool result = false;
		float range = 100;

		// Check if the point p is on the same side of each edge as the other three corners
		if (abs(Common::GetVector2Distance(input, posCursorCenter)) <= 50)
		{
			if (visibility >= 80 && flagID != lastFlagIn)
			{
				lastFlagIn = flagID;
				//for eventual night versions, add 9
				rcCtsGuideScreenshot->GetNode("town_ss_img")->SetPatternIndex(flagID);
				//rcCtsGuideText->GetNode("text_size")->SetPatternIndex(flagID);
				rcCtsName->GetNode("img")->SetPatternIndex(flagID);

				result = true;
			}
		}
		else
		{
			lastFlagIn = -1;
		}
		return result;
	}

	void SetLevelTextCast(const char* text)
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
				rcCtsGuideText->GetNode(buff)->SetText(lines.at(i).c_str());
			}
			else
			{
				rcCtsGuideText->GetNode(buff)->SetText(" ");
			}
		}
	}

	void PopulateStageSelect(int id, int offset = 0)
	{
		if (SUC::Project::s_WorldData.data.size() < id)
		{
			printf("\n[WorldMap] Missing config for FlagID %d", id);
			return;
		}
		stageSelectedWindowMax = TitleWorldMap::s_Flags[id].night && SUC::Project::s_WorldData.data[id].dataNight.size() != 0
			? SUC::Project::s_WorldData.data[id].dataNight.size() - 1
			: SUC::Project::s_WorldData.data[id].data.size() - 1;
		//Common::ClampInt(stageSelectedWindowMax, 0, 6);
		for (size_t i = 0; i <= WM_STAGELIST_MAXLISTVISIBLE; i++)
		{
			rcTextElement[i]->SetHideFlag(true);
		}

		for (size_t i = 0; i <= WM_STAGELIST_MAXLISTVISIBLE; i++)
		{
			if (stageSelectedWindowMax < i + offset)
				break;

			const char* optionName;
			if (TitleWorldMap::s_Flags[id].night && SUC::Project::s_WorldData.data[id].dataNight.size() != 0)
				optionName = SUC::Project::s_WorldData.data[id].dataNight[i + offset].optionName.c_str();
			else
				optionName = SUC::Project::s_WorldData.data[id].data[i + offset].optionName.c_str();

			rcTextElement[i]->SetHideFlag(false);
			rcTextElement[i]->GetNode("Text_blue")->SetText(optionName);
		}
	}

	TitleWorldMap::SaveStageInfo GetInfoForStage(std::string id)
	{
		auto it = std::find(SUC::Project::s_GenerationsStages.begin(), SUC::Project::s_GenerationsStages.end(), id);
		auto returnI = TitleWorldMap::SaveStageInfo();
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

	void PopulateCountryPreviewInfo(int flag)
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

		rcCtsGuide4Mission->GetNode("num_nume")->SetText(stageCount);
		rcCtsGuide4Mission->GetNode("num_deno")->SetText(stageMax);

		rcCtsGuide5Medal->GetNode("s_num_nume")->SetText(redRingsCurrent);
		rcCtsGuide5Medal->GetNode("s_num_deno")->SetText(redRingsMax);

		rcCtsGuide5Medal->GetNode("m_num_nume")->SetText(redRingsCurrent);
		rcCtsGuide5Medal->GetNode("m_num_deno")->SetText(redRingsMax);
	}

	void PopulateStageWindowInfo(std::string id)
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

		rcCtsGuide2BestTime->GetNode("num")->SetText(bestTimeC);
		rcCtsGuide1Hiscore->GetNode("num")->SetText(scoreCount);
		rcCtsGuide3Rank->GetNode("rank_shade")->SetPatternIndex(info.bestRank);
		rcCtsGuide3Rank->GetNode("rank_img")->SetPatternIndex(info.bestRank);

		rcCtsStage5Medal->GetNode("s_num_nume")->SetText(redCount);
		rcCtsStage5Medal->GetNode("s_num_deno")->SetText("05");

		rcCtsStage5Medal->GetNode("m_num_nume")->SetText(redCount);
		rcCtsStage5Medal->GetNode("m_num_deno")->SetText("05");
	}

	CVector2 WorldToUIPosition(const CVector& input)
	{
		const auto camera = TitleWorldMap::s_WorldMapCamera;
		if (!camera) return CVector2(0, 0);
		auto screenPosition = camera->m_MyCamera.m_View * CVector4(input.x(), input.y(), input.z(), 1.0f);
		screenPosition = camera->m_MyCamera.m_Projection * screenPosition;
		screenPosition.head<2>() /= screenPosition.w();
		screenPosition.x() = ((screenPosition.x() * 0.5f + 0.5f) * (System::AspectRatioHelper::OriginalResolution->x() + offsetAspect
			->x()));
		screenPosition.y() = (screenPosition.y() * -0.5f + 0.5f) * (System::AspectRatioHelper::OriginalResolution->y() + offsetAspect
			->y());
		return CVector2(screenPosition.x(), screenPosition.y());
	}

	Sonic::CGameObject* teoqr;

	void TitleWorldMap::Start()
	{
		//Init Anims
		s_IsActive = true;
		System::StageManager::s_LoadingReplacementEnabled = true;
		s_ForceTitleFlow = false;
		TitleWorldMap_CTitleOptionCStateOutroSaving(m_spSave, nullptr);
		//Set lives text
		rcInfoImg1->GetNode("num")->SetText(Common::IntToString(Common::GetLivesCount(), "%02d"));
		introPlayed = false;
		SetHideEverything(false);
		PlayCursorAnim("Intro_Anim");
		CSDCommon::PlayAnimation(rcInfoBg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcInfoImg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcInfoImg2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcInfoImg3, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcInfoImg4, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcHeaderBgW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60);
		//60 so it doesnt fade in from the previous header and footer of HudTitle
		CSDCommon::PlayAnimation(rcFooterBgW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60);
		//60 so it doesnt fade in from the previous header and footer of HudTitle
		CSDCommon::PlayAnimation(rcHeaderImgW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcCtsGuideScreenshot, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcCtsGuideText, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcCtsGuideWindow, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcWorldmapFooterImgA, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

		PlayCursorAnim("Intro_Anim");
		for (size_t i = 0; i < 9; i++) CSDCommon::PlayAnimation(s_Flags[i].flag, "Intro_Anim",
			Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
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


	void __fastcall CTitleWRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
	{
		using Chao::CSD::CProject;
		TitleWorldMap::KillScreen();
		CProject::DestroyScene(rcWorldMap.Get(), rcInfoBg1);
		CProject::DestroyScene(rcWorldMap.Get(), rcInfoImg2);
		CProject::DestroyScene(rcWorldMap.Get(), rcInfoImg3);
		CProject::DestroyScene(rcWorldMap.Get(), rcInfoImg4);
		CProject::DestroyScene(rcWorldMap.Get(), rcHeaderBgW);
		CProject::DestroyScene(rcWorldMap.Get(), rcHeaderImgW);
		CProject::DestroyScene(rcWorldMap.Get(), rcFooterBgW);
		CProject::DestroyScene(rcWorldMap.Get(), rcFooterImgW);
		CProject::DestroyScene(rcWorldMap.Get(), rcWorldmapFooterImgA);

		CProject::DestroyScene(rcWorldMap.Get(), rcCursorLeft);
		CProject::DestroyScene(rcWorldMap.Get(), rcCursorTop);
		CProject::DestroyScene(rcWorldMap.Get(), rcCursorBottom);
		CProject::DestroyScene(rcWorldMap.Get(), rcCursorRight);

		CProject::DestroyScene(rcWorldMap.Get(), rcCtsName);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsCursorEffect);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsGuideScreenshot);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsStageScreenshot);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsGuideWindow);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsGuideText);
		CProject::DestroyScene(rcWorldMap.Get(), rcTextWorldDescription);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsGuide4Mission);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsGuide5Medal);

		CProject::DestroyScene(rcWorldMap.Get(), rcCtsStageWindow);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsStageSelect);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsStageInfoWindow);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsGuide1Hiscore);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsGuide2BestTime);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsGuide3Rank);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsStage4Mission);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsStage5Medal);

		CProject::DestroyScene(rcWorldMap.Get(), rcCtsChoicesBg);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsChoicesWindow);
		CProject::DestroyScene(rcWorldMap.Get(), rcCtsChoicesSelect);

		for (size_t i = 0; i < 9; i++)
		{
			Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), TitleWorldMap::s_Flags[i].flag);
			Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), TitleWorldMap::s_Flags[i].sun_moon);
		}
		for (size_t i = 0; i < 9; i++) Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), rcTextElement[i]);

		rcWorldMap = nullptr;

		timePan = 0;
		camHeight = -20;
		editorMulti = 1;
		playingPan = false;
		introPlayed = false;
		TitleWorldMap::s_WorldmapCursorDisabled = true;
		rotationPitch = -0.4f;
		FOV = 0.84906584f;
		cameraDistance = 5.0f;
		rotationYaw = 0.55f;
		TitleWorldMap::s_TargetDisabled = true;
		cursorSelected = false;
		TitleWorldMap::s_StageSelectWindowSelection = 0;
		isStageWindowOpen = false;
		timeStageSelectDelay = 0;
		lastflagSelectionAmount = 0;
	}

	void ShowTextAct(bool visible)
	{
		for (size_t i = 0; i <= WM_STAGELIST_MAXLISTVISIBLE; i++)
		{
			rcTextElement[i]->SetHideFlag(!visible);
		}
	}

	void SetVisibilityPlayerInfo(bool visible)
	{
		rcInfoBg1->SetHideFlag(!visible);
		rcInfoImg1->SetHideFlag(!visible);
		rcInfoImg2->SetHideFlag(!visible);
		rcInfoImg3->SetHideFlag(!visible);
		rcInfoImg4->SetHideFlag(!visible);
		rcCtsGuide4Mission->SetHideFlag(!visible);
		rcCtsGuide5Medal->SetHideFlag(!visible);
		CSDCommon::FreezeMotion(rcCtsGuideScreenshot, 0);
		CSDCommon::FreezeMotion(rcCtsGuideText, 0);
		CSDCommon::FreezeMotion(rcCtsGuideWindow, 0);
		CSDCommon::PlayAnimation(rcCtsGuideScreenshot, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
			!visible);
		CSDCommon::PlayAnimation(rcCtsGuideText, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
			!visible);
		CSDCommon::PlayAnimation(rcCtsGuideWindow, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
			!visible);
	}

	void SetVisibilityCapitalWindow(bool visible)
	{
		selectedCapital = 0;
		if (visible) //im only doing this so that it doesnt disable when disabling and plays anims instead
		{
			rcCtsChoicesBg->SetHideFlag(!visible);
			rcCtsChoicesWindow->SetHideFlag(!visible);
		}
		rcCtsChoicesSelect->SetHideFlag(!visible);
		CSDCommon::PlayAnimation(rcCtsChoicesBg, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
			!visible);
		CSDCommon::PlayAnimation(rcCtsChoicesWindow, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
			!visible);

		rcTextElement[7]->SetHideFlag(!visible);
		rcTextElement[8]->SetHideFlag(!visible);

		CSDCommon::PlayAnimation(rcTextElement[7], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcTextElement[8], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcCtsChoicesSelect, "Scroll_down_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 100);
	}

	void SetVisibilityStageWindow(bool visible)
	{
		CSDCommon::PlayAnimation(rcCtsStageWindow, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
			!visible);
		CSDCommon::PlayAnimation(rcCtsName_2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
			!visible);
		CSDCommon::PlayAnimation(rcCtsStageSelect, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
			!visible);
		CSDCommon::PlayAnimation(rcStageSelectFlag, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
			!visible);
		CSDCommon::PlayAnimation(rcCtsStageScreenshot, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true,
			!visible);
		CSDCommon::PlayAnimation(rcCtsStageInfoWindow, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0,
			true, !visible);
		CSDCommon::FreezeMotion(rcCtsStageSelect, 0);

		PopulateStageSelect(TitleWorldMap::s_LastFlagSelected);
		rcCtsStageSelect->SetHideFlag(!visible);
		rcCtsStage5Medal->SetHideFlag(!visible);
		rcCtsGuide3Rank->SetHideFlag(!visible);
		rcCtsGuide1Hiscore->SetHideFlag(!visible);
		rcCtsGuide2BestTime->SetHideFlag(!visible);
		if (visible) //im only doing this so that it doesnt disable when disabling and plays anims instead
		{
			CSDCommon::PlayAnimation(rcCtsGuideText, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0);
			SetLevelTextCast("Head for the goal!");
			rcCtsGuideText->SetPosition(0, 40);
			rcCtsStageWindow->SetHideFlag(false);
			rcCtsStageScreenshot->SetHideFlag(false);
			rcCtsName_2->SetHideFlag(false);
			rcStageSelectFlag->SetHideFlag(false);
			rcCtsStageInfoWindow->SetHideFlag(false);
		}
		else

			rcCtsGuideText->SetPosition(140, 40);
	}

	void TitleWorldMap::CreateScreen(Sonic::CGameObject* pParentGameObject)
	{
		if (rcWorldMap && !spWorldMap)
		{
			pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(
				spWorldMap = boost::make_shared<Sonic::CGameObjectCSD>(rcWorldMap, 0.5f, "HUD", false), "main",
				pParentGameObject);
		}
	}

	void TitleWorldMap::KillScreen()
	{
		if (spWorldMap)
		{
			spWorldMap->SendMessage(spWorldMap->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
			spWorldMap = nullptr;
		}
	}

	void TitleWorldMap::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
	{
		if (visible)
			CreateScreen(pParentGameObject);
		else
			KillScreen();
	}

	HOOK(int, __fastcall, TitleWorldMap_CTitleMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3,
		void** a4)
	{
		CTitleWRemoveCallback(This, nullptr, nullptr);
		CalculateAspectOffsets();
		auto lightColor = (Eigen::Vector3f*)0x01A42308;
		lightColor->x() = 0.5976471f;
		lightColor->y() = 0.5835295f;
		lightColor->z() = 0.5364707f;
		if (light == nullptr)
			light = Sonic::CGameDocument::GetInstance()->m_pMember->m_spLightManager;
		light->m_GlobalLightDirection = CVector(1, 1, 1);

		//Set light properties
		light->m_GlobalLightDiffuse = CVector(0.02f, 0.02f, 0.02f);
		//light->m_GlobalLightDirection = CVector(-79.8565f, 0, 4.78983f);
		light->m_GlobalLightSpecular = CVector(0.02f, 0.02f, 0.02f);
		Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
		auto spCsdProject = wrapper.GetCsdProject("ui_worldmap");
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


		//By default the cursor in the worldmap is set to the left anchor 
		rcCursorLeft = rcWorldMap->CreateScene("cts_cursor");
		rcCursorTop = rcWorldMap->CreateScene("cts_cursor");
		rcCursorBottom = rcWorldMap->CreateScene("cts_cursor");
		rcCursorRight = rcWorldMap->CreateScene("cts_cursor");
		TitleWorldMap::s_IsWorldMapCameraInitialized = false;
		constexpr float earthRadius = 5.25f; //Used to normalize flag positions to the globe's curvature
		m_flagPositions.push_back(CVector(0.31f, 0.36f, 2.28f));
		m_flagPositions.push_back(CVector(2.310000f, 2.360000f, 1.111371f));
		m_flagPositions.push_back(CVector(2.810000f, -0.140000f, -6.649425f));
		m_flagPositions.push_back(CVector(2.810000f, -1.890000, 1.742745));
		m_flagPositions.push_back(CVector(-0.190000, 4.610000, -3.543527));
		m_flagPositions.push_back(CVector(-5.190000, 0.110000, -3.363136));
		m_flagPositions.push_back(CVector(0.060000, -2.639999, -6.829812));
		m_flagPositions.push_back(CVector(-4.440000, -2.390000, -0.798426));
		m_flagPositions.push_back(CVector(-3.600000, 3.00000, -1.160));

		// Now normalize all these positions
		for (int i = 0; i < m_flagPositions.size(); ++i)
		{
			m_flagPositions.at(i) = ((m_flagPositions.at(i) - TitleWorldMap::s_PivotPosition).normalized() * earthRadius) +
				TitleWorldMap::s_PivotPosition;
		}
		//Set up sun/moon medals' animations
		for (size_t i = 0; i < 9; i++)
		{
			TitleWorldMap::s_Flags[i] = TitleWorldMap::SFlagUIInformation();

			TitleWorldMap::s_Flags[i].flag = rcWorldMap->CreateScene("cts_parts_flag");
			TitleWorldMap::s_Flags[i].sun_moon = rcWorldMap->CreateScene("cts_parts_sun_moon");

			TitleWorldMap::s_Flags[i].playingMedalTransition = false;
			CSDCommon::PlayAnimation(TitleWorldMap::s_Flags[i].flag, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,
				0);
			TitleWorldMap::s_Flags[i].flag->GetNode("img")->SetPatternIndex(i);

			float shadeAmount = fmax(
				0.0f, -(CVector(-79.8565f, 0, 4.78983f).dot(
					(m_flagPositions.at(i) - CVector(0, 0, -2.34f)).normalized())))
				* 100;
			float visibility = fmax(
				0.0f, -(CVector(0, 0, -20).dot((m_flagPositions.at(i) - CVector(0, 0, -2.34f)).normalized()))) * 100;
			bool isDark = shadeAmount > 50;

			m_isInShade[i] = isDark;
			CSDCommon::PlayAnimation(TitleWorldMap::s_Flags[i].sun_moon, "Switch_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,
				0, isDark ? 0 : 100); //Set image to either sun or moon
			CSDCommon::PlayAnimation(TitleWorldMap::s_Flags[i].sun_moon, "Fade_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,
				0, visibility, visibility);
		}
		TitleWorldMap::CreateScreen(This);
		for (auto e : TitleWorldMap::s_Flags) e.flag->SetHideFlag(true);

		TitleWorldMap::SetHideEverything(true);
		rcCtsGuide4Mission->SetHideFlag(true);
		rcCtsGuide5Medal->SetHideFlag(true);
		rcCtsStageWindow->SetHideFlag(true);
		rcCtsStageSelect->SetHideFlag(true);
		rcCtsName_2->SetHideFlag(true);
		rcCtsCursorEffect->SetHideFlag(true);
		rcStageSelectFlag->SetHideFlag(true);
		rcCtsStageInfoWindow->SetHideFlag(true);
		rcCtsStage4Mission->SetHideFlag(true);
		rcCtsStage5Medal->SetHideFlag(true);
		rcCtsGuide1Hiscore->SetHideFlag(true);
		rcCtsGuide2BestTime->SetHideFlag(true);
		rcCtsGuide3Rank->SetHideFlag(true);
		rcCtsChoicesBg->SetHideFlag(true);
		rcCtsChoicesWindow->SetHideFlag(true);
		rcCtsChoicesSelect->SetHideFlag(true);
		rcCtsStageScreenshot->SetHideFlag(true);

		float sceneFixPosX = 140;
		//Set up scenes to make it work in 16:9, because for some reason unleashed does it in code
		rcHeaderImgW->SetPosition(-sceneFixPosX, 0);
		rcInfoBg1->SetPosition(-sceneFixPosX, 0);
		rcInfoImg1->SetPosition(-sceneFixPosX, 0);
		rcInfoImg2->SetPosition(-sceneFixPosX, 0);
		rcInfoImg3->SetPosition(-sceneFixPosX, 0);
		rcInfoImg4->SetPosition(-sceneFixPosX, 0);
		rcCtsGuideWindow->SetPosition(sceneFixPosX, 0);
		rcCtsGuideScreenshot->SetPosition(sceneFixPosX, 0);
		rcCtsGuideText->SetPosition(sceneFixPosX, 40);
		rcCtsGuide4Mission->SetPosition(sceneFixPosX, 0);
		rcCtsGuide5Medal->SetPosition(sceneFixPosX, 0);


		m_spWorldmapObject = boost::make_shared<CTitleWorldMapSky>(CVector(-79.8565f, 0, 4.78983f));
		// CVector(0, 0, -2.34f)
		m_GlobeModel = boost::make_shared<CTitleWorldMapGlobe>(TitleWorldMap::s_PivotPosition);
		m_GlobeModel->SetPosition(CVector(0, 0, 0));
		m_GlobeModel->m_spMatrixNodeTransform->NotifyChanged();
		Sonic::CGameDocument::GetInstance()->AddGameObject(m_GlobeModel);
		Sonic::CGameDocument::GetInstance()->AddGameObject(m_spWorldmapObject);

		return originalTitleWorldMap_CTitleMain(This, Edx, a2, a3, a4);
	}

	void Flags_Update()
	{
		for (size_t i = 0; i < 9; i++)
		{
			float visibility = fmax(
				0.0f, -(TitleWorldMap::s_WorldMapCamera->m_MyCamera.m_Direction.dot(
					(m_flagPositions.at(i) - CVector(0, 0, -2.34f)).normalized()))) * 100;
			auto uiPos = WorldToUIPosition(m_flagPositions.at(i));

			float shadeAmount = fmax(
				0.0f, -(light->m_GlobalLightDirection.
					dot((m_flagPositions.at(i) - CVector(0, 0, -2.34f)).normalized()))) *
				100;
			bool isDark = shadeAmount > 50;
			if (TitleWorldMap::s_Flags[i].playingMedalTransition
				&& TitleWorldMap::s_Flags[i].sun_moon->m_MotionFrame == 0
				|| TitleWorldMap::s_Flags[i].sun_moon->m_MotionFrame == TitleWorldMap::s_Flags[i].sun_moon->m_MotionEndFrame)
				TitleWorldMap::s_Flags[i].playingMedalTransition = false;

			if (introPlayed)
				CSDCommon::PlayAnimation(TitleWorldMap::s_Flags[i].flag, "Fade_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,
					0, visibility, visibility);

			if (!TitleWorldMap::s_Flags[i].playingMedalTransition)
				CSDCommon::PlayAnimation(TitleWorldMap::s_Flags[i].sun_moon, "Fade_Anim",
					Chao::CSD::eMotionRepeatType_PlayOnce, 0, visibility, visibility);

			TitleWorldMap::s_Flags[i].flag->SetPosition(uiPos.x(), uiPos.y());
			TitleWorldMap::s_Flags[i].sun_moon->SetPosition(uiPos.x() + 36, uiPos.y() - 23);
			TitleWorldMap::s_Flags[i].night = !isDark;

			if (isDark && !m_isInShade[i])
			{
				TitleWorldMap::s_Flags[i].playingMedalTransition = true;
				CSDCommon::PlayAnimation(TitleWorldMap::s_Flags[i].sun_moon, "Switch_Anim",
					Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0, 0, true, true);
			}
			else if (!isDark && m_isInShade[i])
			{
				TitleWorldMap::s_Flags[i].playingMedalTransition = true;
				CSDCommon::PlayAnimation(TitleWorldMap::s_Flags[i].sun_moon, "Switch_Anim",
					Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
			}

			bool inrange = IsInsideCursorRange(uiPos, visibility, i);
			currentFlagSelected = inrange ? i : -1;
			if (currentFlagSelected != -1)
			{
				TitleWorldMap::s_LastFlagSelected = currentFlagSelected;
				if (isDark && !m_isInShade[i])
					MiniAudioHelper::playSound(cursorSelectHandle, 15, "Sunset");
				else if (!isDark && m_isInShade[i])
					MiniAudioHelper::playSound(cursorSelectHandle, 16, "Sunrise");
			}
			m_isInShade[i] = isDark;
			flagSelectionAmount += inrange;
		}
	}

	void CapitalWindow_Update()
	{
		if (timeStageSelectDelay < 5)
			timeStageSelectDelay += 1;
		auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->
			m_CurrentPadStateIndex];
		//Selection increase
		if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickDown) && selectedCapital == 0)
		{
			selectedCapital += 1;
			MiniAudioHelper::playSound(stageSelectHandle, 0, "Cursor", false);
			CSDCommon::PlayAnimation(rcCtsChoicesSelect, "Scroll_up_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		}
		//Selection decrease
		if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickUp) && selectedCapital == 1)
		{
			selectedCapital -= 1;
			MiniAudioHelper::playSound(stageSelectHandle, 0, "Cursor", false);
			CSDCommon::PlayAnimation(rcCtsChoicesSelect, "Scroll_down_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		}
		Common::ClampInt(selectedCapital, 0, 1);

		if (inputPtr->IsTapped(Sonic::eKeyState_A) && timeStageSelectDelay >= 5)
		{
			if (selectedCapital == 0)
			{
				MiniAudioHelper::playSound(stageSelectHandle, 3, "Boot");
				System::StageManager::s_HubModeEnabled = SUC::Project::s_WorldData.data[TitleWorldMap::s_LastFlagSelected].data[
					SUC::Project::GetCapital(TitleWorldMap::s_LastFlagSelected,
						TitleWorldMap::s_Flags[TitleWorldMap::s_LastFlagSelected].night)].isWhiteWorld;

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
				MiniAudioHelper::playSound(stageSelectHandle, 2, "Accept");
				SetVisibilityStageWindow(true);
				SetVisibilityCapitalWindow(false);
				//ShowTextAct(true);
				TitleWorldMap::s_IsCapitalWindowOpened = false;
				timeStageSelectDelay = 0;
			}
		}
	}

	void SetStageSelectionScreenshot()
	{
		/// 
		/// Rewrite all of this and make it use the new blueblur image replace stuff
		/// 
		if (!rcCtsStageScreenshot)
			return;
		char nameCast[16];
		for (size_t i = 0; i < 9; i++)
		{
			sprintf(nameCast, "ss_%02dd", i + 1);
			rcCtsStageScreenshot->GetNode(nameCast)->SetHideFlag(true);
		}
		sprintf(nameCast, "ss_%02dd", TitleWorldMap::s_LastFlagSelected + 1);

		rcCtsStageScreenshot->GetNode(nameCast)->SetHideFlag(false);
	}

	int overflowCount;
	int localSelectionIndex;
	///Stage selection highlight & stage launch
	void StageWindow_Update(Sonic::CGameObject* This)
	{
		auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->
			m_CurrentPadStateIndex];
		//Pressing A requires a delay because otherwise even just tapping it will press A twice when this is called and it'll launch the stage immediately
		if (timeStageSelectDelay < 5)
			timeStageSelectDelay += 1;
		//Enables loading to the stage
		if (inputPtr->IsTapped(Sonic::eKeyState_A) && timeStageSelectDelay >= 5)
		{
			MiniAudioHelper::playSound(stageSelectHandle, 3, "Boot");
			System::StageManager::s_HubModeEnabled = SUC::Project::s_WorldData.data[TitleWorldMap::s_LastFlagSelected].data[
				TitleWorldMap::s_StageSelectWindowSelection].isWhiteWorld;

			Title::ShowLoadingTransition(true);
		}
		DebugDrawText::log(std::format("SELINDEX: {0}\nSTAGESEL: {1}", localSelectionIndex,
			TitleWorldMap::s_StageSelectWindowSelection).c_str(), 0);
		//Selection decrease
		if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickUp) && TitleWorldMap::s_StageSelectWindowSelection > 0)
		{
			if (localSelectionIndex - 1 < 7 && localSelectionIndex - 1 >= 0)
				localSelectionIndex--;
			else
			{
				if (overflowCount > 0)
					overflowCount--;
			}
			if (TitleWorldMap::s_StageSelectWindowSelection >= 0)
				TitleWorldMap::s_StageSelectWindowSelection--;
			PopulateStageSelect(TitleWorldMap::s_LastFlagSelected, overflowCount);

			MiniAudioHelper::playSound(stageSelectHandle, 0, "Cursor", false);
			CSDCommon::PlayAnimation(rcCtsStageSelect, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,
				130 - ((localSelectionIndex + 1) * 10), 130 - (localSelectionIndex) * 10);
		}

		//Selection increase
		if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickDown) && TitleWorldMap::s_StageSelectWindowSelection !=
			stageSelectedWindowMax)
		{
			bool doAnim = true;
			if (localSelectionIndex != stageSelectedWindowMax)
			{
				if (localSelectionIndex <= 5)
				{
					localSelectionIndex++;
				}
				else
				{
					doAnim = false;
					if (overflowCount < 6)
						overflowCount++;
				}
			}

			if (TitleWorldMap::s_StageSelectWindowSelection <= stageSelectedWindowMax)
				TitleWorldMap::s_StageSelectWindowSelection++;
			PopulateStageSelect(TitleWorldMap::s_LastFlagSelected, overflowCount);
			MiniAudioHelper::playSound(stageSelectHandle, 0, "Cursor", false);
			if (TitleWorldMap::s_StageSelectWindowSelection > 0 && doAnim) //technically not needed
				CSDCommon::PlayAnimation(rcCtsStageSelect, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,
					(localSelectionIndex - 1) * 10, localSelectionIndex * 10);
		}

		SetStageSelectionScreenshot();
		Common::ClampInt(TitleWorldMap::s_StageSelectWindowSelection, 0, stageSelectedWindowMax);
		PopulateStageWindowInfo(
			SUC::Project::s_WorldData.data[TitleWorldMap::s_LastFlagSelected].data[TitleWorldMap::s_StageSelectWindowSelection].levelID);
	}

	void LoadTest(Hedgehog::base::CSharedString a1, Sonic::CGameObject* a2)
	{
		uint32_t func = 0x010DA3E0;
		__asm
		{
			lea eax, a1
			mov ecx, a2
			call func
		}
	};
	HOOK(void*, __fastcall, TitleWorldMap_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx,
		float elapsedTime, uint8_t a3)
	{
		auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->
			m_CurrentPadStateIndex];

		if (TitleWorldMap::s_IsActive)
		{
			//Pretty much just "if titleworldmap exists"
			if (TitleWorldMap::s_Flags[0].flag && rcCtsGuide4Mission && rcCtsGuide5Medal)
			{
				if (rcInfoImg2->m_MotionDisableFlag) CSDCommon::PlayAnimation(
					rcInfoImg2, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 0, 0);
				if (rcInfoImg3->m_MotionDisableFlag)CSDCommon::PlayAnimation(
					rcInfoImg3, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 0, 0);
				if (rcInfoImg4->m_MotionDisableFlag)CSDCommon::PlayAnimation(
					rcInfoImg4, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 0, 0);

				multiplierRotationLight = inputPtr->IsDown(Sonic::eKeyState_X) ? 10 : 0.005;

				if (!isStageWindowOpen && !TitleWorldMapPause::s_IsPaused)
					SetCursorPos(CVector2(inputPtr->LeftStickHorizontal * cursorMultiplier,
						-inputPtr->LeftStickVertical * cursorMultiplier));

				flagSelectionAmount = 0;
				if (TitleWorldMap::s_WorldMapCamera)
				{
					if (TitleWorldMap::s_Flags[0].flag->m_MotionDisableFlag && !introPlayed)
						introPlayed = true;


					Flags_Update();

					if (flagSelectionAmount != lastflagSelectionAmount && currentFlagSelected == lastFlagIn)
					{
						if (flagSelectionAmount > 0)
						{
							//Show country info window, etc
							cursorSelected = true;
							CSDCommon::PlayAnimation(rcCtsName, "Intro_1_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,
								0);
							CSDCommon::PlayAnimation(rcCtsCursorEffect, "Select_Anim", Chao::CSD::eMotionRepeatType_Loop,
								1, 0);
							PlayCursorAnim("Select_Anim");
							rcCtsGuide4Mission->SetHideFlag(false);
							rcCtsGuide5Medal->SetHideFlag(false);
							CSDCommon::FreezeMotion(rcCtsGuideScreenshot, 0);
							CSDCommon::FreezeMotion(rcCtsGuideText, 0);
							CSDCommon::FreezeMotion(rcCtsGuideWindow, 0);
							CSDCommon::PlayAnimation(rcCtsGuideScreenshot, "Intro_Anim",
								Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
							CSDCommon::PlayAnimation(rcCtsGuideText, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,
								1, 0);
							CSDCommon::PlayAnimation(rcCtsGuideWindow, "Intro_Anim_2",
								Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

							MiniAudioHelper::playSound(cursorSelectHandle, 800004, "CursorSelect", false);

							PopulateCountryPreviewInfo(TitleWorldMap::s_LastFlagSelected);
							SetLevelTextCast(
								SUC::Project::s_WorldData.data[TitleWorldMap::s_LastFlagSelected].description.c_str());
						}
						else
						{
							//Hide country info
							cursorSelected = false;
							PlayCursorAnim("Select_Anim", true);
							rcCtsGuide4Mission->SetHideFlag(true);
							rcCtsGuide5Medal->SetHideFlag(true);
							CSDCommon::FreezeMotion(rcCtsGuideScreenshot, 0);
							CSDCommon::FreezeMotion(rcCtsGuideText, 0);
							CSDCommon::FreezeMotion(rcCtsGuideWindow, 0);
							CSDCommon::PlayAnimation(rcCtsGuideScreenshot, "Intro_Anim",
								Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, true);
							CSDCommon::PlayAnimation(rcCtsGuideText, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce,
								1, 0, 0, true, true);
							CSDCommon::PlayAnimation(rcCtsGuideWindow, "Intro_Anim_2",
								Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, true);
						}
					}
					if (flagSelectionAmount > 0 && introPlayed)
					{
						//Open Stage Selection
						if (inputPtr->IsTapped(Sonic::eKeyState_A) && !isStageWindowOpen)
						{
							isStageWindowOpen = true;
							TitleWorldMap::s_StageSelectWindowSelection = 0;
							localSelectionIndex = 0;
							PopulateStageSelect(TitleWorldMap::s_LastFlagSelected);
							SetVisibilityPlayerInfo(false);

							MiniAudioHelper::playSound(stageSelectHandle, 2, "OpenWindow", false);
							rcCtsName_2->GetNode("img_1")->SetPatternIndex(TitleWorldMap::s_LastFlagSelected);
							rcStageSelectFlag->GetNode("img")->SetPatternIndex(TitleWorldMap::s_LastFlagSelected);

							if (SUC::Project::GetCapital(TitleWorldMap::s_LastFlagSelected,
								TitleWorldMap::s_Flags[TitleWorldMap::s_LastFlagSelected].night) != -1)
							{
								TitleWorldMap::s_IsCapitalWindowOpened = true;
								ShowTextAct(false);
								SetVisibilityCapitalWindow(true);
							}
							else
							{
								SetVisibilityStageWindow(true);
							}
						}
						if (inputPtr->IsTapped(Sonic::eKeyState_B) && isStageWindowOpen)
						{
							isStageWindowOpen = false;

							SetVisibilityPlayerInfo(true);

							timeStageSelectDelay = 0;
							MiniAudioHelper::playSound(stageSelectHandle, 4, "Cancel", false);

							if (TitleWorldMap::s_IsCapitalWindowOpened)
								SetVisibilityCapitalWindow(false);
							else
								SetVisibilityStageWindow(false);
							ShowTextAct(false);
							TitleWorldMap::s_IsCapitalWindowOpened = false;
						}
						if (isStageWindowOpen && !TitleWorldMap::s_IsCapitalWindowOpened)
						{
							StageWindow_Update(This);
						}
						if (TitleWorldMap::s_IsCapitalWindowOpened)
						{
							CapitalWindow_Update();
						}
					}
					rcCtsName->SetHideFlag(!cursorSelected);
					rcCtsCursorEffect->SetHideFlag(!cursorSelected);
					lastflagSelectionAmount = flagSelectionAmount;
				}
				CheckCursorAnimStatus();
			}
		}
		return originalTitleWorldMap_UpdateApplication(This, Edx, elapsedTime, a3);
	}
#pragma region BBExts/Ahremic Title Camera

	class TransitionTitleCamera : public Sonic::CGameObject3D
	{
	public:
		int m_Unk00;
		boost::shared_ptr<TitleWorldMapCamera> m_spCamera;
	};

	inline void __cdecl ApplyCameraStuff(TransitionTitleCamera* CameraImpl, TitleWorldMapCamera* camera)
	{
		static uint32_t func = 0x10FA1D0;
		__asm
		{
			mov edi, camera
			mov esi, CameraImpl
			call func
		}
	}

	CQuaternion TitleWorldMap::QuaternionFromAngleAxis(float angle, const CVector& axis)
	{
		CQuaternion q;
		float m = sqrt(axis.x() * axis.x() + axis.y() * axis.y() + axis.z() * axis.z());
		float s = sinf(angle / 2) / m;
		q.x() = axis.x() * s;
		q.y() = axis.y() * s;
		q.z() = axis.z() * s;
		q.w() = cosf(angle / 2);
		return q;
	}

	float inline WrapAroundFloat(const float number, const float bounds)
	{
		float result = number;

		if (number > bounds)
		{
			result = number - bounds;
		}

		if (number < 0)
		{
			result = number + bounds;
		}

		return result;
	}

	float LerpEaseInOut(float start, float end, float time, bool inEnabled, bool outEnabled)
	{
		Common::ClampFloat(time, 0, 1);
		time = (time < 0.5f)
			? (inEnabled ? (2.0f * time * time) : time)
			: (outEnabled ? (-1.0f + (4.0f - 2.0f * time) * time) : time); // Ease in/out
		return start + (end - start) * time; // Interpolate and return result
	}

	void PlayPan(TitleWorldMapCamera* camera, const Hedgehog::Universe::SUpdateInfo& updateInfo)
	{
		if (timePan >= 2.5f)
		{
			playingPan = false;
			TitleWorldMap::s_TargetDisabled = false;
		}
		timePan += updateInfo.DeltaTime;
		camHeight = LerpEaseInOut(-20, 0, timePan / 2.15f, true, false);
		rotationPitch = LerpEaseInOut(-0.4f, -0.5f, timePan / 2.15f, true, true);
		cameraDistance = LerpEaseInOut(5.0f, 20.0f, timePan / 2.2f, true, true);
		//FOV = LerpEaseInOut(0.84906584f, 0.44906584f, timePan / 2.15f, false, true);
	}

	float VectorAngle(const CVector& a, const CVector& b)
	{
		const float dot = a.dot(b);
		//return acos(dot / sqrt(a.squaredNorm() * b.squaredNorm()));
		return acos(dot / sqrt(a.squaredNorm()));
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

	void MagnetizeToFlag(const CVector& flagPosition, float deltaTime)
	{
		if (playingPan || TitleWorldMap::s_TargetDisabled || !TitleWorldMap::s_IsActive)
			return;
		// Helpful thing here
		constexpr float halfway = (180.0f * DEG2RAD);
		// First, we need to convert our flag position to target radians.
		// Reminder: Yaw increases when rotating to the right, Pitch is negative facing down & positive facing up.
		// Pitch is easy. Get the angle in radians, then subtract by half max.
		const float rPitch = VectorAngle(flagPosition, CVector(0, 1, 0)) - (90.0f * DEG2RAD);
		// Yaw is tricky due to the wraparound, and because it's a planar rotation.
		// This means we actually need 0 -> 360, which involves a few steps.
		// We can't just do an angle check with forward. We need a PLANAR vector to compare with.
		// TODO: These math operations are getting expensive, so we need to really pre-compute the radians we want to use.
		const CVector planarPosition = CVector(flagPosition.x(), 0, flagPosition.z()).normalized();
		const float rInitialYaw = VectorAngle(planarPosition, CVector(0, 0, 1));
		// Our angle is an unsigned angle from 0 -> 180. We need to convert that to 0 -> 360, and... well this is how you do that lol
		const float rCorrectedYaw = flagPosition.dot(CVector(-1, 0, 0)) > 0.0f
			? (halfway - rInitialYaw) + halfway
			: rInitialYaw;
		// Now, we want to do some bullshit to make the radian lerp-towards work.
		const bool isOverfill = fabs(rotationYaw - rCorrectedYaw) > halfway;
		const bool isFlagLeft = rotationYaw > halfway;
		float rYaw = rCorrectedYaw;
		if (isOverfill)
		{
			const float compensation = (isFlagLeft ? 360.0f : -360.0f) * DEG2RAD;
			rYaw += compensation;
		}
		// Time to lerp!
		constexpr float lerpRate = 2.5f;
		// I like 3.0f, but if we want this to be more like Unleashed's, 2.5f feels about right.
		rotationPitch = lerpUnclampedf(rotationPitch, rPitch, deltaTime * lerpRate);
		rotationYaw = lerpUnclampedf(rotationYaw, rYaw, deltaTime * lerpRate);
	}

	float s_RotationAngle;
	float s_RotationAngleSpace;
	HOOK(void, __fastcall, TitleWorldMap_CameraUpdate, 0x0058CDA0, TransitionTitleCamera* This, void* Edx,
		const Hedgehog::Universe::SUpdateInfo& updateInfo)
	{
		using namespace hh::math;
		auto* const camera = This->m_spCamera.get();
		TitleWorldMap::s_WorldMapCamera = camera;
		if (!camera)
			originalTitleWorldMap_CameraUpdate(This, Edx, updateInfo);

		auto input = Sonic::CInputState::GetInstance()->GetPadState();


		// HACK: Doing camera position stuff here instead of on a "Start" function, or the constructor, lol

		if (!TitleWorldMap::s_IsWorldMapCameraInitialized)
		{
			TitleWorldMap::s_IsWorldMapCameraInitialized = true;
			camera->m_Position = CVector(0, 0, cameraDistance);
			camera->m_TargetPosition = CVector(0, 0, 0);
			light = Sonic::CGameDocument::GetInstance()->m_pMember->m_spLightManager;
		}
		// Some stuff that'll help us in the future.

		auto cameraTargetPosition = CVector(0, camHeight, 0);
		const auto cameraVector = CVector(0, 0, cameraDistance);
		const CVector cameraPosition = cameraVector + cameraTargetPosition;

		// We can make this a parameter or something later.
		constexpr float rotationPitchRate = 1.25f;
		constexpr float rotationYawRate = 1.25f;

		const CVector2 pan(input.LeftStickHorizontal, input.LeftStickVertical);
		constexpr float deadzone = 0.2f; // TODO: MAKE PARAMETER? USE SOMETHING IN GENS?


		const bool hasInput = pan.squaredNorm() > deadzone * deadzone;

		if (!TitleWorldMap::s_WorldmapCursorDisabled && !TitleWorldMapPause::s_IsPaused && hasInput && !isStageWindowOpen)
		{
			rotationPitch -= input.LeftStickVertical * rotationPitchRate * updateInfo.DeltaTime;
			rotationYaw += input.LeftStickHorizontal * rotationYawRate * updateInfo.DeltaTime;
		}
		// Do the thing where we magnetize our input.
		// TODO: Handle the HUD update here too I guess, because this is where a flag will be "selected"
		// Otherwise, make that handled somewhere else. I know you do an overlap check already, but best to do this once.
		else
		{
			constexpr float dotThreshold = 0.95f; // Value I determined to work pretty well.
			for (CVector position : m_flagPositions)
			{
				const CVector direction = (position - TitleWorldMap::s_PivotPosition).normalized();
				if (-direction.dot(camera->m_MyCamera.m_Direction) < dotThreshold && currentFlagSelected == -1)
					continue;
				if (introPlayed)
					MagnetizeToFlag(direction, updateInfo.DeltaTime);
				break;
			}
		}

		// Gotta do this nonsense.
		// UNDONE: This method wouldn't let us select Holoska, & the bottom of earth has nothing right now.
		//constexpr float pitchMaxExtents = 70.0f * DEG2RAD; // Max rotation is 70 degrees in either direction,
		//                                                   // rather than 90, which would get us to the poles of the earth.


		// Min and max extents configured differently so we aren't aimlessly rotating around the south pole (TBD)
		// and so we can actually select Holoska correctly.
		constexpr float pitchBtm = 50.0f * DEG2RAD;
		constexpr float pitchTop = 75.0f * DEG2RAD;

		// Now limit
		rotationPitch = fmax(-pitchTop, fmin(rotationPitch, pitchBtm));
		// Cycle yaw so it doesn't go over 360, so we don't approach Very Large Numbers.
		rotationYaw = WrapAroundFloat(rotationYaw, 360.0 * DEG2RAD);

		const CQuaternion pitch = TitleWorldMap::QuaternionFromAngleAxis(rotationPitch, CVector(1, 0, 0));
		const CQuaternion yaw = TitleWorldMap::QuaternionFromAngleAxis(rotationYaw, CVector(0, 1, 0));

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

		const auto lightPosition = CVector(-79.8565f, 0, 4.78983f);
		//Set up light stuff
		const float rotationForce = rotationhRateLight * multiplierRotationLight * updateInfo.DeltaTime;
		s_RotationAngle += rotationForce;
		// Wrap this around 360 degrees if you feel so inclined
		s_RotationAngle = WrapAroundFloat(s_RotationAngle, 360.0 * DEG2RAD);

		const CQuaternion lightRotation = TitleWorldMap::QuaternionFromAngleAxis(-s_RotationAngle, CVector(0, 1, 0));
		if (m_spWorldmapObject)
		{
			m_spWorldmapObject->SetRotation(lightRotation);
			DebugDrawText::log(std::format("ROTATION0: {0}, {1}, {2}", m_spWorldmapObject->m_spMatrixNodeTransform->m_Transform.m_Rotation.x(), m_spWorldmapObject->m_spMatrixNodeTransform->m_Transform.m_Rotation.y(), m_spWorldmapObject->m_spMatrixNodeTransform->m_Transform.m_Rotation.z()).c_str(), 0);
			//DebugDrawText::log(std::format("ROTATION1: {0}, {1}, {2}", m_spWorldmapObject->m_spModelButtonTransform->m_Transform.m_Rotation.x(), m_spWorldmapObject->m_spModelButtonTransform->m_Transform.m_Rotation.y(), m_spWorldmapObject->m_spModelButtonTransform->m_Transform.m_Rotation.z()).c_str(), 0);
		}
		light->m_GlobalLightDirection = lightRotation * (lightPosition - TitleWorldMap::s_PivotPosition) +
			TitleWorldMap::s_PivotPosition;
		//Set light properties
		light->m_GlobalLightDiffuse = CVector(0.02f, 0.02f, 0.02f);
		//light->m_GlobalLightDirection = CVector(-79.8565f, 0, 4.78983f);

		light->m_GlobalLightSpecular = CVector(5, 5, 5);



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
	void TitleWorldMap::RegisterHooks()
	{
		WRITE_JUMP(0x00584CEE, (void*)0x00588820);
		WRITE_JUMP(0x015B8188, (void*)0x015B8198);
		WRITE_JUMP(0x0058D41F, (void*)0x0058D7D8); //Skip setting light properties every second

		WRITE_JUMP(0x00D0A3F0, (void*)0x00D0A4C1); //Ignore PAM position & rotation
		INSTALL_HOOK(TitleWorldMap_CameraUpdate);
		INSTALL_HOOK(TitleWorldMap_CTitleMain);

		INSTALL_HOOK(UpdatePar);
		INSTALL_HOOK(sub_58C800);
		WRITE_JUMP(0x0058C481, 0x0058C651);

		//INSTALL_HOOK(TitleAddRenderables);
		INSTALL_HOOK(TitleWorldMap_UpdateApplication);
		WRITE_MEMORY(0x016E11F4, void*, CTitleWRemoveCallback);

		//From Brianuu's 06 Title, makes saving not crash
		WRITE_JUMP(0xD22A83, (void*)0xD22B84);
		WRITE_MEMORY(0xD22CE8, uint8_t, 0);
		//WRITE_JUMP(0x00A51B66, 0x00A51BCE);
	}

}