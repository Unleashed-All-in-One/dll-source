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
		SetObject::WerehogPole::RegisterObject();
		SetObject::ETFTimeSwitch::RegisterObject();
		SetObject::Hintring::RegisterObject();
		SetObject::Paraloop::RegisterObject();
		SetObject::SpeedDownCollision::RegisterObject();
		SetObject::ETFStageGate::RegisterObject();
		SetObject::MoonMedal::RegisterObject();
		SetObject::EvilEnemyReckless::RegisterObject();
		SetObject::EvilLiftDoor::RegisterObject();
		SetObject::Icicle::RegisterObject();
		SetObject::Pelican::RegisterObject();
		SetObject::IrremovableMobMykonos::RegisterObject();
		SetObject::StompingSwitch::RegisterObject();
		SetObject::CObjHangOn::RegisterObject();
		BB_INSTALL_SET_OBJECT_MAKE_HOOK(CObjCamera2D);
	}
}
