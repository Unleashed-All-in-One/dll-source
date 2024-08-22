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
		Player::Evil::ClassicPluginExtensions::RegisterHooks();
		Player::Evil::EvilGlobal::Initialize();
		Player::Evil::EvilSonic::RegisterPatches();
		Player::Evil::EvilAttackConfiguration::RegisterHooks(Project::s_ModInfo->CurrentMod->Path);
	}
}