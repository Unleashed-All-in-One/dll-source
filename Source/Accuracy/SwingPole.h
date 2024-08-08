#pragma once
namespace SUC::Accuracy
{
	class SwingPole
	{
	public:
		static void RegisterHooks();
		static void LocusUpdate();
		static void PlayTrail();
		static void TrailUpdate();
		static bool IsSwingJump();
		static const char* GetPoleSwingParticle();

		static inline SharedPtrTypeless s_SwingHandle;
		static inline bool s_LocusPlayed = false;
		static inline bool s_TrailPlayed = false;
	};
}