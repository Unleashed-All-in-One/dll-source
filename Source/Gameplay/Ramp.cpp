SharedPtrTypeless rampParticleHandle;

void CreateRampParticle() {
	Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();

	if (!sonic)
		return;

	void* middlematrixNode = (void*)((uint32_t)sonic + 0x30);
	
	Common::fCGlitterCreate(sonic, rampParticleHandle, middlematrixNode, "ef_ch_sng_lms_jump_delux", 1);
}

void PlayRampAnimation() {
	Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();

	if (!sonic)
		return;

	sonic->ChangeAnimation("JumpBoardLoop");
}

//https://github.com/brianuuu/DllMods/blob/master/Source/ColorsVoice/Mod.cpp
//https://github.com/brianuuu/DllMods/blob/master/Source/Sonic06DefinitiveExperience/NextGenPhysics.cpp
HOOK(void, __fastcall, RampApplyImpulse, 0xE6CFA0, void* This, void* Edx, MsgApplyImpulse* message)
{
	originalRampApplyImpulse(This, Edx, message);

	if (!*pModernSonicContext)
		return;

	switch (message->m_impulseType) // i would just pass in context here but it crashes.
	{
	case ImpulseType::JumpBoardSpecial:
		CreateRampParticle();
		break;
	case ImpulseType::JumpBoard:
		PlayRampAnimation();
		break;
	}
}

void Ramp::Install() {
	INSTALL_HOOK(RampApplyImpulse);
}