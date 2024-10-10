#include "TitleWorldMapSky.h"
#include "../BlueBlurCustom/Sonic/CGlitterPlayer.h"
#include "../BlueBlurCustom/Sonic/Particle/ParticleManager.h"

//boost::shared_ptr<Sonic::CParticleManager> m_spParticleManager;
//void ParticleTest()
//{
//	SharedPtrTypeless handle;
//	// . . .
//	
//	// . . . 
//	m_spParticleManager->
//	fpAddParticle5(m_spParticleManager.get(), handle, nullptr, "worldmap_sun", 0);
//}
class MsgParticlePlayOneShotByMatrix : public hh::fnd::MessageTypeSet
{
	HH_FND_MSG_MAKE_TYPE(0x01680D14);
public:
	hh::math::CMatrix m_Matrix;
	Hedgehog::base::CSharedString m_ParticleName;
	float m_Timescale;
	int idk2;
};

BB_ASSERT_OFFSETOF(MsgParticlePlayOneShotByMatrix, m_ParticleName, 0x50);
BB_ASSERT_OFFSETOF(MsgParticlePlayOneShotByMatrix, m_Timescale, 0x54);
BB_ASSERT_OFFSETOF(MsgParticlePlayOneShotByMatrix, idk2, 0x58);
//char __thiscall sub_E8FEF0(char *this, int a2, int a3)
HOOK(bool, __fastcall, sub_E8FEF0, 0xE8FEF0, char* This, void* Edx, hh::fnd::Message& a2, int a3)
{
	if (std::strstr(a2.GetType(), "MsgParticlePlayOneShotByMatrix") != nullptr)
	{
		auto test = (MsgParticlePlayOneShotByMatrix&)a2;
	}
	return originalsub_E8FEF0(This, Edx, a2, a3);
}

void CTitleWorldMapSky::AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,Sonic::CGameDocument* pGameDocument,const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
{
	Sonic::CGameObject3D::AddCallback(worldHolder, pGameDocument, spDatabase);
	

	m_spModelButtonTransform = boost::make_shared<Sonic::CMatrixNodeTransform>();
	m_spModelButtonTransform->m_Transform.SetPosition(m_Position);
	m_spModelButtonTransform->SetParent(m_spMatrixNodeTransform.get());
	m_spModelButtonTransform->NotifyChanged();

	Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
	m_spModel = boost::make_shared<hh::mr::CSingleElement>(
		hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("title_space"));
	m_spModel->BindMatrixNode(m_spMatrixNodeTransform);
	Sonic::CGameObject::AddRenderable("Object", m_spModel, false);
	//_DWORD *__stdcall Sonic::CWorld::AddRenderable(int world, Hedgehog::Base::CSharedString *in_Category, boost::Hedgehog::Mirage::CRenderable *in_Renderable)
	FUNCTION_PTR(void*, __stdcall, AddRenderableWorld, 0x00D4E3C0, void* world, const Hedgehog::Base::CStringSymbol in_Category,
		const boost::shared_ptr<Hedgehog::Mirage::CRenderable>&in_spRenderable);

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
		m_GlitterPlayer = Sonic::CGlitterPlayer::Make(Sonic::CGameDocument::GetInstance().get().get());
		taaaa = true;
		m_GlitterPlayer->m_pParticleController->m_ParticleManagerActorID = Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager->m_ActorID;
		m_GlitterPlayer->PlayContinuous(Sonic::CGameDocument::GetInstance(), m_spMatrixNodeTransform, "worldmap_sun", 10.0f, 10, 0);
		Sonic::CGameDocument::GetInstance()->GetWorld()->m_pMember->m_spCamera = Sonic::CGameDocument::GetInstance()->GetWorld()->m_pMember->m_spOverrideCamera;

	};
	auto part = m_spParticleManager;
	//Common::ObjectCGlitterPlayerOneShot(this, "ef_ch_sng_yh1_boost1");
	CTitleWorldMapSky::SetRotation(quat);
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