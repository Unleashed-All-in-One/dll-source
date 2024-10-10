#pragma once
namespace Sonic
{
	class GlitterManager
	{
		BB_INSERT_PADDING(0x2306c); //wtf
	};
	class CParticleManager : public Sonic::CGameObject, public Hedgehog::Base::CSynchronizedObject
	{
	public:

		boost::shared_ptr<GlitterManager> m_spGlitterManager;
		boost::shared_ptr<Hedgehog::Mirage::CRenderable> m_spTypicalRenderer;
		boost::shared_ptr<Hedgehog::Mirage::CRenderable> m_spDeformationRenderer;
		boost::shared_ptr<Hedgehog::Mirage::CRenderable> m_spShadowMapRenderer;
		boost::shared_ptr<Hedgehog::Mirage::CRenderable> m_spCustomCategoryRenderer;
		boost::shared_ptr<Hedgehog::Mirage::CRenderable> m_spObjectIconRenderer;
		boost::shared_ptr<Hedgehog::Mirage::CRenderable> m_spPlayableMenuRenderer;
		void* m_FieldE8;
		bool m_FieldEC;
		void* m_FieldED;

		CParticleManager()
		{
			*reinterpret_cast<size_t*>(this) = 0x016CFF90;
			*reinterpret_cast<size_t*>(static_cast<CMessageActor*>(this)) = 0x016CFFC8;
		}

		BB_OVERRIDE_FUNCTION_PTR(void, CGameObject, AddCallback, 0xE8F330, (const Hedgehog::Base::THolder<Sonic::CWorld>&, in_rWorldHolder),
			(Sonic::CGameDocument*, in_pGameDocument), (const boost::shared_ptr<Hedgehog::Database::CDatabase>&, in_spDatabase))
	};
	BB_ASSERT_OFFSETOF(CParticleManager, m_spGlitterManager, 0xAC);
}
