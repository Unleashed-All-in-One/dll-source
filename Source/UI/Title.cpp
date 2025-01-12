#include "Title.h"
#include "../System/MiniAudioHelper.h"
#include "../System/AspectRatioHelper.h"

#include "TitleWorldMap.h"

namespace SUC::UI::TitleScreen
{
	Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreen;
	Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreenLogos;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcTitleLogo_1, rcTitlebg, rcTitleMenu, rcTitleMenuScroll, rcTitleMenuTXT, black_bg, bg_window, fg_window, txt_window, footer_window, bg_transition;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcTitleProgressbar;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcConversionLogo;
	boost::shared_ptr<Sonic::CGameObjectCSD> spTitleScreen;
	boost::shared_ptr<Sonic::CGameObjectCSD> spTitleScreenLogos;
	boost::shared_ptr<Sonic::StageSelectMenu::CDebugStageSelectMenuXml> spDebugMenu;
	std::function<void()> onCompleteFunc;
	Title::ETitleIndexState currentTitleIndex = Title::ETitleIndexState::New_Game;
	Hedgehog::Math::CVector2* scale = new Hedgehog::Math::CVector2(1, 1);
	bool enteredStart, isInSubmenu, moved, hasSavefile, playingStartAnim, reversedAnim, isStartAnimComplete, startButtonAnimComplete, startBgAnimComplete, parsedSave, showedWindow = false;
	bool Title::ms_IsWorldMapActive = false;
	bool inTitle, scrollHorizontally = true;
	int canLoad = 0;
	int maxTitleIndex = 3;
	int holdTimer = 0;
	bool inOptions = false;

	boost::shared_ptr<Sonic::CCamera> spCamera;
	static uint32_t cameraInitRan = 0;
	static boost::shared_ptr<Hedgehog::Sound::CSoundHandle> stageSelectHandle;
	static boost::shared_ptr<Hedgehog::Sound::CSoundHandle> optionsHandle;
	void* TitleStateContextBase;


	FUNCTION_PTR(void, __thiscall, TitleUI_TinyChangeState, 0x773250, void* This, SharedPtrTypeless& spState, const Hedgehog::Base::CSharedString name);
	void Title::SetSubMenu(bool enabled)
	{
		isInSubmenu = enabled;
	}
	void Title::SetScrollDirection(bool horizontal)
	{
		scrollHorizontally = horizontal;
	}
	void ExitingTitle()
	{
		Title::SetScrollDirection(false);
		Title::SetSubMenu(false);
		inTitle = false;
		isStartAnimComplete = false;
	}
	void PlayTitleBGM(void* baseClass, const char* cueName)
	{
		uint32_t func = 0x00570620;
		__asm
		{
			mov esi, cueName
			mov eax, baseClass
			call func
		};
	};
	void __declspec(naked) TitleUI_SetCutsceneTimer()
	{
		static uint32_t pAddr = 0x00571FD1;
		static uint32_t pAddr2 = 0x05722FF;
		static float f = 93.0f;
		__asm
		{
			movss xmm1, f
			comiss xmm0, xmm1
			jg Goto2
			jmp Goto1
			Goto2 :

			jmp[pAddr]

				Goto1 :
				jmp[pAddr2]
		}
	}
	void ContinueToWM()
	{
		if (!Title::ms_IsWorldMapActive)
		{
			Title::ms_IsWorldMapActive = true;
			Title::ToggleUI(true, true);
			PlayTitleBGM(TitleStateContextBase, "Option");
			CSDCommon::FreezeMotion(rcTitleLogo_1);
			TitleWorldMap::Start();
			TitleWorldMap::EnableInput();
		}
	}
	void ShowInstallScreen()
	{
		PlayTitleBGM(TitleStateContextBase, "");
		black_bg->SetHideFlag(false);
		static boost::shared_ptr<hh::db::CRawData> rawData;
		auto database = hh::db::CDatabase::CreateDatabase();
		auto& databaseLoader = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spDatabaseLoader;
		databaseLoader->CreateArchiveList("#SelectStage.ar", "#SelectStage.arl", { 0, 0 });
		databaseLoader->LoadArchiveList(database, "#SelectStage.arl");
		databaseLoader->LoadArchive(database, "#SelectStage.ar", { 0, 0 }, false, false);
		spDebugMenu = boost::make_shared<Sonic::StageSelectMenu::CDebugStageSelectMenuXml>();
		Sonic::CGameDocument::GetInstance()->AddGameObject(spDebugMenu);
		System::StageManager::s_LoadingReplacementEnabled = false;
	}
	bool waitingtest;
	void Title::ShowLoadingTransition(bool enableLoad, const std::function<void()>& onComplete)
	{
		onCompleteFunc = onComplete;
		UnleashedHUD_API::StartFadeout();
		PlayTitleBGM(TitleStateContextBase, "");
		if (enableLoad)
			waitingtest = 1;
	}
	void __declspec(naked) TitleUI_SetCustomExecFunction()
	{
		//https://godbolt.org/
		static uint32_t pAddr = 0x00572E27;
		static uint32_t adNewGame = 0x00572D33;
		static uint32_t adContinue = 0x00572E2F;
		static uint32_t adOptions = 0x00573008;
		static uint32_t adQuit = 0x00573153;
		if (SUC::Project::menuType <= 1)
		{
			__asm // this is an else if chain. i wanted to do a switch statement but it didnt work.
			{
				cmp     currentTitleIndex, 0
				jne	Options
				jmp[adNewGame]
				jmp	FunctionFinish

				Continue :
				cmp     currentTitleIndex, 1
					jne	Options
					call ContinueToWM
					jmp[adContinue]
					jmp	FunctionFinish

					Options :
				cmp     currentTitleIndex, 2
					jne Quit
					jmp[adOptions]
					jmp	FunctionFinish

					Quit :
				cmp     currentTitleIndex, 3
					jne	InstallScreen
					mov isInSubmenu, 1
					jmp[adQuit]

					InstallScreen :
					cmp     currentTitleIndex, 4
					jne	FunctionFinish
					call ShowInstallScreen
					jmp FunctionFinish

					FunctionFinish :
				jmp[pAddr]
			}
		}
	}
	void OnNewGame()
	{
		Title::ShowLoadingTransition(false);
		System::StageManager::s_InStoryMode = true;
		System::SaveManager::DeleteSave();
		auto save = System::SaveManager::GetCurrentSave();
		save->m_Lives = 5;
		UnleashedHUD_API::StartFadeout();
		//SaveManager::SaveToDisk();
	}
	//void __declspec(naked) TitleUI_SetCustomExecFunctionAdvance()
	//{
	//	//https://godbolt.org/
	//	static uint32_t pAddr = 0x0057372E;
	//	static uint32_t adNewGame = 0x0057339E;
	//	static uint32_t adContinue = 0x0057342B;
	//	static uint32_t adOptions = 0x00573557;
	//	static uint32_t adQuit = 0x0057364F;
	//	__asm // NEW: 0| CONTINUE:1 | OPTIONS: 3 | QUIT:  4
	//	{
	//		cmp     currentTitleIndex, 0
	//		jne	Continue
	//		mov isInSubmenu, 1
	//		call ExitingTitle
	//		jmp NewGame
	//		jmp	FunctionFinish
	//
	//		NewGame :
	//		call OnNewGame
	//			jmp[adNewGame]
	//			Continue :
	//			cmp     currentTitleIndex, 1
	//			jne	Options
	//			call ExitingTitle
	//			call ContinueToWM
	//			cmp canLoad, 2
	//			je LoadingForContinue
	//			jmp	FunctionFinish
	//
	//			Options :
	//		cmp     currentTitleIndex, 2
	//			jne Quit
	//			jmp[adOptions]
	//			jmp	FunctionFinish
	//
	//			Quit :
	//		cmp     currentTitleIndex, 3
	//			jne	FunctionFinish
	//			jmp[adQuit]
	//
	//
	//			FunctionFinish :
	//			jmp[pAddr]
	//
	//			LoadingForContinue :
	//			jmp[adContinue]
	//	}
	//}


