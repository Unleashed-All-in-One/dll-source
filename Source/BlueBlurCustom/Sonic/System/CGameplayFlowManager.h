#pragma once
namespace Sonic
{

	class CGameplayFlowManager : public Hedgehog::Universe::TStateMachine<Sonic::CGameplayFlowManager>,
	                             public Hedgehog::Universe::CMessageActor
	{
	public:
		class CMember
		{
		public:
			BB_INSERT_PADDING(0xC);
			Sonic::Message::SRequestChangeModuleInfo* m_ModuleInfo;
		};
		CMember* m_pMember;
	};
	BB_ASSERT_OFFSETOF(CGameplayFlowManager, m_pMember, 0xDC);
}