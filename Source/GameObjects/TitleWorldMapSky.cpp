#include "TitleWorldMapSky.h"

namespace Sonic
{
	class CParticleManager : public Sonic::CGameObject
	{
	public:
		BB_INSERT_PADDING(0x40 + 0xAC) {};

		CParticleManager()
		{
			*reinterpret_cast<size_t*>(this) = 0x016CFF90;
			*reinterpret_cast<size_t*>(static_cast<CMessageActor*>(this)) = 0x016CFFC8;
		}

		BB_OVERRIDE_FUNCTION_PTR(void, CGameObject, AddCallback, 0xE8F330, (const Hedgehog::Base::THolder<Sonic::CWorld>&, in_rWorldHolder),
			(Sonic::CGameDocument*, in_pGameDocument), (const boost::shared_ptr<Hedgehog::Database::CDatabase>&, in_spDatabase))
	};
}

boost::shared_ptr<Sonic::CParticleManager> m_spParticleManager;
//boost::shared_ptr<Sonic::CParticleManager> m_spParticleManager;
//void ParticleTest()
//{
//	SharedPtrTypeless handle;
//	// . . .
//	
//	// . . . 
//	Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager->
//	fpAddParticle5(Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager.get(), handle, nullptr, "worldmap_sun", 0);
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
//char __thiscall sub_E8FEF0(char *this, int a2, int a3)
HOOK(bool, __fastcall, sub_E8FEF0, 0xE8FEF0, char* This, void* Edx, hh::fnd::Message& a2, int a3)
{
	if (std::strstr(a2.GetType(), "MsgParticlePlayOneShotByMatrix") != nullptr)
	{
		auto test = (MsgParticlePlayOneShotByMatrix&)a2;
	}
	return originalsub_E8FEF0(This, Edx, a2, a3);
}
BB_ASSERT_OFFSETOF(MsgParticlePlayOneShotByMatrix, m_ParticleName, 0x50);
void CTitleWorldMapSky::AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,Sonic::CGameDocument* pGameDocument,const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
{
	INSTALL_HOOK(sub_E8FEF0);
	m_spParticleManager = boost::make_shared<Sonic::CParticleManager>();
	//necessary since its stored in gamedoc, all particle functions seem to reference this
	Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager = m_spParticleManager;
	Sonic::CGameDocument::GetInstance()->m_pMember->m_pParticleManager = Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager.get();
	Sonic::CGameDocument::GetInstance()->AddGameObject(m_spParticleManager);
	Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", m_spParticleManager.get());

	FUNCTION_PTR(void, __thiscall, FuncNeededToMakeGlitterWork, 0xD5CB80,
		void* This,
		const Hedgehog::Base::THolder<Sonic::CWorld>&worldHolder,
		Sonic::CGameDocument * pGameDocument,
		const boost::shared_ptr<Hedgehog::Database::CDatabase>&spDatabase);

	FUNCTION_PTR(void*, __cdecl, CGlitterPlayerInit, 0x1255B40, Sonic::CGameDocument * pGameDocument);

	FuncNeededToMakeGlitterWork(this, worldHolder, pGameDocument, spDatabase);
	m_GlitterPlayer = CGlitterPlayerInit(pGameDocument);
	uint32_t* playerThingy = (uint32_t*)m_GlitterPlayer;
	uint32_t* playerThingy2 = playerThingy + 4;
	//*(int*)(playerThingy2 + 136) = m_spParticleManager->m_ActorID;
	
	auto msg = boost::make_shared<MsgParticlePlayOneShotByMatrix>();
	msg->m_ParticleName = "fireworks";
	msg->m_Matrix = m_spMatrixNodeTransform->m_WorldMatrix;
	msg->m_Timescale = 1.0f;
	msg->idk2 = 1;
	m_spParticleManager->SendMessageImm(m_spParticleManager->m_ActorID, msg);
	//Common::ObjectCGlitterPlayerOneShot(this, "fireworks");



	/*m_spModelButtonTransform = boost::make_shared<Sonic::CMatrixNodeTransform>();
	m_spModelButtonTransform->m_Transform.SetPosition(m_Position);
	m_spModelButtonTransform->SetParent(m_spMatrixNodeTransform.get());
	m_spModelButtonTransform->NotifyChanged();*/

	Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
	m_spModel = boost::make_shared<hh::mr::CSingleElement>(
		hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("title_space"));
	m_spModel->BindMatrixNode(m_spMatrixNodeTransform);
	Sonic::CGameObject::AddRenderable("Object", m_spModel, false);
	pGameDocument->AddUpdateUnit("0", this);

}
hh::math::CQuaternion quat;
void CTitleWorldMapSky::SetRotation(const hh::math::CQuaternion& quaternion)
{
	quat = quaternion;
	SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgSetRotation>(quaternion));
	m_spMatrixNodeTransform->m_Transform.SetRotation(quaternion);
	m_spMatrixNodeTransform->NotifyChanged();
}
void CTitleWorldMapSky::UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
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