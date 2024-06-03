#pragma region UI

boost::shared_ptr<Sonic::CGameObjectCSD> spExp;
Chao::CSD::RCPtr<Chao::CSD::CProject> rcExp;
Chao::CSD::RCPtr<Chao::CSD::CScene> exp_count;

bool displayedHUD = false;
float m_timerFade;

float expProgress = 0.0f; // can u make this saveable :happi:
float expProgressPrevious = 0.0f; // can u make this saveable :happi:
int expLevel = 0; // can u make this saveable :happi:

void CreateScreenEXP(Sonic::CGameObject* pParentGameObject)
{
	if (rcExp && !spExp)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spExp = boost::make_shared<Sonic::CGameObjectCSD>(rcExp, 0.5f, "HUD", false), "main", pParentGameObject);
}

void KillScreenEXP()
{
	if (!spExp)
		return;
	spExp->SendMessage(spExp->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
	spExp = nullptr;
}

void __fastcall CHudSonicStageRemoveCallbackEXP(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{	
	KillScreenEXP();
	Chao::CSD::CProject::DestroyScene(rcExp.Get(), exp_count);
	rcExp = nullptr;
	exp_count = nullptr;
}

HOOK(void, __fastcall, CHudSonicStageDelayProcessImpEXP, 0x109A8D0, Sonic::CGameObject* This)
{
	originalCHudSonicStageDelayProcessImpEXP(This);
	if (!*pModernSonicContext)
		return

	CHudSonicStageRemoveCallbackEXP(This, nullptr, nullptr);

	Sonic::CCsdDatabaseWrapper wrapperExp(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

	auto spCsdProjectExp = wrapperExp.GetCsdProject("ui_playscreen_exp");

	rcExp = spCsdProjectExp->m_rcProject;
	exp_count = rcExp->CreateScene("exp_count");
	exp_count->SetPosition(0, 0);
	CSDCommon::FreezeMotion(*exp_count);
	if (SaveManager::getCurrentSave() != nullptr)
	{
		expProgress = SaveManager::getCurrentSave()->getSaveBoolKeyValue("EXPProgress");
		expLevel = SaveManager::getCurrentSave()->getSaveBoolKeyValue("EXPLevel");
	}
	if (expLevel > 99)
		expLevel = 99;
	CreateScreenEXP(This);
}
void EXPCollect::addProgress(float progress)
{
	expProgressPrevious = expProgress;
	expProgress += progress;
}

void EXPCollect::displayHUD()
{
	m_timerFade = 0;
	if (displayedHUD)
		return;

	displayedHUD = true;
	exp_count->SetMotionFrame(0);	
}

HOOK(void, __fastcall, HudSonicEXPUpdate, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	originalHudSonicEXPUpdate(This, Edx, in_rUpdateInfo);
	if (!displayedHUD)
		return;

	m_timerFade += in_rUpdateInfo.DeltaTime;

	float frameBefore = exp_count->m_MotionFrame;
	if(m_timerFade >= 5)
	{
		CSDCommon::PlayAnimation(*exp_count, "size", Chao::CSD::eMotionRepeatType_PlayOnce, 1, expProgress);
		CSDCommon::FreezeMotion(*exp_count, expProgress);
		CSDCommon::PlayAnimation(*exp_count, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, false, true);		
		displayedHUD = false;
		m_timerFade = 0;		
	}
	else
	{
		CSDCommon::PlayAnimation(*exp_count, "size", Chao::CSD::eMotionRepeatType_PlayOnce, 1, Common::LerpFloat(expProgressPrevious, expProgress, m_timerFade <= 2 ? m_timerFade / 2 : 1));
		CSDCommon::FreezeMotion(*exp_count, expProgress);
		CSDCommon::PlayAnimation(*exp_count, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, frameBefore + 1);		
	}

	char text[4];
	sprintf(text, "%02d", expLevel);
	exp_count->GetNode("exp")->SetText(text);
}
#pragma endregion
#pragma region Orbs
std::vector<Sonic::CGameObject*> expCache;

void ChaosEnergyLogic(uint32_t amount)
{
	Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

	if (!context)
		return;

	for (uint32_t i = 0; i < amount; i++)
	{
		boost::shared_ptr<EXPCollect> newEXP = boost::make_shared<EXPCollect>(context->m_spMatrixNode->m_Transform.m_Position);
		expCache.push_back(newEXP.get());
		// modification example : ((EXPCollect*)expCache.at(i))->test = true;
		context->m_pPlayer->m_pMember->m_pGameDocument->AddGameObject(newEXP);
	}
}

uint32_t __fastcall getEnemyChaosEnergyTypeImpl(uint32_t* pEnemy, uint32_t amount)
{
	//TODO: find way of getting enemy position to spawn exp
	switch (pEnemy[0])
	{
	case 0x016F593C:
		ChaosEnergyLogic(1); // EFighter
		break;
	case 0x016F70BC:
		ChaosEnergyLogic(3); // Spinner
		break;
	case 0x016FB1FC:
		ChaosEnergyLogic(1); // EFighterMissile
		break;
	case 0x016FB62C:
		ChaosEnergyLogic(4); // AirCannon
		break;
	case 0x016F912C:
		ChaosEnergyLogic(2); // Mole
		break;
	default:
		ChaosEnergyLogic(3);
		break;
	}

	return 0;
}

void __declspec(naked) getEnemyChaosEnergyType()
{
	static uint32_t returnAddress = 0xBE05F7;
	__asm
	{
		mov		edx, ecx
		mov		ecx, esi
		call	getEnemyChaosEnergyTypeImpl
		mov		ecx, eax

		// original function
		mov     edx, [esi]
		mov     eax, [edx + 9Ch]
		jmp[returnAddress]
	}
}

void EXPCollect::removeEXPCollect(Sonic::CGameObject* exp)
{
	expCache.erase(std::find(expCache.begin(), expCache.end(), exp));
}
//if (SaveManager::getCurrentSave() != nullptr)
//{
//	SaveManager::getCurrentSave()->keysFloat.push_back(new KeyFloat("EXPProgress", expProgress));
//	SaveManager::getCurrentSave()->keysFloat.push_back(new KeyFloat("EXPLevel", expLevel));
//}
#pragma endregion
void EXPCollect::applyPatches()
{
	WRITE_STRING(0x1613B98, ""); // disable stinky hud particles EW!
	WRITE_JUMP(0xBE05EF, getEnemyChaosEnergyType);
	INSTALL_HOOK(CHudSonicStageDelayProcessImpEXP);
	INSTALL_HOOK(HudSonicEXPUpdate);
}