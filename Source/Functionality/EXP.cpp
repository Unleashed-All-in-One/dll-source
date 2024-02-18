std::vector<Sonic::CGameObject*> expCache;

void ChaosEnergyLogic(uint32_t amount) {
	Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

	if (!context)
		return;

	for (uint32_t i = 0; i < amount; i++)
	{
		boost::shared_ptr<EXPCollect> newEXP = boost::make_shared<EXPCollect>(context->m_spMatrixNode->m_Transform.m_Position);
		expCache.push_back(newEXP.get());
		// modification example : ((EXPCollect*)expCache.at(i))->test = true;
		context->m_pPlayer->m_pMember->m_pGameDocument->AddGameObject(newEXP);
	}
}

uint32_t __fastcall getEnemyChaosEnergyTypeImpl(uint32_t* pEnemy, uint32_t amount)
{
	switch (pEnemy[0])
	{
		case 0x016F593C: 
			ChaosEnergyLogic(1); // EFighter
			break;
		case 0x016F70BC:
			ChaosEnergyLogic(3); // Spinner
			break;
		case 0x016FB1FC: 
			ChaosEnergyLogic(1); // EFighterMissile
			break;
		case 0x016FB62C: 
			ChaosEnergyLogic(4); // AirCannon
			break;
		case 0x016F912C: 
			ChaosEnergyLogic(2); // Mole
			break;
		default:
			ChaosEnergyLogic(3);
			break;
	}

	return 0;
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
	WRITE_STRING(0x1613B98, ""); // disable stinky hud particles EW!
	WRITE_JUMP(0xBE05EF, getEnemyChaosEnergyType);
}

void EXPCollect::removeEXPCollect(Sonic::CGameObject* exp) {
	expCache.erase(std::find(expCache.begin(), expCache.end(), exp));
}