Hedgehog::Math::CVector ObjectUtility::MoveAroundPivot(Hedgehog::Math::CVector& player, const Hedgehog::Math::CVector& pivot, const Hedgehog::Math::CVector& rotationAngles)
{
	if (rotationAngles.x() == 0)
		return player;

	float x = player.x();
	float y = player.z();

	float px = pivot.x();
	float py = pivot.z();
	float s = sin(rotationAngles.x());
	float c = cos(rotationAngles.x());

	x -= px;
	y -= py;

	double nx = (x * c) - (y * s);
	double ny = (x * s) + (y * c);

	x = nx + px;
	y = ny + py;
	player.x() = x;
	player.z() = y;
	return Hedgehog::Math::CVector(x, player.y(), y);
}

Hedgehog::Math::CQuaternion ObjectUtility::QuaternionFromAngleAxisUtil(float angle, const Hedgehog::Math::CVector& axis)
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
float ObjectUtility::Distance(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2)
{
	Eigen::Vector3f diff = v1 - v2;
	double distanceSquared = diff.dot(diff);
	return std::sqrt(distanceSquared);
}

Eigen::Vector3f ObjectUtility::ClampMagnitudeMax(const Eigen::Vector3f& vec, float minMagnitude, float maxMagnitude) {
	float magnitude = vec.norm();
	Eigen::Vector3f clampedVec = vec;


	if (magnitude > maxMagnitude)
	{
		clampedVec *= maxMagnitude / magnitude;
	}

	return clampedVec;
}

Hedgehog::Math::CQuaternion ObjectUtility::QuaternionFaceTowards(const Eigen::Vector3f& targetPoint, const Eigen::Vector3f& position, Hedgehog::Math::CQuaternion rotation) 
{
	Eigen::Vector3f currentForward = rotation * Eigen::Vector3f::UnitZ();
	Eigen::Vector3f targetDirection = (targetPoint - position).normalized();

	Eigen::Quaternionf rotationQuaternion;
	rotationQuaternion.setFromTwoVectors(currentForward, targetDirection);

	return rotation = rotationQuaternion * rotation;
}