	//old but dont remove

	//void __declspec(naked) TitleUI_MoveUp()
	//{
	//	static uint32_t pAddr = 0x010BA693;
	//	static uint32_t movement = 0x80000;
	//	if (inTitle && scrollHorizontally && !isInSubmenu) //this is global to all of gens for some reason
	//		movement = 0x200000;
	//	else
	//		movement = 0x80000;
	//	__asm
	//	{
	//		test    esi, movement
	//		jmp[pAddr]
	//	}
	//}
	//void __declspec(naked) TitleUI_MoveDown()
	//{
	//	static uint32_t pAddr = 0x010BA6A4;
	//	static uint32_t movement = 0x40000;
	//	if (inTitle && scrollHorizontally) //this is global to all of gens for some reason
	//		movement = 0x100000;
	//	else
	//		movement = 0x40000;
	//	__asm
	//	{
	//		test    ebx, movement
	//		jmp[pAddr]
	//	}
	//}
	void __fastcall CTitleRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
	{
		//rcTitleLogo_1, rcTitlebg, rcTitleMenu, rcTitleMenuScroll, rcTitleMenuTXT, black_bg, bg_window, fg_window, txt_window, footer_window, bg_transition
		Title::KillScreen();
		Chao::CSD::CProject::DestroyScene(rcTitleScreenLogos.Get(), rcTitleLogo_1);
		if (rcConversionLogo)
			Chao::CSD::CProject::DestroyScene(rcTitleScreenLogos.Get(), rcConversionLogo);
		Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitlebg);
		Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleMenu);
		Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleMenuScroll);
		Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleMenuTXT);
		Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), black_bg);
		Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), bg_window);
		Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), fg_window);
		Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), txt_window);
		Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), footer_window);
		Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), bg_transition);

		//rcTitleOptionTXT1, rcTitleOptionTXT2, rcTitleOptionTXT3, rcTitleProgressbar
		Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleProgressbar);
		rcTitleScreen = nullptr;
		rcTitleScreenLogos = nullptr;
		enteredStart, isInSubmenu, moved, hasSavefile, playingStartAnim, reversedAnim, isStartAnimComplete, startButtonAnimComplete, startBgAnimComplete, showedWindow = false;
		inTitle, parsedSave = false;
		currentTitleIndex = Title::ETitleIndexState::New_Game;
		holdTimer = 0;
		canLoad = 0;
		Title::ms_IsWorldMapActive = 0;
	}
	void Title::ToggleUI(bool visible, bool logoVisible)
	{
		if (rcTitleLogo_1)
		{
			/*if (logoVisible)
			{
				CSDCommon::PlayAnimation(rcTitleLogo_1, "Outro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			}
			else*/
			rcTitleLogo_1->SetHideFlag(visible);
		}
		if (rcTitlebg)          rcTitlebg->SetHideFlag(visible);
		if (rcTitleMenu)        rcTitleMenu->SetHideFlag(visible);
		if (rcTitleMenuScroll)  rcTitleMenuScroll->SetHideFlag(visible);
		if (rcTitleMenuTXT)     rcTitleMenuTXT->SetHideFlag(visible);
		if (rcConversionLogo)   rcConversionLogo->SetHideFlag(visible);
		if (rcTitleProgressbar) rcTitleProgressbar->SetHideFlag(visible);
	}
	void Title::CreateScreen(Sonic::CGameObject* pParentGameObject)
	{
		if (rcTitleScreenLogos && !spTitleScreenLogos)
		{
			pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spTitleScreenLogos = boost::make_shared<Sonic::CGameObjectCSD>(rcTitleScreenLogos, 0.5f, "HUD", false), "main", pParentGameObject);
		}
		if (rcTitleScreen && !spTitleScreen)
		{
			pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spTitleScreen = boost::make_shared<Sonic::CGameObjectCSD>(rcTitleScreen, 0.5f, "HUD", false), "main", pParentGameObject);
		}
	}
	void Title::KillScreen()
	{
		if (spTitleScreen)
		{
			spTitleScreen->SendMessage(spTitleScreen->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
			spTitleScreen = nullptr;
		}
		if (spTitleScreenLogos)
		{
			spTitleScreenLogos->SendMessage(spTitleScreenLogos->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
			spTitleScreenLogos = nullptr;
		}
	}
	void Title::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
	{
		if (visible)
			CreateScreen(pParentGameObject);
		else
			KillScreen();
	}
	void Title::IntroAnimation(Chao::CSD::RCPtr<Chao::CSD::CScene> scene)
	{
		scene->SetMotion("Intro_Anim");
		scene->SetMotionFrame(0.0f);
		scene->m_MotionSpeed = 1;
		scene->m_MotionRepeatType = Chao::CSD::eMotionRepeatType_PlayOnce;
		scene->Update(0.0f);
	}
	void UpdateTitleText()
	{
		if (currentTitleIndex < 0)
			currentTitleIndex = Title::ETitleIndexState::Quit;
		else if (currentTitleIndex > maxTitleIndex)
			currentTitleIndex = Title::ETitleIndexState::New_Game;

		if (!rcTitleMenuTXT)
			return;

		rcTitleMenuTXT->SetPosition(0, 0);
		rcTitleMenuTXT->GetNode("txt_0")->SetHideFlag(!(currentTitleIndex == 0));
		rcTitleMenuTXT->GetNode("txt_1")->SetHideFlag(!(currentTitleIndex == 1));
		rcTitleMenuTXT->GetNode("txt_2")->SetHideFlag(!(currentTitleIndex == 2));
		rcTitleMenuTXT->GetNode("txt_3")->SetHideFlag(!(currentTitleIndex == 3));
		rcTitleMenuTXT->GetNode("txt_4")->SetHideFlag(!(currentTitleIndex == 4));
	}
	HOOK(void, __fastcall, Title_CMain_CState_SelectMenuBegin, 0x572750, hh::fnd::CStateMachineBase::CStateBase* This)
	{
		/*originalTitle_CMain_CState_SelectMenuBegin(This);*/
		isInSubmenu = false;
		if (!parsedSave)
		{
			auto saveObject = System::SaveManager::GetCurrentSave(false);
			//hasSavefile = saveObject != nullptr;
			hasSavefile = true;
			currentTitleIndex = hasSavefile ? Title::ETitleIndexState::Continue : Title::ETitleIndexState::New_Game;
			UpdateTitleText();
			parsedSave = true;
		}
		TitleStateContextBase = This->GetContextBase();
	}

	HOOK(int, __fastcall, Title_CMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3, void** a4)
	{

		CTitleRemoveCallback(This, nullptr, nullptr);
		Title::SetScrollDirection(true);
		Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
		auto spCsdProject = wrapper.GetCsdProject("ui_title_unleashed");
		auto spCsdProjectLogos = wrapper.GetCsdProject("ui_title_logos");
		rcTitleScreen = spCsdProject->m_rcProject;
		rcTitleScreenLogos = spCsdProjectLogos->m_rcProject;
		char buffer[8];
		//s_LogoType
		//0: Conversion
		//1: USA
		//2: JPN
		//3: Generations Demo
		sprintf(buffer, "title_%d", SUC::Project::s_LogoType == 0 || SUC::Project::s_LogoType == 1 ? 1 : SUC::Project::s_LogoType);
		rcTitleLogo_1 = rcTitleScreenLogos->CreateScene(buffer);



		if (SUC::Project::s_LogoType == 0)
		{
			rcConversionLogo = rcTitleScreenLogos->CreateScene("conversionlogo");
			rcTitleLogo_1->SetPosition(68, 3);
			rcTitleLogo_1->SetScale(0.9f, 0.9f);
			rcConversionLogo->SetPosition(68, 3);
			rcConversionLogo->SetScale(0.9f, 0.9f);
			CSDCommon::PlayAnimation(rcConversionLogo, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		}
		switch (SUC::Project::menuType)
		{
		case 0:
		{
			maxTitleIndex = 3;
			break;
		}
		case 1:
		{
			maxTitleIndex = 4;
			break;
		}
		case 2:
		{
			maxTitleIndex = 1;
			break;
		}
		}
		rcTitlebg = rcTitleScreen->CreateScene("bg");
		rcTitleMenu = rcTitleScreen->CreateScene("menu");
		rcTitleMenuScroll = rcTitleScreen->CreateScene("menu_scroll");
		rcTitleMenuTXT = rcTitleScreen->CreateScene("txt");
		rcTitleProgressbar = rcTitleScreen->CreateScene("progress");
		black_bg = rcTitleScreen->CreateScene("black_bg");
		bg_window = rcTitleScreen->CreateScene("bg_window");
		fg_window = rcTitleScreen->CreateScene("window_box");
		txt_window = rcTitleScreen->CreateScene("window_text");
		footer_window = rcTitleScreen->CreateScene("window_footer");
		bg_transition = rcTitleScreen->CreateScene("bg_transition");
		bg_window->SetHideFlag(true);
		fg_window->SetHideFlag(true);
		txt_window->SetHideFlag(true);
		footer_window->SetHideFlag(true);
		bg_transition->SetHideFlag(true);
		rcTitleProgressbar->SetHideFlag(true);
		CSDCommon::PlayAnimation(fg_window, "Size_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
		CSDCommon::FreezeMotion(fg_window, 48 /* TIMES the maximum characters in a line in the text.*/);
		black_bg->SetHideFlag(true);
		if (rcTitleMenuTXT)
		{
			rcTitleMenuTXT->GetNode("txt_4")->SetHideFlag(true);
			CSDCommon::FreezeMotion(rcTitleMenuTXT);
			rcTitleMenuTXT->SetPosition(0, 650000);
		}
		if (rcTitleMenuScroll)
		{
			CSDCommon::PlayAnimation(rcTitleMenuScroll, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
			CSDCommon::FreezeMotion(rcTitleMenuScroll, 0);
			rcTitleMenuScroll->SetPosition(0, 650000);
		}
		if (rcTitleMenu)
		{
			CSDCommon::FreezeMotion(rcTitleMenu);
			CSDCommon::PlayAnimation(rcTitleMenu, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		}
		if (rcTitlebg)
		{
			CSDCommon::FreezeMotion(rcTitlebg);
			CSDCommon::PlayAnimation(rcTitlebg, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		}
		if (rcTitleLogo_1)
			CSDCommon::PlayAnimation(rcTitleLogo_1, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

		playingStartAnim = true;
		Title::CreateScreen(This);
		return originalTitle_CMain(This, Edx, a2, a3, a4);
	}
	HOOK(DWORD, *__cdecl, Title_CMain_CState_SelectMenu, 0x11D1210, hh::fnd::CStateMachineBase::CStateBase* This)
	{
		if (enteredStart)
			return originalTitle_CMain_CState_SelectMenu(This);

		TitleWorldMap::PlayPanningAnim();
		if (rcTitleMenu)
		{
			CSDCommon::FreezeMotion(rcTitleMenu);
			rcTitleMenu->SetPosition(0, 0);
		}
		if (rcTitlebg)
			CSDCommon::PlayAnimation(rcTitlebg, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		if (rcTitleMenu)
			CSDCommon::PlayAnimation(rcTitleMenu, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		if (rcTitleMenuTXT)
		{
			CSDCommon::PlayAnimation(rcTitleMenuTXT, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			rcTitleMenuTXT->SetPosition(0, 0);
		}
		if (rcTitleMenuScroll)
			rcTitleMenuScroll->SetPosition(0, 0);

		enteredStart = true;
		UpdateTitleText();

		return originalTitle_CMain_CState_SelectMenu(This);
	}
	HOOK(int, __cdecl, Title_CMain_CState_WaitStart, 0x11D1410, hh::fnd::CStateMachineBase::CStateBase* a1)
	{
		//if(!titleMusicHandle)
		//Common::playSoundStatic(titleMusicHandle, 800030);
		if (enteredStart && rcTitleMenu && rcTitlebg)
		{
			CSDCommon::PlayAnimation(rcTitleMenu, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, false, true);
			CSDCommon::PlayAnimation(rcTitlebg, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 30, 100, false, true);
			currentTitleIndex = Title::ETitleIndexState::New_Game;
		}
		if (rcTitleMenuTXT)
			rcTitleMenuTXT->SetPosition(0, 65000);
		if (rcTitleMenuScroll)
			rcTitleMenuScroll->SetPosition(0, 65000);


		reversedAnim = true;
		playingStartAnim = true;
		isStartAnimComplete = false;
		inTitle = true;
		enteredStart = false;
		return originalTitle_CMain_CState_WaitStart(a1);
	}
	bool IsLeftDown() {
		auto inputState = Sonic::CInputState::GetInstance();
		auto inputPtr = &inputState->m_PadStates[inputState->m_CurrentPadStateIndex];
		return inputPtr->IsDown(Sonic::eKeyState_LeftStickLeft) || inputPtr->IsDown(Sonic::eKeyState_DpadLeft);
	}
	bool IsRightDown() {
		auto inputState = Sonic::CInputState::GetInstance();
		auto inputPtr = &inputState->m_PadStates[inputState->m_CurrentPadStateIndex];
		return inputPtr->IsDown(Sonic::eKeyState_LeftStickRight) || inputPtr->IsDown(Sonic::eKeyState_DpadRight);
	}
	int saveOffset;
	HOOK(void*, __fastcall, Title_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx, float elapsedTime, uint8_t a3)
	{
		auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];

		if (waitingtest)
		{
			if (bg_transition)
			{

				//if (UnleashedHUD_API::IsLoadingFadeoutCompleted())
				//{
					if(onCompleteFunc != nullptr)
						onCompleteFunc();
					//System::SequenceHelpers::LoadStage(System::StageManager::GetStageToLoad());
					//System::SequenceHelpers::SetPlayerType(GENERIC_SONIC);
					waitingtest = 0;
				//}
			}
		}

		if (inTitle && !Title::ms_IsWorldMapActive)
		{

			if (currentTitleIndex == 3 && isInSubmenu && inputPtr->IsDown(Sonic::eKeyState_B)) //janky way of knowing when the quit prompt is aborted
				isInSubmenu = false;

			if (rcTitleLogo_1) //Logo loop anim
			{
				if (rcTitleLogo_1->m_MotionDisableFlag == 1 && !isStartAnimComplete && playingStartAnim)
				{
					isStartAnimComplete = true;
					CSDCommon::PlayAnimation(rcTitleLogo_1, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_Loop, 1, 0);

				}
			}
			if (rcTitleMenu) //PRESS START loop anim
			{
				if (rcTitleMenu->m_MotionDisableFlag == 1 && !isStartAnimComplete && playingStartAnim)
				{
					CSDCommon::PlayAnimation(rcTitleMenu, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
				}
			}
			if (rcTitlebg && enteredStart) //Pulse header anim
			{
				if (rcTitlebg->m_MotionDisableFlag == 1 && !startBgAnimComplete && playingStartAnim)
				{
					startBgAnimComplete = true;
					CSDCommon::PlayAnimation(rcTitlebg, "Usual_Anim_2", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
				}
			}
#pragma region UIScroll
			if (rcTitleMenu && !isInSubmenu && enteredStart)
			{
				if (IsRightDown() && !moved)
				{
					currentTitleIndex = (Title::ETitleIndexState)((int)(currentTitleIndex)+1);
					if (currentTitleIndex > maxTitleIndex)
						currentTitleIndex = Title::ETitleIndexState::New_Game;

					if (currentTitleIndex == Title::ETitleIndexState::Continue && !hasSavefile)
						currentTitleIndex = Title::ETitleIndexState::Options;
					Common::PlaySoundStaticCueName(stageSelectHandle, "sys_worldmap_cursor");
					CSDCommon::PlayAnimation(rcTitleMenuScroll, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					CSDCommon::PlayAnimation(rcTitleMenu, "Scroll_Anim_2_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 100, false, true);
					UpdateTitleText();
					moved = true;
				}
				if (IsLeftDown() && !moved)
				{
					currentTitleIndex = (Title::ETitleIndexState)((int)(currentTitleIndex)-1);
					Common::PlaySoundStaticCueName(stageSelectHandle, "sys_worldmap_cursor");
					if (currentTitleIndex < 0 || currentTitleIndex > maxTitleIndex) //uint32s become huge when going negative
						currentTitleIndex = (Title::ETitleIndexState)maxTitleIndex;

					if (currentTitleIndex == Title::ETitleIndexState::Continue && !hasSavefile)
						currentTitleIndex = Title::ETitleIndexState::New_Game;
					CSDCommon::PlayAnimation(rcTitleMenuScroll, "Scroll_Anim_2_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					CSDCommon::PlayAnimation(rcTitleMenu, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 100, false, true);
					UpdateTitleText();
					moved = true;
				}

				if ((!IsLeftDown() && !IsRightDown()) && moved)
				{
					holdTimer += 1;
					moved = false;
					holdTimer = 0;
				}
				if ((IsLeftDown() || IsRightDown()) && moved)
				{
					holdTimer = holdTimer + 1.0f;
					printf("\nTIMER %d", holdTimer);
					if (holdTimer > 20)
					{
						if (currentTitleIndex > 0 && currentTitleIndex < maxTitleIndex)
							moved = false;
						holdTimer = 0;
					}
				}

			}
#pragma endregion
		}

		return originalTitle_UpdateApplication(This, Edx, elapsedTime, a3);
	}
	HOOK(int, __fastcall, Title_CMain_ExecSubMenu, 0x572D00, DWORD* This)
	{
		int returned = 0;
		if (currentTitleIndex > 2 && !isInSubmenu)
		{
			isInSubmenu = true;
			returned = originalTitle_CMain_ExecSubMenu(This);
		}
		else if (currentTitleIndex < 2)
			isInSubmenu = false;

		if (!isInSubmenu && currentTitleIndex != 1)
			return originalTitle_CMain_ExecSubMenu(This);
		else
			return returned; //will 9/10 times return 0

	}
	void AttachConverseText(Sonic::CGameObject* a1, void* objectReturn, const char* fcoName)
	{
		uint32_t func = 0x01102C30;
		uint32_t Hedgehog__Base__CHolderBase____ct = 0x0065FBE0;
		uint32_t stuff = 0x1E66B34;
		__asm
		{
			push fcoName
			lea     eax, a1
			lea     esi, objectReturn
			call    func
		};
	};
	void ShowWindowWithTextMaybe(const char* cast, void* handle, const char* displayText)
	{
		uint32_t func = 0x01102B40;
		uint32_t Hedgehog__Base__CHolderBase____ct = 0x0065FBE0;
		uint32_t stuff = 0x1E66B34;
		__asm
		{
			push displayText
			push handle
			mov eax, cast
			call    func
		};
	};
	void Title::HideWindow()
	{
		Common::PlaySoundStaticCueName(stageSelectHandle, "sys_worldmap_cansel");
		bg_window->SetHideFlag(true);
		fg_window->SetHideFlag(true);
		txt_window->SetHideFlag(true);
		footer_window->SetHideFlag(true);
	}
	void Title::ShowWindow(const char* text)
	{
		CSDCommon::SplitTextToSeparateCasts(*txt_window, "text_line_%d", "\n\n\n\n\n\n\n", 199, 7);
		CSDCommon::PlayAnimation(bg_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(fg_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(txt_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(footer_window, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		bg_window->SetHideFlag(false);
		fg_window->SetHideFlag(false);
		txt_window->SetHideFlag(false);
		footer_window->SetHideFlag(false);
		auto pos = fg_window->GetNode("text_area")->GetPosition();
		txt_window->SetPosition(pos.x(), pos.y());
		CSDCommon::SplitTextToSeparateCasts(*txt_window, "text_line_%d", text, 199, 7);
	}
	HOOK(void, __fastcall, sub_571F80, 0x571F80, int This)
	{
		auto inputState = Sonic::CInputState::GetInstance();
		auto inputPtr = &inputState->m_PadStates[inputState->m_CurrentPadStateIndex];
		switch (*(DWORD*)(This + 36))
		{
		case 6:
		{

			if (!showedWindow)
			{
				Title::ShowWindow("This game utilizes\nan autosave feature.\nPlease do not turn off\nthe console or remove\nany storage device when\nthe autosave icon º appears\non the screen.");
				showedWindow = true;
			}

			if (inputPtr->IsTapped(Sonic::eKeyState_A))
			{
				Title::HideWindow();
				originalsub_571F80(This);
			}
			return;
		}
		default:
			originalsub_571F80(This);
		}

	}
#pragma region OptionsSubmenu


	
	uint32_t* TitleUI_GetOptionFlag()
	{
		uint32_t flagsAddress = Common::ComposeAddressFromOffsets(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x18 });
		return (uint32_t*)flagsAddress;
	}
	float* TitleUI_GetMusicVolume()
	{
		// range [0.0 - 0.63]
		uint32_t musicVolumeAddress = Common::ComposeAddressFromOffsets(0x1E77290, { 0x38 });
		return (float*)musicVolumeAddress;
	}

	float* TitleUI_GetEffectVolume()
	{
		// range [0.0 - 0.63]
		uint32_t effectVolumeAddress = Common::ComposeAddressFromOffsets(0x1E77290, { 0x3C });
		return (float*)effectVolumeAddress;
	}
	void TitleUI_SetMusicVolume(float volume)
	{
		uint32_t* pCSoundModuleManager = *(uint32_t**)0x1E77290;
		FUNCTION_PTR(void, __thiscall, SetMusicVolume, 0x75EEF0, void* This, int a2, float volume);

		SetMusicVolume(pCSoundModuleManager, 0, volume);
		Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter->m_pSaveData->MusicVolume = (uint8_t)volume;
	}

	void TitleUI_SetEffectVolume(float volume)
	{
		uint32_t* pCSoundModuleManager = *(uint32_t**)0x1E77290;
		FUNCTION_PTR(void, __thiscall, SetMusicVolume, 0x75EEF0, void* This, int a2, float volume);

		SetMusicVolume(pCSoundModuleManager, 1, volume);
		Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter->m_pSaveData->SEVolume = (uint8_t)volume;
	}
	int currentOptionMainIndex;
	bool enteredOptionSub1;
	int currentOptionSubIndex1;
	bool enteredOptionSub2;
	int currentOptionSubIndex2;
	enum OptionsMainEnum
	{
		VOICE = 0,
		SUBTITLES = 1,
		SOUND = 2,
		CAMERA = 3,
		BRIGHTNESS = 4
	};
	static SharedPtrTypeless spOutState;
	std::string temp[5] = { "VOICE", "SUBTITLES", "SOUND", "CAMERA", "BRIGHTNESS" };
	std::string optionsLastCastMainTxt;
	std::string optionsLastCastSub1Txt;
	std::string optionsLastCastSub2Txt;
	void OptionsRefreshText()
	{
		if (!optionsLastCastMainTxt.empty())
			rcTitleMenuTXT->GetNode(optionsLastCastMainTxt.c_str())->SetHideFlag(true);
		if (!optionsLastCastSub1Txt.empty())
			rcTitleMenuTXT->GetNode(optionsLastCastSub1Txt.c_str())->SetHideFlag(true);
		if (!optionsLastCastSub2Txt.empty())
			rcTitleMenuTXT->GetNode(optionsLastCastSub2Txt.c_str())->SetHideFlag(true);

		optionsLastCastMainTxt = std::format("txt_2_{0}", (currentOptionMainIndex == 0 ? currentOptionMainIndex : currentOptionMainIndex + 1) + 1).c_str();
		if (enteredOptionSub1)
			optionsLastCastSub1Txt = std::format("txt_2_{0}_{1}", (currentOptionMainIndex == 0 ? currentOptionMainIndex : currentOptionMainIndex + 1) + 1, currentOptionSubIndex1 + 1).c_str();
		if (enteredOptionSub2 && currentOptionMainIndex == OptionsMainEnum::CAMERA)
			optionsLastCastSub2Txt = std::format("txt_2_5_1_{0}", currentOptionSubIndex2 + 1).c_str();

		rcTitleMenuTXT->GetNode(optionsLastCastMainTxt.c_str())->SetHideFlag(false);
		if (enteredOptionSub1)
			rcTitleMenuTXT->GetNode(optionsLastCastSub1Txt.c_str())->SetHideFlag(false);
		if (enteredOptionSub2 && currentOptionMainIndex == OptionsMainEnum::CAMERA)
			rcTitleMenuTXT->GetNode(optionsLastCastSub2Txt.c_str())->SetHideFlag(false);
	}
	void OpenOptions()
	{
		isInSubmenu = true;
		inOptions = true;
		CSDCommon::PlayAnimation(rcTitleMenu, "Intro_Anim_3", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcTitleMenuTXT, "Intro_Anim_3", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		OptionsRefreshText();
	}
	void OptionsUpdateSlider(float progress)
	{
		Common::PlaySoundStaticCueName(optionsHandle, "sys_actstg_twn_speechbutton");
		CSDCommon::PlayAnimation(rcTitleProgressbar, "Size_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
		CSDCommon::FreezeMotion(rcTitleProgressbar, ((progress) / 0.63f) * 100.0f);
	}
	void OptionsUpdate(Hedgehog::Universe::CStateMachineBase::CStateBase* TitleState)
	{
		auto inputState = Sonic::CInputState::GetInstance();
		auto inputPtr = &inputState->m_PadStates[inputState->m_CurrentPadStateIndex];

		DebugDrawText::log(std::format("##OPTIONS##\nMAININDEX: {0}\nSUB1: {1}\nSUB2: {2}\nENABLED1: {3}\nENABLED2: {4}", currentOptionMainIndex, currentOptionSubIndex1, currentOptionSubIndex2, enteredOptionSub1, enteredOptionSub2).c_str(), 0);
		//Logic Update
		switch ((OptionsMainEnum)currentOptionMainIndex)
		{
		case OptionsMainEnum::SUBTITLES:
		{
			if (enteredOptionSub1)
			{
				if (inputPtr->IsTapped(Sonic::eKeyState_A))
				{
					if (currentOptionSubIndex1 == 0)
					{
						*TitleUI_GetOptionFlag() |= 0x10; //on
					}
					else
					{
						*TitleUI_GetOptionFlag() &= 0xFFFFFFEF; //off
					}
					bool isEnabled = (*TitleUI_GetOptionFlag() & 0x10) != 0;

					DebugDrawText::log("SUBTITLES SET", 10);
				}
				rcTitleMenu->GetNode("box_off")->SetHideFlag(false);
				//if its enabled and the option is set to ON, or if its disabled and its set to OFF, make the box illuminate
				if ((*TitleUI_GetOptionFlag() & 0x10) != 0 && currentOptionSubIndex1 == 0 || (*TitleUI_GetOptionFlag() & 0x10) == 0 && currentOptionSubIndex1 == 1)
				{
					rcTitleMenu->GetNode("box_on")->SetHideFlag(false);
					rcTitleMenuScroll->GetNode("box_on")->SetHideFlag(false);
				}
				else
				{
					rcTitleMenu->GetNode("box_on")->SetHideFlag(true);
					rcTitleMenuScroll->GetNode("box_on")->SetHideFlag(true);
				}
			}
			break;
		}
		case OptionsMainEnum::SOUND:
		{
			if (enteredOptionSub2)
			{
				if (currentOptionSubIndex1 == 0) //music
				{
					float musicVol = *TitleUI_GetMusicVolume();
					if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickLeft))
					{
						if (musicVol != 0)
							musicVol -= 0.63f / 10.0f;
						TitleUI_SetMusicVolume(musicVol);
						OptionsUpdateSlider(musicVol);
					}
					if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickRight))
					{
						if (musicVol != 0.63f)
							musicVol += 0.63f / 10.0f;
						TitleUI_SetMusicVolume(musicVol);
						OptionsUpdateSlider(musicVol);
					}
				}
				else //sfx
				{
					float sfxVol = *TitleUI_GetEffectVolume();
					if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickLeft))
					{
						if (sfxVol != 0)
							sfxVol -= 0.63f / 10.0f;
						TitleUI_SetEffectVolume(sfxVol);
						OptionsUpdateSlider(sfxVol);
					}
					if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickRight))
					{
						if (sfxVol != 0.63f)
							sfxVol += 0.63f / 10.0f;
						TitleUI_SetEffectVolume(sfxVol);
						OptionsUpdateSlider(sfxVol);
					}
				}
			}
			break;
		}
		}

		//UI Update
		if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickLeft))
		{
			if (!enteredOptionSub1 && !enteredOptionSub2)
			{
				CSDCommon::PlayAnimation(rcTitleMenuScroll, "Scroll_Anim_3_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				CSDCommon::PlayAnimation(rcTitleMenu, "Scroll_Anim_3_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 100, false, true);

				Common::PlaySoundStaticCueName(optionsHandle, "sys_worldmap_cursor");
				currentOptionMainIndex--;
				//Repeat
				if (currentOptionMainIndex < 0)
					currentOptionMainIndex = 4;
			}
			if (enteredOptionSub1 && !enteredOptionSub2)
			{
				Common::PlaySoundStaticCueName(optionsHandle, "sys_worldmap_cursor");
				currentOptionSubIndex1--;
				if (currentOptionSubIndex1 < 0)
				{
					currentOptionSubIndex1 = 1;
				}
			}
			OptionsRefreshText();
		}
		if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickRight))
		{
			if (!enteredOptionSub1 && !enteredOptionSub2)
			{
				Common::PlaySoundStaticCueName(optionsHandle, "sys_worldmap_cursor");
				CSDCommon::PlayAnimation(rcTitleMenuScroll, "Scroll_Anim_3_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0);
				CSDCommon::PlayAnimation(rcTitleMenu, "Scroll_Anim_3_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 100, false, true);
				currentOptionMainIndex++;
				//Repeat
				if (currentOptionMainIndex > 4)
					currentOptionMainIndex = 0;
			}
			if (enteredOptionSub1 && !enteredOptionSub2)
			{
				Common::PlaySoundStaticCueName(optionsHandle, "sys_worldmap_cursor");
				currentOptionSubIndex1++;
				if (currentOptionSubIndex1 > 1)
				{
					currentOptionSubIndex1 = 0;
				}
			}
			OptionsRefreshText();
		}
		if (inputPtr->IsTapped(Sonic::eKeyState_A))
		{
			if (!enteredOptionSub1 && !enteredOptionSub2)
			{
				enteredOptionSub1 = true;
				if (currentOptionMainIndex == 4)
				{
					CSDCommon::PlayAnimation(rcTitleMenu, "Select_Anim_4", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					rcTitleProgressbar->SetHideFlag(false);
					float volMusic = 0.63f;
					if (currentOptionSubIndex1 == 0)
						volMusic = *TitleUI_GetMusicVolume();
					else
						volMusic = *TitleUI_GetEffectVolume();
					CSDCommon::PlayAnimation(rcTitleProgressbar, "Size_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
					CSDCommon::FreezeMotion(rcTitleProgressbar, ((volMusic) / 0.63f) * 100.0f);
					CSDCommon::PlayAnimation(rcTitleProgressbar, "Intro_Anim_4", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				}
				else
				{
					rcTitleProgressbar->SetHideFlag(true);
					CSDCommon::PlayAnimation(rcTitleMenu, "Intro_Anim_4", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					CSDCommon::PlayAnimation(rcTitleMenuTXT, "Intro_Anim_4", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				}
				Common::PlaySoundStaticCueName(optionsHandle, "sys_worldmap_decide");
				OptionsRefreshText();
				return;
			}

			//Enter submenu 1
			if ((enteredOptionSub1 && !enteredOptionSub2) && (currentOptionMainIndex == OptionsMainEnum::SOUND || currentOptionSubIndex1 == OptionsMainEnum::CAMERA))
			{
				rcTitleMenu->GetNode("box_on")->SetHideFlag(true);
				rcTitleMenu->GetNode("box_off")->SetHideFlag(true);
				if (currentOptionMainIndex == 2)
				{
					CSDCommon::PlayAnimation(rcTitleMenu, "Select_Anim_5", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					rcTitleProgressbar->SetHideFlag(false);
					CSDCommon::PlayAnimation(rcTitleProgressbar, "Intro_Anim_5", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				}
				else
				{

				}

				OptionsRefreshText();
				enteredOptionSub2 = true;
				Common::PlaySoundStaticCueName(optionsHandle, "sys_worldmap_decide");
				return;
			}
			//Enter submenu 2 / apply option
			if (enteredOptionSub1 && enteredOptionSub2)
			{
				return;
			}
		}
		if (inputPtr->IsTapped(Sonic::eKeyState_B))
		{
			//Exit from options
			if (!enteredOptionSub1 && !enteredOptionSub2)
			{
				isInSubmenu = false;
				inOptions = false;

				CSDCommon::PlayAnimation(rcTitleMenu, "Intro_Anim_3", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
				CSDCommon::PlayAnimation(rcTitleMenuTXT, "Intro_Anim_3", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
				rcTitleProgressbar->SetHideFlag(true);
				TitleUI_TinyChangeState(TitleState, spOutState, "SelectMenu");
				System::SaveManager::GenerationsSave();

				Common::PlaySoundStaticCueName(optionsHandle, "sys_worldmap_cansel");
				return;
			}
			//Exit from first submenu
			if (enteredOptionSub1 && !enteredOptionSub2)
			{
				if (currentOptionMainIndex == 4)
				{
					CSDCommon::PlayAnimation(rcTitleProgressbar, "Intro_Anim_4", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
				}
				else
				{
					CSDCommon::PlayAnimation(rcTitleMenuTXT, "Intro_Anim_4", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
				}
				CSDCommon::PlayAnimation(rcTitleMenu, "Intro_Anim_4", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
				enteredOptionSub1 = false;
				currentOptionSubIndex1 = 0;
				Common::PlaySoundStaticCueName(optionsHandle, "sys_worldmap_cansel");
				return;
			}
			//Exit from second submenu
			if (enteredOptionSub1 && enteredOptionSub2)
			{
				CSDCommon::PlayAnimation(rcTitleMenu, "Select_Anim_5", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
				CSDCommon::PlayAnimation(rcTitleMenu, "Intro_Anim_4", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				CSDCommon::FreezeMotion(rcTitleMenu, 100);
				CSDCommon::PlayAnimation(rcTitleProgressbar, "Intro_Anim_5", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
				enteredOptionSub2 = false;
				currentOptionSubIndex2 = 0;
				Common::PlaySoundStaticCueName(optionsHandle, "sys_worldmap_cansel");
				return;
			}
			OptionsRefreshText();
		}
	}
#pragma endregion
	//This function is largely useless as this wont run at all at times but Advance will, however it can be used to intercept specific buttons' functionalities
	HOOK(int, __fastcall, TitleUI_SetCustomExecFunctionNoASM, 0x00572D00, Hedgehog::Universe::CStateMachineBase::CStateBase* This)
	{
		isInSubmenu = true;
		if (currentTitleIndex == 2)
		{
			OpenOptions();
			return currentTitleIndex;
		}

		return originalTitleUI_SetCustomExecFunctionNoASM(This);
	}
	HOOK(void, __stdcall, OutpuDebugString, 0x013C40C4, LPCSTR lpOutputString)
	{
		DebugDrawText::log(lpOutputString);
		originalOutpuDebugString(lpOutputString);
	}
	HOOK(void, __fastcall, TitleUI_SetCustomExecFunctionAdvance, 0x005732A0, Hedgehog::Universe::CStateMachineBase::CStateBase* This)
	{
		DWORD* v2 = (DWORD*)((DWORD*)This)[2];
		//This calls Title_GetSelectionIndex
		*((byte*)v2 + 429) = 1;
		int i = (*(int(__thiscall**)(DWORD*))(*v2 + 56))(v2);
		switch (currentTitleIndex)
		{
		case 0:
		{
			OnNewGame();
			originalTitleUI_SetCustomExecFunctionAdvance(This);
			break;
		}
		case 1:
		{
			if (canLoad)
			{
				originalTitleUI_SetCustomExecFunctionAdvance(This);
				break;
			}
			ExitingTitle();
			ContinueToWM();
			break;
		}
		case 2:
		{
			OptionsUpdate(This);
			break;
		}
		default:
			originalTitleUI_SetCustomExecFunctionAdvance(This);

		}
	}

	HOOK(int, __fastcall, Title_GetSelectionIndex, 0x0056FBB0, DWORD* This)
	{
		if (currentTitleIndex == 2) //options in gens is the 6th option for whatever reason
			return 5;
		if (currentTitleIndex == 3) //quit in gens is the 9th option for whatever reason
			return 8;
		return currentTitleIndex;
	}

	void Title::RegisterHooks()
	{
		//Disable loading save at the start
		WRITE_JUMP(0x005724FA, 0x00572501);

		//Should remove the hints options screen from New Game
		//WRITE_JUMP(0x005733D5, 0x005733D7);
		//Set up title screen so that it resembles Unleashed function-wise
		// //571A25 - cause of crash with direct9ex 11
		WRITE_JUMP(0x00571FCA, TitleUI_SetCutsceneTimer); //Set title AFK wait amount - it varies depending on framerate
		//WRITE_JUMP(0x00572D23, TitleUI_SetCustomExecFunction); //Override Button Function
		//WRITE_JUMP(0x005732C3, TitleUI_SetCustomExecFunctionAdvance); //Override button after-function
		WRITE_JUMP(0x00572B2E, (void*)0x00572B45); //Disable scroll sound	
		//Try forcing out loading the world map if the save is invalid
		//WRITE_JUMP(0x0057342B, 0x0057346B);
		//UI
		INSTALL_HOOK(TitleUI_SetCustomExecFunctionNoASM);
		INSTALL_HOOK(TitleUI_SetCustomExecFunctionAdvance);
		INSTALL_HOOK(Title_GetSelectionIndex);
		INSTALL_HOOK(Title_UpdateApplication);
		WRITE_MEMORY(0x016E11F4, void*, CTitleRemoveCallback);
		WRITE_JUMP(0x572D3A, (void*)0x57326B); // Don't create delete save dialog
		WRITE_NOP(0x00572976, 6); //Always skip waiting for anim in ExecSubMenu
		WRITE_NOP(0x00572930, 6); //Always skip waiting for anim in ExecSubMenu

		//Prevent going back to PRESS START screen after entering
		WRITE_JUMP(0x00572B92, (void*)0x572CF7);

		WRITE_JUMP(0x00573136, 0x00573142); //Remove options music
		//INSTALL_HOOK(OutpuDebugString);
		//State Hooks
		INSTALL_HOOK(sub_571F80);
		INSTALL_HOOK(Title_CMain);
		INSTALL_HOOK(Title_CMain_ExecSubMenu);
		INSTALL_HOOK(Title_CMain_CState_SelectMenuBegin);
		INSTALL_HOOK(Title_CMain_CState_WaitStart);
		INSTALL_HOOK(Title_CMain_CState_SelectMenu);

		scale = new Hedgehog::Math::CVector2
		(System::AspectRatioHelper::NativeResolution->x() / 1280,
			(size_t)System::AspectRatioHelper::NativeResolution->y() / 720);
	}

}
