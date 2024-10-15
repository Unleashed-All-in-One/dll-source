#pragma once
#include "../BlueBlurCustom/Sonic/CGlitterPlayer.h"
class CTitleWorldMapSun : public Sonic::CGameObject3D
{
public:
	boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spChildNode;
	Sonic::CGlitterPlayer* m_GlitterPlayer;
	void AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder, Sonic::CGameDocument* pGameDocument,
		const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase) override;
	void UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo) override;
	void SetPosition(const hh::math::CVector& in_Position) const;
};
