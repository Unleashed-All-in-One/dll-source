#pragma once
class HudButtonGuide
{
public:
	enum ButtonType
	{
		//btn_1
		BUTTON_A,
		BUTTON_B,
		BUTTON_X,
		BUTTON_Y,
		//btn_2
		BUTTON_LEFTBUMPER,
		BUTTON_RIGHTBUMPER,
		//btn_3
		BUTTON_LEFTTRIGGER,
		BUTTON_RIGHTTRIGGER
	};
	enum AnimationType
	{
		QUICKSTEP_RIGHT,
		QUICKSTEP_LEFT,
		BOOST,
		SLIDING,
		DOOR,
		CHASER,
		QUICKSTEP_BOTH
	};

	static void applyPatches();
	static Sonic::EKeyState getKeyStateFromButtonType(ButtonType in_buttonType);
	static void configure(ButtonType in_ButtonType, AnimationType in_AnimType);
	static void spawn(Sonic::CGameObject* in_Parent, ButtonType in_ButtonType = BUTTON_X, AnimationType in_AnimType = BOOST);
	static void hide();
};