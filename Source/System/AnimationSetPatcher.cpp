#include "AnimationSetPatcher.h"

namespace SUC::System
{
	int GetClassicAnimCount() { return AnimationSetPatcher::s_ClassicCustomAnimations.size(); }
	FUNCTION_PTR(void*, __stdcall, fpCreateAnimationState, 0xCDFA20, void* This, boost::shared_ptr<void>& spAnimationState, const Hedgehog::Base::CSharedString& name, const Hedgehog::Base::CSharedString& name2);
	FUNCTION_PTR(uint32_t*, __stdcall, fpGetAnimationTransitionData, 0xCDFB40, void* A2, const Hedgehog::Base::CSharedString& name);

	HOOK(bool, __fastcall, CAnimationControlSingle_Debug, 0x6D84F0, uint32_t** This, void* Edx, float a2, int a3)
	{
		std::string name((char*)(This[58][2]));
		if (name.find("sn_") != std::string::npos)
		{
			printf("%s\n", name.c_str());
			DebugDrawText::log(std::format("Anim File : %s", name.c_str()).c_str(), 0);
		}
		return originalCAnimationControlSingle_Debug(This, Edx, a2, a3);
	}

	HOOK(int*, __fastcall, CSonic_AnimationBlending, 0xE14A90, void* This, void* Edx, int a2, float a3)
	{
		return nullptr;
	}

	HOOK(void, __stdcall, CSonicContextChangeAnimation, 0xCDFC80, void* This, int a2, Hedgehog::Base::CSharedString& name)
	{
		printf("[SonicUnleashedConversion] Animation change: %s\n", name.c_str());
		originalCSonicContextChangeAnimation(This, a2, name);
	}
	//---------------------------------------------------
	// CSonicClassic
	//---------------------------------------------------
	HOOK(void*, __cdecl, InitializeClassicSonicAnimationList, 0x01281D50)
	{
		void* result = originalInitializeClassicSonicAnimationList();
		{
			int animCount = GetClassicAnimCount();
			AnimationSetPatcher::CAnimationStateSet* m_AnimationContainer = (AnimationSetPatcher::CAnimationStateSet*)0x15DCE60;
			AnimationSetPatcher::CAnimationStateInfo* m_AnimEntries = new AnimationSetPatcher::CAnimationStateInfo[m_AnimationContainer->m_Count + 2 * animCount];

			std::copy(m_AnimationContainer->m_pEntries, m_AnimationContainer->m_pEntries + m_AnimationContainer->m_Count, m_AnimEntries);

			for (size_t i = 0; i < 2 * animCount; i++)
			{
				const size_t animIndex = i % animCount;

				AnimationSetPatcher::CAnimationStateInfo& entry = m_AnimEntries[m_AnimationContainer->m_Count + i];

				entry.m_Name = AnimationSetPatcher::s_ClassicCustomAnimations[animIndex].m_stateName;
				entry.m_FileName = AnimationSetPatcher::s_ClassicCustomAnimations[animIndex].m_fileName;
				entry.m_Speed = AnimationSetPatcher::s_ClassicCustomAnimations[animIndex].m_speed;
				entry.m_PlaybackType = !AnimationSetPatcher::s_ClassicCustomAnimations[animIndex].m_isLoop;
				entry.field10 = 0;
				entry.field14 = -1.0f;
				entry.field18 = -1.0f;
				entry.field1C = 0;
				entry.field20 = -1;
				entry.field24 = -1;
				entry.field28 = -1;
				entry.field2C = -1;
			}
			WRITE_MEMORY(&m_AnimationContainer->m_pEntries, void*, m_AnimEntries);
			WRITE_MEMORY(&m_AnimationContainer->m_Count, size_t, m_AnimationContainer->m_Count + 2 * GetClassicAnimCount());
		}
		return result;
	}
	HOOK(void, __fastcall, CSonicClassicCreateAnimationStates, 0x00DDF1C0, void* This, void* Edx, void* A2, void* A3)
	{
		originalCSonicClassicCreateAnimationStates(This, Edx, A2, A3);

		FUNCTION_PTR(void*, __stdcall, createAnimationState, 0xCDFA20,
			void* This, boost::shared_ptr<void>&spAnimationState, const hh::base::CSharedString & name, const hh::base::CSharedString & alsoName);

		for (size_t i = 0; i < 2 * GetClassicAnimCount(); i++)
		{
			const size_t animIndex = i % GetClassicAnimCount();
			const hh::base::CSharedString animName = AnimationSetPatcher::s_ClassicCustomAnimations[animIndex].m_stateName;

			boost::shared_ptr<void> animationState;
			createAnimationState(A2, animationState, animName, animName);
		}
	}
	//---------------------------------------------------
	// CSonic
	//---------------------------------------------------
	std::vector<SUC::NewAnimationData> AnimationSetPatcher::s_NewAnimationDataGeneric;
	HOOK(void*, __cdecl, InitializeSonicAnimationList, 0x1272490)
	{
		void* pResult = originalInitializeSonicAnimationList();
		{
			AnimationSetPatcher::CAnimationStateSet* pList = (AnimationSetPatcher::CAnimationStateSet*)0x15E8D40;
			AnimationSetPatcher::CAnimationStateInfo* pEntries = new AnimationSetPatcher::CAnimationStateInfo[pList->m_Count + AnimationSetPatcher::s_NewAnimationDataGeneric.size()];
			std::copy(pList->m_pEntries, pList->m_pEntries + pList->m_Count, pEntries);

			AnimationSetPatcher::InitializeAnimationList(pEntries, pList->m_Count, AnimationSetPatcher::s_NewAnimationDataGeneric);
			WRITE_MEMORY(&pList->m_pEntries, void*, pEntries);
			WRITE_MEMORY(&pList->m_Count, size_t, pList->m_Count + AnimationSetPatcher::s_NewAnimationDataGeneric.size());
		}

		return pResult;
	}

