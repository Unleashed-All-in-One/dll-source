#include "TitleWorldMapPause.h"

#include "Title.h"
#include "TitleWorldMap.h"
using namespace hh::math;
namespace SUC::UI::TitleScreen
{

	bool TitleWorldMapPause::s_IsPaused;


	Chao::CSD::RCPtr<Chao::CSD::CProject> rcWMPause;
	boost::shared_ptr<Sonic::CGameObjectCSD> spWMPause;
	Chao::CSD::RCPtr<Chao::CSD::CScene> pauseBG, pauseHeader, pauseWindow, pauseSelect, pauseText;
	int m_cursorPos = 0;
	uint32_t m_prevCursorPos = 0;
	bool active;
	SharedPtrTypeless m_SoundHandleEnter, m_SoundHandleExit, m_SoundHandleCursor, m_SoundHandleAccept;
	float originalVolume;
	void TitleWorldMapPause::Start()
	{


	}

	void __fastcall CTitleWorldmapPauseRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
	{
		TitleWorldMapPause::KillScreen();
		Chao::CSD::CProject::DestroyScene(rcWMPause.Get(), pauseBG);
		Chao::CSD::CProject::DestroyScene(rcWMPause.Get(), pauseHeader);
		Chao::CSD::CProject::DestroyScene(rcWMPause.Get(), pauseWindow);
		Chao::CSD::CProject::DestroyScene(rcWMPause.Get(), pauseSelect);
		Chao::CSD::CProject::DestroyScene(rcWMPause.Get(), pauseText);

		rcWMPause = nullptr;
	}
	void TitleWorldMapPause::CreateScreen(Sonic::CGameObject* pParentGameObject)
	{
		if (rcWMPause && !spWMPause)
		{
			pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spWMPause = boost::make_shared<Sonic::CGameObjectCSD>(rcWMPause, 0.5f, "HUD_Pause", false), "main", pParentGameObject);
		}
	}
	void TitleWorldMapPause::KillScreen()
	{
		if (spWMPause)
		{
			spWMPause->SendMessage(spWMPause->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
			spWMPause = nullptr;
		}
	}
	void TitleWorldMapPause::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
	{
		if (visible)
			CreateScreen(pParentGameObject);
		else
			KillScreen();
	}

	HOOK(int, __fastcall, TitleWorldMapPause_CTitleMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3, void** a4)
	{
		CTitleWorldmapPauseRemoveCallback(This, nullptr, nullptr);

		Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
		auto spCsdProject = wrapper.GetCsdProject("ui_worldmap_pause");
		rcWMPause = spCsdProject->m_rcProject;

		pauseBG = rcWMPause->CreateScene("bg");
		pauseBG->SetHideFlag(true);
		pauseHeader = rcWMPause->CreateScene("status_title");
		pauseHeader->SetHideFlag(true);
		pauseWindow = rcWMPause->CreateScene("bg_1");
		pauseWindow->SetHideFlag(true);
		pauseSelect = rcWMPause->CreateScene("bg_1_select");
		pauseSelect->SetHideFlag(true);
		pauseWindow->SetPosition(0, 20);
		pauseText = rcWMPause->CreateScene("pause_text");
		pauseText->SetHideFlag(true);
		TitleWorldMapPause::CreateScreen(This);
		return originalTitleWorldMapPause_CTitleMain(This, Edx, a2, a3, a4);
	}
	void HudPause_OpenPauseWindow(bool isPam)
	{
		m_cursorPos = 0;

		if (!rcWMPause) return;
		pauseWindow->GetNode("center")->SetScale(62.0f, 1.25f);
		pauseWindow->GetNode("text_area")->SetScale(62.0f, 0.9f);
		CSDCommon::PlayAnimation(pauseWindow, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	void HudPause_OpenPauseScreen(bool isPam)
	{
		if (!rcWMPause) return;
		CSDCommon::PlayAnimation(pauseBG, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(pauseHeader, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

		pauseSelect->SetPosition(0, -74.0f);
		pauseSelect->GetNode("img")->SetScale(38.0f, 1.0f);
		CSDCommon::PlayAnimation(pauseSelect, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		pauseSelect->SetMotion("Scroll_Anim");
		CSDCommon::FreezeMotion(pauseSelect, 0);

		pauseBG->SetHideFlag(false);
		pauseHeader->SetHideFlag(false);
		pauseWindow->SetHideFlag(false);
		pauseSelect->SetHideFlag(false);
		HudPause_OpenPauseWindow(isPam);
	}

	void HudPause_ClosePauseScreen()
	{
		if (!rcWMPause) return;
		pauseText->SetHideFlag(true);
		Common::PlaySoundStatic(m_SoundHandleExit, 1000001);
		active = false;
		TitleWorldMapPause::s_IsPaused = false;
		pauseBG->SetHideFlag(true);
		pauseHeader->SetHideFlag(true);
		pauseWindow->SetHideFlag(true);
		pauseSelect->SetHideFlag(true);
	}
	void ChangeSelection()
	{
		if (m_prevCursorPos != m_cursorPos)
			Common::PlaySoundStatic(m_SoundHandleCursor, 1000004);
		Common::ClampInt(m_cursorPos, 0, 5);
		CSDCommon::PlayAnimation(pauseText, "Intro_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		CSDCommon::PlayAnimation(pauseText, std::format("Select_{0}", m_cursorPos + 1).c_str(), Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		CSDCommon::PlayAnimation(pauseSelect, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		CSDCommon::FreezeMotion(pauseSelect, 0);
		//if (m_prevCursorPos == m_cursorPos + 1)
		//{
		//	// Scroll up
		//	CSDCommon::PlayAnimation(pauseSelect, "Scroll_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 120.0f - m_cursorPos * 10.0f, 120.0f - m_prevCursorPos * 10.0f, false, true);
		//}
		//else if (m_prevCursorPos == m_cursorPos - 1)
		//{

		//	CSDCommon::PlayAnimation(pauseSelect, "Scroll_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, m_prevCursorPos * 10.0f, m_cursorPos * 10.0f);
		//	// Scroll down
		//}
		//else
		//{
		CSDCommon::PlayAnimation(pauseSelect, "Scroll_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::FreezeMotion(pauseSelect, m_cursorPos * 10.0f);
		//}
	}
	void PauseCase(int pos)
	{
		Common::PlaySoundStatic(m_SoundHandleAccept, 1000005);

		switch (pos)
		{
		case 0:
		{
			HudPause_ClosePauseScreen();
			break;
		}
		case 1:
		{
			DebugDrawText::log("I don't know how to summon the Skills Shop, so this message will do for now.");

			break;
		}
		case 2:
		{
			DebugDrawText::log("I don't know how to summon the Skills Inventory, so this message will do for now.");

			break;
		}
		case 3:
		{
			DebugDrawText::log("Exit");
			TitleWorldMap::SetHideEverything(true);
			TitleWorldMap::s_IsActive = false;
			Title::SetSubMenu(false);
			Title::ToggleUI(false);
			break;
		}
		case 4:
		{
			StageManager::ActiveReplacement = false;
			StageManager::NextLevelLoad = "pla201";
			StageManager::LoadingReplacementEnabled = true;
			SequenceHelpers::loadStage("pla201");
			break;
		}
		}
	}
	HOOK(void*, __fastcall, TitleWorldMapPause_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx, float elapsedTime, uint8_t a3)
	{
		if (!TitleWorldMap::s_IsActive)
			return originalTitleWorldMapPause_UpdateApplication(This, Edx, elapsedTime, a3);

		auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
		if (inputPtr->IsTapped(Sonic::eKeyState_Start) && TitleWorldMap::s_IsActive && !active)
		{
			HudPause_OpenPauseScreen(false);
			Common::PlaySoundStatic(m_SoundHandleEnter, 1000000);
			active = true;
			TitleWorldMapPause::s_IsPaused = true;
			pauseText->SetHideFlag(false);
		}

		if (active)
		{

			if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickDown))
			{
				m_cursorPos += 1;
				ChangeSelection();
			}
			if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickUp))
			{
				m_cursorPos -= 1;
				ChangeSelection();
			}
			if (inputPtr->IsTapped(Sonic::eKeyState_B))
			{
				HudPause_ClosePauseScreen();
			}
			if (inputPtr->IsTapped(Sonic::eKeyState_A))
			{
				PauseCase(m_cursorPos);
			}
		}
		m_prevCursorPos = m_cursorPos;
		return originalTitleWorldMapPause_UpdateApplication(This, Edx, elapsedTime, a3);
	}

	void TitleWorldMapPause::RegisterHooks()
	{
		INSTALL_HOOK(TitleWorldMapPause_CTitleMain);
		INSTALL_HOOK(TitleWorldMapPause_UpdateApplication);
		WRITE_MEMORY(0x016E11F4, void*, CTitleWorldmapPauseRemoveCallback);

		////From Brianuu's 06 Title, makes saving not crash
		//WRITE_JUMP(0xD22A83, (void*)0xD22B84);
		//WRITE_MEMORY(0xD22CE8, uint8_t, 0);

		//WRITE_JUMP(0x00A51B66, 0x00A51BCE);
	}
}