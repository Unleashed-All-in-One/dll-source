#pragma once
//=========================================================
// Mod Info						
//=========================================================
#define MOD_NAME			"Sonic Unleashed Conversion"
#define INI_FILE			"UnleashedConversion.ini"
#define STAGE_LIST_FILE		"stage_list.json"
#define ARCHIVE_LIST_FILE	"archivelist.json"
#define QUEUE_LIST_FILE		"sequence.json"
#define SAVE_FILE_NAME		"save.swa"
#define LUA_FILE			"main.lua"
#define CLIENT_ID			1248726119402967091


#define HINTSFILE_NAME_FORMAT "{0}\\disk\\Hints\\hints_{1}.json"

//=========================================================
// DebugDrawText (Parameter Editor) Colors
//=========================================================
#define TEXT_WHITE \
	DebugDrawText::Color(1,1,1,1)
#define TEXT_BLACK \
	DebugDrawText::Color(0,0,0,1)
#define TEXT_BLUE \
	DebugDrawText::Color(0,0,1,1)
#define TEXT_CYAN \
	DebugDrawText::Color(0,1,1,1)
#define TEXT_GRAY \
	DebugDrawText::Color(0.5f,0.5f,0.5f,1)
#define TEXT_GREEN \
	DebugDrawText::Color(0,1,0,1)
#define TEXT_MAGENTA \
	DebugDrawText::Color(1,0,1,1)
#define TEXT_RED \
	DebugDrawText::Color(1,0,0,1)
#define TEXT_YELLOW \
	DebugDrawText::Color(1,0.9f,0.016f,1)
//=========================================================
// DebugDrawText (Parameter Editor) Colors
//=========================================================
#define STAGEACT_FADEWAIT *(float*)0x01A426B4
#define GOALCAMERA_OFFSET_RIGHT *(float*)0x01A48C7C

#define SONIC_GENERAL_CONTEXT \
	Sonic::Player::CPlayerSpeedContext::GetInstance()
#define SONIC_MODERN_CONTEXT  \
	Sonic::Player::CSonicContext::GetInstance()
#define SONIC_CLASSIC_CONTEXT \
	Sonic::Player::CSonicClassicContext::GetInstance()
#define SONIC_SUPER_CONTEXT   \
	Sonic::Player::CSonicSpContext::GetInstance()

#define IS_SUPER_SONIC \
	Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID == 2
#define IS_CLASSIC_SONIC \
	Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID == 1
#define IS_MODERN_SONIC \
	Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID == 0

#define PARTICLE_CLAW "slash"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define ms_IsUpdateForRender *(bool*)0x1A430EC

class CollisionLayerID
{
public:
	static inline const int& Common = *reinterpret_cast<int*>(0x01E0AF30);
	static inline const int& BasicTerrain = *reinterpret_cast<int*>(0x01E0AFAC);
	static inline const int& PlayerEvent = *reinterpret_cast<int*>(0x01E0AFD8);
};