#include <Hedgehog/Universe/Engine/hhStateMachineBase.h>
#include "..\BlueBlurCustom\Sonic\System\ApplicationDocument.h"
#include "..\BlueBlurCustom\Sonic\System\CGameplayFlowManager.h"
namespace Sonic::Sequence
{
	class CStoryImpl
	{
	public:
		BB_INSERT_PADDING(0x90);
		char m_Field90;
		char m_Field91;
	};
}
namespace SUC::System
{
	Sonic::Sequence::CSequenceMode* ChangeSequenceMode(const char* in_NewSequenceName)
	{
		IS_DEMO = true;
		Sonic::Sequence::CModeCreaterListImpl* m_pModeFactory = Sonic::Sequence::CModeCreaterListImpl::GetInstance();

		boost::shared_ptr<Sonic::Sequence::CSequenceMainImpl> spSequenceMain =
			Sonic::CApplicationDocument::GetInstance()->m_pMember->
			m_spSequenceMain;
		spSequenceMain->m_pStorySequence->m_Field90 = 1;
		spSequenceMain->m_pStorySequence->m_Field91 = 1;
		auto it = m_pModeFactory->m_SequenceModes.find(in_NewSequenceName);
		if (it != m_pModeFactory->m_SequenceModes.end())
		{
			spSequenceMain->m_CurrentSequenceModeName = in_NewSequenceName;

			Sonic::Sequence::CSequenceMode* m_pNewSequenceMode = it->second();
			spSequenceMain->m_spCurrentSequenceMode->EndModule();
			//spSequenceMain->m_Field88 = 0;
			//spSequenceMain->m_Field8C = 0;
			//spSequenceMain->m_NextModuleIndex = 0;
			spSequenceMain->m_spCurrentSequenceMode.reset();
			spSequenceMain->m_spCurrentSequenceMode = boost::shared_ptr<Sonic::Sequence::CSequenceMode>(m_pNewSequenceMode);
			m_pNewSequenceMode->ChangeModule();
			SequenceHelpers::ResetStorySequence();
			return m_pNewSequenceMode;
		}
		return nullptr;
	}
	void SequenceHelpers::ChangeModule(ModuleFlow in_Flow)
	{
		const char* m_NewMode = "";
		switch (in_Flow)
		{
		case StageAct:
		{
			m_NewMode = "Stage";
			break;

		}
		default:
		{
			m_NewMode = ((const char**)0x01A42760)[(int)in_Flow];
			break;
		}
		}
		ChangeModuleByName(m_NewMode);
	}
	Sonic::Sequence::CSequenceMode* SequenceHelpers::ChangeModuleByName(const char* in_Name)
	{
		return ChangeSequenceMode(in_Name);
	}
	void SequenceHelpers::QueueEvent(const char* in_EventName)
	{
		auto message = Sonic::Message::MsgEventRequest(in_EventName, 0);
		Sonic::Event::EventQueue::ProcessMessage(&message);
	}
	/// <summary>
	/// Sends a message to CEventQueue to queue an event.
	/// </summary>
	/// <param name="in_EventName">Event archive name</param>
	/// <param name="in_EventModule">Event module, if it's prerendered use "Event"
	/// If it's realtime, use "StageEvent"</param>
	void SequenceHelpers::PlayEvent(const char* in_EventName, ModuleFlow in_EventModule)
	{
		auto message = Sonic::Message::MsgEventRequest(in_EventName, 0);
		Sonic::Event::EventQueue::ProcessMessage(&message);
		SequenceHelpers::ChangeModule(in_EventModule);
	}

	std::string SequenceHelpers::GetCurrentStageName(bool withoutNumber = false)
	{
		std::string returnVal = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter->m_pStageParameter->TerrainArchiveName.c_str();
		if (withoutNumber)
		{
			returnVal.erase(returnVal.length() - 3);
		}
		return returnVal;
	}
	void SequenceHelpers::ResetStorySequence()
	{
		//you have to run this twice. dont ask me why, what works works.
		FUNCTION_PTR(void, __thiscall, StorySeqProcessStorySequenceEvent, 0x00D76980, void* StorySeq, Sonic::Message::MsgStorySequenceEvent * a2);

		auto message2 = Sonic::Message::MsgStorySequenceEvent(0, 0);
		StorySeqProcessStorySequenceEvent(Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spSequenceMain->m_pStorySequence, &message2);
		StorySeqProcessStorySequenceEvent(Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spSequenceMain->m_pStorySequence, &message2);
	}
	void SequenceHelpers::SetStageInfo(const SLoadInfo* in_LoadInfo)
	{
		const auto& m_GameParam = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter;

		m_GameParam->m_pStageParameter->TerrainArchiveName = in_LoadInfo->StageArchiveName.c_str();
		m_GameParam->m_pStageParameter->ModeName = "Stage";
		//2c is the event name
		//m_GameParam->m_pStageParameter->Field2C = in_LoadInfo->StageArchiveName.c_str();
		m_GameParam->m_pStageParameter->Field30 = in_LoadInfo->StageArchiveName.c_str();
		Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID = in_LoadInfo->PlayerType;
	}
	/// <summary>
	/// Sends a message to CMainSequence to trigger a stage load.
	/// </summary>
	/// <param name="in_EventName">Stage archive name</param>
	void SequenceHelpers::LoadStage(const char* in_StageName, int sequenceEventExtra, bool resetStorySequence)
	{
		ChangeModule(ModuleFlow::StageAct);
		SetStageInfo(new SLoadInfo(in_StageName, SLoadInfo::MODERN, false));
	}
	/// <summary>
	/// Uses the SetupPlayer and SetupPlayerForce functions from CStorySequence to change player classes
	/// NOTE: This does not switch players in realtime.
	/// </summary>
	/// <param name="in_PlayerType"></param>
	void SequenceHelpers::SetPlayerType(int in_PlayerType, bool forced)
	{
		Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID = in_PlayerType;
	}
	void SequenceHelpers::Update()
	{
		if (Sonic::CApplicationDocument::GetInstance())
		{
			if (Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spSequenceMain)
			{
				auto e = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spSequenceMain;

				//const char* string = SUC::Format("Field88: %d\nField8C: %d\nField90: %d\nField98: %d\nField9C: %d", e->m_Field88, e->m_Field8C, e->m_NextModuleIndex, e->m_Field98, e->m_Field9C);
				//DebugDrawText::log(string, 0, 1000, TEXT_GREEN);
			}
		}
	}
	//void __thiscall Sonic::Sequence::CStoryImpl::LuanneFunctions::SetupPlayer(int this, int a2, Luanne_IntegerMessageContainer *intMessage)
	HOOK(void, __fastcall, SetupPlayer, 0x00D72360, void* This, void* Edx, int a2, void* message)
	{
		return;
	}
	HOOK(char, __fastcall, sub_D76B10, 0xD76B10, void* Tthis, void*edx,  int a2, int a3)
	{
		return 1;
	}
	void SequenceHelpers::RegisterHooks()
	{
		INSTALL_HOOK(SetupPlayer);
		INSTALL_HOOK(sub_D76B10);
		//INSTALL_HOOK(ConstructStorySequence);
		//INSTALL_HOOK(CStoryLua_SetupPlayer);
		//INSTALL_HOOK(CStoryLua_SetupPlayerForce);
		//INSTALL_HOOK(CStoryLua_SetupStage);
	}
}