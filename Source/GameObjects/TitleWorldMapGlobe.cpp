#include "TitleWorldMapGlobe.h";

void CTitleWorldMapGlobe::AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder, Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
{
	m_spModelButtonTransform = boost::make_shared<Sonic::CMatrixNodeTransform>();
	m_spModelButtonTransform->m_Transform.SetPosition(m_Position);
	m_spModelButtonTransform->NotifyChanged();
	m_spModelButtonTransform->SetParent(m_spMatrixNodeTransform.get());

	Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
	pGameDocument->AddUpdateUnit("0", this);

	auto spModelData = hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("title_globe");
	m_spModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
	AddRenderable("Object", m_spModel, false);
	m_spEffectMotionAll = boost::make_shared<Hedgehog::Motion::CSingleElementEffectMotionAll>();
	m_spModel->BindMatrixNode(m_spModelButtonTransform);
	m_spModel->BindEffect(m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("sea-0000", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("sea-0001", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud-0000", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud-0002", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud2-0000", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud2-0002", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud3_Shadow-0001", spDatabase, spModelData, m_spEffectMotionAll);
	ObjectUtility::AssignUVAnimation("cloud3_Shadow-0000", spDatabase, spModelData, m_spEffectMotionAll);
}

void CTitleWorldMapGlobe::UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
	FUNCTION_PTR(void, __thiscall, fpUpdateMotionAll, 0x752F00, Hedgehog::Motion::CSingleElementEffectMotionAll * This, float dt);
	fpUpdateMotionAll(m_spEffectMotionAll.get(), updateInfo.DeltaTime);
	m_spModelButtonTransform->m_Transform.SetPosition(m_Position);
}
void CTitleWorldMapGlobe::Kill()
{
	SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
}