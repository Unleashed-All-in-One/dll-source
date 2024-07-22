#pragma once
class TitleWorldMapCamera : public Sonic::CGameObject, public Hedgehog::Universe::TStateMachine<TitleWorldMapCamera>
{
public:
	class CMyCamera : public Hedgehog::Mirage::CCamera
	{
	public:
		Hedgehog::Math::CMatrix m_InputView;
	} m_MyCamera;

	BB_INSERT_PADDING(0x10);
	Hedgehog::Math::CVector m_Position;
	Hedgehog::Math::CVector m_TargetPosition;
	Hedgehog::Math::CVector m_UpVector;
	BB_INSERT_PADDING(0x1DC);
	uint32_t m_ActorID;
	BB_INSERT_PADDING(0x08);
	float m_FieldOfView;
	float m_FieldOfViewTarget;
	BB_INSERT_PADDING(0x50);
};