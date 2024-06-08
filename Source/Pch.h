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
#include <regex>
#include <filesystem>
//#include <queue>

// HE1ML
#include <ModLoader.h>

//IMGUI
#include <d3d11.h>
#include <d3d9.h>
#include "System/DebugMenu/imgui.h"
#include "System/DebugMenu/imgui_impl_dx11.h"
#include "System/DebugMenu/imgui_impl_dx9.h"
#include "System/DebugMenu/imgui_impl_win32.h"
#include "System/DebugMenu/DDSTextureLoader9.h"
#include "System/DebugMenu/DDSTextureLoader11.h"
#include "System/DebugMenu/CsdImGui.h"

// Other
#include <tweeny-3.2.0.h>
#include <INIReader.h>
#include <json/json.h>
#include <rapidxml/rapidxml.hpp>
#include <Helpers.h>
#include <Light.h>
#include "System/DiscordGameSDK/discord.h"
#include "UnleashedHUDAPI.h"
#include "ScoreGenerationsAPI/ScoreGenerationsAPI.h"

// Alternative Audio Backend
#define WITH_MINIAUDIO
#include <soloud.h>
#include <soloud_wav.h>

#define DEBUG_DRAW_TEXT_DLL_IMPORT
#include <DebugDrawText.h>
#include <Common.h>
#include <StateManager.h>
#include "System/ArchiveTreePatcher.h"
#include "System/SequenceHelpers.h"
#include "Project.h"
#include "Temporary.h"

//Gameplay
#include "Gameplay/QSSRestore.h"
#include "Gameplay/Sweepkick.h"
#include "Gameplay/Ramp.h"
#include "Gameplay/ShortJump.h"
#include "Gameplay/Drift.h"
#include "Gameplay/Pole.h"

//System
#include "System/StageManager.h"
#include "System/DiscordStatus.h"
#include "System/AnimationSetPatcher.h"
#include "System/EnemyTrigger.h"
#include "System/MiniAudioHelper.h"
#include "System/LetterboxHelper.h"
#include "System/CSDCommon.h"
#include "System/UpdateDirector.h"
#include "System/SoundController.h"
#include "System/ImguiInitializer.h"
#include "System/SaveManager.h"
#include "System/LuaManager.h"
#include "System/SetEditorTest.h"
#include "CustomSetObjects/ObjectUtility.h"

//Debug Menu
#include "System/DebugMenu/Context.h"
#include "System/DebugMenu/ImguiManager.h"
#include "System/DebugMenu/ImGuiTheme.h"
#include "System/DebugMenu/SynchronizedObject.h"
#include "System/DebugMenu/TypesParamRemove.h"

//Functionality
#include "Functionality/Patches.h"
#include "Functionality/PauseBgm.h"
#include "Functionality/FallCam.h"
#include "Functionality/RingEnergy.h"
#include "Functionality/EXP.h"

//UI
#include "UI/Title.h"
#include "UI/TitleWorldMap.h"
#include "UI/TitleOption.h"
#include "UI/TitleWorldMapPause.h"
#include "UI/EventViewer.h"
#include "UI/HelpCaptionUI.h"
#include "UI/SubtitleUI.h"
#include "UI/ItemboxUI.h"
#include "UI/ETFStageGateUI.h"
#include "UI/EvilQTEUI.h"
#include "UI/HudButtonGuide.h"

//Set objects
#include "CustomSetObjects/Hintring.h"
#include "CustomSetObjects/DebugObject.h"
#include "CustomSetObjects/ETFTimeSwitch.h"
#include "CustomSetObjects/Paraloop.h"
#include "CustomSetObjects/SpeedDownCollision.h"
#include "CustomSetObjects/ETFStageGate.h"
#include "CustomSetObjects/MoonMedal.h"
#include "CustomSetObjects/EvilEnemyReckless.h"
#include "CustomSetObjects/EvilLiftDoor.h"
#include "CustomSetObjects/Holoska/Icicle.h"
#include "CustomSetObjects/Holoska/StompingSwitch.h"
#include "CustomSetObjects/Mykonos/Pelican.h"
#include "CustomSetObjects/Mykonos/IrremovableMobMykonos.h"
//#include "CustomSetObjects/ObjCamera2D.h"

//Rest
#include "Testing/TestingCode.h"
