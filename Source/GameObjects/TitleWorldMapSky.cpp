#include "TitleWorldMapSky.h"

void CTitleWorldMapSky::AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,Sonic::CGameDocument* pGameDocument,const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
{
	pGameDocument->AddUpdateUnit("0", this);
	m_spModel = boost::make_shared<hh::mr::CSingleElement>(
		hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("title_space"));

	m_spModelButtonTransform = boost::make_shared<Sonic::CMatrixNodeTransform>();
	m_spModelButtonTransform->m_Transform.SetPosition(m_Position);
	m_spModelButtonTransform->SetParent(m_spMatrixNodeTransform.get());
	m_spModelButtonTransform->NotifyChanged();
	m_spModel->BindMatrixNode(m_spModelButtonTransform);
	AddRenderable("Object", m_spModel, false);
}
void CTitleWorldMapSky::UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
	
}
void CTitleWorldMapSky::Kill()
{
	SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
}