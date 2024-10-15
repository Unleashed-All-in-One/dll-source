#include "TitleWorldMapGlobe.h"

#include "../UI/TitleWorldMap.h"
boost::shared_ptr<hh::mr::CSingleElement> m_spFlagLocations;
hh::math::CVector GetPositionFromNode(boost::shared_ptr<Hedgehog::Mirage::CMatrixNodeSingleElementNode> in_Node)
{
	auto vec = in_Node->m_LocalMatrix.matrix().col(3);
	return (Hedgehog::Math::CVector(vec.x(), vec.y(), vec.z()));
}
float m_Angle;
void CTitleWorldMapGlobe::AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder, Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
{
	m_spModelButtonTransform = boost::make_shared<Sonic::CMatrixNodeTransform>();
	m_spModelButtonTransform->SetParent(m_spMatrixNodeTransform.get());
	m_spModelButtonTransform->m_Transform.SetPosition(hh::math::CVector(0,0,0));
	m_spModelButtonTransform->NotifyChanged();

	m_spModelButtonTransform2 = boost::make_shared<Sonic::CMatrixNodeTransform>();
	m_spModelButtonTransform2->SetParent(m_spModelButtonTransform.get());
	m_spModelButtonTransform2->m_Transform.SetPosition(hh::math::CVector(0, 0, 0));
	m_spModelButtonTransform2->NotifyChanged();

	Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
	pGameDocument->AddUpdateUnit("1", this);

	auto spModelData = hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("wmap_union04");
	auto spModelData2 = hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("wmap_piece02");
	auto spModelDataCpre = hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("GeoSphere01");
	auto spModelDataFlags = hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("wm_flags");
	m_spModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
	auto m_spModel2 = boost::make_shared<hh::mr::CSingleElement>(spModelData2);
	//auto m_spModel2 = boost::make_shared<hh::mr::CSingleElement>(spModelData2);
	m_spModelCore = boost::make_shared<hh::mr::CSingleElement>(spModelDataCpre);
	m_spFlagLocations= boost::make_shared<hh::mr::CSingleElement>(spModelDataFlags);
	m_spModel2->BindMatrixNode(m_spModelButtonTransform);
	//m_spModelCore->BindMatrixNode(m_spModelButtonTransform);
	//m_spFlagLocations->BindMatrixNode(m_spModelButtonTransform);
	AddRenderable("Object", m_spModel, true);
	//AddRenderable("Object", m_spModel2, true);
	AddRenderable("Object", m_spModelCore, true);
	//AddRenderable("Object", m_spFlagLocations, true);


	m_spModel->BindMatrixNode(m_spModelButtonTransform);
	m_spModelCore->BindMatrixNode(m_spModelButtonTransform);


	m_spEffectMotionAll = boost::make_shared<Hedgehog::Motion::CSingleElementEffectMotionAll>();
	m_spEffectMotionAll2 = boost::make_shared<Hedgehog::Motion::CSingleElementEffectMotionAll>();
	
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
	m_spModel->BindEffect(m_spEffectMotionAll);
	m_spModelCore->BindEffect(m_spEffectMotionAll2);
	Sonic::CGameObject3D::AddCallback(worldHolder, pGameDocument, spDatabase);
}

void CTitleWorldMapGlobe::UpdateSerial(const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
	FUNCTION_PTR(void, __thiscall, fpUpdateMotionAll, 0x752F00, Hedgehog::Motion::CSingleElementEffectMotionAll * This, float dt);
	fpUpdateMotionAll(m_spEffectMotionAll.get(),updateInfo.DeltaTime);
	fpUpdateMotionAll(m_spEffectMotionAll2.get(),updateInfo.DeltaTime);

	const auto& m_Countries = SUC::UI::TitleScreen::TitleWorldMap::m_spTitle->m_Country;
	m_Angle += 0.01f;
	m_Angle = SUC::Util::WrapAroundFloat(m_Angle, 360);
	m_Rotation = (Eigen::Quaternionf(Eigen::AngleAxisf(m_Angle * DEG_TO_RAD, Hedgehog::Math::CVector(0,1,0))));
	m_spModelButtonTransform->m_Transform.SetRotation(m_Rotation);
	m_spModelButtonTransform->NotifyChanged();
	//m_spModelButtonTransform->m_Transform.UpdateMatrix();
	//m_spModelButtonTransform->NotifyChanged();
	//m_spModel->m_spInstanceInfo->m_Transform = m_spModelButtonTransform->m_WorldMatrix;
	//m_spModel->m_spInstanceInfo->m_Transform = m_spModelButtonTransform->m_Transform.m_Matrix;
	//m_spModelCore->m_spInstanceInfo->m_Transform = m_spModelButtonTransform->m_Transform.m_Matrix;
	SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgSetRotation>(m_spModelButtonTransform->m_Transform.m_Rotation));
		//m_Countries[0]->m_Position = GetPositionFromNode(m_spFlagLocations->GetNode("flag pos_eggmanland"));

}
void CTitleWorldMapGlobe::Kill()
{
	SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
}