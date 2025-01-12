#include "ShortJump.h"
namespace SUC::Gameplay
{
	int m_LastHurdleIndex = 0;
	int m_currentHurdleIndex = 1;
	bool m_ShortJumpMove = false;
	float m_JumpTimer = 0.0f;

	void PlayAnimBegin()
	{
		auto context = SONIC_GENERAL_CONTEXT;
		auto playerVelocity = context->m_Velocity;
		m_ShortJumpMove = playerVelocity.norm() >= 7.0f;
		if (m_ShortJumpMove)
		{
			m_currentHurdleIndex = (m_LastHurdleIndex == 0) ? 1 : 0;
			m_LastHurdleIndex = m_currentHurdleIndex;

			Hedgehog::Base::CSharedString hurdleAnimName = (m_currentHurdleIndex == 0) ? "JumpHurdleL" : "JumpHurdleR";
			Common::ChangePlayerAnimation(hurdleAnimName);
			m_JumpTimer = ShortJump::s_HurdleTime;
		}
		else
		{
			Common::ChangePlayerAnimation("JumpShortBegin");
			m_JumpTimer = ShortJump::s_JumpTime;
		}
	}
	void PlayAnimTop()
	{
		if (!m_ShortJumpMove)
			Common::ChangePlayerAnimation("JumpShortTop");
	}
	void __declspec(naked) ShortTop()
	{
		static uint32_t shortTop = 0x011BF036;
		__asm
		{
			call PlayAnimTop
			jmp[shortTop]
		}
	}
	void __declspec(naked) ShortBegin()
	{
		static uint32_t shortBegin = 0x011BF26E;
		__asm
		{
			call PlayAnimBegin
			jmp[shortBegin]
		}
	}
	HOOK(int, __fastcall, HurdleJumpBegin, 0xDF1760, int a1, void* Edx)
	{
		int result = originalHurdleJumpBegin(a1, Edx);

		auto m_AnimName = SONIC_GENERAL_CONTEXT->m_pPlayer->m_spAnimationStateMachine->GetCurrentState()->GetStateName();
		m_LastHurdleIndex = m_AnimName == "JumpHurdleL" ? 1 : 0;

		return result;
	}
	HOOK(void, __fastcall, ShortJumpResetUpdate, 0xE6BF20, void* This, void* Edx, float* dt)
	{
		originalShortJumpResetUpdate(This, Edx, dt);
		auto context = SONIC_MODERN_CONTEXT;
		if (!context)
			return;
		auto m_AnimName = context->m_pPlayer->m_spAnimationStateMachine->GetCurrentState()->GetStateName();
		bool isAnim = m_AnimName == "JumpHurdleL" || m_AnimName == "JumpHurdleR" || m_AnimName =="JumpShortBegin" || m_AnimName == "JumpShortTop";

		if (isAnim && m_JumpTimer > 0.0f)
		{
			m_JumpTimer -= *dt;			
		}
		else if (isAnim && m_JumpTimer <= 0.0f) 
		{
			context->ChangeState(Sonic::Player::ePlayerSpeedState_Fall);
			Common::ChangePlayerAnimation("Fall");
		}
	}
	void ShortJump::RegisterHooks()
	{
		INSTALL_HOOK(ShortJumpResetUpdate);
		INSTALL_HOOK(HurdleJumpBegin);
		WRITE_JUMP(0x011BF008, ShortTop);
		WRITE_JUMP(0x011BF247, ShortBegin);
	}
}