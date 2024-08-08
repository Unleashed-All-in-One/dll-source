#pragma once
namespace SUC::Gameplay
{
	class Sweepkick
	{
	public:
		static void RegisterHooks();

		static inline bool s_UseLight = true;
		static inline bool s_TransitionToStand = true;
		static inline bool s_UseSupportShockwave = true;
		static inline float s_SupportShockwaveDelay = 0.2f;
		static inline float s_LightInDelay = 0.1f;
		static inline float s_LightLifeTime = 0.35f;
		static inline float s_LerpSpeedIn = 10.0f;
		static inline float s_LerpSpeedOut = 5.0f;
		static inline float s_LightRange = 1.5f;
		static inline float s_SweepInputTime = 0.3f;
		static inline Eigen::Vector3f s_Offset = Eigen::Vector3f(0.0f, 0.6f, 0.0f);
		static inline LightRGB s_ColorGeneric = LightRGB(0.0f, 0.55f, 1.0f, 1.25f);
		static inline LightRGB s_ColorSuper = LightRGB(1.0f, 0.7f, 0.3f, 1.25f);
	};
}