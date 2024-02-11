#pragma once

#define WIN32_LEAN_AND_MEAN

#include <BlueBlur.h>
#include <BlueBlurCustom.h>

// Detours
#include <Windows.h>
#include <detours.h>

// std
#include <stdint.h>
#include <stdio.h>
#include <array>
#include <set>
#include <vector>
#include <sstream>
#include <iostream>
#include <thread>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <filesystem>
//#include <queue>

// LostCodeLoader
#include <LostCodeLoader.h>

// Other
#include <INIReader.h>
#include <json/json.h>
#include <Helpers.h>
#include <Light.h>
#include "System/DiscordGameSDK/discord.h"
#include "UnleashedHUDAPI.h"

// Alternative Audio Backend
#define WITH_MINIAUDIO
#include <soloud.h>
#include <soloud_wav.h>

#define DEBUG_DRAW_TEXT_DLL_IMPORT
#include <DebugDrawText.h>
#include <Common.h>
#include <StateManager.h>
#include "System/SequenceHelpers.h"
#include "Configuration.h"
#include "Temporary.h"

//Gameplay
#include "Gameplay/QSSRestore.h"
#include "Gameplay/Sweepkick.h"
#include "Gameplay/Ramp.h"
#include "Gameplay/ShortJump.h"
#include "Gameplay/Drift.h"

//System
#include "System/LevelLoadingManager.h"
#include "System/DiscordStatus.h"
#include "System/ArchiveTreePatcher.h"
#include "System/EnemyTrigger.h"
#include "System/MiniAudioHelper.h"
#include "System/LetterboxHelper.h"
#include "System/CSDCommon.h"
#include "System/UpdateDirector.h"

//Functionality
#include "Functionality/PauseBgm.h"
#include "Functionality/FallCam.h"
#include "Functionality/RingEnergy.h"

//UI
#include "UI/Title.h"
#include "UI/TitleWorldMap.h"
#include "UI/TitleOption.h"
#include "UI/TitleWorldMapPause.h"
#include "UI/EventViewer.h"
#include "UI/SubtitleUI.h"
#include "UI/ItemboxUI.h"

//Set objects
#include "CustomSetObjects/Hintring.h"
#include "CustomSetObjects/DebugObject.h"
#include "CustomSetObjects/ETFTimeSwitch.h"
#include "CustomSetObjects/Paraloop.h"
#include "CustomSetObjects/SpeedDownCollision.h"

//Rest
#include "Testing/TestingCode.h"