
//void __thiscall sub_E9FD10(char *this, int a2, int a3, int a4)
HOOK(void, __fastcall, sub_E9FD10, 0xE9FD10, char* This, void* Edx, int a2, int a3, int a4)
{
	originalsub_E9FD10(This, Edx, a2, a3, a4);
	if (!Sonic::Player::CSonicClassicContext::GetInstance())
	{
		int second = (int)(This + 244);
		auto v5 = *(void**)(second + 4);
		auto v6 = (void*)*((DWORD*)v5 + 1);
		Hedgehog::Base::CSharedString* string = (Hedgehog::Base::CSharedString*)v6 + 3;
		*string = "Contact";
	}
}
void GetPosIronPole(DWORD* This)
{
	auto v3 = ((double(__thiscall*)(DWORD*)) * (DWORD*)(*This + 100))(This);
	auto v4 = This[46];
	auto v7 = v3 * 0.5;
	CVector4 vector4 = CVector4(v7, 0, 0, 1);
	//Matrix4x4 *__cdecl Matrix::FromQuaternion(Matrix4x4 *a1, Hedgehog::Math::CQuaternion *a2)
	CMatrix44 out_Matrix;
	FUNCTION_PTR(CMatrix44*, __cdecl, FromQuat, 0x006F10A0, CMatrix44* a1, Hedgehog::Math::CQuaternion * a2);
	FUNCTION_PTR(Hedgehog::Math::CVector*, __thiscall, TransformCoordinate, 0x009BE6B0, CMatrix44 * This, Hedgehog::Math::CVector * result, const Hedgehog::Math::CVector4 * value);
	auto v5 = FromQuat(&out_Matrix, (Hedgehog::Math::CQuaternion * )(v4 + 6));
	CVector result;
	TransformCoordinate(v5, &result, &vector4);
		//Hedgehog::Math::CMatrix::TransformCoordinate(v5, (Hedgehog::Math::CVector *)&result, &value);
}
bool isDoingLerp;
float timer;
bool inRangeForOne;
CVector posStart;
SharedPtrTypeless soundTest;


//void __thiscall sub_FFF100(int this, const struct Hedgehog::Universe::SUpdateInfo *a2)
HOOK(void, __fastcall, Sonic_CObjIronPole_UpdateSerial, 0xFFF100, Sonic::CGameObject3D* This, void* Edx, const struct Hedgehog::Universe::SUpdateInfo* a2)
{
	if (Sonic::Player::CSonicClassicContext::GetInstance() == nullptr)
	{
		originalSonic_CObjIronPole_UpdateSerial(This, Edx, a2);
		return;
	}
	TransformUtilities::DoWerehogArmHomingIfClose(This->m_spMatrixNodeTransform, 5, a2->DeltaTime);
	/*Sonic::Player::CPlayerSpeedContext* playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	float distance = abs(TransformUtilities::Distance(playerContext->m_spMatrixNode->m_Transform.m_Position, This->m_spMatrixNodeTransform->m_Transform.m_Position));
	if (distance < 5)
	{		
		DebugDrawText::log("CanDo", 0);
		if (isDoingLerp)
		{
			if (timer >= 1)
			{
				isDoingLerp = false;
				timer = 0;
			}
			timer += a2->DeltaTime;

			playerContext->m_spMatrixNode->m_Transform.SetPosition(
				CVector(
					Common::lerpUnclampedf(posStart.x(), This->m_spMatrixNodeTransform->m_Transform.m_Position.x(), easeInOutQuart(timer / 1)),
					Common::lerpUnclampedf(posStart.y(), This->m_spMatrixNodeTransform->m_Transform.m_Position.y(), easeInOutQuart(timer / 1)),
					Common::lerpUnclampedf(posStart.z(), This->m_spMatrixNodeTransform->m_Transform.m_Position.z(), easeInOutQuart(timer / 1))
				)
			);
			playerContext->m_Velocity = CVector(0, 0, 0);
		}
		if (inputPtr->IsTapped(Sonic::eKeyState_B) && !isDoingLerp)
		{
			Common::PlaySoundStaticCueName(soundTest, "es_armstretch");
			isDoingLerp = true;
			posStart = playerContext->m_spMatrixNode->m_Transform.m_Position;
		}
	}
	else
	{
		inRangeForOne = false;
	}*/
	originalSonic_CObjIronPole_UpdateSerial(This, Edx, a2);
}
void Patches::applyPatches()
{
	INSTALL_HOOK(Sonic_CObjIronPole_UpdateSerial);
	INSTALL_HOOK(sub_E9FD10);
    //Makes Sonic's result animations work again for modern
    WRITE_JUMP(0X00CFDBD5, 0x00CFD840);

	//Patch out a weird check for the results music
	WRITE_JUMP(0x00CFD472, 0x00CFD492);
}