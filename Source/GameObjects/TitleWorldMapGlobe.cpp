#include "TitleWorldMapGlobe.h"

void CTitleWorldMapGlobe::AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder, Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
{
	m_spModelButtonTransform = boost::make_shared<Sonic::CMatrixNodeTransform>();
	m_spModelButtonTransform->m_Transform.SetPosition(hh::math::CVector(0,0,0));
	m_spModelButtonTransform->NotifyChanged();
	m_spModelButtonTransform->SetParent(m_spMatrixNodeTransform.get());

	Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
	pGameDocument->AddUpdateUnit("1", this);

	auto spModelData = hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("wmap_union04");
	auto spModelDataCpre = hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("GeoSphere01");
	m_spModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
	m_spModelCore = boost::make_shared<hh::mr::CSingleElement>(spModelDataCpre);
	AddRenderable("Object", m_spModel, false);
	AddRenderable("Object", m_spModelCore, false);
	m_spEffectMotionAll = boost::make_shared<Hedgehog::Motion::CSingleElementEffectMotionAll>();
	m_spEffectMotionAll2 = boost::make_shared<Hedgehog::Motion::CSingleElementEffectMotionAll>();
	m_spModel->BindMatrixNode(m_spModelButtonTransform);
	m_spModelCore->BindMatrixNode(m_spModelButtonTransform);
	m_spModel->BindEffect(m_spEffectMotionAll);
	m_spModelCore->BindEffect(m_spEffectMotionAll2);
	ObjectUtility::AssignUVAnimation("sea-0000", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("sea-0001", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud-0000", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud-0002", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud2-0000", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud2-0002", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud3_Shadow-0001", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud3_Shadow-0000", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("worldmap_core-0000", spDatabase, spModelDataCpre, m_spEffectMotionAll2);
	ObjectUtility::AssignUVAnimation("worldmap_core-0002", spDatabase, spModelDataCpre, m_spEffectMotionAll2);
	ObjectUtility::AssignUVAnimation("worldmap_core-0003", spDatabase, spModelDataCpre, m_spEffectMotionAll2);
	ObjectUtility::AssignUVAnimation("worldmap_core-0004", spDatabase, spModelDataCpre, m_spEffectMotionAll2);
}

void CTitleWorldMapGlobe::UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
	FUNCTION_PTR(void, __thiscall, fpUpdateMotionAll, 0x752F00, Hedgehog::Motion::CSingleElementEffectMotionAll * This, float dt);
	fpUpdateMotionAll(m_spEffectMotionAll.get(), updateInfo.DeltaTime);
	fpUpdateMotionAll(m_spEffectMotionAll2.get(), updateInfo.DeltaTime);
}
void CTitleWorldMapGlobe::Kill()
{
	SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
}