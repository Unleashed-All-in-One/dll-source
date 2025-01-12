#include "RampParticle.h"
namespace SUC::Accuracy
{
	SharedPtrTypeless rampParticleHandle;

	void CreateRampParticle()
	{
		if (!SONIC_MODERN_CONTEXT)
			return;


		Common::SpawnParticle(SONIC_MODERN_CONTEXT, rampParticleHandle, SONIC_MODERN_CONTEXT->m_spField30.get(), "ef_ch_sng_lms_jump_delux", 1);
	}
	void PlayRampAnimation()
	{
		if (!SONIC_MODERN_CONTEXT)
			return;

		Common::ChangePlayerAnimation("JumpBoardLoop");
	}
	//https://github.com/brianuuu/DllMods/blob/master/Source/ColorsVoice/Mod.cpp
	//https://github.com/brianuuu/DllMods/blob/master/Source/Sonic06DefinitiveExperience/NextGenPhysics.cpp
	HOOK(void, __fastcall, RampApplyImpulse, 0xE6CFA0, void* This, void* Edx, MsgApplyImpulse* message)
	{
		originalRampApplyImpulse(This, Edx, message);

		if (!SONIC_MODERN_CONTEXT)
			return;

		if (message->m_ImpulseType == MsgApplyImpulse::eImpulseType_JumpBoardSpecial)
			CreateRampParticle();
		else if (message->m_ImpulseType == MsgApplyImpulse::eImpulseType_JumpBoard)
			PlayRampAnimation();
	}
	void RampParticle::RegisterHooks()
	{
		INSTALL_HOOK(RampApplyImpulse);
	}
}