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
		Player::Evil::EvilAttackConfiguration::RegisterHooks(Project::s_ModInfo->CurrentMod->Path);
		Player::Evil::EvilGlobal::initializeValues();
		Player::Evil::EvilSonic::registerPatches();
		Player::Evil::ClassicPluginExtensions::registerPatches();
	}
}
