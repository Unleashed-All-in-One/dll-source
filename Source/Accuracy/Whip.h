#pragma once
namespace SUC::Accuracy
{	
	class Whip : public Sonic::CObjectBase, public Sonic::IAnimationContext, public Sonic::CAnimationStateMachine
	{
	public:
		enum EWhipStartMode
		{
			Invalid,
			Stand,
			CrouchingStartFront
		};
		static inline boost::shared_ptr<Whip> s_SingletonWhip;
		static inline EWhipStartMode s_StartMode;
		static inline hh::math::CVector s_SpawnPosition;
		boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;
		boost::shared_ptr<hh::mot::CSingleElementEffectMotionAll> m_spEffectMotionAll;
		boost::shared_ptr<Hedgehog::Animation::CAnimationPose> m_AnimatorPose;
		static void RegisterHooks();
		bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override;
		bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override;		
		bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override;
		void UpdateParallel(const Hedgehog::Universe::SUpdateInfo& in_rUpdateInfo) override;
		void SetupResult();
		Hedgehog::Animation::CAnimationPose* GetAnimationPose() override;
		Hedgehog::Math::CVector GetVelocityForAnimationSpeed() override;
		Hedgehog::Math::CVector GetVelocityForAnimationChange() override;
	};
}