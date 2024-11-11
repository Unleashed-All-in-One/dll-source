#pragma once
#include "../../ClassicPluginExtensions.h"

class EvilStateJump : public SUC::Player::Evil::CEvilBasicStateBase
{
public:
	static inline float ms_PadIntensity = 0.85000002f;
	bool m_DashMode = false;
	BB_STATE_NAME("Evil_Jump")
		EvilStateJump()
	{
		ms_MovementSpeed = 4;
	}
	void AddJumpThrust(float m_Power)
	{
		SONIC_CLASSIC_CONTEXT->m_pPlayer->m_PostureStateMachine.ChangeState("Air");
		SONIC_CLASSIC_CONTEXT->m_Grounded = true;
		SONIC_CLASSIC_CONTEXT->SetVerticalVelocity(Hedgehog::Math::CVector(0, m_Power, 0));
		SONIC_CLASSIC_CONTEXT->HandleHorizontalOrVerticalVelocityChanged();
	}
	void EnterState() override
	{
		auto t = SONIC_CLASSIC_CONTEXT->m_pPlayer->m_spAnimationStateMachine->GetCurrentState();
		if (t->GetStateName() == "Dash_M")
			m_DashMode = true;
		if(m_DashMode)
		{
			SONIC_CLASSIC_CONTEXT->m_pPlayer->m_spAnimationStateMachine->ChangeState("Dash_JumpS");
			AddJumpThrust(8);
		}
		else
		{
			SONIC_CLASSIC_CONTEXT->m_pPlayer->m_spAnimationStateMachine->ChangeState("JumpStand");
			AddJumpThrust(10);
		}
	}
	float prevPadIntensity = 0;
	void UpdateState() override
	{
		if(!m_DashMode)
		{
			auto singleton = InputSingleton;
			if(singleton->IsTapped(Sonic::eKeyState_A))
			{
				SONIC_CLASSIC_CONTEXT->m_pPlayer->m_spAnimationStateMachine->ChangeState("JumpSecond");
				AddJumpThrust(10);
			}
			CEvilBasicStateBase::UpdateState();
			
		}
		float m_PadIntensity = GetPadIntensity();

		//if (abs(m_PadIntensity - prevPadIntensity) > 0.25f)
		//{
		//	SONIC_CLASSIC_CONTEXT->m_pPlayer->m_StateMachine.ChangeState("EvilRunStop");
		//	return;
		//}
		//else
		//{
		//	if (m_PadIntensity < ms_PadIntensity)
		//		SONIC_CLASSIC_CONTEXT->m_pPlayer->m_StateMachine.ChangeState("Evil_WalkE");
		//
		//}
	}
};
