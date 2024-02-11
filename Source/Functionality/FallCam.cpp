float factor = 0.0f;
bool pause = false;

bool fadePlayed = false;

const float FADE_INTRO = 3.0f;
const float DEAD_TO_RESTART = 0.15f;

float introTimer = 0.0f;
bool playIntroTimer = false;

boost::shared_ptr<Sonic::CGameObject> fadeSingleton;

class FadeObject : public Sonic::CGameObject {
	boost::shared_ptr<Sonic::CGameObjectCSD> spFade;
	Chao::CSD::RCPtr<Chao::CSD::CProject> rcFade;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rect;
public:
	~FadeObject() {
		if (spFade) {
			spFade->SendMessage(spFade->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
			spFade = nullptr;
		}
		Chao::CSD::CProject::DestroyScene(rcFade.Get(), rect);
		rcFade = nullptr;
		rect = nullptr;
	}

	void AddCallback
	(
		const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,
		Sonic::CGameDocument* pGameDocument,
		const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase
	) override
	{
		// Update unit 1 is unaffected by time slowing down
		Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
		pGameDocument->AddUpdateUnit("1", this);

		Sonic::CCsdDatabaseWrapper wrapperFade(m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

		auto spCsdProjectFade = wrapperFade.GetCsdProject("ui_fade");

		rcFade = spCsdProjectFade->m_rcProject;
		rect = rcFade->CreateScene("fade");
		CSDCommon::FreezeMotion(*rect);
		rect->SetHideFlag(true);

		spFade = boost::make_shared<Sonic::CGameObjectCSD>(rcFade, 0.5f, "HUD_A1", false);
		Sonic::CGameDocument::GetInstance()->AddGameObject(spFade, "main", this);
	}

	void Kill() {
		SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		fadeSingleton = nullptr;
	}

	void Play() {
		if (!rect)
			return;
		
		rect->SetHideFlag(false);
		CSDCommon::PlayAnimation(*rect, "Fade_Black", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0f, 0.0f, 0.0f, true);
	}

	void SetVisibility(bool visible) {
		if (!rect)
			return;

		rect->SetHideFlag(!visible);
	}
};


HOOK(bool, __fastcall, FallCam_MsgStartPause, 0x010BC130, void* This, void* Edx, void* a2)
{
	pause = true;
	return originalFallCam_MsgStartPause(This, Edx, a2);
}

HOOK(int, __fastcall, FallCam_MsgFinishPause, 0x010BC110, void* This, void* Edx, void* a2)
{
	pause = false;
	return originalFallCam_MsgFinishPause(This, Edx, a2);
}

void PlayFade() {
	auto* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

	if (!context)
		return;

	if (fadeSingleton)
		((FadeObject*)fadeSingleton.get())->Kill();
	
	fadeSingleton = boost::make_shared<FadeObject>();
	
	context->m_pPlayer->m_pMember->m_pGameDocument->AddGameObject(fadeSingleton);
	
	((FadeObject*)fadeSingleton.get())->Play();
}

void StopFade() {
	fadePlayed = false;
	if (fadeSingleton)
		((FadeObject*)fadeSingleton.get())->Kill();
}

void CameraFollow(Sonic::Player::CPlayerSpeedContext* context, Sonic::CCamera::CMyCamera& camera, float delta) {
	Hedgehog::Math::CVector playerRight = context->m_spMatrixNode->m_Transform.m_Rotation * Hedgehog::Math::CVector::UnitX();
	Hedgehog::Math::CVector playerUp = context->m_spMatrixNode->m_Transform.m_Rotation * Hedgehog::Math::CVector::UnitY();
	Hedgehog::Math::CVector playerForward = context->m_spMatrixNode->m_Transform.m_Rotation * Hedgehog::Math::CVector::UnitZ();

	factor += delta * 0.05f;
	
	Common::ClampFloat(factor, 0.0f, 1.0f);
	
	camera.m_Direction = context->m_HorizontalRotation * Eigen::Vector3f::UnitZ();

	Eigen::AngleAxisf rotPitch(FallCam::AngleBetween(camera.m_Direction, context->m_spMatrixNode->m_Transform.m_Position - camera.m_Position), playerRight);
	Eigen::AngleAxisf rotYaw(180.0f * DEG_TO_RAD, playerUp);
	Eigen::AngleAxisf rotRoll(0.0f * DEG_TO_RAD, playerForward);

	Hedgehog::Math::CQuaternion rotationSlerp = Hedgehog::Math::CQuaternion(camera.m_View.rotation().inverse()).slerp(factor, rotPitch * rotYaw * rotRoll * context->m_spMatrixNode->m_Transform.m_Rotation);

	camera.m_View = (Eigen::Translation3f(camera.m_Position) * rotationSlerp).inverse().matrix();
	camera.m_InputView = camera.m_View;
}

HOOK(void, __fastcall, FallCam_CCameraUpdateParallel, 0x10FB770, Sonic::CCamera* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo) {
	if (!*pModernSonicContext) {
		originalFallCam_CCameraUpdateParallel(This, Edx, in_rUpdateInfo);
		return;
	}

	Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

	if (!context) {
		originalFallCam_CCameraUpdateParallel(This, Edx, in_rUpdateInfo);
		return;
	}

	Sonic::CCamera::CMyCamera& camera = This->m_MyCamera;

	Hedgehog::Base::CSharedString anim = context->GetCurrentAnimationName();
	
	bool isDeadFall = Common::IsPlayerDead() && (anim == "Fall" || anim == "FallLarge");

	float time = DEAD_TO_RESTART + FADE_INTRO;

	context->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_DeadToRestartTime] = time;
	context->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_DeadBgmWaitTime] = time - 0.15f;
	context->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_DeadBgmFadeTime] = 0.15f;

	if (playIntroTimer) {
		introTimer += in_rUpdateInfo.DeltaTime;
		
		if (introTimer >= 2.0f && !fadePlayed) {
			PlayFade();
			fadePlayed = true;
		}
	}

	if ((isDeadFall || Common::IsPlayerDead()) && !playIntroTimer) {
		playIntroTimer = true;
		introTimer = 0.0f;
	}

	if (!isDeadFall) {
		originalFallCam_CCameraUpdateParallel(This, Edx, in_rUpdateInfo);
		return;
	}

	if (pause) {
		camera.m_View = (Eigen::Translation3f(camera.m_Position) * Hedgehog::Math::CQuaternion(camera.m_View.rotation().inverse())).inverse().matrix();
		camera.m_InputView = camera.m_View;
		originalFallCam_CCameraUpdateParallel(This, Edx, in_rUpdateInfo);
		return;
	}
	else if (!context->m_Is2DMode)
		CameraFollow(context, camera, in_rUpdateInfo.DeltaTime);
}

HOOK(int, __fastcall, FallCam_MsgRestartStage, 0xE76810, uint32_t* This, void* Edx, void* message)
{
	int result = originalFallCam_MsgRestartStage(This, Edx, message);

	if (!*pModernSonicContext)
		return result;

	StopFade();

	factor = 0.0f;

	playIntroTimer = false;

	return result;
}

void FallCam::Install() {
	INSTALL_HOOK(FallCam_MsgStartPause);
	INSTALL_HOOK(FallCam_MsgFinishPause);
	INSTALL_HOOK(FallCam_CCameraUpdateParallel);
	INSTALL_HOOK(FallCam_MsgRestartStage);
}