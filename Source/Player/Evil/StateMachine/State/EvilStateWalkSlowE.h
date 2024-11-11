#pragma once
#include "EvilStateWalkE.h"
#include "../../ClassicPluginExtensions.h"


class EvilStateWalkSlowE : public SUC::Player::Evil::CEvilBasicStateBase
{
public:
	static inline float ms_PadIntensity = 0.1f;
	BB_STATE_NAME("Evil_WalkSlowE")
	EvilStateWalkSlowE()
	{
		ms_MovementSpeed = 1;
	}

	void EnterState() override
	{
		float m_PadIntensity = GetPadIntensity();
		if (m_PadIntensity != 0)
		SONIC_CLASSIC_CONTEXT->m_pPlayer->m_spAnimationStateMachine->ChangeState("WalkSlow_M");
	}
	void UpdateState() override
	{
		CEvilBasicStateBase::UpdateState();
		CEvilBasicStateBase::CheckForJump();
		float m_PadIntensity = GetPadIntensity();
		auto input = InputSingleton;		
		if (m_PadIntensity == 0)
		{
			SONIC_CLASSIC_CONTEXT->m_pPlayer->m_StateMachine.ChangeState("Stand");
			return;
		}
		if(m_PadIntensity >= ms_PadIntensity)
		{
			if(m_Timer >= CEvilBasicStateBase::ms_StateChangeDelay)
				SONIC_CLASSIC_CONTEXT->m_pPlayer->m_StateMachine.ChangeState<EvilStateWalkE>();

			m_Timer += GetDeltaTime();
		}
			
	}
};
