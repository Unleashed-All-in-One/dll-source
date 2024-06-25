Chao::CSD::RCPtr<Chao::CSD::CProject> rcQTE; //ui_qte
boost::shared_ptr<Sonic::CGameObjectCSD> spQTE;

//root/qte/guide
Chao::CSD::RCPtr<Chao::CSD::CScene> rcGuideEffect;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcGuideEffect2;

//root/qte/qte_btn
Chao::CSD::RCPtr<Chao::CSD::CScene> rcQteBtn;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcQteBtn2;

bool isShowing;
bool isMultiMode = false;
bool bypassGrindrail = false;
SharedPtrTypeless soundHandle;
HudButtonGuide::ButtonType buttonType;
std::string m_animationNames[7] = { "qs_R", "qs_L", "boost", "sliding", "door", "chaser", "qs_L" };
Sonic::EKeyState buttonToPress;

Sonic::EKeyState HudButtonGuide::getKeyStateFromButtonType(ButtonType in_buttonType)
{
	switch(in_buttonType)
	{
	case BUTTON_A:
		return Sonic::eKeyState_A;
	case BUTTON_B:
		return Sonic::eKeyState_B;
	case BUTTON_X:
		return Sonic::eKeyState_X;
	case BUTTON_Y:
		return Sonic::eKeyState_Y;
	case BUTTON_LEFTBUMPER:
		return Sonic::eKeyState_LeftBumper;
	case BUTTON_RIGHTBUMPER:
		return Sonic::eKeyState_RightBumper;
	case BUTTON_LEFTTRIGGER:
		return Sonic::eKeyState_LeftTrigger;
	case BUTTON_RIGHTTRIGGER:
		return Sonic::eKeyState_RightTrigger;
	}
}
void HudButtonGuide::configure(ButtonType in_ButtonType, AnimationType in_AnimType)
{
	buttonToPress = getKeyStateFromButtonType(in_ButtonType);
	if (rcGuideEffect)
		Chao::CSD::CProject::DestroyScene(rcQTE.Get(), rcGuideEffect);
	if (rcGuideEffect2)
		Chao::CSD::CProject::DestroyScene(rcQTE.Get(), rcGuideEffect2);

	if (rcQteBtn)
		Chao::CSD::CProject::DestroyScene(rcQTE.Get(), rcQteBtn);
	if (rcQteBtn2)
		Chao::CSD::CProject::DestroyScene(rcQTE.Get(), rcQteBtn2);

	rcGuideEffect = rcQTE->CreateScene(m_animationNames[(int)in_AnimType].c_str());
	int buttonSceneNameIndex = 1;
	switch (in_ButtonType)
	{
	case BUTTON_A:
	case BUTTON_B:
	case BUTTON_X:
	case BUTTON_Y:
	{
		buttonSceneNameIndex = 1;
		break;
	}

	case BUTTON_LEFTBUMPER:
	case BUTTON_RIGHTBUMPER:
	{
		buttonSceneNameIndex = 2;
		break;
	}
	case BUTTON_LEFTTRIGGER:
	case BUTTON_RIGHTTRIGGER:
	{
		buttonSceneNameIndex = 3;
		break;
	}
	}
	//btn_1
	char buff[6];
	sprintf(buff, "btn_%d", buttonSceneNameIndex);
	rcQteBtn = rcQTE->CreateScene(buff);

	int buttonImageIndex = 0;
	if (in_ButtonType >= BUTTON_A && in_ButtonType < BUTTON_LEFTBUMPER)
		buttonImageIndex = in_ButtonType;

	if (in_ButtonType == BUTTON_LEFTBUMPER || in_ButtonType == BUTTON_RIGHTBUMPER)
		buttonImageIndex = in_ButtonType - BUTTON_LEFTBUMPER;

	if (in_ButtonType == BUTTON_LEFTTRIGGER || in_ButtonType == BUTTON_RIGHTTRIGGER)
		buttonImageIndex = in_ButtonType - BUTTON_LEFTTRIGGER;

	if(in_AnimType != QUICKSTEP_BOTH)
	{
		rcQteBtn->GetNode("position")->SetPosition(1280 / 2, 720 / 2);
		rcQteBtn->GetNode("img")->SetPatternIndex(buttonImageIndex);
	}
	else
	{
		isMultiMode = true;
		//in case of quickstep, Btn and Effect are LEFT, Btn2 and Effect2 are RIGHT
		rcQteBtn2 = rcQTE->CreateScene(buff);
		rcGuideEffect2 = rcQTE->CreateScene(m_animationNames[0].c_str());
		rcQteBtn->GetNode("position")->SetPosition((1280 / 2) - 200, 720 / 2);
		rcQteBtn2->GetNode("position")->SetPosition((1280 / 2) + 200, 720 / 2);
		CSDCommon::PlayAnimation(*rcQteBtn, "L_pattern", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(*rcQteBtn2, "R_pattern", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	CSDCommon::PlayAnimation(*rcQteBtn, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*rcGuideEffect, "Intro_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
	if(rcQteBtn2)
		CSDCommon::PlayAnimation(*rcQteBtn2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	if(rcGuideEffect2)
		CSDCommon::PlayAnimation(*rcGuideEffect2, "Intro_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
	isShowing = true;
}
void HudButtonGuide::spawn(Sonic::CGameObject* in_Parent, ButtonType in_ButtonType, AnimationType in_AnimType)
{
	Sonic::CCsdDatabaseWrapper wrapper(in_Parent->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
	auto spCsdProject = wrapper.GetCsdProject("ui_qte_swa");
	rcQTE = spCsdProject->m_rcProject;
	configure(in_ButtonType, in_AnimType);
	in_Parent->m_pMember->m_pGameDocument->AddGameObject(spQTE = boost::make_shared<Sonic::CGameObjectCSD>(rcQTE, 0.5f, "HUD", false), "main", in_Parent);
}
void HudButtonGuide::hide()
{
	bypassGrindrail = false;
	isShowing = false;
	if(rcQteBtn)
		CSDCommon::PlayAnimation(*rcQteBtn, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, true, true);
	if(rcQteBtn2)
		CSDCommon::PlayAnimation(*rcQteBtn2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, true, true);
	if(rcGuideEffect)
		CSDCommon::PlayAnimation(*rcGuideEffect, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	if (rcGuideEffect2)
		CSDCommon::PlayAnimation(*rcGuideEffect2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
}
void CallBoostGuide()
{
	if (!spQTE)
		HudButtonGuide::spawn(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer, HudButtonGuide::BUTTON_X, HudButtonGuide::BOOST);
	else
		HudButtonGuide::configure(HudButtonGuide::BUTTON_X, HudButtonGuide::BOOST);
}
void CallQuickstepGuide()
{
	if (!spQTE)
		HudButtonGuide::spawn(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer, HudButtonGuide::BUTTON_LEFTBUMPER, HudButtonGuide::QUICKSTEP_BOTH);
	else
		HudButtonGuide::configure(HudButtonGuide::BUTTON_LEFTBUMPER, HudButtonGuide::QUICKSTEP_BOTH);
}
void __fastcall CHudSonicStageRemoveCallbackQTE(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	if (spQTE)
	{
		spQTE->SendMessage(spQTE->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spQTE = nullptr;
	}
	if(rcQTE)
	{
		if(rcQteBtn)
			Chao::CSD::CProject::DestroyScene(rcQTE.Get(), rcQteBtn);
		if(rcGuideEffect)
			Chao::CSD::CProject::DestroyScene(rcQTE.Get(), rcGuideEffect);		
	}
	rcQTE = nullptr;
}
void __declspec(naked) ASM_MsgStartBoostSign()
{
	static uint32_t retn = 0x00528864;
	__asm
	{
		call CallBoostGuide
		jmp[retn]
	}
}void __declspec(naked) ASM_MsgStartQuickStepSign()
{
	static uint32_t retn = 0x00528994;
	__asm
	{
		call CallQuickstepGuide
		jmp[retn]
	}
}


//void __thiscall ProcMsgStartCommonButtonSign(int this, int a2)
HOOK(void, __fastcall, ProcMsgStartCommonButtonSign, 0x5289A0, int This, void* Edx, Sonic::Message::MsgStartCommonButtonSign* a2)
{
	if (a2->m_ButtonType == 3)
		return;
	if (a2->m_ButtonType == -1 && a2->m_ButtonType2 == 10) //grindrail quickstep
	{
		Common::PlaySoundStatic(soundHandle, 3000812983);
		CallQuickstepGuide();
	}
	else
	originalProcMsgStartCommonButtonSign(This, Edx, a2);
}
HOOK(void, __fastcall, CHudSonicStageUpdateQTE, 0x1098A50, void* This, void* Edx, float* dt)
{
	if(isShowing)
	{
		Sonic::SPadState* inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
		if(isMultiMode)
		{
			if(inputPtr->IsDown(Sonic::eKeyState_RightBumper))
				HudButtonGuide::hide();
		}
		if (inputPtr->IsDown(buttonToPress))
		{
			HudButtonGuide::hide();
		}		
	}
	originalCHudSonicStageUpdateQTE(This, Edx, dt);
}
HOOK(void, __fastcall, MsgStartBoostSign, 0x528740, void* This, void* Edx, void* message)
{
	CallBoostGuide();
	Common::PlaySoundStatic(soundHandle, 3000812983);
	//originalMsgStartBoostSign(This, Edx, message);
}
HOOK(void, __fastcall, MsgStartQuickStepSign, 0x528870, void* This, void* Edx, int* message)
{
	CallQuickstepGuide();
	Common::PlaySoundStatic(soundHandle, 3000812983);
	//originalMsgStartQuickStepSign(This, Edx, message);
}
void HudButtonGuide::applyPatches()
{
	//these 2 asm hooks can be replaced with function hooks
	//WRITE_JUMP(0x005287A0, ASM_MsgStartBoostSign);
	//WRITE_JUMP(0x005288D0, ASM_MsgStartQuickStepSign);
	INSTALL_HOOK(MsgStartBoostSign);
	INSTALL_HOOK(MsgStartQuickStepSign);
	INSTALL_HOOK(CHudSonicStageUpdateQTE);
	INSTALL_HOOK(ProcMsgStartCommonButtonSign);
	WRITE_MEMORY(0x16A467C, void*, CHudSonicStageRemoveCallbackQTE);
}
