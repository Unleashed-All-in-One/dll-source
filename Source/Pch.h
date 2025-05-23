#pragma once

#define WIN32_LEAN_AND_MEAN
#include <BlueBlur.h>
#include "BlueBlurCustom/DoNotCommit/hhMath.h"
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

#include "Macros.h"
// Other
#include "BlueBlurCustom/Hedgehog/Motion/hhSingleEffectMotionAll.h"
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
#include <soloud.h>
#include <soloud_wav.h>

#define DEBUG_DRAW_TEXT_DLL_IMPORT
#include <DebugDrawText.h>
#include <Common.h>
#include "System/ArchiveTreePatcher.h"
#include "System/SequenceHelpers.h"
#include "Project.h"
#include "Temporary.h"
#include "Utility.h"

//System
#include "System/AnimationSetPatcher.h"
#include "System/ImGuiContextManager.h"
#include "SetObject/ObjectUtility.h"
#include "System/SoundController.h"
#include "System/CObjCamera.h"
#include "System/LuaManager.h"
#include "System/CSDCommon.h"
#include "System/SaveManager.h"
#include "System/StageManager.h"

//Debug Menu
#include "System/DebugMenu/DebugMenuRenderer.h"
#include "System/DebugMenu/DebugMenu.h"
#include "System/DebugMenu/ImGuiTheme.h"
#include "System/DebugMenu/SynchronizedObject.h"
#include "System/DebugMenu/TypesParamRemove.h"
#include "System/DebugMenu/ImGuizmo.h"
