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
		struct CAnimationStateSet
		{
			Hedgehog::Animation::SMotionInfo* m_pEntries;
			size_t m_Count;
		};
		typedef std::vector<SUC::NewAnimationData> NewAnimationDataList;
		static void RegisterHooks();
		static void RegisterClassicAnimation(std::string stateName, std::string fileName, float speed = 1, bool doLoop = false);

		static NewAnimationDataList s_NewAnimationDataGeneric;
		static NewAnimationDataList s_NewAnimationDataSuper;
		static inline std::vector<NewAnimationData> s_ClassicCustomAnimations;
		static inline std::vector<Hedgehog::Animation::SMotionInfo> m_AnimEntriesClassic;
		static inline std::vector<Hedgehog::Animation::SMotionInfo> m_AnimEntriesGeneric;
		static inline std::vector<Hedgehog::Animation::SMotionInfo> m_AnimEntriesSuper;
	};
}