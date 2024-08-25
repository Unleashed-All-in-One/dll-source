#pragma once
namespace SUC::Player::Evil
{
	enum ResourceType
	{
		CSB,
		Effect
	};
	enum class CollisionBoneType
	{
		None,
		RHand,
		LHand,
		RLeg,
		LLeg,
		MiddleHand,
		Head,
		Hips,
		MiddleLeg // huuuh?
	};
	struct CollisionParam
	{
		float Size;
		float SizeDelta;
		CollisionBoneType BoneType;
		float StartFrame;
		float EndFrame;
	};
	struct Param
	{
		std::string FileName;
		std::string Cue;
	};
	struct Trigger_Frame
	{
		int Type;
		int Start;
		int End;
	};
	struct Trigger
	{
		int ResourceID;
		Trigger_Frame Frame;
		bool IsFollowNode;
		bool IsInheritPositionOnly;
		std::string NodeName;
		Hedgehog::math::CVector Translation;
		Hedgehog::math::CQuaternion Rotation;
		Hedgehog::math::CVector Scale;
	};
	struct Resource
	{
		int ID;
		ResourceType Type;
		Param Params;
	};
	struct ResourceInfo
	{
		std::vector<Resource> Resources;
	};
	struct TriggerInfo
	{
		std::vector<Trigger> Resources;
	};
	struct EffectConfig
	{
		std::string REffect_Name1;
		std::string LEffect_Name1;
	};
	struct CollisionConfig
	{
		bool DebugDraw;
		std::vector<CollisionParam> BoneInfo;
		//sonic team were really funny people, they decided to not separate attack
		//hitboxes into separate xml nodes, instead they made it so that the number
		//after any variable corresponds to an entry in a list.
	};
	struct MoveRatioHelper
	{
		float FrameStart;
		float FrameValue;
		float FrameValue_Y;
	};
	class Motion
	{
	public:
		std::string MotionName;
		std::string FileName;
		float MotionBlendTimeS;
		float MotionBlendTimeE;
		float MotionBlendTimeEIdle;
		float MotionMoveSpeedRatio;
		float MotionMoveSpeedRatio_Y;
		std::vector< MoveRatioHelper> MotionMoveSpeedRatio_H;
		std::vector< MoveRatioHelper> MotionMoveSpeedRatio_H_Y;
		std::vector<float> MotionMoveSpeedRatioFrameStart;
		std::vector<float> MotionMoveSpeedRatioFrame;
		std::vector<float> MotionMoveSpeedRatioFrameY;
		float MotionFirstSpeed;
		float MiddleSpeed;
		float EndSpeed;
		float MotionSpeed_FirstFrame;
		float MotionSpeed_MiddleFrame;
		int MotionSpeed_EndFrame;
		std::string AttackScope;
		float DontCatchTime;
		bool DontJump;
		bool IsGravity;
		int ShakeCameraStartFrame;
		float GroundHeightRepair;
		std::string MoveType;
		std::string ComposedFileName;
		EffectConfig Effect;
		CollisionConfig Collision;

		ResourceInfo ResourceInfos;
		TriggerInfo TriggerInfos;
		Motion()
		{
			MotionMoveSpeedRatio_H = std::vector<MoveRatioHelper>();
			MotionMoveSpeedRatio_H_Y = std::vector<MoveRatioHelper>();
			MotionMoveSpeedRatioFrameStart = std::vector<float>(21, -1);
			MotionMoveSpeedRatioFrame = std::vector<float>(21, -1);
			MotionMoveSpeedRatioFrameY = std::vector<float>(21, -1);
		}
	};
	struct WerehogAttackNew
	{
		std::string ActionName;
		std::string MotionName;
		int ValidLevel_Min;
		int ValidLevel_Max;
		bool ValidCommon;
		bool ValidBerserker;
		std::string KEY__YDown;
		std::string KEY__XDown;
		std::string KEY__ADown;
		std::string KEY__Land;
		std::string KEY__AirCombo;
		std::string KEY__End;
		bool Guard;
		bool Avoid;
		float KEY__StartFrame;
		float KEY__EndFrame;
		float WaitEndMotionEndFrame;
		float WaitEndMotionSpeed;
		float EndMotionSpeed;
		float LandStartFrame;
		float ActionValidHeightMin;
		bool IsGravity;
		Motion Werehog_AssociatedMotion;
		//From the respective attacks tbres files

	};
	/// This class manages the EvilAttackAction XML files.
	///	It also stores all of the attacks.
	class EvilAttackConfiguration
	{
	public:
		static void RegisterHooks(const char* path);
		static std::vector<WerehogAttackNew> s_Attacks;
		static std::vector<WerehogAttackNew> s_StartAttacks;
		static std::vector<Motion> s_AnimationTable;
		static std::string GetBoneNameFromCollisionParam(int index)
		{
			std::vector<std::string> collisionBoneNames = { "None", "RHand", "LHand", "RLeg", "LLeg", "MiddleHand", "Head", "Hips", "MiddleLeg" };
			return collisionBoneNames[index];
		}
	};
}