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

// LostCodeLoader
#include <LostCodeLoader.h>

// Other
#include <INIReader.h>
#include <json/json.h>
#include <Helpers.h>

// Alternative Audio Backend
#define WITH_MINIAUDIO
#include <soloud.h>
#include <soloud_wav.h>

#define DEBUG_DRAW_TEXT_DLL_IMPORT
#include <DebugDrawText.h>
#include <Common.h>
#include "Configuration.h"

#include "Gameplay/QSSRestore.h"

#include "System/ArchiveTreePatcher.h"
#include "System/EnemyTrigger.h"
#include "System/MiniAudioHelper.h"
#include "System/LetterboxHelper.h"
#include "System/CSDCommon.h"

#include "UI/Title.h"
#include "UI/TitleWorldMap.h"
#include "UI/TitleOption.h"