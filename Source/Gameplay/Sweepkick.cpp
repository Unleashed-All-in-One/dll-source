#include "Sweepkick.h"
int m_IsBButtonPressed = 0;
float m_InputResetTimer = -1;
float m_SweepkickCollisionTime = 0.0f;
SharedPtrTypeless m_SquatKickParticleHandle;
SharedPtrTypeless m_SquatKickSoundHandle;
Hedgehog::Math::CQuaternion m_SquatKickRotation;
bool m_IsSweepkickActive = false;

SpawnableLight m_SweepLight;

HOOK(void, __fastcall, CSonicStateSquatKickBegin, 0x12526D0, void* This)
{
	originalCSonicStateSquatKickBegin(This);

	Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

	if (!context)
		return;

	boost::shared_ptr<Hedgehog::Mirage::CMatrixNodeSingleElementNode> RefNode = context->m_pPlayer->m_spCharacterModel->GetNode("Reference");
	Common::fCGlitterCreate(context, m_SquatKickParticleHandle, &RefNode, (Common::IsPlayerSuper() ? "ef_ch_sps_lms_sliding_kick" : "ef_ch_sng_lms_sliding_kick"), 0);
	
	Common::PlaySoundStatic(m_SquatKickSoundHandle, 2002497);

	if (Sweepkick::useLight) {
		m_SweepLight.Color = Common::IsPlayerSuper() ? Sweepkick::colorSuper : Sweepkick::color;
		m_SweepLight.Play();
	}

	m_SweepkickCollisionTime = Sweepkick::supportShockwaveDelay;
}

HOOK(void, __fastcall, CSonicStateSquatKickAdvance, 0x1252810, void* This) {
	Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

	if (!context)
		return;

	if (m_SweepkickCollisionTime <= 0 && Sweepkick::useSupportShockwave) 
		Common::CreatePlayerSupportShockWave(context->m_spMatrixNode->m_Transform.m_Position, 0.15f, 3.0f, 0.1f);

	originalCSonicStateSquatKickAdvance(This);
}

// Original Code by Briannu
bool __fastcall CSonicStateSquatKickAdvanceTransitionOutImpl(char const* name)
{
	Eigen::Vector3f inputDirection;
	if (strcmp(name, "Stand") == 0 && Sweepkick::transitionToStand && Common::GetWorldInputDirection(inputDirection) && inputDirection.isZero())
	{
		StateManager::ChangeState(StateAction::Squat, *PLAYER_CONTEXT);
		Common::SonicContextChangeAnimation("SquatToStand");
		return true;
	}

	return false;
}

void __declspec(naked) CSonicStateSquatKickAdvanceTransitionOut()
{
	static uint32_t returnAddress = 0x1252924;
	static uint32_t sub_E4FF30 = 0xE4FF30;
	__asm
	{
		push    eax
		push    ecx

		mov     ecx, [eax]
		call    CSonicStateSquatKickAdvanceTransitionOutImpl
		mov     bl, al

		pop     ecx
		pop     eax

		test    bl, bl
		jnz     jump
		call[sub_E4FF30]

		jump:
		jmp[returnAddress]
	}
}

void Onm_IsBButtonPressed(Sonic::Player::CPlayerSpeedContext* sonic, Hedgehog::Base::CSharedString state) {
	if (!sonic)
		return;

	if (m_InputResetTimer < 0.0f)
		m_InputResetTimer = Sweepkick::sweepInputTime;

	m_IsBButtonPressed++;

	bool canSquatKick = m_IsBButtonPressed == 2 
		&& m_InputResetTimer > 0.0f 
		&& (state == "Squat" || state == "Sliding" || state == "Walk" || state == "SlidingEnd" || state == "StompingLand" || state == "SquatCharge") 
		&& !Common::IsPlayerControlLocked();

	if (!canSquatKick)
		return;
	
	m_IsBButtonPressed = 0;
	m_SquatKickRotation = sonic->m_spMatrixNode->m_Transform.m_Rotation;
	sonic->ChangeState("SquatKick");
	m_IsSweepkickActive = true;
}

