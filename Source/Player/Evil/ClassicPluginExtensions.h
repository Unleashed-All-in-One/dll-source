#pragma once
namespace SUC::Player::Evil
{
	class CEvilBasicStateBase : public Hedgehog::Universe::CStateMachineBase::CStateBase
	{
	public:
		float ms_MovementSpeed = 1;
		float m_Timer = 0;
		
		void PadMovement() const;
		void UpdateState() override;
		static float GetPadIntensity();
		static inline float ms_StateChangeDelay = 0.5f;
	};
	class ClassicPluginExtensions
	{

	public:
		static void RegisterHooks();
	};
}