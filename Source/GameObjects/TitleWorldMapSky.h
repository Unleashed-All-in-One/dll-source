#pragma once
class CTitleWorldMapSky : public Sonic::CGameObject3D
{
	INSERT_PADDING(0x4);
	Sonic::CGlitterPlayer* m_GlitterPlayer;
	boost::shared_ptr<hh::mr::CSingleElement> m_spModel;
	Hedgehog::Math::CVector m_Position;
	Hedgehog::Math::CVector m_Velocity;
	SharedPtrTypeless handle;
	boost::shared_ptr<Sonic::CParticleManager> m_spParticleManager;
	boost::shared_ptr<Hedgehog::Mirage::CMatrixNodeSingleElementNode> m_Node;
public:
	boost::shared_ptr<Sonic::CMatrixNodeTransform> spSun;
	CTitleWorldMapSky(const Hedgehog::Math::CVector& _Position)
		: m_Position(_Position)
	{
	}

	void AddCallback
	(
		const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,
		Sonic::CGameDocument* pGameDocument,
		const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase
	) override;

	void SetRotation(const hh::math::CQuaternion& quaternion);

	void UpdateParallel
	(
		const Hedgehog::Universe::SUpdateInfo& updateInfo
	) override;

	void Kill();
};