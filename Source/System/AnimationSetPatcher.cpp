#include "AnimationSetPatcher.h"

namespace SUC::System
{

	std::vector<SUC::NewAnimationData> AnimationSetPatcher::s_NewAnimationDataGeneric;
	std::vector<SUC::NewAnimationData> AnimationSetPatcher::s_NewAnimationDataSuper;
	std::vector<Hedgehog::Animation::SMotionInfo> m_AnimEntriesClassic;
	std::vector<Hedgehog::Animation::SMotionInfo> m_AnimEntriesGeneric;
	std::vector<Hedgehog::Animation::SMotionInfo> m_AnimEntriesSuper;
	/// <summary>
	/// Registers new animation states (requires AddMotionEntriesToSet first)
	/// </summary>
	/// <param name="in_AnimData">List of new animations to add</param>
	/// <param name="in_StateMachine"></param>
	void AddAnimationStates(const std::vector<NewAnimationData>& in_AnimData, Sonic::CAnimationStateMachine* in_StateMachine)
	{
		for (auto m_AnimEntries : in_AnimData)
		{
			in_StateMachine->AddAnimationState(m_AnimEntries.m_stateName);
		}
	}
	/// <summary>
	/// Adds new animations to hardcoded animation sets.
	/// </summary>
	/// <param name="r_AnimationList">Reference to combined lists of animations (keep in scope)</param>
	/// <param name="in_NewAnims">List of new animations to add</param>
	/// <param name="in_AnimSet">Original hardcoded animation set</param>
	void AddMotionEntriesToSet(std::vector<Hedgehog::Animation::SMotionInfo>& r_AnimationList, const std::vector<NewAnimationData> in_NewAnims, const AnimationSetPatcher::CAnimationStateSet* in_AnimSet)
	{
		r_AnimationList = std::vector<Hedgehog::Animation::SMotionInfo>(
			in_AnimSet->m_Count + in_NewAnims.size(), {"", ""});

		std::copy(in_AnimSet->m_pEntries, in_AnimSet->m_pEntries + in_AnimSet->m_Count, r_AnimationList.data());

		for (size_t i = 0; i < in_NewAnims.size(); i++)
		{
			Hedgehog::Animation::SMotionInfo& entry = r_AnimationList[in_AnimSet->m_Count + i];
			entry.pName = AnimationSetPatcher::s_ClassicCustomAnimations[i].m_stateName;
			entry.pFileName = AnimationSetPatcher::s_ClassicCustomAnimations[i].m_fileName;
			entry.Speed = AnimationSetPatcher::s_ClassicCustomAnimations[i].m_speed;
			entry.RepeatType = !AnimationSetPatcher::s_ClassicCustomAnimations[i].m_isLoop;
			entry.StartFrame = 0;
			entry.EndFrame = -1.0f;
			entry.Field18 = -1.0f;
			entry.Field1C = 0;
			entry.Field20 = -1;
			entry.Field24 = -1;
			entry.Field28 = -1;
			entry.Field2C = -1;
		}
		WRITE_MEMORY(&in_AnimSet->m_pEntries, void*, r_AnimationList.data());
		WRITE_MEMORY(&in_AnimSet->m_Count, size_t, in_AnimSet->m_Count + in_NewAnims.size());
	}
	//---------------------------------------------------
	// CSonicClassic
	//---------------------------------------------------
	HOOK(void*, __cdecl, InitializeClassicSonicAnimationList, 0x01281D50)
	{
		void* result = originalInitializeClassicSonicAnimationList();
		AddMotionEntriesToSet(m_AnimEntriesClassic,AnimationSetPatcher::s_ClassicCustomAnimations, (AnimationSetPatcher::CAnimationStateSet*)0x15DCE60);
		return result;
	}	
	HOOK(void, __fastcall, CSonicClassicCreateAnimationStates, 0x00DDF1C0, void* This, void* Edx, Sonic::CAnimationStateMachine* A2, void* A3)
	{
		originalCSonicClassicCreateAnimationStates(This, Edx, A2, A3);
		AddAnimationStates(AnimationSetPatcher::s_ClassicCustomAnimations, A2);
	}
	//---------------------------------------------------
	// CSonic
	//---------------------------------------------------
	HOOK(void*, __cdecl, InitializeSonicAnimationList, 0x1272490)
	{
		void* pResult = originalInitializeSonicAnimationList();
		AddMotionEntriesToSet(m_AnimEntriesGeneric, AnimationSetPatcher::s_NewAnimationDataGeneric, (AnimationSetPatcher::CAnimationStateSet*)0x15E8D40);

		return pResult;
	}

