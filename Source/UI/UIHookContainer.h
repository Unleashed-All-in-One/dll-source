#pragma once
#include "TitleWorldMap.h"
#include "Title.h"
#include "TitleWorldMapPause.h"
#include "EventViewer.h"
#include "SubtitleUI.h"
#include "HudButtonGuide.h"
#include "HudStatus.h"

namespace SUC::Hooks
{
	static void InstallUIHooks()
	{
		using namespace SUC::UI;
		TitleScreen::Title::RegisterHooks();
		TitleScreen::TitleWorldMap::RegisterHooks();
		TitleScreen::TitleWorldMapPause::RegisterHooks();
		Event::EventViewer::RegisterHooks();
		Event::SubtitleUI::RegisterHooks();
		HudButtonGuide::RegisterHooks();
		HudStatus::RegisterHooks();
	}
}
