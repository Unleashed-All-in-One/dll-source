#include "ExperienceObject.h"

namespace SUC::Accuracy
{
	boost::shared_ptr<Sonic::CGameObjectCSD> spExp;
	Chao::CSD::RCPtr<Chao::CSD::CProject> rcExp;
	Chao::CSD::RCPtr<Chao::CSD::CScene> exp_count;
	std::vector<Sonic::CGameObject*> expCache;

	bool displayedHUD = false;
	float m_timerFade;
	float expProgress = 0.0f;
	float expProgressPrevious = 0.0f;
	int expLevel = 0;
	float maxExpProgress;

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
	void ExperienceObject::AddProgress(float progress)
	{
		expProgressPrevious = expProgress;
		expProgress += progress;
	}
	void ExperienceObject::DisplayHUD()
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
		if (m_timerFade >= 5)
		{
			CSDCommon::PlayAnimation(exp_count, "size", Chao::CSD::eMotionRepeatType_PlayOnce, 1, expProgress);
			CSDCommon::FreezeMotion(exp_count, expProgress);
			CSDCommon::PlayAnimation(exp_count, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, false, true);
			displayedHUD = false;
			m_timerFade = 0;
		}
		else
		{
			float progress = Common::LerpFloat(expProgressPrevious, expProgress, m_timerFade <= 2 ? m_timerFade / 2 : 1);
			if (progress / maxExpProgress > 1)
			{
				expLevel++;
				expProgressPrevious = 0;
				expProgress = maxExpProgress - progress;
				progress = 0;
			}
			CSDCommon::PlayAnimation(exp_count, "size", Chao::CSD::eMotionRepeatType_PlayOnce, 1, (progress / maxExpProgress) * 100);
			CSDCommon::FreezeMotion(exp_count, expProgress);
			CSDCommon::PlayAnimation(exp_count, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, frameBefore + 1);
		}

		char text[4];
		sprintf(text, "%02d", expLevel);
		exp_count->GetNode("exp")->SetText(text);
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
		CSDCommon::FreezeMotion(exp_count);
		if (SUC::System::SaveManager::GetCurrentSave() != nullptr)
		{
			expProgress = SUC::System::SaveManager::GetCurrentSave()->GetSaveBoolKeyValue("EXPProgress");
			expLevel = SUC::System::SaveManager::GetCurrentSave()->GetSaveBoolKeyValue("EXPLevel");
		}
		if (expLevel > 99)
			expLevel = 99;
		maxExpProgress = 22.0f + (11.0f * expLevel);
		CreateScreenEXP(This);
	}
#pragma endregion
#pragma region Orbs
	void ExperienceObject::RemoveEXPCollect(Sonic::CGameObject* exp)
	{
		expCache.erase(std::find(expCache.begin(), expCache.end(), exp));
	}
	void ChaosEnergyLogic(uint32_t in_Amount, hh::math::CVector in_StartPosition)
	{
		Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

		if (!context)
			return;

		DebugDrawText::log(SUC::Format("[EXP] SpawnPoint: (%.3f, %.3f, %.3f)", in_StartPosition.x(), in_StartPosition.y(), in_StartPosition.z()));
		for (uint32_t i = 0; i < in_Amount; i++)
		{
			boost::shared_ptr<ExperienceObject> newEXP = boost::make_shared<ExperienceObject>(in_StartPosition);
			expCache.push_back(newEXP.get());
			// modification example : ((EXPCollect*)expCache.at(i))->test = true;
			context->m_pPlayer->m_pMember->m_pGameDocument->AddGameObject(newEXP);
		}
	}	
	uint32_t __fastcall GetEnemyChaosEnergyTypeImpl(uint32_t* in_pEnemy, uint32_t in_Amount)
	{
		auto v5 = (*(int*(__thiscall**)(uint32_t*))(*in_pEnemy + 176))(in_pEnemy);
		auto matrix = (hh::math::CMatrix44*)(*(int(__thiscall**)(int*))(*v5 + 12))(v5);
		
		int m_ExperienceOrbCount = 3;
		//Since no enemy is mapped, check the enemy class's vftable
		//to determine the orb count
		switch ((ExperienceObject::EEnemyVtableEntries)in_pEnemy[0])
		{
		case ExperienceObject::EFIGHTERMISSILE:
		case ExperienceObject::EFIGHTER:
			m_ExperienceOrbCount = 1; // EFighter
			break;
		case ExperienceObject::SPINNER:
			m_ExperienceOrbCount = 3; // Spinner
			break;
		case ExperienceObject::AIRCANNON:
			m_ExperienceOrbCount = 4; // AirCannon
			break;
		case ExperienceObject::MOLE:
			m_ExperienceOrbCount = 2; // Mole
			break;
		}
		auto m_EnemyGO = reinterpret_cast<Sonic::CGameObject3D*>(in_pEnemy);
		ChaosEnergyLogic(m_ExperienceOrbCount, matrix->col(3));
		return 0;
	}
	void __declspec(naked) ASM_GetEnemyChaosEnergyType()
	{
		static uint32_t returnAddress = 0xBE05F7;
		__asm
		{
			mov		edx, ecx
			mov		ecx, esi
			call	GetEnemyChaosEnergyTypeImpl
			mov		ecx, eax

			// original function
			mov     edx, [esi]
			mov     eax, [edx + 9Ch]
			jmp[returnAddress]
		}
	}
	//if (SaveManager::GetCurrentSave() != nullptr)
	//{
	//	SaveManager::GetCurrentSave()->keysFloat.push_back(new KeyFloat("EXPProgress", expProgress));
	//	SaveManager::GetCurrentSave()->keysFloat.push_back(new KeyFloat("EXPLevel", expLevel));
	//}
#pragma endregion
	void ExperienceObject::RegisterHooks()
	{
		//Disable HUD particles
		WRITE_STRING(0x1613B98, "");
		WRITE_JUMP(0xBE05EF, ASM_GetEnemyChaosEnergyType);
		INSTALL_HOOK(CHudSonicStageDelayProcessImpEXP);
		INSTALL_HOOK(HudSonicEXPUpdate);
	}
}