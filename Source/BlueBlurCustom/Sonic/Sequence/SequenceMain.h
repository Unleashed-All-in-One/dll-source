#pragma once
namespace Sonic
{
	namespace Sequence
	{		
		class CStoryImpl;
		class CSequenceMainImpl : public Hedgehog::Universe::CMessageActor
		{
		public:
			Hedgehog::Base::CSharedString m_CurrentSequenceModeName;
			boost::shared_ptr<CSequenceMode> m_spCurrentSequenceMode;
			BB_INSERT_PADDING(0xC);
			CStoryImpl* m_pStorySequence;
			BB_INSERT_PADDING(0x8);
		};
		BB_ASSERT_OFFSETOF(CSequenceMainImpl, m_CurrentSequenceModeName, 124);
		BB_ASSERT_OFFSETOF(CSequenceMainImpl, m_pStorySequence, 0x94);
	}
}