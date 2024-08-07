#pragma once
#include "AccuracyPatches.h"
#include "EXP.h"
#include "FallCam.h"
#include "PauseBgm.h"
#include "RingEnergy.h"

namespace SUC::Hooks
{
	static void InstallAccuracyHooks()
	{
		AccuracyPatches::applyPatches();
		PauseBgm::applyPatches();
		FallCam::applyPatches();
		RingEnergy::applyPatches();
		EXPCollect::applyPatches();
	}
}
