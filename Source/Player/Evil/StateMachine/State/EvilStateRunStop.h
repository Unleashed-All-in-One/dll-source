#pragma once
class EvilStateRunStop : public Hedgehog::Universe::CStateMachineBase::CStateBase
{
public:
	BB_STATE_NAME("EvilRunStop")
	float m_Timer = 0;
	void EnterState() override
	{
		SONIC_CLASSIC_CONTEXT->m_pPlayer->m_spAnimationStateMachine->ChangeState("RunEnd_M");
	}
	void UpdateState() override
	{
		m_Timer += GetDeltaTime();
		if(m_Timer > 0.2f)
		{
			SONIC_CLASSIC_CONTEXT->m_pPlayer->m_StateMachine.ChangeState("Stand");
		}
	}
};