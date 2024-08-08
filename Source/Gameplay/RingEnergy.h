#pragma once
namespace SUC::Gameplay
{
	class RingEnergy
	{
	public:
		static void RegisterHooks();
		static inline float m_RingAdd = 2.0f;
		static inline float m_SuperRingAdd = 14.0f;
		static inline float m_DriftAdd = 5.0f;
	};
}