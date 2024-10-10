#pragma once
namespace BBExt
{
	class CPhantomCallback;
	class CCharacterProxy;

	class CAabbAndCastUnit : public Sonic::CPhysicsUnit
	{
	public:
		hk2010_2_0::hkReferencedObject* m_pReferencedObject48{};
		void* m_pCachingShapePhantom{};
		boost::shared_ptr<CPhantomCallback> m_spPhantomCallback{};
	};

	static constexpr uint32_t pCCharacterProxyIntegrate = 0x010E3CF0;
	static void __declspec(noinline) fCCharacterProxyIntegrate(CCharacterProxy* in_pProxy, float in_DeltaTime)
	{
		__asm
		{
			mov eax, in_pProxy
			push in_DeltaTime
			call[pCCharacterProxyIntegrate]
		}
	}

	static constexpr uint32_t pCCharacterProxySetRotation = 0x010E0990;
	static void __declspec(noinline) fCCharacterProxySetRotation(CCharacterProxy* in_pProxy, const Hedgehog::Math::CQuaternion& in_rRotation)
	{
		__asm
		{
			mov edi, in_pProxy
			mov esi, in_rRotation
			call[pCCharacterProxySetRotation]
		}
	}

	class CCharacterProxy : public CAabbAndCastUnit
	{
	public:

		void Integrate(float in_DeltaTime)
		{
			fCCharacterProxyIntegrate(this, in_DeltaTime);
		}

		void SetRotation(const Hedgehog::Math::CQuaternion& in_rRotation)
		{
			fCCharacterProxySetRotation(this, in_rRotation);
		}

		void ScanContacts(void* out_pContactData, float in_DeltaTime, const Hedgehog::Math::CVector& in_rDirection)
		{
			BB_FUNCTION_PTR(void, __stdcall, func, 0x010E2160, CCharacterProxy * This, void* contactData, float deltaTime, const Hedgehog::Math::CVector & direction);
			func(this, out_pContactData, in_DeltaTime, in_rDirection);
		}

		void SetAngle(float angle)
		{
			cosAngleThing = cos(angle);
		}

		uint32_t __EntityListener;
		uint32_t __PhantomListener;
		float m_Field60;
		uint32_t m_Field64;
		float m_Float68;
		float m_Field6C;
		float m_Field70;
		float m_Field74;
		float m_Field78;
		float m_Field7C;
		float m_Field80;
		float cosAngleThing;
		uint32_t m_Field88;
		uint32_t m_Field8C;
		Hedgehog::Math::CVector m_UpAxis;
		float m_FieldA0;
		uint32_t m_FieldA4;
		float m_NormalStrength;
		float m_FieldAC;
		uint32_t m_DamageID;
		uint32_t m_FieldB4;
		uint32_t m_FieldB8;
		uint32_t m_FieldBC;
		Hedgehog::Math::CVector m_FieldC0;
		uint32_t m_FieldD0;
		uint32_t m_FieldD4;
		uint32_t m_FieldD8;
		uint32_t m_FieldDC;
		Hedgehog::Math::CVector m_FieldE0;
		uint32_t m_FieldF0;
		uint32_t m_FieldF4;
		uint32_t m_FieldF8;
		bool m_IsDirty;
		bool m_FieldFD;
		bool m_FieldFE;
		bool m_FieldFF;
		uint32_t m_Field100;
		uint32_t m_Field104;
		uint32_t m_Field108;
		uint32_t m_Field10C;
		Hedgehog::Math::CVector m_Field110;
		Hedgehog::Math::CVector m_Position;
		Hedgehog::Math::CVector m_Velocity;
		Hedgehog::Math::CVector m_Field140;
		float m_VelocityStrengthScale;
		uint32_t m_Field154;
		uint32_t m_Field158;
		uint32_t m_Field15C;
		//Hedgehog::Math::CVector m_Field160;
		//Hedgehog::Math::CVector m_Field170;
		//Hedgehog::Math::CVector m_Field180;
		Hedgehog::Math::CQuaternion m_hkRotationMatrix;
		uint32_t m_Field190;
		uint32_t m_Field194;
		uint32_t m_Field198;
		uint32_t m_Field19C;
		uint32_t m_Field1A0;
		uint32_t m_Field1A4;
		uint32_t m_Field1A8;
		uint32_t m_Field1AC;
		uint32_t m_Field1B0;
		uint32_t m_Field1B4;
		uint32_t m_Field1B8;
		uint32_t m_Field1BC;
		Hedgehog::Math::CVector m_Field1C0;
		Hedgehog::Math::CQuaternion m_Rotation;
		Hedgehog::Math::CVector m_Field1E0;
		float m_Field1F0;
		uint32_t m_Field1F4;
		uint32_t m_Field1F8;
		uint32_t m_Field1FC;
	};

	BB_ASSERT_OFFSETOF(CCharacterProxy, cosAngleThing, 0x84);
	BB_ASSERT_OFFSETOF(CCharacterProxy, m_Position, 0x120);
	BB_ASSERT_OFFSETOF(CCharacterProxy, m_Velocity, 0x130);
}