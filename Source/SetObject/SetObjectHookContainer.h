#pragma once
#include "CObjCamera2D.h"
#include "DebugObject.h"
#include "ETFStageGate.h"
#include "ETFTimeSwitch.h"
#include "EvilEnemyReckless.h"
#include "EvilLiftDoor.h"
#include "Hintring.h"
#include "MoonMedal.h"
#include "Paraloop.h"
#include "SpeedDownCollision.h"
#include "Holoska/Icicle.h"
#include "Holoska/StompingSwitch.h"
#include "Mykonos/IrremovableMobMykonos.h"
#include "Mykonos/Pelican.h"
#include "Werehog/CObjHangOn.h"

namespace SUC::Hooks
{
	static void InstallSetObjectHooks()
	{
		WerehogPole::registerObject();
		ETFTimeSwitch::registerObject();
		Hintring::registerObject();
		Paraloop::registerObject();
		SpeedDownCollision::registerObject();
		ETFStageGate::registerObject();
		MoonMedal::registerObject();
		EvilEnemyReckless::registerObject();
		EvilLiftDoor::registerObject();
		Icicle::registerObject();
		Pelican::registerObject();
		IrremovableMobMykonos::registerObject();
		StompingSwitch::registerObject();
		CObjHangOn::registerObject();
		BB_INSTALL_SET_OBJECT_MAKE_HOOK(CObjCamera2D);
	}
}
