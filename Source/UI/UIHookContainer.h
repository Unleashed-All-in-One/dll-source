#pragma once
#include "TitleWorldMap.h"
#include "Title.h"
#include "TitleOption.h"
#include "TitleWorldMapPause.h"
#include "EventViewer.h"
#include "SubtitleUI.h"
#include "HudButtonGuide.h"
namespace SUC::Hooks
{
	static void InstallUIHooks()
	{
		Title::applyPatches();
		TitleWorldMap::applyPatches();
		TitleWorldMapPause::applyPatches();
		EventViewer::applyPatches();
		SubtitleUI::applyPatches();
		SubtitleUI::m_captionData.init();
		HudButtonGuide::applyPatches();
		// NOTE: Because we're essentially remaking the title experience to be the exact same, we might want to make the options change in the
		// same way unleashed does it (via an small options sub-tab-thing and remove TitleOption entirely.
		// Right now, exiting from Options will cause a crash due to the saving/loading in the WorldMap.
		TitleOption::applyPatches();
	}
}
