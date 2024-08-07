#pragma once
namespace SUC::UI
{
	class HudButtonGuide
	{
	public:
		enum EButtonType
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
		enum EAnimationType
		{
			QUICKSTEP_RIGHT,
			QUICKSTEP_LEFT,
			BOOST,
			SLIDING,
			DOOR,
			CHASER,
			QUICKSTEP_BOTH
		};

		static void RegisterHooks();
		static Sonic::EKeyState GetKeyStateFromButtonType(EButtonType in_buttonType);
		static void Configure(EButtonType in_ButtonType, EAnimationType in_AnimType);
		static void Spawn(Sonic::CGameObject* in_Parent, EButtonType in_ButtonType = BUTTON_X, EAnimationType in_AnimType = BOOST);
		static void Hide();
	};
}