float ObjectUtility::easeInOutQuart(float x)
{
	return x < 0.5
		? (1 - sqrt(1 - pow(2 * x, 2))) / 2
		: (sqrt(1 - pow(-2 * x + 2, 2)) + 1) / 2;
}
bool ObjectUtility::DoWerehogArmHomingIfClose(Hedgehog::Math::CVector target, float maxDistance, float deltaTime)
{
	Sonic::Player::CPlayerSpeedContext* playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	Hedgehog::Math::CVector front = playerContext->m_spMatrixNode->m_Transform.m_Rotation * Hedgehog::Math::CVector(0, 0, -1);
	float distance = abs(ObjectUtility::Distance(playerContext->m_spMatrixNode->m_Transform.m_Position + front, target));
	if (distance < maxDistance)
	{
		if (inputPtr->IsTapped(Sonic::eKeyState_B) && playerContext->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName() != "EvilArmSwing")
		{
			SUC::Project::s_TempArmswingNode = target;
			playerContext->ChangeState("EvilArmSwing");
			return true;
		}
	}
	else
	{
		return false;
	}
}
bool ObjectUtility::IsResourceAvailableHKX(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase, std::string in_assetName)
{
	auto rawData = in_spDatabase->GetRawData(_strdup(std::format("{0}.anm.hkx", in_assetName).c_str()));
	if (rawData && rawData->IsMadeAll())
		return true;
	else
		return false;
}
bool ObjectUtility::IsResourceAvailableMODEL(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase, std::string in_assetName)
{
	auto rawData = in_spDatabase->GetRawData(_strdup(std::format("{0}", in_assetName).c_str()));
	if (rawData && rawData->IsMadeAll())
		return true;
	else
		return false;
}
///Registers a UV Animation (uv-anim) motion to in_spEffectMotionAll. Fun Fact: Gens also has a function that basically just does this!
void ObjectUtility::AssignUVAnimation(const char* in_animationName,const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase, const boost::shared_ptr<Hedgehog::Mirage::CModelData>& in_spModelData, const boost::shared_ptr<hh::mot::CSingleElementEffectMotionAll>& in_spEffectMotionAll)
{
	FUNCTION_PTR(void, __thiscall, fpGetTexCoordAnimData, 0x7597E0,
		hh::mot::CMotionDatabaseWrapper const& wrapper,
		boost::shared_ptr<Hedgehog::Motion::CTexcoordAnimationData>&texCoordAnimData,
		hh::base::CSharedString const& name,
		uint32_t flag
	);

	FUNCTION_PTR(void, __thiscall, fpCreateUVAnim, 0x7537E0,
		Hedgehog::Motion::CSingleElementEffectMotionAll * This,
		boost::shared_ptr<hh::mr::CModelData> const& modelData,
		boost::shared_ptr<Hedgehog::Motion::CTexcoordAnimationData> const& texCoordAnimData
	);

	boost::shared_ptr<Hedgehog::Motion::CTexcoordAnimationData> texCoordAnimData;
	hh::mot::CMotionDatabaseWrapper motWrapper(in_spDatabase.get());
	fpGetTexCoordAnimData(motWrapper, texCoordAnimData, in_animationName, 0);
	fpCreateUVAnim(in_spEffectMotionAll.get(), in_spModelData, texCoordAnimData);
}
void ObjectUtility::CreateModel(const std::string& in_ModelName, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase, boost::shared_ptr<hh::mr::CSingleElement>& in_spModelResult)
{
	hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
	boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(in_ModelName.c_str(), 0);
	in_spModelResult = boost::make_shared<hh::mr::CSingleElement>(spModelData);
}
void ObjectUtility::RegisterAnimations(const boost::shared_ptr<Hedgehog::Animation::CAnimationPose>& animPose, std::vector<SUC::NewAnimationData> anims, const boost::shared_ptr<hh::mr::CSingleElement>& model, Sonic::CAnimationStateMachine* stateMachine)
{
	std::vector<hh::anim::SMotionInfo> pEntries = std::vector<hh::anim::SMotionInfo>(0, {"",""});
	for (size_t i = 0; i < anims.size(); i++)
	{
		pEntries.push_back(Hedgehog::Animation::SMotionInfo(anims[i].m_stateName, anims[i].m_fileName, 1, !anims[i].m_isLoop));
	}
	animPose->AddMotionInfo(&pEntries.front(), anims.size());
	//animationStateMachine = boost::make_shared< Sonic::CAnimationStateMachine>();
	//auto test = animationStateMachine->GetContext();
	animPose->CreateAnimationCache();
	model->BindPose(animPose);
	for (size_t i = 0; i < anims.size(); i++)
	{
		stateMachine->AddAnimationState(anims[i].m_stateName);
	}
}
void ObjectUtility::SetTransformScale(Hedgehog::Mirage::CTransform& in_Transform, Hedgehog::math::CVector in_Scale)
{
	//thanks skyth! -Nextin
	auto& rMatrix = in_Transform.m_Matrix.matrix();
	const float scale = max(rMatrix.col(0).head<3>().norm(),
		max(rMatrix.col(1).head<3>().norm(), rMatrix.col(2).head<3>().norm()));

	rMatrix.col(0) /= (scale / in_Scale.x());
	rMatrix.col(1) /= (scale / in_Scale.y());
	rMatrix.col(2) /= (scale / in_Scale.z());
}
float ObjectUtility::GetAnimTime(Sonic::CAnimationStateMachine* in_AnimStateMachine)
{
	in_AnimStateMachine->Update(Hedgehog::Universe::SUpdateInfo(0,0, ""));
	return in_AnimStateMachine->GetAnimationState(in_AnimStateMachine->GetCurrentState()->m_Name)->GetLocalTime();
}