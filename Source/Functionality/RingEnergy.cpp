
void OnRing(hh::fnd::Message& msg, bool superRing = false) {
	if (!*pModernSonicContext)
		return;

	Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();
	if (context->m_pPlayer->m_ActorID != msg.m_SenderActorID)
		return;

	if (*Common::GetPlayerBoost() < 100.0f)
		*Common::GetPlayerBoost() = std::clamp(*Common::GetPlayerBoost() + (superRing ? 4.0f : 2.0f), 0.0f, 100.0f);
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

//Patch "Never Receive Boost From Rings" in "Gameplay" by "Hyper"
void NOP(int floatInstrAddr, int paramStrAddr)
{
	WRITE_MEMORY(floatInstrAddr, ::byte, 0xD9, 0xEE); /* fldz */
	WRITE_NOP(floatInstrAddr + 2, 6);
	WRITE_MEMORY(paramStrAddr, ::byte, 0x00);
}

void RingEnergy::Install() {
	NOP(0x120628B, 0x15FA690); /* ChaosEnergyRecoverRateByRing */
	NOP(0x1206335, 0x15FA6DC); /* ChaosEnergyRecoverRateByRingBonus */
	NOP(0x12063DF, 0x15FA72C); /* ChaosEnergyRecoverRateByRingPenalty */
	
	INSTALL_HOOK(CObjRingProcMsgHitEventCollision);
	INSTALL_HOOK(CObjSuperRingProcMsgHitEventCollision);

	static float const ChaosEnergyRecoveryRate1 = 0.02f; // Enemy (0.03 default)
	WRITE_MEMORY(0xD96724, float*, &ChaosEnergyRecoveryRate1);

	static float const ChaosEnergyRecoveryRate3 = 0.0f; // EnemyBonus (0.02 default)
	WRITE_MEMORY(0xD96736, float*, &ChaosEnergyRecoveryRate3);
	WRITE_MEMORY(0xE60C94, float*, &ChaosEnergyRecoveryRate3);
}