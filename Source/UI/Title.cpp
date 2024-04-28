Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreen;
Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreenLogos;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcTitleLogo_1, rcTitlebg, rcTitleMenu, rcTitleMenuScroll, rcTitleMenuTXT, black_bg, bg_window, fg_window, txt_window, footer_window, bg_transition;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcConversionLogo;
boost::shared_ptr<Sonic::CGameObjectCSD> spTitleScreen;
boost::shared_ptr<Sonic::CGameObjectCSD> spTitleScreenLogos;
boost::shared_ptr<Sonic::StageSelectMenu::CDebugStageSelectMenuXml> spDebugMenu;

Title::TitleIndexState currentTitleIndex = Title::TitleIndexState::New_Game;
Hedgehog::Math::CVector2* scale = new Hedgehog::Math::CVector2(1, 1);
bool enteredStart, isInSubmenu, moved, hasSavefile, playingStartAnim, reversedAnim, isStartAnimComplete, startButtonAnimComplete, startBgAnimComplete, parsedSave, showedWindow = false;
bool Title::inWorldMap = false;
bool inTitle, scrollHorizontally = true;
int canLoad = 0;
bool Title::inInstall = 0;
int maxTitleIndex = 3;
int holdTimer = 0;

boost::shared_ptr<Sonic::CCamera> spCamera;
static uint32_t cameraInitRan = 0;
static AudioHandle stageSelectHandle;
void* TitleStateContextBase;


