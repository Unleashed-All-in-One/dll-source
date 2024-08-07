#pragma once
#include "TitleWorldMap.h"
#include "Title.h"
#include "TitleWorldMapPause.h"
#include "EventViewer.h"
#include "SubtitleUI.h"
#include "HudButtonGuide.h"
namespace SUC::Hooks
{
	static void InstallUIHooks()
	{
		UI::TitleScreen::Title::RegisterHooks();
		UI::TitleScreen::TitleWorldMap::RegisterHooks();
		UI::TitleScreen::TitleWorldMapPause::RegisterHooks();
		UI::Event::EventViewer::RegisterHooks();
		UI::Event::SubtitleUI::RegisterHooks();
		UI::Event::SubtitleUI::s_CaptionData.Initialize();
		HudButtonGuide::applyPatches();
	}
}
