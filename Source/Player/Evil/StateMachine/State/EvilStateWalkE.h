#pragma once
#include "../../ClassicPluginExtensions.h"

class EvilStateWalkE : public SUC::Player::Evil::CEvilBasicStateBase
{
public:
	static inline float ms_PadIntensity = 0.5f;
	BB_STATE_NAME("Evil_WalkE")
		EvilStateWalkE()
	{
		ms_MovementSpeed = 2.2f;
	}

	void EnterState() override
	{
		float m_PadIntensity = GetPadIntensity();
		if (m_PadIntensity != 0)
		SONIC_CLASSIC_CONTEXT->m_pPlayer->m_spAnimationStateMachine->ChangeState("Walk_M");
	}
	void UpdateState() override
	{
		CEvilBasicStateBase::UpdateState();
		CEvilBasicStateBase::CheckForJump();
		float m_PadIntensity = GetPadIntensity();
		if (m_PadIntensity == 0)
		{
			SONIC_CLASSIC_CONTEXT->m_pPlayer->m_StateMachine.ChangeState("Stand");
			return;
		}
		if (m_PadIntensity >= ms_PadIntensity)
		{
			if (m_Timer >= CEvilBasicStateBase::ms_StateChangeDelay)
				SONIC_CLASSIC_CONTEXT->m_pPlayer->m_StateMachine.ChangeState("Evil_RunE");

			m_Timer += GetDeltaTime();
		}
		else if(m_PadIntensity < ms_PadIntensity)
		{
			SONIC_CLASSIC_CONTEXT->m_pPlayer->m_StateMachine.ChangeState("Evil_WalkSlowE");			
		}
	}
};
