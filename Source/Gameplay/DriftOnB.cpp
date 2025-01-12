#include "DriftOnB.h"
namespace SUC::Gameplay
{
	HOOK(void, __fastcall, BDriftUpdate, 0xE6BF20, void* This, void* Edx, float* dt)
	{
		originalBDriftUpdate(This, Edx, dt);

		if (!SONIC_MODERN_CONTEXT)
			return;

		if (!DriftOnB::CheckForBDrift())
			return;

		Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();
		Hedgehog::Base::CSharedString state = sonic->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
		Sonic::SPadState input = Sonic::CInputState::GetInstance()->GetPadState();

		if (input.IsDown(Sonic::eKeyState_B) && abs(input.LeftStickHorizontal) >= 0.85f && (state == "Walk" || state == "Sliding"))
			sonic->ChangeState("Drift");
		else if (input.IsUp(Sonic::eKeyState_B) && state == "Drift" && input.IsUp(Sonic::eKeyState_LeftTrigger) && input.IsUp(Sonic::eKeyState_RightTrigger))
			sonic->ChangeState("Walk");
	}
	bool DriftOnB::CheckForBDrift()
	{
		auto sonic = SONIC_MODERN_CONTEXT;

		if (!sonic)
			return false;

		return abs(sonic->m_Velocity.norm()) >= 25.0f && !sonic->m_Is2DMode && !sonic->m_spForwardPathController && !
			sonic->GetStateFlag(Sonic::Player::CPlayerSpeedContext::eStateFlag_OutOfControl) && !sonic->
			m_spGrindPathController && !sonic->GetStateFlag(
				Sonic::Player::CPlayerSpeedContext::eStateFlag_InvokeSkateBoard);
	}
	void DriftOnB::RegisterHooks()
	{
		INSTALL_HOOK(BDriftUpdate);
	}
}