#include "PauseBgm.h"
namespace SUC::Accuracy
{
	HOOK(void, __stdcall, _HudPreparePause, 0x10A1140, void* in_Hud)
	{
		PauseBgm::s_StartingVolume = SoundController::GetBGMVolume(0);
		SoundController::SetBGMVolume(PauseBgm::s_StartingVolume * 0.25f);

		original_HudPreparePause(in_Hud);
	}
	HOOK(void, __stdcall, _HudPrepareUnPause, 0x010A1480, void* in_Hud)
	{
		SoundController::SetBGMVolume(PauseBgm::s_StartingVolume);
		original_HudPrepareUnPause(in_Hud);
	}
	void PauseBgm::RegisterHooks()
	{
		INSTALL_HOOK(_HudPreparePause);
		INSTALL_HOOK(_HudPrepareUnPause);
		WRITE_NOP(0x010A128D, 0x010A1292 - 0x010A128D); // Nop music pause
	}
}