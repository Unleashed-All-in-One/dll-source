#pragma once
namespace SUC::Player::Evil
{
	class CStateRunningJump : public Sonic::Player::CPlayerSpeedContext::CStateSpeedBase
	{
	public:
		static constexpr const char* ms_pStateName = "EvilRunningJump";
		static Hedgehog::math::CVector target;
		float lastFrame = -1;

		static float easeInOutQuart(float x)
		{
			return x < 0.5
				? (1 - sqrt(1 - pow(2 * x, 2))) / 2
				: (sqrt(1 - pow(-2 * x + 2, 2)) + 1) / 2;
		}
		void EnterState() override
		{
			EvilGlobal::s_CanAttack = false;
			lastFrame = -1;
			GetContext()->SetStateFlag(Sonic::Player::CPlayerSpeedContext::eStateFlag_IgnorePadInput, true);
		}
		void ForceExit()
		{
			EvilGlobal::s_CanAttack = true;
			GetContext()->m_pPlayer->m_StateMachine.ChangeState("Fall");
			GetContext()->SetStateFlag(Sonic::Player::CPlayerSpeedContext::eStateFlag_IgnorePadInput, false);			
		}
		void UpdateState() override
		{
			if (GetContext()->m_Grounded)
			{
				ForceExit();
				return;
			}

			const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
			GetContext()->m_pPlayer->SendMessageImm(GetContext()->m_pPlayer->m_ActorID, spAnimInfo);
			if (spAnimInfo->m_Frame > 15)
				ForceExit();
			lastFrame = spAnimInfo->m_Frame;
		}
		void LeaveState()
		{
			EvilGlobal::s_CanAttack = true;
			GetContext()->SetStateFlag(Sonic::Player::CPlayerSpeedContext::eStateFlag_IgnorePadInput, false);
		}
	};
}