#pragma once
#include "Evil/EvilAttackConfiguration.h"
#include "Evil/EvilGlobal.h"
#include "Evil/EvilSonic.h"
#include "Evil/ClassicPluginExtensions.h"
namespace SUC::Hooks
{
	static void InstallPlayerHooks()
	{
		/// Werehog
		EvilAttackConfiguration::Install(Project::s_ModInfo->CurrentMod->Path);
		EvilGlobal::initializeValues();
		EvilSonic::registerPatches();
		ClassicPluginExtensions::registerPatches();
	}
}