	HOOK(void, __fastcall, CSonicCreateAnimationStates, 0xE1B6C0, void* This, void* Edx, Sonic::CAnimationStateMachine* A2, void* A3)
	{
		originalCSonicCreateAnimationStates(This, Edx, A2, A3);
		AddAnimationStates(AnimationSetPatcher::s_NewAnimationDataGeneric, A2);
	}

	//---------------------------------------------------
	// CSonicSpRenderableSsn
	//---------------------------------------------------
	
	HOOK(void*, __cdecl, InitializeSuperSonicAnimationList, 0x1291D60)
	{
		void* pResult = originalInitializeSuperSonicAnimationList();


		AddMotionEntriesToSet(m_AnimEntriesSuper, AnimationSetPatcher::s_NewAnimationDataSuper, (AnimationSetPatcher::CAnimationStateSet*)0x1A55980);

		return pResult;
	}

	HOOK(void, __fastcall, CSonicSpRenderableSsnCreateAnimationStates, 0xDA31B0, void* This, void* Edx, void* A1, Sonic::CAnimationStateMachine* A2)
	{
		originalCSonicSpRenderableSsnCreateAnimationStates(This, Edx, A1, A2);
		AddAnimationStates(AnimationSetPatcher::s_NewAnimationDataSuper, A2);
	}

	//---------------------------------------------------
	// Static functions
	//---------------------------------------------------
	
	void AnimationSetPatcher::RegisterClassicAnimation(std::string stateName, std::string fileName, float speed, bool doLoop)
	{		
		s_ClassicCustomAnimations.push_back(NewAnimationData(_strdup(stateName.c_str()), _strdup(fileName.c_str()), speed, doLoop, nullptr));
		auto msg = std::format("Registered animation for Classic ({0}, {1})", stateName, fileName);
		SUC::LogMessage(msg.c_str());
	}
	void AnimationSetPatcher::RegisterHooks()
	{
		//INSTALL_HOOK(CSonic_AnimationBlending);

		//m_newAnimationData.emplace_back("CrawlEnter", "sn_crawlS", 1.0f, false, nullptr);
		//m_newAnimationData.emplace_back("CrawlExit", "sn_crawlE", 1.0f, false, nullptr);
		//m_newAnimationData.emplace_back("CrawlLoop", "sn_crawl_loop", 1.75f, true, nullptr);

		//m_newAnimationDataSuper.emplace_back("CrawlEnter", "ssn_crawlS", 1.0f, false, nullptr);
	   // m_newAnimationDataSuper.emplace_back("CrawlExit", "ssn_crawlE", 1.0f, false, nullptr);
	   // m_newAnimationDataSuper.emplace_back("CrawlLoop", "ssn_crawl_loop", 1.75f, true, nullptr);
		
		//INSTALL_HOOK(CAnimationControlSingle_Debug);
		INSTALL_HOOK(InitializeClassicSonicAnimationList);
		INSTALL_HOOK(CSonicClassicCreateAnimationStates);
		
		INSTALL_HOOK(InitializeSonicAnimationList);
		INSTALL_HOOK(CSonicCreateAnimationStates);
	}
}