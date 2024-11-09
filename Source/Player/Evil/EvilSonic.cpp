
#include "EvilSonic.h"

#include "ClassicPluginExtensions.h"
#include "EvilGlobal.h"
#include "EvilAttackConfiguration.h"
#include "StateMachine/State/EvilStateRunningJump.h"
#include "StateMachine/State/EvilStateArmSwing.h"
#include "StateMachine/State/EvilStateAttackAction_byList.h"
#include "StateMachine/State/EvilStateDamageNormal.h"
#include "StateMachine/State/EvilStateArmSwing.h"

namespace SUC::Player::Evil
{
	using namespace hh::math;
	using namespace Sonic;
	struct MsgResetCamera : public hh::fnd::MessageTypeSet
	{
	public:
		HH_FND_MSG_MAKE_TYPE(0x016821B0);

		MsgResetCamera() {}
	};
	enum WerehogState
	{
		Normal,
		Dash,
		Guard
	};

	struct MsgRestartStage : public Hedgehog::Universe::MessageTypeSet
	{
	public:
		HH_FND_MSG_MAKE_TYPE(0x01681FA0);
	};
	SharedPtrTypeless sound, soundUnleash, soundUnleashStart;
	SharedPtrTypeless soundRegularJump;
	SharedPtrTypeless indexParticle_L, indexParticle_R;
	SharedPtrTypeless middleParticle_L, middleParticle_R;
	SharedPtrTypeless pinkyParticle_L, pinkyParticle_R;
	SharedPtrTypeless ringParticle_L, ringParticle_R;
	SharedPtrTypeless thumbParticle_L, thumbParticle_R;
	SharedPtrTypeless punch, referenceEffect;
	SharedPtrTypeless shield;
	SharedPtrTypeless berserk[5];
	std::vector<Sonic::EKeyState> currentButtonChain;
	WerehogAttackNew attackCache;
	bool canJump;
	int jumpcount;
	int comboAttackIndex;
	int lastAttackIndex;
	bool isUsingShield;

	float tempTimerWalk;
	float deltaTime;
	float timerCombo;
	float timerAttack;
	float timerDamage = 0.3f;
	int comboProgress = 0;
	bool unleashMode;
	bool playingAttack;
	bool isGrounded;
	Sonic::EKeyState lastTap;
	bool cameraAnimTempExecuted = false;

	//find a better way please
	bool init = false;
	boost::shared_ptr<Sonic::CGameObject3D> collision1;
	Hedgehog::math::CVector Evil::CStateArmSwing::target;
	WerehogState currentState;

