#include "TitleWorldMapSky.h"
#include "../BlueBlurCustom/Sonic/CGlitterPlayer.h"

void CTitleWorldMapSky::AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,Sonic::CGameDocument* pGameDocument,const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
{
	Sonic::CGameObject3D::AddCallback(worldHolder, pGameDocument, spDatabase);
	

	spSun = boost::make_shared<Sonic::CMatrixNodeTransform>();
	spSun->m_Transform.SetPosition(m_Position);
	spSun->SetParent(m_spMatrixNodeTransform.get());
	spSun->NotifyChanged();

	Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
	m_spModel = boost::make_shared<hh::mr::CSingleElement>(
		hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("title_space"));
	m_spModel->BindMatrixNode(m_spMatrixNodeTransform);
	Sonic::CGameObject::AddRenderable("Object", m_spModel, false);

	m_GlitterPlayer = Sonic::CGlitterPlayer::Make(Sonic::CGameDocument::GetInstance().get().get());
	
		//m_GlitterPlayer->m_pParticleController->m_ParticleManagerActorID = Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager->m_ActorID;
		
	
	//m_GlitterPlayer = Sonic::CGlitterPlayer::Make(Sonic::CGameDocument::GetInstance().get().get());
	//
	//m_GlitterPlayer->m_pParticleController->m_ParticleManagerActorID = Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager->m_ActorID;
	//m_GlitterPlayer->PlayContinuous(Sonic::CGameDocument::GetInstance(), m_spModelButtonTransform, "worldmap_sun", 10.0f, 10, 0);

	//m_spParticleManager = boost::make_shared<Sonic::CParticleManager>();
	////necessary since its stored in gamedoc, all particle functions seem to reference this
	//
	//Sonic::CGameDocument::GetInstance()->AddGameObject(m_spParticleManager);
	//m_spParticleManager->AddCallback(worldHolder, pGameDocument, spDatabase);
	//Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", m_spParticleManager.get());
	//const auto& world = Sonic::CGameDocument::GetInstance()->GetWorld().get().get();
	//AddRenderableWorld(world, "SparkleObject", m_spParticleManager->m_spTypicalRenderer);
	//AddRenderableWorld(world, "Sparkle_FB", m_spParticleManager->m_spDeformationRenderer);
	//AddRenderableWorld(world, "Sparkle_Stencil", m_spParticleManager->m_spPlayableMenuRenderer);
	//AddRenderableWorld(world, "SMO", m_spParticleManager->m_spShadowMapRenderer);
	//AddRenderableWorld(world,"Object_Icon", m_spParticleManager->m_spObjectIconRenderer);
	//pGameDocument->AddUpdateUnit("0", m_spParticleManager.get());
	//Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager = m_spParticleManager;
	//Sonic::CGameDocument::GetInstance()->m_pMember->m_pParticleManager = m_spParticleManager.get();
	//SpawnParticleManager(worldHolder, pGameDocument, spDatabase);
	pGameDocument->AddUpdateUnit("0", this);
	//void __stdcall GameplayFlowStage::CreateBGMComponent(GameplayFlowStage1 *stage)
	//FUNCTION_PTR(void, __stdcall, Thging, 0xD06E30, void * This);
	//Thging((((Hedgehog::Universe::CStateMachineBase*)Sonic::CApplicationDocument::GetInstance()->m_pMember->m_pGameplayFlowManager)->GetCurrentState().get()));
	
}
hh::math::CQuaternion quat;
static bool taaaa = false;
void CTitleWorldMapSky::SetRotation(const hh::math::CQuaternion& quaternion)
{
	quat = quaternion;
	SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgSetRotation>(quaternion));
	m_spMatrixNodeTransform->m_Transform.SetRotation(quaternion);
	m_spMatrixNodeTransform->NotifyChanged();
}
void CTitleWorldMapSky::UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
	if(!taaaa)
	{
		m_GlitterPlayer->PlayContinuous(Sonic::CGameDocument::GetInstance(), spSun, "word_map_sun", 1, 1, 0);
		taaaa = true;
	}
	auto part = m_spParticleManager;
	//Common::ObjectCGlitterPlayerOneShot(this, "ef_ch_sng_yh1_boost1");
	//CTitleWorldMapSky::SetRotation(TitleWorldMap::QuaternionFromAngleAxis(-s_RotationAngle, CVector(0, 1, 0)););
}
void CTitleWorldMapSky::Kill()
{
	if (m_GlitterPlayer)
	{
		typedef void* __fastcall CGlitterPlayerDestructor(void* This, void* Edx, bool freeMem);
		CGlitterPlayerDestructor* destructorFunc = *(CGlitterPlayerDestructor**)(*(uint32_t*)m_GlitterPlayer);
		destructorFunc(m_GlitterPlayer, nullptr, true);
	}
	SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
}