#include "RingEnergy.h"
namespace SUC::Gameplay
{
	void OnRing(hh::fnd::Message& msg, bool superRing = false)
	{
		if (!*pModernSonicContext)
			return;

		Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();
		if (context->m_pPlayer->m_ActorID != msg.m_SenderActorID)
			return;

		if (*Common::GetPlayerBoost() < 100.0f)
			*Common::GetPlayerBoost() = std::clamp(*Common::GetPlayerBoost() + (superRing ? RingEnergy::m_SuperRingAdd : RingEnergy::m_RingAdd), 0.0f, 100.0f);
	}
	HOOK(void, __fastcall, CObjRingProcMsgHitEventCollision, 0x10534B0, Sonic::CGameObject3D* This, void* Edx, hh::fnd::Message& in_rMsg)
	{
		OnRing(in_rMsg);
		originalCObjRingProcMsgHitEventCollision(This, Edx, in_rMsg);
	}
	HOOK(void, __fastcall, CObjSuperRingProcMsgHitEventCollision, 0x11F2F10, Sonic::CGameObject3D* This, void* Edx, hh::fnd::Message& in_rMsg)
	{
		OnRing(in_rMsg, true);
		originalCObjSuperRingProcMsgHitEventCollision(This, Edx, in_rMsg);
	}
	HOOK(void, __fastcall, RingEnergyUpdate, 0xE6BF20, void* This, void* Edx, float* dt)
	{
		originalRingEnergyUpdate(This, Edx, dt);

		if (!*pModernSonicContext)
			return;

		if (Common::GetSonicStateFlags()->Drifting && *Common::GetPlayerBoost() < 100.0f)
			*Common::GetPlayerBoost() = std::clamp(*Common::GetPlayerBoost() + (Common::GetSonicStateFlags()->Boost ? RingEnergy::m_DriftAdd * 0.4f : RingEnergy::m_DriftAdd) * *dt, 0.0f, 100.0f);
	}
	void RingEnergy::RegisterHooks()
	{
		INSTALL_HOOK(RingEnergyUpdate);
		INSTALL_HOOK(CObjRingProcMsgHitEventCollision);
		INSTALL_HOOK(CObjSuperRingProcMsgHitEventCollision);
	}
}