	std::string lastMusicCue;
	void ChangeAnimation(const char* in_Name)
	{
		const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		const auto spAnimInfo = boost::make_shared<MsgChangePlayerAnimation>(in_Name);
		playerContext->m_pPlayer->m_AnimationStateMachine->ChangeState(in_Name);
	}
	void RegisterClassicAnimations()
	{
		//Register some of the basic non-attack anims
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_damageMB", "evilsonic_damageMB");
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_guard_idle", "evilsonic_guard_idle");
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_BerserkerS", "evilsonic_BerserkerS");
		System::AnimationSetPatcher::RegisterClassicAnimation("JumpEvil1", "evilsonic_jumpVS");
		System::AnimationSetPatcher::RegisterClassicAnimation("JumpEvil2", "evilsonic_jumpVS2");
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_dashS", "evilsonic_dashS");
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_dash", "evilsonic_dash");
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_start", "evilsonic_start");
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_runS", "evilsonic_runS");
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_pillar_idle", "evilsonic_pillar_idle");
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_pillar_up", "evilsonic_pillar_up", 1, true);
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_pillar_upH", "evilsonic_pillar_upH", 1, false);
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_gate", "evilsonic_gate", 1, false);
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_pillar_turnL", "evilsonic_pillar_turnL");
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_pillar_turnR", "evilsonic_pillar_turnR");
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_pillar_fall", "evilsonic_pillar_fall", 1, true);
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_run", "evilsonic_run", 3);
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_runE", "evilsonic_runE");
		System::AnimationSetPatcher::RegisterClassicAnimation("Evilsonic_dash_jumpS", "evilsonic_dash_jumpS");
		System::AnimationSetPatcher::RegisterClassicAnimation("WalkSlow_M", "evilsonic_walk_slow", 4, true);
		System::AnimationSetPatcher::RegisterClassicAnimation("Walk_M", "evilsonic_walk", 4, true);
		System::AnimationSetPatcher::RegisterClassicAnimation("Run_M", "evilsonic_run",4 ,true);
	}
	bool IsCurrentAnimationName(std::string in)
	{
		auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
		playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
		return std::strstr(spAnimInfo->m_Name.c_str(), in.c_str()) != nullptr;
	}
	void AddImpulse(CVector impulse, bool relative)
	{
		if (!relative)
		{
			SONIC_GENERAL_CONTEXT->m_Velocity = impulse;
		}
		else
		{
			SONIC_GENERAL_CONTEXT->m_Velocity += impulse;
		}
	}
	void AddJumpThrust(CSonicContext* sonicContext, bool Condition)
	{
		uint32_t func = 0x00E57C50;
		__asm
		{
			push Condition
			mov edi, sonicContext
			call func
		};
	};
	void PlayAnim(std::string name)
	{
		const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
		playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
		const char* animName = name.c_str();
		ChangeAnimation(animName);
	}
	float GetVelocity()
	{
		if (currentState == WerehogState::Guard)
			return 1;
		if (currentState == WerehogState::Normal && tempTimerWalk < 0.5f)
			return 2.2f;
		if (currentState == WerehogState::Normal)
			return 5.5f;
		if (currentState == WerehogState::Dash)
			return 12;
		return 1;
	}
	void RegisterInputs()
	{
		auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
		auto state = inputPtr->TappedState;
		DebugDrawText::log(std::to_string(state).c_str(), 0);
		lastTap = inputPtr->DownState;
		DebugDrawText::log(std::to_string(lastTap).c_str(), 0);
		if (timerCombo < EvilGlobal::s_Param->timerComboMax)
		{
			if (lastTap == eKeyState_A || state == eKeyState_X || state == eKeyState_Y)
				currentButtonChain.push_back(state);
			if (currentButtonChain.size() > 8)
			{
				currentButtonChain.erase(currentButtonChain.begin());
			}
			DebugDrawText::log("Current chain:", 0);
			for (size_t i = 0; i < currentButtonChain.size(); i++)
			{
				DebugDrawText::log(std::to_string(currentButtonChain.at(i)).c_str(), 0);
			}
		}
		else
			currentButtonChain.clear();
	}
	void DespawnParticlesHand()
	{
		indexParticle_L.reset();
		middleParticle_L.reset();
		pinkyParticle_L.reset();
		ringParticle_L.reset();
		thumbParticle_L.reset();


		indexParticle_R.reset();
		middleParticle_R.reset();
		pinkyParticle_R.reset();
		ringParticle_R.reset();
		thumbParticle_R.reset();
		punch.reset();
	}
	void SpawnParticleOnHand(std::string glitterNameLeft, std::string glitterNameRight, std::string boneless)
	{
		///
		///							O
		///						   /|\
		///						  / | \
		///	  glitterNameRight-> /  |  \ <- glitterNameLeft
		///							|
		///						   / \
		///						  /   \
		///					     /  v  \
		///						 boneless
		///		

		const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		DespawnParticlesHand();
		if (!boneless.empty())
		{
			auto ref = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Reference");
			if (!referenceEffect)
				Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), referenceEffect, &ref, boneless.c_str(), 1);
		}
		if (!glitterNameLeft.empty())
		{
			if (PARTICLE_CLAW == glitterNameLeft)
			{
				auto index = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Index1_L");
				auto middle = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Middle1_L");
				auto pinky = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Pinky1_L");
				auto ring = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Ring1_L");
				auto thumb = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Thumb1_L");
				if (!indexParticle_L)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), indexParticle_L, &index, glitterNameLeft.c_str(), 1);
				if (!middleParticle_L)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), middleParticle_L, &middle, glitterNameLeft.c_str(), 1);
				if (!pinkyParticle_L)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), pinkyParticle_L, &pinky, glitterNameLeft.c_str(), 1);
				if (!ringParticle_L)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), ringParticle_L, &ring, glitterNameLeft.c_str(), 1);
				if (!thumbParticle_L)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), thumbParticle_L, &thumb, glitterNameLeft.c_str(), 1);
			}
			else
			{
				auto hand = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Arm09Sub_L");
				if (!punch)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), punch, &hand, glitterNameLeft.c_str(), 1);
			}
		}
		if (!glitterNameRight.empty())
		{
			if (PARTICLE_CLAW == glitterNameRight)
			{
				auto index = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Index1_R");
				auto middle = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Middle1_R");
				auto pinky = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Pinky1_R");
				auto ring = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Ring1_R");
				auto thumb = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Thumb1_R");
				if (!indexParticle_R)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), indexParticle_R, &index, glitterNameRight.c_str(), 1);
				if (!middleParticle_R)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), middleParticle_R, &middle, glitterNameRight.c_str(), 1);
				if (!pinkyParticle_R)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), pinkyParticle_R, &pinky, glitterNameRight.c_str(), 1);
				if (!ringParticle_R)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), ringParticle_R, &ring, glitterNameRight.c_str(), 1);
				if (!thumbParticle_R)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), thumbParticle_R, &thumb, glitterNameRight.c_str(), 1);
			}
			else
			{
				auto hand = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Arm09Sub_R");
				if (!punch)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), punch, &hand, glitterNameRight.c_str(), 1);
			}
		}
	}
	void KillBerserkEffect()
	{
		berserk[0].reset();
		berserk[1].reset();
		berserk[2].reset();
	}
	void CreateBerserkEffect()
	{
		KillBerserkEffect();
		auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		auto node0 = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Needle1_U_C");
		auto node1 = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_R");
		auto node2 = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_L");
		if (!berserk[0])
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), berserk[0], &node0, "evil_berserk01", 1);
		if (!berserk[1])
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), berserk[1], &node1, "evil_berserk01", 1);
		if (!berserk[2])
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), berserk[2], &node2, "evil_berserk01", 1);
	}
	void PlaySoundStaticCueName2(SharedPtrTypeless& soundHandle, Hedgehog::base::CSharedString cueID, CVector a4)
	{
		uint32_t* syncObject = *(uint32_t**)0x1E79044;
		if (syncObject)
		{
			FUNCTION_PTR(void*, __thiscall, sub_75FA60, 0x75FB00, void* This, SharedPtrTypeless&, const Hedgehog::base::CSharedString & cueId, const CVector & a4);
			sub_75FA60((void*)syncObject[8], soundHandle, cueID, a4);
		}
	}
	void ExecuteAttackCommand(std::string attack, int attackIndex, bool starter = false)
	{
		lastAttackIndex = attackIndex;
		auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		Common::SonicContextSetCollision(SonicCollision::TypeSonicSquatKick, true);

		auto node = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_R");
		DebugDrawText::log(std::format("Matrix 0 {0}, {1}, {2}", node->GetWorldMatrix().data()[0], node->GetWorldMatrix().data()[1], node->GetWorldMatrix().data()[2]).c_str(), 10);
		//EvilGlobal::shockwaveGameObject = Common::CreatePlayerSupportShockWaveReturnGameObject(playerContext->m_spMatrixNode->m_Transform.m_Position + (playerContext->m_spMatrixNode->m_Transform.m_Rotation * CVector(0, 0, 2)), 2, 2, 100);
		//PlayAnim(EvilGlobal::GetStateNameFromTable(attack));
		/*Common::PlaySoundStatic(sound, attacks.at(attackIndex).cueIDs[comboIndex]);*/
		EvilGlobal::s_LatestAttackName = attack;

		auto resourcelist = EvilAttackConfiguration::s_AnimationTable.at(attackIndex).ResourceInfos.Resources;
		int resourceIndex = 0;
		int effectIndex = 0;
		for (size_t i = 0; i < resourcelist.size(); i++)
		{
			if (resourcelist[i].Type == CSB)
			{
				resourceIndex = i;
				break;
			}
		}
		for (size_t i = 0; i < resourcelist.size(); i++)
		{
			if (resourcelist[i].Type == Effect)
			{
				effectIndex = i;
				break;
			}
		}
		playerContext->m_Velocity = CVector(0, 0, 0);
		playerContext->HandleVelocityChanged();
		playerContext->m_pPlayer->m_StateMachine.ChangeState<Evil::CStateAttackAction_byList>();
		AddImpulse(((playerContext->m_spMatrixNode->m_Transform.m_Rotation * Eigen::Vector3f::UnitZ()) * ((EvilGlobal::GetMotionFromName(attack).MotionMoveSpeedRatio) * 10)) * 100, true);
		sound.reset();
		lastMusicCue = resourcelist[resourceIndex].Params.Cue;
		Common::PlaySoundStaticCueName(sound, Hedgehog::base::CSharedString(resourcelist[resourceIndex].Params.Cue.c_str()));

		SpawnParticleOnHand(EvilGlobal::GetMotionFromName(attack).Effect.LEffect_Name1, EvilGlobal::GetMotionFromName(attack).Effect.REffect_Name1, resourcelist[effectIndex].Params.Cue);
		timerAttack = 0;
	}
	void SearchThenExecute(std::string input, bool starter, Sonic::EKeyState state)
	{
		if (starter)
		{
			for (size_t i = 0; i < EvilAttackConfiguration::s_StartAttacks.size(); i++)
			{
				if (EvilAttackConfiguration::s_StartAttacks[i].ActionName == input)
				{
					if (EvilAttackConfiguration::s_StartAttacks[i].MotionName.empty())
						return;
					comboAttackIndex = i;
					timerCombo = 0;
					playingAttack = true;
					attackCache = EvilAttackConfiguration::s_StartAttacks[i];
					ExecuteAttackCommand(EvilAttackConfiguration::s_StartAttacks[i].MotionName, comboAttackIndex, true);
					break;
				}
			}
		}
		else
		{
			for (size_t i = 0; i < EvilAttackConfiguration::s_Attacks.size(); i++)
			{
				if (input == EvilAttackConfiguration::s_Attacks[i].ActionName)
				{
					std::string container;
					if (state == eKeyState_X)
						container = EvilAttackConfiguration::s_Attacks[i].KEY__XDown;
					if (state == eKeyState_Y)
						container = EvilAttackConfiguration::s_Attacks[i].KEY__YDown;
					if (state == eKeyState_A)
						container = EvilAttackConfiguration::s_Attacks[i].KEY__ADown;

					if (container.empty())
						continue;
					ExecuteAttackCommand(container, comboAttackIndex, false);
					for (size_t x = 0; x < EvilAttackConfiguration::s_Attacks.size(); x++)
					{
						///FIX
						if (EvilGlobal::s_LatestAttackName == container)
						{
							attackCache = EvilAttackConfiguration::s_Attacks[x];
							break;
						}
					}

					comboAttackIndex = i;
					timerCombo = 0;
					playingAttack = true;
					break;
				}
			}

			if (!playingAttack)
			{
				timerAttack = EvilGlobal::s_Param->timerAttackMax;
				timerCombo = EvilGlobal::s_Param->timerComboMax;
				currentButtonChain.clear();
			}
		}
	}
	float SetPlayerVelocity()
	{
		switch (currentState)
		{
		case WerehogState::Dash:
		{
			return 20;
		}
		case WerehogState::Normal:
		{
			return 10;
		}
		case WerehogState::Guard:
		{
			return 2;
		}
		}
		return 10;
	}
	void Particle_Checker()
	{
		if (timerAttack > EvilGlobal::s_Param->timerAttackMax)
			DespawnParticlesHand();
	}
	void CheckForThinPlatform()
	{
		Eigen::Vector4f outPos;

		const uint32_t result = *(uint32_t*)((uint32_t) * (void**)((uint32_t)*PLAYER_CONTEXT + 0x110) + 172);
		float* position = (float*)(*(uint32_t*)(result + 16) + 112);
		const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		Eigen::Vector4f const rayLeftStart(position[0] - 1, position[1], position[2], 1.0f);
		Eigen::Vector4f const rayLeftEnd(position[0] - 1, position[1], position[2] - 5, 1.0f);
		Eigen::Vector4f outNormal;
		if (Common::fRaycast(rayLeftStart, rayLeftStart, outPos, outNormal, *(uint32_t*)0x1E0AFB4))
		{
			DebugDrawText::log("Left Hit!", 0);
		}
	}
	std::string getEVSId()
	{
		uint32_t* appdocMember = (uint32_t*)Sonic::CApplicationDocument::GetInstance()->m_pMember;
		auto gameParameters = *((DWORD*)appdocMember + 109);
		Hedgehog::Base::CSharedString* evsIDLoc = (Hedgehog::Base::CSharedString*)(*((DWORD*)gameParameters + 32) + 44);
		return evsIDLoc->c_str();

	}
	HOOK(void, __fastcall, CPlayerSpeedContext_AddCallback, 0xE799F0, Sonic::Player::CPlayer* This, void* Edx,
		const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder, Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
	{
		originalCPlayerSpeedContext_AddCallback(This, Edx, worldHolder, pGameDocument, spDatabase);

		if (IS_CLASSIC_SONIC)
		{
			const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
			playerContext->m_ChaosEnergy = 0;
			canJump = true;
			isGrounded = true;
			playingAttack = false;
			playerContext->m_pPlayer->m_PostureStateMachine.ChangeState("StartCrouchingPosture");
		}

	}
	HOOK(void, __fastcall, Classic_UpdateSerial, 0xDDABA0, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
	{
		originalClassic_UpdateSerial(This, Edx, in_rUpdateInfo);
		deltaTime = in_rUpdateInfo.DeltaTime;
		if (IS_CLASSIC_SONIC && getEVSId() == "")
		{
			ImGuiMenu::WerehogMenu::s_Visible = true;
			const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
			if (abs(playerContext->m_Velocity.x() + playerContext->m_Velocity.z()) > 0)
				tempTimerWalk += in_rUpdateInfo.DeltaTime;
			else
				tempTimerWalk = 0;
			const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
			playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
			//if (playerContext->m_pPlayer->m_StateMachine.GetCurrentState() == nullptr)
			//	return;
			Hedgehog::Base::CSharedString stateCheck = playerContext->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
			std::string stateCheckS(stateCheck.c_str());
			//DebugDrawText::log((std::string("Current Player Anim: ") + std::string(spAnimInfo->m_Name.c_str())).c_str(), 0);
			//DebugDrawText::log((std::string("tempTimerWalk: ") + std::to_string(tempTimerWalk)).c_str(), 0);
			//
			//DebugDrawText::log((std::string("Current Player Posture: ") + std::string(playerContext->m_pPlayer->m_PostureStateMachine.GetCurrentState()->m_Name.c_str())).c_str(), 0);
			if (!IS_CLASSIC_SONIC)
			{
				return;
			}
			DebugDrawText::log((std::string("Current Player State: ") + stateCheckS).c_str(), 0);
			if (timerDamage <= EvilGlobal::s_Param->timerDamageMax)
				timerDamage += in_rUpdateInfo.DeltaTime;
			// Force disable extended boost.
			*(uint32_t*)((uint32_t)*SONIC_GENERAL_CONTEXT->ms_pInstance + 0x680) = 1;
			SONIC_GENERAL_CONTEXT->m_ChaosEnergy = min(SONIC_GENERAL_CONTEXT->m_ChaosEnergy, 100);
			
			//Set rotational parameters every frame since they sometimes reset, maybe find a better way to do this
			//playerContext->m_JumpThrust = CVector(playerContext->m_JumpThrust.x(), 1, playerContext->m_JumpThrust.z());
			playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_BoostEnableChaosEnergy] = 1000.0f;
			playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_AccelerationForce] = 100.0f;
			playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_DecelerationForceHigh] = 200.0f;
			playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_DecelerationForceLow] = 200.0f;
			float rotForce = 1200.0f;
			if (currentState == WerehogState::Dash)
			{
				rotForce = 30.0f;
				playerContext->m_WorldInput /= 10;

			}
			else if (currentState == WerehogState::Guard)
				rotForce = 0;
			playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_RotationForce0] = rotForce;
			playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_RotationForce1] = rotForce;
			playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_RotationForce2] = rotForce;
			playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::eStateFlag_EnableAirOnceAction] = false;
			playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_AirBoostEnableChaosEnergy] = 1000.0f;
			playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_EnableHurdleJumpMinVelocity] = 1000.0f;

			SONIC_CLASSIC_CONTEXT->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_MaxHorzVelocity] = GetVelocity();



			isGrounded = playerContext->m_Grounded;
			/*sonic->m_spParameter->m_scpNode->m_ValueMap.erase(Sonic::Player::ePlayerSpeedParameter_BoostEnableChaosEnergy);
			sonic->m_spParameter->m_scpNode->m_ValueMap.erase(Sonic::Player::ePlayerSpeedParameter_AirBoostEnableChaosEnergy);*/

			//hideous
			DebugDrawText::log("### WEREHOG INFO ### ", 0, 0, DebugDrawText::Color(1, 0, 1, 1));
			DebugDrawText::log(std::format("Timer Combo: {0}", timerCombo).c_str(), 0, 1);
			DebugDrawText::log(std::format("Timer Combo Max: {0}", EvilGlobal::s_Param->timerComboMax).c_str(), 0, 2);
			DebugDrawText::log(std::format("Timer Attack: {0}", timerAttack).c_str(), 0, 3);
			DebugDrawText::log(std::format("Timer Attack Max: {0}", EvilGlobal::s_Param->timerAttackMax).c_str(), 0, 4);
			DebugDrawText::log(std::format("Combo progress: {0}", comboProgress).c_str(), 0, 5);
			DebugDrawText::log(std::format("Boost amount: {0}", SONIC_GENERAL_CONTEXT->m_ChaosEnergy).c_str(), 0, 6);
			DebugDrawText::log(std::format("HP: {0}", EvilGlobal::s_Param->lifeCurrentAmount).c_str(), 0, 7);
			DebugDrawText::log(std::format("Attack Anim: {0}", EvilGlobal::s_LatestAttackName).c_str(), 0, 8);
			DebugDrawText::log(std::format("Timer Combo: {0}", timerCombo).c_str(), 0, 9);
			DebugDrawText::log(std::format("Posture: {0}", SONIC_GENERAL_CONTEXT->m_pPlayer->m_PostureStateMachine.GetCurrentState()->m_Name.c_str()).c_str(), 0, 10);
			DebugDrawText::log("-----------------------------------------------", 0, 11, DebugDrawText::Color(1, 0, 1, 1));


			auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
			if (inputPtr->IsDown(eKeyState_X) || inputPtr->IsDown(eKeyState_Y) || inputPtr->IsDown(eKeyState_A))
			{
				timerCombo = 0;
			}
			RegisterInputs();
			Particle_Checker();
			if (isGrounded && jumpcount > 0)
			{
				canJump = true;
				jumpcount = 0;
			}
			if (unleashMode && SONIC_GENERAL_CONTEXT->m_ChaosEnergy > 0)
			{
				playerContext->m_ChaosEnergy -= in_rUpdateInfo.DeltaTime * 5;
			}
			else
			{
				unleashMode = false;
				KillBerserkEffect();
				soundUnleash.reset();
			}
			if (inputPtr->IsDown(eKeyState_DpadDown))
			{
				auto node = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_R");

			}
			if ((inputPtr->IsDown(eKeyState_RightBumper) && SONIC_GENERAL_CONTEXT->m_ChaosEnergy == 100.0f) && !unleashMode && isGrounded)
			{
				playerContext->m_Velocity = CVector(0, 0, 0);
				PlayAnim("Evilsonic_BerserkerS");
				CreateBerserkEffect();
				Common::PlaySoundStatic(soundUnleashStart, 126);
				Common::PlaySoundStatic(soundUnleash, 127);
				unleashMode = true;
			}
			if (inputPtr->IsDown(eKeyState_LeftBumper) && currentState != WerehogState::Guard)
			{
				currentState = WerehogState::Guard;
				//ef_ch_sng_yh1_boost2
				auto node0 = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hips");
				if (!shield)
					Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), shield, &node0, "evil_guard_sphere01", 1);

				PlayAnim("Evilsonic_guard_idle");
			}
			else
			{
				currentState = WerehogState::Normal;
				shield.reset();
			}
			CheckForThinPlatform();

			if (inputPtr->IsDown(Sonic::eKeyState_RightTrigger))
			{
				currentState = WerehogState::Dash;
			}
			else
			{
				currentState = WerehogState::Normal;
			}
			if (!playingAttack && isGrounded)
			{

				//DebugDrawText::log(std::format("SPEED_MAGNITUDE: {0}", abs(playerContext->m_Velocity.norm())).c_str(), 0);
				//playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
				//if (currentState == WerehogState::Dash)
				//{
				//	if (abs(playerContext->m_Velocity.norm() > 12))
				//	{
				//		if ((!IsCurrentAnimationName("Evilsonic_dash"))
				//			|| (IsCurrentAnimationName("Evilsonic_dash") && spAnimInfo->m_Frame >= 10))
				//		{
				//			PlayAnim("Evilsonic_dash");
				//		}
				//	}
				//}
				//if (currentState == WerehogState::Normal)
				//{
				//	/*	if(std::strstr(spAnimInfo->m_Name.c_str(), "Evilsonic_dash") != nullptr)
				//			PlayAnim("Evilsonic_dashE");*/
				//	if (abs(playerContext->m_Velocity.norm() > 5))
				//	{
				//		if ((!IsCurrentAnimationName("Evilsonic_run"))
				//			|| (IsCurrentAnimationName("Evilsonic_run") && spAnimInfo->m_Frame >= 51))
				//			PlayAnim("Evilsonic_run");
				//	}
				//
				//}
			}

			if (timerCombo > EvilGlobal::s_Param->timerComboMax)
			{
				comboProgress = 0;
				Common::SonicContextSetCollision(SonicCollision::TypeSonicSquatKick, false);
			}
			else
			{
				//Attack state prepare logic
				if (EvilGlobal::s_CanAttack)
				{
					if (timerAttack > EvilGlobal::s_Param->timerAttackMax && playingAttack)
					{
						playingAttack = false;
						comboProgress++;
					}

					if ((timerCombo > 0.1f && comboProgress > 0 || comboProgress == 0) && (timerAttack > EvilGlobal::s_Param->timerAttackMax) && currentButtonChain.size() > comboProgress)
					{
						if (currentButtonChain[currentButtonChain.size() - 1] == eKeyState_X || currentButtonChain[currentButtonChain.size() - 1] == eKeyState_Y || currentButtonChain[currentButtonChain.size() - 1] == eKeyState_A)
						{

							if (comboProgress == 0)
							{
								std::string attackName = "Start_";
								if (currentState == WerehogState::Dash)
								{
									if (isGrounded)
										attackName += "Dash_Run_";
									else
										attackName += "Dash_";

								}
								if (currentState == WerehogState::Guard)
									attackName += "Guard_";
								if (!isGrounded)
									attackName += "Air_";
								if (!isGrounded || currentState == WerehogState::Dash)
								{
									switch (currentButtonChain[comboProgress])
									{
									case eKeyState_X:
									{
										attackName += "XButtonDown";
										break;
									}
									case eKeyState_Y:
									{
										attackName += "YButtonDown";
										break;
											
									}
									}
								}
								else
								{
									switch (currentButtonChain[comboProgress])
									{
									case eKeyState_X:
									{
										attackName += "XButtonUP";
										break;
									}
									case eKeyState_Y:
									{
										attackName += "YButtonUP";
										break;
									}
									}
								}
								SearchThenExecute(attackName, true, eKeyState_None);
							}

							if (comboProgress >= 1)
							{
								SearchThenExecute(EvilGlobal::s_LatestAttackName, false, currentButtonChain[comboProgress]);
							}

						}
					}

				}
			}
			DebugDrawText::log((std::string("Jump count: ") + std::to_string(jumpcount)).c_str(), 0);
			if (inputPtr->IsTapped(Sonic::eKeyState_A) && !playingAttack)
			{
				isGrounded = false;
				if (jumpcount == 1 && canJump)
				{
					canJump = false;
					Sonic::Player::CPlayerSpeedContext::GetInstance()->m_Velocity.y() = 0;
					AddJumpThrust(Sonic::Player::CPlayerSpeedContext::GetInstance(), true);
					Common::PlaySoundStatic(sound, 42);

					PlayAnim("JumpEvil2");
					jumpcount++;
				}

			}

			timerCombo += in_rUpdateInfo.DeltaTime;
			timerAttack += in_rUpdateInfo.DeltaTime;
		}
	}
	HOOK(void, __cdecl, Classic_InitParameters, 0x00D65180, Sonic::CParameterFile* This)
	{
		//boost::shared_ptr<Sonic::CParameterGroup> parameterGroup = boost::make_shared<Sonic::CParameterGroup>();
		//auto group = This->CreateParameterGroup("EvilSonic", "Parameters for the Werehog Mod from NextinHKRY");
		////meow
		//auto cat = group->CreateParameterCategory("Test", "test");
		//cat->CreateParamFloat(&EvilGlobal::parameters->timerComboMax, "Timer Combo Maximum");
		//cat->CreateParamFloat(&EvilGlobal::parameters->timerAttackMax, "Timer Attack Maximum");
		//cat->CreateParamFloat(&EvilGlobal::parameters->timerDamageMax, "Timer Damage Maximum");
		//cat->CreateParamFloat(&EvilGlobal::parameters->lifeCurrentAmount, "Life Amount");
		//cat->CreateParamFloat(&EvilGlobal::parameters->lifeMaxAmount, "Life Max Amount");
		//cat->CreateParamFloat(&EvilGlobal::parameters->attackVelocityDivider, "Atttack Velocity Divider");
		//
		//parameterGroup->Flush();

		originalClassic_InitParameters(This);
	}
	HOOK(void, __fastcall, Classic_SetMaximumVelocity, 0x00DC2020, CSonicContext* This)
	{
		if (*pClassicSonicContext && Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID == 1)
		{
			float* maxSpeed = Common::GetPlayerMaxSpeed();
			*maxSpeed = max(*maxSpeed, SetPlayerVelocity());
		}
	}
	HOOK(double, __fastcall, Classic_GetMaximumVelocity, 0x00DC1F20, CSonicContext* This, void* Edx, int a2)
	{
		return SONIC_GENERAL_CONTEXT->m_MaxVelocity;
	}
	HOOK(void, __fastcall, Classic_DamageProcessor, 0xDEA340, Sonic::Player::CSonic* This, void* _, Sonic::Message::MsgDamage& in_rMsg)
	{
		if (timerDamage >= EvilGlobal::s_Param->timerDamageMax)
		{
			EvilGlobal::s_Param->lifeCurrentAmount -= 1;
			if (EvilGlobal::s_Param->lifeCurrentAmount > 0)
			{
				timerDamage = 0;
				if (in_rMsg.m_DamageType == 1)
				{
					Sonic::Player::CPlayerSpeedContext::GetInstance()->ChangeState<Evil::CStateDamageNormal>();
				}
				else
				{
					//Sonic::Player::CPlayerSpeedContext::GetInstance()->ChangeState<Evil::CStateDamageFly>();
				}
			}
			else
			{
				//Sonic::Player::CPlayerSpeedContext::GetInstance()->ChangeState<Evil::CStateDamageDead>();
				SONIC_GENERAL_CONTEXT->m_RingCount = 0;
				originalClassic_DamageProcessor(This, _, in_rMsg);
			}

		}
	}
	HOOK(void, __fastcall, Classic_RestartStageProcessor, 0xE76810, Sonic::Player::CPlayer* This, void* Edx, hh::fnd::Message& message)
	{
		originalClassic_RestartStageProcessor(This, Edx, message);
		EvilGlobal::s_Param->lifeCurrentAmount = EvilGlobal::s_Param->lifeMaxAmount;
	}
	HOOK(void, __fastcall, CSonicStateWalk_Update, 0x00DED4E0, int* This)
	{
		if (IS_CLASSIC_SONIC && timerAttack < EvilGlobal::s_Param->timerAttackMax && timerCombo < EvilGlobal::s_Param->timerComboMax)
		{
			//if (playingAttack)
			//	PlayAnim(EvilGlobal::GetStateNameFromTable(EvilGlobal::s_LatestAttackName));
		}
		else
			originalCSonicStateWalk_Update(This);
	}
	HOOK(int, __fastcall, CSonicStateHomingAttack_Begin, 0x01232040, CQuaternion* This)
	{
		if (IS_CLASSIC_SONIC)
		{
			/*if (SONIC_GENERAL_CONTEXT->m_HomingAttackTargetActorID)
			{
				return originalCSonicStateHomingAttack_Begin(This);
			}
			else*/
			return 0;
		}
		return originalCSonicStateHomingAttack_Begin(This);
	}
	HOOK(void, __fastcall, CSonicClassicStateMoveStop_Update, 0x11146c0, void* This, void* Edx)
	{
		Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_StateMachine.ChangeState("Walk");
	}
	HOOK(char, __fastcall, CSonicClassicStateJumpShort_StartJump, 0x01114CB0, int* This)
	{
		if (playingAttack == true && IS_CLASSIC_SONIC)
			return 0;
		auto retu = originalCSonicClassicStateJumpShort_StartJump(This);
		return  retu;

	}
	HOOK(void, __fastcall, CPlayerSpeedContext_CStateJumpBall_Start, 0x01235250, int This)
	{
		originalCPlayerSpeedContext_CStateJumpBall_Start(This);
		if (IS_CLASSIC_SONIC)
		{
			if (jumpcount == 0)
			{
				if (currentState == WerehogState::Dash)
				{
					PlayAnim("Evilsonic_dash_jumpS");
					Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_StateMachine.ChangeState<Evil::CStateRunningJump>();
				}
				else
					PlayAnim("JumpEvil1");

				Common::PlaySoundStatic(sound, 42);
			}
			else
			{
				if (currentState == WerehogState::Dash)
					return;
				if (canJump)
				{
					AddJumpThrust(Sonic::Player::CPlayerSpeedContext::GetInstance(), true);
					PlayAnim("JumpEvil2");
				}
			}
			jumpcount++;
		}
	}
	HOOK(char, __fastcall, CPlayerSpeedContext_ChangeToHomingAttackX, 0x00DFFE30, CSonicContext* This, void* Edx, int a2)
	{
		if (IS_CLASSIC_SONIC)
			return 0;
		else
			return originalCPlayerSpeedContext_ChangeToHomingAttackX(This, Edx, a2);
	}
	HOOK(void, __fastcall, CSonicClassicStateNormalDamageDead_Begin, 0x01252170, int* a1, void* Edx)
	{

		Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_ActorID, boost::make_shared<Sonic::Message::MsgDead>(true));
		Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_ActorID, boost::make_shared<MsgResetCamera>());
	}
	HOOK(void, __fastcall, CSonicSetMaxSpeedBasis, 0xDFBCA0, int* This)
	{
		originalCSonicSetMaxSpeedBasis(This);

		if (*pClassicSonicContext && Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID == 1)
		{
			float* maxSpeed = Common::GetPlayerMaxSpeed();
			*maxSpeed = max(*maxSpeed, SetPlayerVelocity());
		}
	}
	HOOK(void, __fastcall, NormalDamageDeadUpdate, 0x012523C0, int* a1, void* Edx)
	{

	}
	HOOK(void, __fastcall, NormalDamageDeadAfter, 0x12520D0, int* a1, void* Edx)
	{
	}
	

	//Hedgehog::Math::CVector *__thiscall sub_1241B50(CTempState *this)
	HOOK(void, __fastcall, CClassicStateWalk_Update, 0x012419B0, void* This, void* Edx)
	{
		SONIC_GENERAL_CONTEXT->m_pPlayer->m_StateMachine.ChangeState("Evil_WalkSlowE");
	}
	//sub_53B830()
	HOOK(int, __fastcall, Footsteps, 0x53B830, int This,void* Edx, void* a2, int a3, int a4, float a5)
	{
		if (!IS_CLASSIC_SONIC)
			return originalFootsteps(This, Edx, a2, a3, a4, a5);
		else
			return 0;
	}
	extern "C" __declspec(dllexport) float API_GetLife()
	{
		return EvilGlobal::s_Param->lifeCurrentAmount;
	}
	extern "C" __declspec(dllexport) bool API_IsWerehogActive()
	{
		return IS_CLASSIC_SONIC;
	}
	//BOOL __thiscall GetIsOutOfControl(CSonicContext *this)
	HOOK(char, __fastcall, GetIsOutOfControl, 0x00E4D350, Sonic::Player::CPlayerSpeedContext* This, void* Edx)
	{
		if (IS_CLASSIC_SONIC)
		{
			auto m_StateName = This->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
			return This->GetStateFlag(Sonic::Player::CPlayerSpeedContext::eStateFlag_OutOfControl) || m_StateName != "Stand";
		}
		else
			originalGetIsOutOfControl(This, Edx);
	}
	HOOK(Hedgehog::Math::CVector*, __fastcall, SonicClassicGetAnimSpeed, 0xDD9240, void* This, void* Edx, void* a2)
	{
			if(SONIC_CLASSIC_CONTEXT->m_spParameter)
			{
				return new Hedgehog::Math::CVector(EvilGlobal::s_AnimationSpeed, EvilGlobal::s_AnimationSpeed, EvilGlobal::s_AnimationSpeed);
			}			
		
		return originalSonicClassicGetAnimSpeed(This,Edx,a2);
	}
	//Hedgehog::Math::CVector *__thiscall sub_DD92B0(_DWORD *this, Hedgehog::Math::CVector *a2)

	void EvilSonic::RegisterPatches()
	{
		EvilGlobal::s_CanAttack = true;
		RegisterClassicAnimations();
		INSTALL_HOOK(CPlayerSpeedContext_AddCallback);
		INSTALL_HOOK(Footsteps);
		INSTALL_HOOK(GetIsOutOfControl);
		INSTALL_HOOK(SonicClassicGetAnimSpeed);
		INSTALL_HOOK(Classic_UpdateSerial);
		INSTALL_HOOK(Classic_InitParameters);

		/// ------------------------------
		///	Classic patches
		/// ------------------------------

		INSTALL_HOOK(Classic_SetMaximumVelocity);
		INSTALL_HOOK(Classic_GetMaximumVelocity);
		INSTALL_HOOK(Classic_DamageProcessor);
		INSTALL_HOOK(Classic_RestartStageProcessor);

		/// ------------------------------
		///	Original States patches
		/// ------------------------------

		INSTALL_HOOK(CClassicStateWalk_Update);
		INSTALL_HOOK(CSonicStateWalk_Update);
		INSTALL_HOOK(CSonicStateHomingAttack_Begin);
		INSTALL_HOOK(CSonicClassicStateMoveStop_Update);
		INSTALL_HOOK(CSonicClassicStateJumpShort_StartJump);
		INSTALL_HOOK(CPlayerSpeedContext_CStateJumpBall_Start);
		INSTALL_HOOK(CPlayerSpeedContext_ChangeToHomingAttackX);
		INSTALL_HOOK(CSonicClassicStateNormalDamageDead_Begin);

		//Jump over check for start anims
		WRITE_JUMP(0X00D900C7, 0X00D9019A);

		//Force Jump stuff
		WRITE_JUMP(0x01114A14, 0x01114A39);
		WRITE_JUMP(0x01114D95, 0x01114DDA);

		//Disable Spindash
		WRITE_JUMP(0x00DC28D9, 0x00DC2946);

		//Unmap stomp/spin for classic
		WRITE_JUMP(0X00DC5F7E, 0X00DC6054);
		WRITE_JUMP(0X012523C0, 0x012530E0);

		//Disable MoveStop
		//WRITE_JUMP(0x0111C020, 0x0111C066);

		//Force the jump sound to be silent - disable if we redo classic
		//WRITE_MEMORY(0x00E57E4E, uint32_t, -1);

	}
}