#pragma once
#include "AnimationSetPatcher.h"
#include "ArchiveTreePatcher.h"
#include "EnemyTrigger.h"
#include "SequenceHelpers.h"

namespace SUC::Hooks
{
	static void InstallSystemHooks()
	{
		using namespace SUC::System;
		std::string path = Project::s_ModInfoPath;
		path = path.substr(0, path.find_last_of("/\\"));
		// Functionality patches that don't necessarily need their own file
		//---------------System---------------
		StageManager::Initialize();
		ArchiveTreePatcher::RegisterHooks();
		AnimationSetPatcher::RegisterHooks();
		EnemyTrigger::RegisterHooks();
		AspectRatioHelper::Initialize(1280, 720);
		//MiniAudioHelper::initialize(path.c_str());
		Discord::DiscordStatus::Initialize();
		SequenceHelpers::RegisterHooks();
		ImGuiMenu::ImGuiContextManager::Initialize();
		ImGuiMenu::DebugMenuRenderer::SetModDirectoryPath(SUC::Project::GetDirectoryPath(Project::s_ModInfoPath));
		SaveManager::RegisterHooks();
		LuaManager::Initialize();
	}
}
