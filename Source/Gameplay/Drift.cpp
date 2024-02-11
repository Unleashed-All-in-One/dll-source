HOOK(void, __fastcall, BDriftUpdate, 0xE6BF20, void* This, void* Edx, float* dt) {
	originalBDriftUpdate(This, Edx, dt);

	if (!*pModernSonicContext)
		return;
	
	if (!Drift::checkForBDrift())
		return;
	
	Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();
	Hedgehog::Base::CSharedString state = sonic->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
	Sonic::SPadState input = Sonic::CInputState::GetInstance()->GetPadState();

	if (input.IsDown(Sonic::eKeyState_B) && abs(input.LeftStickHorizontal) >= 0.85f && (state == "Walk" || state == "Sliding"))
		sonic->ChangeState("Drift");
	else if (input.IsUp(Sonic::eKeyState_B) && state == "Drift" && input.IsUp(Sonic::eKeyState_LeftTrigger) && input.IsUp(Sonic::eKeyState_RightTrigger))
		sonic->ChangeState("Walk");
}

void Drift::applyPatches() {
	INSTALL_HOOK(BDriftUpdate);
}