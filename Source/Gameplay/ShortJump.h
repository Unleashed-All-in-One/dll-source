#pragma once
namespace SUC::Gameplay
{
	class ShortJump
	{
	public:
		static void RegisterHooks();

		static inline float s_HurdleTime = 0.75f;
		static inline float s_JumpTime = 0.15f;
	};
}