SharedPtrTypeless rampParticleHandle;

void CreateRampParticle() {
	if (!*pModernSonicContext)
		return;

	void* middlematrixNode = (void*)((uint32_t)*PLAYER_CONTEXT + 0x30);
	Common::fCGlitterCreate(*PLAYER_CONTEXT, rampParticleHandle, middlematrixNode, "ef_ch_sng_lms_jump_delux", 1);
}

void PlayRampAnimation() {
	if (!*pModernSonicContext)
		return;

	Common::SonicContextChangeAnimation("JumpBoardLoop");
}

//https://github.com/brianuuu/DllMods/blob/master/Source/ColorsVoice/Mod.cpp
//https://github.com/brianuuu/DllMods/blob/master/Source/Sonic06DefinitiveExperience/NextGenPhysics.cpp
HOOK(void, __fastcall, RampApplyImpulse, 0xE6CFA0, void* This, void* Edx, MsgApplyImpulse* message)
{
	originalRampApplyImpulse(This, Edx, message);

	if (!*pModernSonicContext)
		return;

	if (message->m_impulseType == ImpulseType::JumpBoardSpecial)
		CreateRampParticle();
	else if (message->m_impulseType == ImpulseType::JumpBoard)
		PlayRampAnimation();
}

void Ramp::applyPatches() {
	INSTALL_HOOK(RampApplyImpulse);
}