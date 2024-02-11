int lastHurdleIndex = 0;
int currentHurdleIndex = 1;

bool shortJumpMove = false;
float jumpTimer = 0.0f;

void playAnimBegin() {
	Eigen::Vector3f playerVelocity;
	if (!Common::GetPlayerVelocity(playerVelocity))
		return;
	
	shortJumpMove = playerVelocity.norm() >= 7.0f;
	if (shortJumpMove) {
		currentHurdleIndex = (lastHurdleIndex == 0) ? 1 : 0;
		lastHurdleIndex = currentHurdleIndex;

		Hedgehog::Base::CSharedString hurdleAnimName = (currentHurdleIndex == 0) ? "JumpHurdleL" : "JumpHurdleR";
		Common::SonicContextChangeAnimation(hurdleAnimName);
		jumpTimer = ShortJump::hurdleTime;
	}
	else {
		Common::SonicContextChangeAnimation("JumpShortBegin");
		jumpTimer = ShortJump::jumpTime;
	}
}

void playAnimTop() {
	if (!shortJumpMove)
		Common::SonicContextChangeAnimation("JumpShortTop");
}

void __declspec(naked) ShortTop()
{
	static uint32_t shortTop = 0x011BF036;
	__asm
	{
		call playAnimTop
		jmp[shortTop]
	}
}

void __declspec(naked) ShortBegin()
{
	static uint32_t shortBegin = 0x011BF26E;
	__asm
	{
		call playAnimBegin
		jmp[shortBegin]
	}
}

HOOK(int, __fastcall, HurdleJumpBegin, 0xDF1760, int a1, void* Edx) {
	int result = originalHurdleJumpBegin(a1, Edx);
	
	lastHurdleIndex = Common::SonicContextIsAnimation("JumpHurdleL") ? 1 : 0;
	
	return result;
}

HOOK(void, __fastcall, ShortJumpResetUpdate, 0xE6BF20, void* This, void* Edx, float* dt) {
	originalShortJumpResetUpdate(This, Edx, dt);

	if (!*pModernSonicContext)
		return;

	bool isAnim = Common::SonicContextIsAnimation("JumpHurdleL") || Common::SonicContextIsAnimation("JumpHurdleR") || Common::SonicContextIsAnimation("JumpShortBegin") ||Common::SonicContextIsAnimation("JumpShortTop");
	
	if (isAnim && jumpTimer > 0.0f)
		jumpTimer -= *dt;
	else if (isAnim && jumpTimer <= 0.0f) {
		StateManager::ChangeState(StateAction::Fall, *PLAYER_CONTEXT);
		Common::SonicContextChangeAnimation("Fall");
	}
}

void ShortJump::Install() {
	INSTALL_HOOK(ShortJumpResetUpdate);
	INSTALL_HOOK(HurdleJumpBegin);
	WRITE_JUMP(0x011BF008, ShortTop);
	WRITE_JUMP(0x011BF247, ShortBegin);
}