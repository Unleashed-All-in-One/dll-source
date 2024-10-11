
#include "TitleWorldMapSun.h"
bool m_Started;
void CTitleWorldMapSun::AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder, Sonic::CGameDocument* pGameDocument,
	const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
{
	m_spChildNode = boost::make_shared<Sonic::CMatrixNodeTransform>();
	m_spChildNode->m_Transform.SetPosition(hh::math::CVector::Identity());
	m_spChildNode->SetParent(m_spMatrixNodeTransform.get());
	m_spChildNode->NotifyChanged();
	m_spMatrixNodeTransform->m_Transform.SetPosition(hh::math::CVector(0, 0, 20));
	m_spMatrixNodeTransform->NotifyChanged();
	Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);

	m_GlitterPlayer = Sonic::CGlitterPlayer::Make(Sonic::CGameDocument::GetInstance().get().get());
	
	pGameDocument->AddUpdateUnit("0", this);
	pGameDocument->AddUpdateUnit("1", this);
}
void CTitleWorldMapSun::UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
	//if(!m_Started)
	//{
	//	//m_GlitterPlayer->m_pParticleController->m_ParticleManagerActorID = Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager->m_ActorID;
	//	m_GlitterPlayer->PlayContinuous(Sonic::CGameDocument::GetInstance(), m_spChildNode, "worldmap_sun",100.0f, 10, 0);
	//	m_Started = true;
	//}
	Sonic::CGameDocument::GetInstance()->GetWorld()->m_pMember->m_spCamera = Sonic::CGameDocument::GetInstance()->GetWorld()->m_pMember->m_spOverrideCamera;
}
void CTitleWorldMapSun::SetPosition(const hh::math::CVector& in_Position) const
{
	m_spMatrixNodeTransform->m_Transform.SetPosition(in_Position);
	m_spMatrixNodeTransform->m_Transform.UpdateMatrix();
	m_spMatrixNodeTransform->NotifyChanged();
}