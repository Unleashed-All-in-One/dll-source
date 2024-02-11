HOOK(void, __fastcall, PoleUpdate, 0xE6BF20, void* This, void* Edx, float* dt) {
	originalPoleUpdate(This, Edx, dt);

	if (!*pModernSonicContext)
		return;

	Pole::LocusUpdate();
	Pole::TrailUpdate();
}

void Pole::applyPatches() {
	INSTALL_HOOK(PoleUpdate);
}