HOOK(void, __fastcall, SweepkickOnUpdate, 0xE6BF20, void* This, void* Edx, float* dt) {
	originalSweepkickOnUpdate(This, Edx, dt);

	if (!*pModernSonicContext)
		return;

	Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();
	Hedgehog::Base::CSharedString state = sonic->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
	Sonic::SPadState input = Sonic::CInputState::GetInstance()->GetPadState();

	if (!sonic)
		return;

	if (m_SweepkickCollisionTime > 0)
		m_SweepkickCollisionTime -= *dt;

	if (input.IsTapped(Sonic::eKeyState_B) && !Common::GetSonicStateFlags()->OutOfControl)
		Onm_IsBButtonPressed(sonic, state);

	if ((state == "SquatKick" || m_IsSweepkickActive) && sonic->m_Velocity.norm() == 0.0f)
		sonic->m_spMatrixNode->m_Transform.SetRotation(m_SquatKickRotation);

	if (m_IsSweepkickActive && (state != "Squat" && state != "Sliding" && state != "Stand" && state != "Walk" && state != "SlidingEnd" && state != "StompingLand" && state != "SquatCharge" && state != "SquatKick")) {
		Common::fCGlitterEnd(sonic, m_SquatKickParticleHandle, true);
		m_SweepLight.Stop(true);
		m_IsSweepkickActive = false;
	}

	if (m_IsSweepkickActive && state == "Walk" && Common::GetSonicStateFlags()->OutOfControl) {
		Common::fCGlitterEnd(sonic, m_SquatKickParticleHandle, true);
		m_SweepLight.Stop(true);
		m_IsSweepkickActive = false;
	}

	if (m_IsSweepkickActive && Sweepkick::useLight) {
		m_SweepLight.Position = m_SweepLight.playerLocalPos(Sweepkick::offset);
		m_SweepLight.Update(*dt);
	}

	if (m_InputResetTimer >= 0.0f)
		m_InputResetTimer -= *dt;
	else
		m_IsBButtonPressed = 0;
}

HOOK(int, __fastcall, ProcMsgRestartStageSweepkick, 0xE76810, uint32_t* This, void* Edx, void* message)
{
	if (*pModernSonicContext && Sweepkick::useLight)
		m_SweepLight.Stop(true);
	
	return originalProcMsgRestartStageSweepkick(This, Edx, message);
}

void Sweepkick::applyPatches() {
	m_SweepLight.FadeInSpeed = Sweepkick::lerpSpeedIn;
	m_SweepLight.FadeOutSpeed = Sweepkick::lerpSpeedOut;
	m_SweepLight.LifeTime = Sweepkick::lightLifeTime;
	m_SweepLight.Range = Sweepkick::lightRange;
	m_SweepLight.StartDelay = Sweepkick::lightInDelay;
	m_SweepLight.Color = Sweepkick::color;

	// Enable sweep kick attack collision immediately
	static double const c_sweepKickActivateTime = 0.0;
	WRITE_MEMORY(0x125299E, double*, &c_sweepKickActivateTime);

	// Change SquatKick's collision the same as sliding
	WRITE_MEMORY(0xDFCD6D, uint8_t, 0x5); // switch 6 cases
	static uint32_t const collisionSwitchTable[6] =
	{
		0xDFCD7B, // normal
		0xDFCDC0, // slide
		0xDFCD7B, // boost
		0xDFCD7B,
		0xDFCDFA, // unused
		0xDFCDC0  // squat kick
	};

	WRITE_MEMORY(0xDFCD77, uint32_t*, collisionSwitchTable);
	WRITE_JUMP(0x125291F, CSonicStateSquatKickAdvanceTransitionOut);

	INSTALL_HOOK(SweepkickOnUpdate);
	INSTALL_HOOK(CSonicStateSquatKickBegin);
	INSTALL_HOOK(CSonicStateSquatKickAdvance);
	INSTALL_HOOK(ProcMsgRestartStageSweepkick);
}