	HOOK(void, __fastcall, CSonicCreateAnimationStates, 0xE1B6C0, void* This, void* Edx, void* A2, void* A3)
	{
		originalCSonicCreateAnimationStates(This, Edx, A2, A3);
		AnimationSetPatcher::CreateAnimationState(A2, AnimationSetPatcher::s_NewAnimationDataGeneric);
	}

	//---------------------------------------------------
	// CSonicSpRenderableSsn
	//---------------------------------------------------
	AnimationSetPatcher::CAnimationStateInfo* SuperSonicAnimationList = nullptr;
	uint32_t SuperSonicAnimationListSize = 0;
	void __declspec(naked) UpdateSuperSonicAnimationListSize()
	{
		static uint32_t returnAddress = 0xDA31CF;
		__asm
		{
			push    SuperSonicAnimationListSize
			push    SuperSonicAnimationList
			jmp[returnAddress]
		}
	}

	std::vector<SUC::NewAnimationData> AnimationSetPatcher::s_NewAnimationDataSuper;
	HOOK(void*, __cdecl, InitializeSuperSonicAnimationList, 0x1291D60)
	{
		void* pResult = originalInitializeSuperSonicAnimationList();
		{
			AnimationSetPatcher::CAnimationStateInfo* pEntriesOriginal = (AnimationSetPatcher::CAnimationStateInfo*)0x1A55980;
			uint8_t* count = (uint8_t*)0xDA31C9;
			SuperSonicAnimationListSize = (*count + AnimationSetPatcher::s_NewAnimationDataSuper.size());
			SuperSonicAnimationList = new AnimationSetPatcher::CAnimationStateInfo[SuperSonicAnimationListSize];
			std::copy(pEntriesOriginal, pEntriesOriginal + *count, SuperSonicAnimationList);

			AnimationSetPatcher::InitializeAnimationList(SuperSonicAnimationList, *count, AnimationSetPatcher::s_NewAnimationDataSuper);
			WRITE_JUMP(0xDA31C8, UpdateSuperSonicAnimationListSize);
		}

		return pResult;
	}

	HOOK(void, __fastcall, CSonicSpRenderableSsnCreateAnimationStates, 0xDA31B0, void* This, void* Edx, void* A1, void* A2)
	{
		originalCSonicSpRenderableSsnCreateAnimationStates(This, Edx, A1, A2);
		AnimationSetPatcher::CreateAnimationState(A2, AnimationSetPatcher::s_NewAnimationDataSuper);
	}

	//---------------------------------------------------
	// Static functions
	//---------------------------------------------------
	void AnimationSetPatcher::InitializeAnimationList(CAnimationStateInfo* pEntries, size_t const count, NewAnimationDataList const& dataList)
	{
		for (size_t i = 0; i < dataList.size(); i++)
		{
			SUC::NewAnimationData const& data = dataList[i];

			pEntries[count + i].m_Name = data.m_stateName;
			pEntries[count + i].m_FileName = data.m_fileName;
			pEntries[count + i].m_Speed = data.m_speed;
			pEntries[count + i].m_PlaybackType = !data.m_isLoop;
			pEntries[count + i].field10 = 0;
			pEntries[count + i].field14 = -1.0f;
			pEntries[count + i].field18 = -1.0f;
			pEntries[count + i].field1C = 0;
			pEntries[count + i].field20 = -1;
			pEntries[count + i].field24 = -1;
			pEntries[count + i].field28 = -1;
			pEntries[count + i].field2C = -1;
		}
	}

