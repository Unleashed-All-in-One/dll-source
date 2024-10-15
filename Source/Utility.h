#pragma once
#include "BlueBlurCustom/Sonic/Particle/ParticleManager.h"
namespace SUC::Util
{
	static void SpawnParticleManager()
	{
		const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder = Sonic::CGameDocument::GetInstance()->GetWorld().get();
		Sonic::CGameDocument* pGameDocument = Sonic::CGameDocument::GetInstance().get().get();
		const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spApplicationDatabase;
		FUNCTION_PTR(void*, __stdcall, AddRenderableWorld, 0x00D4E3C0, void* world, const Hedgehog::Base::CStringSymbol in_Category,
			const boost::shared_ptr<Hedgehog::Mirage::CRenderable>&in_spRenderable);

		Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager = boost::make_shared<Sonic::CParticleManager>();
		const auto& m_spParticleManager = Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager;
		//necessary since its stored in gamedoc, all particle functions seem to reference this

		Sonic::CGameDocument::GetInstance()->AddGameObject(m_spParticleManager);
		m_spParticleManager->AddCallback(worldHolder, pGameDocument, spDatabase);
		Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", m_spParticleManager.get());
		const auto& world = Sonic::CGameDocument::GetInstance()->GetWorld().get().get();
		AddRenderableWorld(world, "SparkleObject", m_spParticleManager->m_spTypicalRenderer);
		AddRenderableWorld(world, "Sparkle_FB", m_spParticleManager->m_spDeformationRenderer);
		AddRenderableWorld(world, "Sparkle_Stencil", m_spParticleManager->m_spPlayableMenuRenderer);
		AddRenderableWorld(world, "SMO", m_spParticleManager->m_spShadowMapRenderer);
		AddRenderableWorld(world, "Object_Icon", m_spParticleManager->m_spObjectIconRenderer);
		pGameDocument->AddUpdateUnit("0", m_spParticleManager.get());
		Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager = m_spParticleManager;
		Sonic::CGameDocument::GetInstance()->m_pMember->m_pParticleManager = m_spParticleManager.get();
	}
	static hh::math::CVector Spherize(hh::math::CVector in_Vector, float in_Radius, hh::math::CVector in_Offset = hh::math::CVector::Identity())
	{
		return ((in_Vector - in_Offset).normalized() * in_Radius) +
			in_Offset;
	}
	
	static Hedgehog::Math::CQuaternion QuaternionFromAngleAxis(float angle, const Hedgehog::Math::CVector& axis)
	{
		Hedgehog::Math::CQuaternion q;
		float m = sqrt(axis.x() * axis.x() + axis.y() * axis.y() + axis.z() * axis.z());
		float s = sinf(angle / 2) / m;
		q.x() = axis.x() * s;
		q.y() = axis.y() * s;
		q.z() = axis.z() * s;
		q.w() = cosf(angle / 2);
		return q;
	}
	static float WrapAroundFloat(const float number, const float bounds)
	{
		float result = number;

		if (number > bounds)
		{
			result = number - bounds;
		}

		if (number < 0)
		{
			result = number + bounds;
		}

		return result;
	}
	static float VectorAngle(const Hedgehog::Math::CVector& a, const Hedgehog::Math::CVector& b)
	{
		const float dot = a.dot(b);
		//return acos(dot / sqrt(a.squaredNorm() * b.squaredNorm()));
		return acos(dot / sqrt(a.squaredNorm()));
	}
}