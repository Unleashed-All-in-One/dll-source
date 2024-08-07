#pragma once
#include "AnimationSetPatcher.h"
#include "ArchiveTreePatcher.h"
#include "EnemyTrigger.h"
#include "EnemyTrigger.h"

namespace SUC::Hooks
{
	static void InstallSystemHooks()
	{
		std::string path = Project::s_ModInfoPath;
		path = path.substr(0, path.find_last_of("/\\"));
		// Functionality patches that don't necessarily need their own file
		//---------------System---------------
		System::StageManager::Initialize();
		System::ArchiveTreePatcher::RegisterHooks();
		System::AnimationSetPatcher::RegisterHooks();
		System::EnemyTrigger::RegisterHooks();
		System::AspectRatioHelper::Initialize(1280, 720);
		//MiniAudioHelper::initialize(path.c_str());
		System::Discord::DiscordStatus::Initialize();
		SequenceHelpers::applyPatches();
		ImGuiMenu::ImguiInitializer::Initialize();
		Context::setModDirectoryPath(SUC::Project::GetDirectoryPath(Project::s_ModInfoPath));
		SaveManager::applyPatches();
		LuaManager::initialize();
	}
}
