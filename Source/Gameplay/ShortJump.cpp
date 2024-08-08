#include "ShortJump.h"
namespace SUC::Gameplay
{
	int m_LastHurdleIndex = 0;
	int m_currentHurdleIndex = 1;
	bool m_ShortJumpMove = false;
	float m_JumpTimer = 0.0f;

	void PlayAnimBegin()
	{
		Eigen::Vector3f playerVelocity;
		if (!Common::GetPlayerVelocity(playerVelocity))
			return;

		m_ShortJumpMove = playerVelocity.norm() >= 7.0f;
		if (m_ShortJumpMove)
		{
			m_currentHurdleIndex = (m_LastHurdleIndex == 0) ? 1 : 0;
			m_LastHurdleIndex = m_currentHurdleIndex;

			Hedgehog::Base::CSharedString hurdleAnimName = (m_currentHurdleIndex == 0) ? "JumpHurdleL" : "JumpHurdleR";
			Common::SonicContextChangeAnimation(hurdleAnimName);
			m_JumpTimer = ShortJump::s_HurdleTime;
		}
		else
		{
			Common::SonicContextChangeAnimation("JumpShortBegin");
			m_JumpTimer = ShortJump::s_JumpTime;
		}
	}
	void PlayAnimTop()
	{
		if (!m_ShortJumpMove)
			Common::SonicContextChangeAnimation("JumpShortTop");
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

		m_LastHurdleIndex = Common::SonicContextIsAnimation("JumpHurdleL") ? 1 : 0;

		return result;
	}
	HOOK(void, __fastcall, ShortJumpResetUpdate, 0xE6BF20, void* This, void* Edx, float* dt)
	{
		originalShortJumpResetUpdate(This, Edx, dt);

		if (!*pModernSonicContext)
			return;

		bool isAnim = Common::SonicContextIsAnimation("JumpHurdleL") || Common::SonicContextIsAnimation("JumpHurdleR") || Common::SonicContextIsAnimation("JumpShortBegin") || Common::SonicContextIsAnimation("JumpShortTop");

		if (isAnim && m_JumpTimer > 0.0f)
			m_JumpTimer -= *dt;
		else if (isAnim && m_JumpTimer <= 0.0f) {
			StateManager::ChangeState(StateAction::Fall, *PLAYER_CONTEXT);
			Common::SonicContextChangeAnimation("Fall");
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