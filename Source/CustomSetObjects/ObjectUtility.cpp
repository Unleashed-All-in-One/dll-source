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
CQuaternion ObjectUtility::QuaternionFromAngleAxisUtil(float angle, const CVector& axis)
{
	CQuaternion q;
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
CQuaternion ObjectUtility::QuaternionFaceTowards(const Eigen::Vector3f& targetPoint, const Eigen::Vector3f& position, CQuaternion rotation) 
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
	CVector front = playerContext->m_spMatrixNode->m_Transform.m_Rotation * CVector(0, 0, -1);
	float distance = abs(ObjectUtility::Distance(playerContext->m_spMatrixNode->m_Transform.m_Position + front, target));
	if (distance < maxDistance)
	{
		if (inputPtr->IsTapped(Sonic::eKeyState_B) && playerContext->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName() != "EvilArmSwing")
		{
			Project::nodeForArmswing = target;
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
void ObjectUtility::RegisterAnimations(const boost::shared_ptr<Hedgehog::Animation::CAnimationPose>& animPose, std::vector<NewAnimationData> anims, const boost::shared_ptr<hh::mr::CSingleElement>& model, Sonic::CAnimationStateMachine* stateMachine)
{
	Hedgehog::Animation::SMotionInfo* pEntries = new Hedgehog::Animation::SMotionInfo[anims.size()];
	for (size_t i = 0; i < anims.size(); i++)
	{
		pEntries[i].Name = anims[i].m_stateName;
		pEntries[i].FileName = anims[i].m_fileName;
		pEntries[i].Speed = anims[i].m_speed;
		pEntries[i].RepeatType = !anims[i].m_isLoop;
		pEntries[i].StartFrame = 0;
		pEntries[i].EndFrame = -1.0f;
		pEntries[i].Field18 = -1.0f;
		pEntries[i].Field1C = 0;
		pEntries[i].Field20 = -1;
		pEntries[i].Field24 = -1;
		pEntries[i].Field28 = -1;
		pEntries[i].Field2C = -1;
	}

	animPose->AddMotionInfo(pEntries, anims.size());
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