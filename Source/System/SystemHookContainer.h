#pragma once
namespace SUC::Hooks
{
	static void InstallSystemHooks()
	{
		std::string path = Project::s_ModInfoPath;
		path = path.substr(0, path.find_last_of("/\\"));
		// Functionality patches that don't necessarily need their own file
		//---------------System---------------
		StageManager::initialize();
		ArchiveTreePatcher::applyPatches();
		AnimationSetPatcher::applyPatches();
		EnemyTrigger::applyPatches();
		LetterboxHelper::initialize(1280, 720);
		MiniAudioHelper::initialize(path.c_str());
		DiscordStatus::initialize();
		SequenceHelpers::applyPatches();
		ImGuiMenu::ImguiInitializer::Initialize();
		Context::setModDirectoryPath(SUC::Project::GetDirectoryPath(Project::s_ModInfoPath));
		SaveManager::applyPatches();
		LuaManager::initialize();
	}
}