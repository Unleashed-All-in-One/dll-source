#pragma once
#include "QSSRestore.h"
#include "Sweepkick.h"
#include "ShortJump.h"
#include "DriftOnB.h"
#include "RingEnergy.h"

namespace SUC::Hooks
{
	static void InstallGameplayHooks()
	{
		Gameplay::QSSRestore::RegisterHooks();
		Gameplay::Sweepkick::RegisterHooks();
		Gameplay::ShortJump::RegisterHooks();
		Gameplay::DriftOnB::RegisterHooks();
		Gameplay::RingEnergy::RegisterHooks();
	}
}