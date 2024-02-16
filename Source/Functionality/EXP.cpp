std::vector<boost::shared_ptr<Sonic::CGameObject>> expCache;

void ChaosEnergyLogic() {
	Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

	if (!context)
		return;

	boost::shared_ptr<Sonic::CGameObject> newEXP = boost::make_shared<EXPCollect>(context->m_spMatrixNode->m_Transform.m_Position);

	expCache.push_back(newEXP);

	context->m_pPlayer->m_pMember->m_pGameDocument->AddGameObject(newEXP);
}

uint32_t __fastcall getEnemyChaosEnergyTypeImpl(uint32_t* pEnemy, uint32_t amount)
{
	ChaosEnergyLogic();
	return amount;
}

void __declspec(naked) getEnemyChaosEnergyType()
{
	static uint32_t returnAddress = 0xBE05F7;
	__asm
	{
		mov		edx, ecx
		mov		ecx, esi
		call	getEnemyChaosEnergyTypeImpl
		mov		ecx, eax

		// original function
		mov     edx, [esi]
		mov     eax, [edx + 9Ch]
		jmp[returnAddress]
	}
}

void EXPCollect::applyPatches() {
	WRITE_JUMP(0xBE05EF, getEnemyChaosEnergyType);
}