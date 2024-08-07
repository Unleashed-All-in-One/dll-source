#pragma once
class CTitleWorldMapGlobe : public Sonic::CGameObject3D
{
	boost::shared_ptr<hh::mr::CSingleElement> m_spModel;
	hh::math::CVector m_Position;
	boost::shared_ptr<hh::mot::CSingleElementEffectMotionAll> m_spEffectMotionAll;
public:
	boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spModelButtonTransform;
	CTitleWorldMapGlobe(const Hedgehog::Math::CVector& _Position)
		: m_Position(_Position){}

	void AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,Sonic::CGameDocument* pGameDocument,
		const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase) override;
	void UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo) override;
	void Kill();
};
