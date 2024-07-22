#pragma once
class CTitleWorldMapSky : public Sonic::CGameObject3D
{
	boost::shared_ptr<hh::mr::CSingleElement> m_spModel;
	CVector m_Position;
	CVector m_Velocity;
	SharedPtrTypeless handle;
	boost::shared_ptr<Sonic::CParticleManager> m_spParticleManager;
	boost::shared_ptr<Hedgehog::Mirage::CMatrixNodeSingleElementNode> m_Node;
public:
	boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spModelButtonTransform;
	CTitleWorldMapSky(const CVector& _Position)
		: m_Position(_Position)
	{
	}

	void AddCallback
	(
		const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,
		Sonic::CGameDocument* pGameDocument,
		const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase
	) override;

	void UpdateParallel
	(
		const Hedgehog::Universe::SUpdateInfo& updateInfo
	) override;

	void Kill();
};