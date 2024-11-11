#pragma once
#include "../../ClassicPluginExtensions.h"

class EvilStateRunE : public SUC::Player::Evil::CEvilBasicStateBase
{
public:
	static inline float ms_PadIntensity = 0.85000002f;
	BB_STATE_NAME("Evil_RunE")
	EvilStateRunE()
	{
		ms_MovementSpeed = 5.5f;
	}

	void EnterState() override
	{
		float m_PadIntensity = GetPadIntensity();
		prevPadIntensity = m_PadIntensity;
		if (m_PadIntensity != 0)
		SONIC_CLASSIC_CONTEXT->m_pPlayer->m_spAnimationStateMachine->ChangeState("Run_M");
	}
	float prevPadIntensity = 0;
	void UpdateState() override
	{
		CEvilBasicStateBase::UpdateState();
		CEvilBasicStateBase::CheckForJump();
		float m_PadIntensity = GetPadIntensity();

		if (abs(m_PadIntensity - prevPadIntensity) > 0.25f)
		{
			SONIC_CLASSIC_CONTEXT->m_pPlayer->m_StateMachine.ChangeState("EvilRunStop");
			return;
		}
		else
		{
			if (m_PadIntensity < ms_PadIntensity)
			SONIC_CLASSIC_CONTEXT->m_pPlayer->m_StateMachine.ChangeState("Evil_WalkE");
			
		}
	}
};
