#pragma once
namespace SUC
{
	struct NewAnimationData
	{
		char const* m_stateName;
		char const* m_fileName;
		float m_speed;
		bool m_isLoop;
		char const* m_destinationState;

		NewAnimationData
		(
			char const* _stateName,
			char const* _fileName,
			float _speed,
			bool _isLoop,
			char const* _destinationState
		)
			: m_stateName(_stateName)
			, m_fileName(_fileName)
			, m_speed(_speed)
			, m_isLoop(_isLoop)
			, m_destinationState(_destinationState)
		{}
	};
}
namespace SUC::System
{
	//Replace most of this with BB at some point.
	class AnimationSetPatcher
	{
	public:
		struct CAnimationStateInfo
		{
			const char* m_Name;
			const char* m_FileName;
			float m_Speed;
			int32_t m_PlaybackType;
			int32_t field10;
			float field14;
			float field18;
			int32_t field1C;
			int32_t field20;
			int32_t field24;
			int32_t field28;
			int32_t field2C;
		};

		struct CAnimationStateSet
		{
			CAnimationStateInfo* m_pEntries;
			size_t m_Count;
		};

		struct CAnimationStateSet2
		{
			hh::anim::SMotionInfo* m_pEntries;
			size_t m_Count;
		};

		typedef std::vector<SUC::NewAnimationData> NewAnimationDataList;
		static void RegisterHooks();
		static void InitializeAnimationList(CAnimationStateInfo* pEntries, size_t const count, NewAnimationDataList const& dataList);
		static void CreateAnimationState(void* A2, NewAnimationDataList const& dataList);
		static void RegisterClassicAnimation(std::string stateName, std::string fileName, float speed = 1, bool doLoop = false);

		static NewAnimationDataList s_NewAnimationDataGeneric;
		static NewAnimationDataList s_NewAnimationDataSuper;
		static inline std::vector<NewAnimationData> s_ClassicCustomAnimations;
	};
}