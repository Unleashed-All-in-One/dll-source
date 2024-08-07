#pragma once
#include "QSSRestore.h"
#include "Sweepkick.h"
#include "Ramp.h"
#include "ShortJump.h"
#include "Drift.h"
#include "Pole.h"
namespace SUC::Hooks
{
	static void InstallGameplayHooks()
	{
		QSSRestore::applyPatches();
		Sweepkick::applyPatches();
		Ramp::applyPatches();
		ShortJump::applyPatches();
		Drift::applyPatches();
		Pole::applyPatches();
	}
}