void Title::setSubmenu(bool enabled)
{
	isInSubmenu = enabled;
}
void Title::setScrollDirection(bool horizontal)
{
	scrollHorizontally = horizontal;
}
void ExitingTitle()
{
	Title::setScrollDirection(false);
	Title::setSubmenu(false);
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
	if (!Title::inWorldMap)
	{
		Title::inWorldMap = true;
		Title::setHideEverything(true, true);
		PlayTitleBGM(TitleStateContextBase, "Option");
		CSDCommon::FreezeMotion(*rcTitleLogo_1);
		TitleWorldMap::Start();
		TitleWorldMap::EnableInput();
	}
}
void ShowInstallScreen()
{

	PlayTitleBGM(TitleStateContextBase, "");
	Title::inInstall = true;
	black_bg->SetHideFlag(false);
	static boost::shared_ptr<hh::db::CRawData> rawData;
	auto database = hh::db::CDatabase::CreateDatabase();
	auto& databaseLoader = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spDatabaseLoader;
	databaseLoader->CreateArchiveList("#SelectStage.ar", "#SelectStage.arl", { 0, 0 });
	databaseLoader->LoadArchiveList(database, "#SelectStage.arl");
	databaseLoader->LoadArchive(database, "#SelectStage.ar", { 0, 0 }, false, false);
	spDebugMenu = boost::make_shared<Sonic::StageSelectMenu::CDebugStageSelectMenuXml>();
	Sonic::CGameDocument::GetInstance()->AddGameObject(spDebugMenu);
	TitleWorldMap::LoadingReplacementEnabled = false;
}
void Title::showTransition(bool enableLoad)
{
	bg_transition->SetHideFlag(false);
	CSDCommon::PlayAnimation(*bg_transition, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	PlayTitleBGM(TitleStateContextBase, "");
	if(enableLoad)
	canLoad = 1;
}
void __declspec(naked) TitleUI_SetCustomExecFunction()
{
	//https://godbolt.org/
	static uint32_t pAddr = 0x00572E27;
	static uint32_t adNewGame = 0x00572D33;
	static uint32_t adContinue = 0x00572E2F;
	static uint32_t adOptions = 0x00573008;
	static uint32_t adQuit = 0x00573153;
	if (Project::menuType <= 1)
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
	Title::showTransition(false);
	StageManager::InStory = true;
	SaveManager::deleteSave();
	auto save = SaveManager::getCurrentSave();
	save->lives = 5;
	UnleashedHUD_API::StartFadeout();
	//SaveManager::saveToDisk();
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
	Title::killScreen();
	Chao::CSD::CProject::DestroyScene(rcTitleScreenLogos.Get(), rcTitleLogo_1);
	if(rcConversionLogo)
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
	rcTitleScreen = nullptr;
	rcTitleScreenLogos = nullptr;
	enteredStart, isInSubmenu, moved, hasSavefile, playingStartAnim, reversedAnim, isStartAnimComplete, startButtonAnimComplete, startBgAnimComplete, showedWindow = false;
	inTitle, parsedSave = false;
	currentTitleIndex = Title::TitleIndexState::New_Game;
	holdTimer = 0;
	canLoad = 0;
	Title::inWorldMap = 0;
}
void Title::setHideEverything(bool visible, bool logoVisible)
{
	if (rcTitleLogo_1)
	{
		/*if (logoVisible)
		{
			CSDCommon::PlayAnimation(*rcTitleLogo_1, "Outro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		}
		else*/
		rcTitleLogo_1->SetHideFlag(visible);
	}
	if (rcTitlebg)
		rcTitlebg->SetHideFlag(visible);
	if (rcTitleMenu)
		rcTitleMenu->SetHideFlag(visible);
	if (rcTitleMenuScroll)
		rcTitleMenuScroll->SetHideFlag(visible);
	if (rcTitleMenuTXT)
		rcTitleMenuTXT->SetHideFlag(visible);
	if (rcConversionLogo)
		rcConversionLogo->SetHideFlag(visible);
}
void Title::createScreen(Sonic::CGameObject* pParentGameObject)
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
void Title::killScreen()
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
void Title::toggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
		createScreen(pParentGameObject);
	else
		killScreen();
}
void Title::introAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene)
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
		currentTitleIndex = Title::TitleIndexState::Quit;
	else if (currentTitleIndex > maxTitleIndex)
		currentTitleIndex = Title::TitleIndexState::New_Game;

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
		auto saveObject = SaveManager::getCurrentSave(false);
		hasSavefile = saveObject != nullptr;
		currentTitleIndex = hasSavefile ? Title::TitleIndexState::Continue : Title::TitleIndexState::New_Game;
		UpdateTitleText();
		parsedSave = true;
	}
	TitleStateContextBase = This->GetContextBase();
}
HOOK(int, __fastcall, Title_CMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3, void** a4)
{
	CTitleRemoveCallback(This, nullptr, nullptr);
	Title::setScrollDirection(true);
	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
	auto spCsdProject = wrapper.GetCsdProject("ui_title_unleashed");
	auto spCsdProjectLogos = wrapper.GetCsdProject("ui_title_logos");
	rcTitleScreen = spCsdProject->m_rcProject;
	rcTitleScreenLogos = spCsdProjectLogos->m_rcProject;
	Title::inInstall = false;
	char buffer[8];
	//LogoType
	//0: Conversion
	//1: USA
	//2: JPN
	//3: Generations Demo
	sprintf(buffer, "title_%d", Project::logoType == 0 || Project::logoType == 1 ? 1 : Project::logoType);
	rcTitleLogo_1 = rcTitleScreenLogos->CreateScene(buffer);

	

	if (Project::logoType == 0)
	{
		rcConversionLogo = rcTitleScreenLogos->CreateScene("conversionlogo");
		rcTitleLogo_1->SetPosition(68, 3);
		rcTitleLogo_1->SetScale(0.9f, 0.9f);
		rcConversionLogo->SetPosition(68, 3);
		rcConversionLogo->SetScale(0.9f, 0.9f);
		CSDCommon::PlayAnimation(*rcConversionLogo, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	switch (Project::menuType)
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
	CSDCommon::PlayAnimation(*fg_window, "Size_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
	CSDCommon::FreezeMotion(*fg_window, 48 /* TIMES the maximum characters in a line in the text.*/);
	black_bg->SetHideFlag(true);
	if (rcTitleMenuTXT)
	{
		rcTitleMenuTXT->GetNode("txt_4")->SetHideFlag(true);
		CSDCommon::FreezeMotion(*rcTitleMenuTXT);
		rcTitleMenuTXT->SetPosition(0, 650000);
	}
	if (rcTitleMenuScroll)
	{
		CSDCommon::PlayAnimation(*rcTitleMenuScroll, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
		CSDCommon::FreezeMotion(*rcTitleMenuScroll, 0);
		rcTitleMenuScroll->SetPosition(0, 650000);
	}
	if (rcTitleMenu)
	{
		CSDCommon::FreezeMotion(*rcTitleMenu);
		CSDCommon::PlayAnimation(*rcTitleMenu, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	if (rcTitlebg)
	{
		CSDCommon::FreezeMotion(*rcTitlebg);
		CSDCommon::PlayAnimation(*rcTitlebg, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	if (rcTitleLogo_1)
		CSDCommon::PlayAnimation(*rcTitleLogo_1, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

	playingStartAnim = true;
	Title::createScreen(This);
	return originalTitle_CMain(This, Edx, a2, a3, a4);
}
HOOK(DWORD, *__cdecl, Title_CMain_CState_SelectMenu, 0x11D1210, hh::fnd::CStateMachineBase::CStateBase* This)
{
	if (enteredStart)
		return originalTitle_CMain_CState_SelectMenu(This);

	TitleWorldMap::PlayPanningAnim();
	if (rcTitleMenu)
		CSDCommon::FreezeMotion(*rcTitleMenu);
	if (rcTitlebg)
		CSDCommon::PlayAnimation(*rcTitlebg, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	if (rcTitleMenu)
		CSDCommon::PlayAnimation(*rcTitleMenu, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	if (rcTitleMenuTXT)
	{
		CSDCommon::PlayAnimation(*rcTitleMenuTXT, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
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
		CSDCommon::PlayAnimation(*rcTitleMenu, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, false, true);
		CSDCommon::PlayAnimation(*rcTitlebg, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 30, 100, false, true);
		currentTitleIndex = Title::TitleIndexState::New_Game;
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
	
	if (canLoad == 1)
	{
		if (bg_transition)
		{
			if (bg_transition->m_MotionDisableFlag)
			{
				SequenceHelpers::loadStage(StageManager::getStageToLoad());
				canLoad = 0;
			}
		}
	}

	if (inTitle && !Title::inWorldMap)
	{

		if (currentTitleIndex == 3 && isInSubmenu && inputPtr->IsDown(Sonic::eKeyState_B)) //janky way of knowing when the quit prompt is aborted
			isInSubmenu = false;

		if (rcTitleLogo_1) //Logo loop anim
		{
			if (rcTitleLogo_1->m_MotionDisableFlag == 1 && !isStartAnimComplete && playingStartAnim)
			{
				isStartAnimComplete = true;
				CSDCommon::PlayAnimation(*rcTitleLogo_1, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_Loop, 1, 0);

			}
		}
		if (rcTitleMenu) //PRESS START loop anim
		{
			if (rcTitleMenu->m_MotionDisableFlag == 1 && !isStartAnimComplete && playingStartAnim)
			{
				CSDCommon::PlayAnimation(*rcTitleMenu, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
			}
		}
		if (rcTitlebg && enteredStart) //Pulse header anim
		{
			if (rcTitlebg->m_MotionDisableFlag == 1 && !startBgAnimComplete && playingStartAnim)
			{
				startBgAnimComplete = true;
				CSDCommon::PlayAnimation(*rcTitlebg, "Usual_Anim_2", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
			}
		}
#pragma region UIScroll
		if (rcTitleMenu && !isInSubmenu && enteredStart)
		{
			if (IsRightDown() && !moved)
			{
				currentTitleIndex = (Title::TitleIndexState)((int)(currentTitleIndex)+1);
				if (currentTitleIndex > maxTitleIndex)
					currentTitleIndex = Title::TitleIndexState::New_Game;

				if (currentTitleIndex == Title::TitleIndexState::Continue && !hasSavefile)
					currentTitleIndex = Title::TitleIndexState::Options;
				MiniAudioHelper::playSound(stageSelectHandle, 0, "Cursor", false);
				CSDCommon::PlayAnimation(*rcTitleMenuScroll, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				CSDCommon::PlayAnimation(*rcTitleMenu, "Scroll_Anim_2_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 100, false, true);
				UpdateTitleText();
				moved = true;
			}
			if (IsLeftDown() && !moved)
			{
				currentTitleIndex = (Title::TitleIndexState)((int)(currentTitleIndex)-1);
				MiniAudioHelper::playSound(stageSelectHandle, 0, "Cursor", false);
				if (currentTitleIndex < 0 || currentTitleIndex > maxTitleIndex) //uint32s become huge when going negative
					currentTitleIndex = (Title::TitleIndexState)maxTitleIndex;

				if (currentTitleIndex == Title::TitleIndexState::Continue && !hasSavefile)
					currentTitleIndex = Title::TitleIndexState::New_Game;
				CSDCommon::PlayAnimation(*rcTitleMenuScroll, "Scroll_Anim_2_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				CSDCommon::PlayAnimation(*rcTitleMenu, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 100, false, true);
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
	MiniAudioHelper::playSound(stageSelectHandle, 1000005, "", false, false);
	bg_window->SetHideFlag(true);
	fg_window->SetHideFlag(true);
	txt_window->SetHideFlag(true);
	footer_window->SetHideFlag(true);
}
void Title::ShowWindow(const char* text)
{
	CSDCommon::SplitTextToSeparateCasts(*txt_window, "text_line_%d", "\n\n\n\n\n\n\n", 199, 7);
	CSDCommon::PlayAnimation(*bg_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*fg_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*txt_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*footer_window, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
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

	printf("\nSTATE: %ld", *(DWORD*)(This + 36));
}
HOOK(void, __cdecl, DebugDrawTextDraw, 0x750820, void*, float x, float y, void*, uint8_t* color, wchar_t* text, ...)
{
	va_list va;
	va_start(va, text);

	wchar_t formatted[1024];
	_vsnwprintf_s(formatted, _countof(formatted), text, va);

	char formattedMultiByte[1024];
	WideCharToMultiByte(CP_UTF8, 0, formatted, -1, formattedMultiByte, sizeof(formattedMultiByte), 0, 0);

	size_t resx = (size_t)(LetterboxHelper::Resolution->x() * scale->x());
	size_t resy = (size_t)(LetterboxHelper::Resolution->y() * scale->y());
	size_t gg = *(size_t*)0x180C6B0;
	size_t aspect = (resx / resy) * scale->x();
	DebugDrawText::draw(formattedMultiByte, { (size_t)(x / *(size_t*)0x180C6B0 * resx) % resx, (size_t)(y / *(size_t*)0x1B24560 * resy) % resy }, aspect);
}

HOOK(void, __fastcall, TitleUI_SetCustomExecFunctionAdvance, 0x005732A0, DWORD* This)
{
	DWORD* v2 = (DWORD*)This[2];
	//This calls Title_GetSelectionIndex
	*((byte*)v2 + 429) = 1;
	int i = (*(int (__thiscall **)(DWORD *))(*v2 + 56))(v2);
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
		default:
			originalTitleUI_SetCustomExecFunctionAdvance(This);

	}
}

HOOK(int, __fastcall, Title_GetSelectionIndex, 0x0056FBB0, DWORD* This)
{
	isInSubmenu = true;
	return currentTitleIndex;
}


void Title::applyPatches()
{
	//Disable loading save at the start
	WRITE_JUMP(0x005724FA, 0x00572501);

	//Should remove the hints options screen from New Game
	//WRITE_JUMP(0x005733D5, 0x005733D7);
	//Set up title screen so that it resembles Unleashed function-wise
	// //571A25 - cause of crash with direct9ex 11
	WRITE_JUMP(0x00571FCA, TitleUI_SetCutsceneTimer); //Set title AFK wait amount - it varies depending on framerate
	WRITE_JUMP(0x00572D23, TitleUI_SetCustomExecFunction); //Override Button Function
	//WRITE_JUMP(0x005732C3, TitleUI_SetCustomExecFunctionAdvance); //Override button after-function
	WRITE_JUMP(0x00572B2E, (void*)0x00572B45); //Disable scroll sound	
	//Try forcing out loading the world map if the save is invalid
	//WRITE_JUMP(0x0057342B, 0x0057346B);
	//UI
	INSTALL_HOOK(TitleUI_SetCustomExecFunctionAdvance);
	INSTALL_HOOK(Title_GetSelectionIndex);
	INSTALL_HOOK(Title_UpdateApplication);
	WRITE_MEMORY(0x016E11F4, void*, CTitleRemoveCallback);
	WRITE_JUMP(0x572D3A, (void*)0x57326B); // Don't create delete save dialog
	WRITE_NOP(0x00572976, 6); //Always skip waiting for anim in ExecSubMenu
	WRITE_NOP(0x00572930, 6); //Always skip waiting for anim in ExecSubMenu

	//Prevent going back to PRESS START screen after entering
	WRITE_JUMP(0x00572B92, (void*)0x572CF7);

	//State Hooks
	INSTALL_HOOK(sub_571F80);
	INSTALL_HOOK(Title_CMain);
	INSTALL_HOOK(Title_CMain_ExecSubMenu);
	INSTALL_HOOK(Title_CMain_CState_SelectMenuBegin);
	INSTALL_HOOK(Title_CMain_CState_WaitStart);
	INSTALL_HOOK(Title_CMain_CState_SelectMenu);
	INSTALL_HOOK(DebugDrawTextDraw);

	scale = new Hedgehog::Math::CVector2(LetterboxHelper::NativeResolution->x() / 1280, (size_t)LetterboxHelper::NativeResolution->y() / 720);
}