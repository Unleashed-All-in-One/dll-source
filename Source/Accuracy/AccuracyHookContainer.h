#pragma once
#include "AccuracyPatches.h"
#include "ExperienceObject.h"
#include "FallCam.h"
#include "PauseBgm.h"
#include "RampParticle.h"
#include "SwingPole.h"
#include "Whip.h"

namespace SUC::Hooks
{
	static void InstallAccuracyHooks()
	{
		Accuracy::AccuracyPatches::RegisterHooks();
		Accuracy::PauseBgm::RegisterHooks();
		Accuracy::FallCam::RegisterHooks();
		Accuracy::ExperienceObject::RegisterHooks();
		Accuracy::RampParticle::RegisterHooks();
		Accuracy::SwingPole::RegisterHooks();
		Accuracy::Whip::RegisterHooks();
	}
}