	void AnimationSetPatcher::CreateAnimationState(void* A2, NewAnimationDataList const& dataList)
	{
		boost::shared_ptr<void> spAnimationState;
		for (SUC::NewAnimationData const& data : dataList)
		{
			fpCreateAnimationState(A2, spAnimationState, data.m_stateName, data.m_stateName);
		}

		// Set transition data
		for (SUC::NewAnimationData const& data : dataList)
		{
			if (!data.m_destinationState) continue;

			// Initialise data on destination state
			bool found = false;
			for (SUC::NewAnimationData const& destData : dataList)
			{
				if (data.m_destinationState == destData.m_stateName)
				{
					uint32_t* pTransitionDestData = fpGetAnimationTransitionData(A2, destData.m_stateName);
					*(uint64_t*)(*pTransitionDestData + 96) = 1;
					*(uint64_t*)(*pTransitionDestData + 104) = 0;
					*(uint32_t*)(*pTransitionDestData + 112) = 1;
					found = true;
					break;
				}
			}

			if (found)
			{
				uint32_t* pTransitionData = fpGetAnimationTransitionData(A2, data.m_stateName);
				*(uint64_t*)(*pTransitionData + 96) = 1;
				*(uint64_t*)(*pTransitionData + 104) = 0;
				*(uint32_t*)(*pTransitionData + 112) = 1;
				*(float*)(*pTransitionData + 140) = -1.0f;
				*(bool*)(*pTransitionData + 144) = true;
				*(Hedgehog::Base::CSharedString*)(*pTransitionData + 136) = data.m_destinationState;
			}
			else
			{
				MessageBox(NULL, L"Animation transition destination does not exist, please check your code!", NULL, MB_ICONERROR);
			}
		}
	}
	void AnimationSetPatcher::RegisterClassicAnimation(std::string stateName, std::string fileName, float speed, bool doLoop)
	{		
		s_ClassicCustomAnimations.push_back(NewAnimationData(_strdup(stateName.c_str()), _strdup(fileName.c_str()), speed, doLoop, nullptr));
		SUC::LogMessage(std::format("Registered animation for Classic (%s, %s)", stateName, fileName).c_str());
	}
	void AnimationSetPatcher::RegisterHooks()
	{
		//INSTALL_HOOK(CSonic_AnimationBlending);

		//m_newAnimationData.emplace_back("CrawlEnter", "sn_crawlS", 1.0f, false, nullptr);
		//m_newAnimationData.emplace_back("CrawlExit", "sn_crawlE", 1.0f, false, nullptr);
		//m_newAnimationData.emplace_back("CrawlLoop", "sn_crawl_loop", 1.75f, true, nullptr);
		s_NewAnimationDataGeneric.emplace_back("JumpBoardLoop", "sn_jumpstand_s", 1.0f, false, nullptr);

		//m_newAnimationDataSuper.emplace_back("CrawlEnter", "ssn_crawlS", 1.0f, false, nullptr);
	   // m_newAnimationDataSuper.emplace_back("CrawlExit", "ssn_crawlE", 1.0f, false, nullptr);
	   // m_newAnimationDataSuper.emplace_back("CrawlLoop", "ssn_crawl_loop", 1.75f, true, nullptr);

		s_NewAnimationDataSuper.emplace_back("SquatKick", "ssn_squat_kick", 1.0f, false, nullptr);
		
		//INSTALL_HOOK(CAnimationControlSingle_Debug);
		INSTALL_HOOK(InitializeClassicSonicAnimationList);
		INSTALL_HOOK(CSonicClassicCreateAnimationStates);
		if (!s_NewAnimationDataGeneric.empty())
		{
			INSTALL_HOOK(InitializeSonicAnimationList);
			INSTALL_HOOK(CSonicCreateAnimationStates);
		}

		if (!s_NewAnimationDataSuper.empty())
		{
			INSTALL_HOOK(InitializeSuperSonicAnimationList);
			INSTALL_HOOK(CSonicSpRenderableSsnCreateAnimationStates);
		}
	}
}