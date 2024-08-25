#pragma once
using namespace hh::math;
namespace SUC::SetObject
{
	class MoonMedal :public Sonic::CObjectBase, public Sonic::CSetObjectListener
	{
	public:
		BB_SET_OBJECT_MAKE("MoonMedal")
			boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
		boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;
		boost::shared_ptr<Hedgehog::Animation::CAnimationPose> animatorTest;

		void* m_GlitterPlayer;
		bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
		{
			const char* assetName = "cmn_boj_km_medalMoon_000";
			hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
			boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
			m_spSpawnedModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
			m_spSpawnedModel->BindMatrixNode(m_spMatrixNodeTransform);
			//m_spSpawnedModel->BindAnimationPose(animatorTest);
			Sonic::CGameObject::AddRenderable("Object", m_spSpawnedModel, true);
			return true;
		}
		bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
		{
			if (in_Flag)
			{
				if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
				{
					const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
					if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
					{
						m_spSpawnedModel->m_Enabled = false;
					}
					return true;
				}
			}
			return Sonic::CObjectBase::ProcessMessage(in_rMsg, in_Flag);
		};
		bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
		{
			m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
			m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
			m_spNodeEventCollision->NotifyChanged();
			m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());

			hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(1, 1, 1);
			AddEventCollision("Object", shapeEventTrigger1, CollisionLayerID::PlayerEvent, true, m_spNodeEventCollision);
			return true;
		}
		float t;
		void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
		{
			//m_spSpawnedModel->BindAnimationPose()
		}
		void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
		{
		}

		static void RegisterObject();
	};
}