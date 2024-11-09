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
		SONIC_CLASSIC_CONTEXT->m_pPlayer->m_AnimationStateMachine->ChangeState("Run_M");
	}
	void UpdateState() override
	{
		CEvilBasicStateBase::UpdateState();
		float m_PadIntensity = GetPadIntensity();
		if (m_PadIntensity == 0) SONIC_CLASSIC_CONTEXT->m_pPlayer->m_StateMachine.ChangeState("Stand");
		
	}
};
