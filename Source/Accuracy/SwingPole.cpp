#include "SwingPole.h"
namespace SUC::Accuracy
{
	const char* SwingPole::GetPoleSwingParticle()
	{
		Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();

		if (!sonic)
			return "";

		if (sonic->m_Is2DMode)
			return "ef_ch_sng_lms_bar_trace02";

		return "ef_ch_sng_lms_bar_trace01";
	}
	void SwingPole::LocusUpdate()
	{
		if (IsSwingJump() && !s_LocusPlayed)
		{
			s_LocusPlayed = true;
			Common::SonicContextRequestLocusEffect();
		}
		else if (!IsSwingJump() && s_LocusPlayed)
			s_LocusPlayed = false;
	}
	void SwingPole::PlayTrail()
	{
		Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();

		if (!sonic)
			return;

		if (!Common::DoesNodeExistOnSonic("Foot_L"))
			return;

		boost::shared_ptr<Hedgehog::Mirage::CMatrixNodeSingleElementNode> FootNode = sonic->m_pPlayer->m_spCharacterModel->GetNode("Foot_L");
		Common::SpawnParticle(sonic, s_SwingHandle, &FootNode, GetPoleSwingParticle(), 1);
	}
	void SwingPole::TrailUpdate()
	{
		Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();

		if (!sonic)
			return;

		if (!Common::DoesNodeExistOnSonic("Foot_L"))
			return;
		auto animName = sonic->m_pPlayer->m_spAnimationStateMachine->GetCurrentState()->GetStateName();

		if (animName == "PoleSpinLoop" && !s_TrailPlayed)
		{
			PlayTrail();
			s_TrailPlayed = true;
		}
		else if (animName != "PoleSpinLoop" && s_TrailPlayed)
		{
			Common::KillParticle(sonic, s_SwingHandle, true);
			s_TrailPlayed = false;
		}
	}
	bool SwingPole::IsSwingJump()
	{
		return strstr(SONIC_GENERAL_CONTEXT->m_pPlayer->m_spAnimationStateMachine->GetCurrentState()->GetStateName().c_str(), "PoleSpinJump");
	}
	HOOK(void, __fastcall, PoleUpdate, 0xE6BF20, void* This, void* Edx, float* dt)
	{
		originalPoleUpdate(This, Edx, dt);

		if (!SONIC_MODERN_CONTEXT)
			return;

		SwingPole::LocusUpdate();
		SwingPole::TrailUpdate();
	}
	void SwingPole::RegisterHooks()
	{
		INSTALL_HOOK(PoleUpdate);
	}
}
