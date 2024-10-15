#pragma once
#include "../BlueBlurCustom/Hedgehog/Motion/hhSingleEffectMotionAll.h"

class CTitleWorldMapGlobe : public Sonic::CGameObject3D
{
	boost::shared_ptr<hh::mr::CSingleElement> m_spModel;
	boost::shared_ptr<hh::mr::CSingleElement> m_spModelCore;
	//hh::math::CVector m_Position;
	boost::shared_ptr<hh::mot::CSingleElementEffectMotionAll> m_spEffectMotionAll;
	boost::shared_ptr<hh::mot::CSingleElementEffectMotionAll> m_spEffectMotionAll2;
	boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spModelButtonTransform;
	boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spModelButtonTransform2;
public:
	hh::math::CQuaternion m_Rotation;
	void AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,Sonic::CGameDocument* pGameDocument,
		const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase) override;
	void UpdateSerial(const Hedgehog::Universe::SUpdateInfo& updateInfo) override;
	void Kill();
};
