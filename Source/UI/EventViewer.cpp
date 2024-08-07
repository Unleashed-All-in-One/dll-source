#include "EventViewer.h"

#include <rapidxml/rapidxml_utils.hpp>

using namespace rapidxml;
namespace SUC::UI::Event
{
	
	float frame = 0;
	bool doCount = true;
	EventViewer::InspireResource* resource;
	EventViewer::Trigger* nextTrigger;
	AudioHandle soundhandle;
	SharedPtrTypeless handle;
	int currentEventIndex = 0;
	int startFrameTime;
	Chao::CSD::RCPtr<Chao::CSD::CProject> rcNewUi;
	Chao::CSD::RCPtr<Chao::CSD::CScene> basicSceneTemplate;
	boost::shared_ptr<Sonic::CGameObjectCSD> spNewUi;
	DWORD* objecttt;
	bool isGoingToSkip;
	bool isInEventViewer;
	std::vector<EventViewer::SoundHandleEvent> soundHandles;
	int getSoundHandleIndex(int triggerID)
	{
		for (size_t i = 0; i < soundHandles.size(); i++)
		{
			if (soundHandles[i].triggerID == triggerID)
			{
				return i;
			}
		}
		return -1;
	}
	void CreateScreen(Sonic::CGameObject* pParentGameObject)
	{
		if (rcNewUi && !spNewUi)
			pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spNewUi = boost::make_shared<Sonic::CGameObjectCSD>(rcNewUi, 0.5f, "HUD_B1", false), "main", pParentGameObject);
	}
	void KillScreen()
	{
		if (spNewUi)
		{
			spNewUi->SendMessage(spNewUi->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
			spNewUi = nullptr;
		}
	}
	void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
	{
		if (visible)
			CreateScreen(pParentGameObject);
		else
			KillScreen();
	}
	void __fastcall CHudSonicStageRemoveCallback2(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
	{
		KillScreen();

		Chao::CSD::CProject::DestroyScene(rcNewUi.Get(), basicSceneTemplate);

		rcNewUi = nullptr;
	}
	HOOK(volatile signed int*, __stdcall, CreateBTNSKip, 0x00B21A30, DWORD* This)
	{

		auto returnee = originalCreateBTNSKip(This);
		objecttt = This;
		isGoingToSkip = true;
		UnleashedHUD_API::StartFadeout();
		return returnee;
	}

	void EventViewer::Update()
	{
		DebugDrawText::log(std::to_string(StageManager::LastSavedQueueIndex).c_str(), 0);
		if (isInEventViewer)
		{
			if (isGoingToSkip)
			{
				//if (UnleashedHUD_API::IsLoadingFadeoutCompleted())
				//{
					//StageManager::setCorrectStage();
				isGoingToSkip = false;
				objecttt[77] = 2;
				//}
			}
		}
	}
	//Hedgehog::Base::CSharedString *__thiscall sub_10FE500(DWORD *this, Hedgehog::Base::CSharedString *a2)
	HOOK(Hedgehog::Base::CSharedString*, __fastcall, sub_10fe500, 0x10FE500, DWORD* EventQueue, void* Edx, Hedgehog::Base::CSharedString* a2)
	{
		return originalsub_10fe500(EventQueue, Edx, a2);
	}
	//_DWORD *__thiscall sub_645C40(_DWORD *this, char a2)
	HOOK(char, __fastcall, CEventQueueImpl_ProcessMessage, 0x10FE9E0, void* This, void* Edx, DWORD* a1, int a2)
	{
		isInEventViewer = true;
		return originalCEventQueueImpl_ProcessMessage(This, Edx, a1, a2);
	}
	void PlayNewSound(std::string cueName, int triggerID)
	{
		//Common::PlaySoundStaticCueName(handle, resource->resources[i].cueName.c_str())
	}
	void AudioPlayer(float frameTime)
	{
		for (size_t i = 0; i < resource->triggers.size(); i++)
		{
			if (resource->triggers[i].notPlayingAnymore)
				continue;
			if (frameTime >= resource->triggers[i].startFrame && !resource->triggers[i].hasBeenUsed)
			{
				DebugDrawText::log(std::format("PLAYSOUND: {0}", resource->resources[i].cueName).c_str(), 5);
				soundHandles.push_back(EventViewer::SoundHandleEvent(resource->resources[i].cueName.c_str(), i, resource->resources[i].volume));
				resource->triggers[i].hasBeenUsed = true;
			}
			if (frameTime >= resource->triggers[i].endFrame && resource->triggers[i].endFrame != -1 && resource->triggers[i].hasBeenUsed)
			{
				int handleIndex = getSoundHandleIndex(i);

				DebugDrawText::log(std::format("STOPSOUND: {0}", soundHandles[handleIndex].cueName).c_str(), 5);
				soundHandles[handleIndex].Kill();
				soundHandles.erase(soundHandles.begin() + handleIndex);
				resource->triggers[i].notPlayingAnymore = true;
			}
		}

	}
	HOOK(void, __fastcall, EventUpdate, 0x00B24A40, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
	{
		doCount = !doCount;
		if (doCount)
			frame += (1.0f / in_rUpdateInfo.DeltaTime) / 60.0f;
		if (resource == nullptr)
		{
			return originalEventUpdate(This, Edx, in_rUpdateInfo);
		}
		AudioPlayer(frame);

		DebugDrawText::log(std::format("EVENT_FRAME CUSTOM: {0}", frame).c_str(), 0);
		return originalEventUpdate(This, Edx, in_rUpdateInfo);
	}
	HOOK(long, __fastcall, CEventSceneStart, 0xB1ECF0, int This, void* Edx, int a2, int a3, int _38)
	{
		frame = 0;
		doCount = true;
		auto string = std::format("{0}\\disk\\EventSceneResource\\{1}_voice_English.inspire_resource.xml", SUC::Project::s_ModPath, StageManager::getEventID());
		if (!std::filesystem::exists(string))
		{
			printf("\n[SonicUnleashedConversion] Missing InspireResource file for Voice.");
		}
		else
		{
			resource = new EventViewer::InspireResource(string.c_str());
		}

		auto string2 = std::format("{0}\\disk\\EventSceneResource\\{1}_bgm.inspire_resource.xml", SUC::Project::s_ModPath, StageManager::getEventID());
		if (!std::filesystem::exists(string2))
		{
			printf("\n[SonicUnleashedConversion] Missing InspireResource file for BGM.");
		}
		else
		{
			auto res2 = new EventViewer::InspireResource(string2.c_str());

			resource->resources.insert(resource->resources.end(), res2->resources.begin(), res2->resources.end());
			resource->triggers.insert(resource->triggers.end(), res2->triggers.begin(), res2->triggers.end());
		}
		auto string3 = std::format("{0}\\disk\\EventSceneResource\\{1}_se.inspire_resource.xml", SUC::Project::s_ModPath, StageManager::getEventID());
		if (!std::filesystem::exists(string3))
		{
			printf("\n[SonicUnleashedConversion] Missing InspireResource file for SE.");
		}
		else
		{
			auto res3 = new EventViewer::InspireResource(string3.c_str());

			resource->resources.insert(resource->resources.end(), res3->resources.begin(), res3->resources.end());
			resource->triggers.insert(resource->triggers.end(), res3->triggers.begin(), res3->triggers.end());
		}
		//currentEventIndex = 0;
		return originalCEventSceneStart(This, Edx, a2, a3, _38);
	}
	//void __thiscall CriSetVolumeMaybe(_DWORD *this, int a2, float a3)
	HOOK(void, __fastcall, SetVolMaybe, 0x0075EEF0, DWORD* This, void* Edx, int a2, float a3)
	{
		originalSetVolMaybe(This, Edx, a2, a3);
		DebugDrawText::log(std::format("SETVOLMAYBE: {0}", a3).c_str(), 10);
	}
	void EventViewer::RegisterHooks()
	{
		INSTALL_HOOK(SetVolMaybe);
		INSTALL_HOOK(CreateBTNSKip);
		INSTALL_HOOK(EventUpdate);
		INSTALL_HOOK(CEventSceneStart);
		INSTALL_HOOK(sub_10fe500);
		INSTALL_HOOK(CEventQueueImpl_ProcessMessage);
		//WRITE_JUMP(0x00B21AD0, InterceptProject);
		//WRITE_JUMP(0x00B21A33, InterceptGameObject);

		//Patch out cutscene stage music
		WRITE_JUMP(0x00B2300D, 0x00B235E1);

		WRITE_JUMP(0x00B23B9C, 0X00B23BAB);


		WRITE_JUMP(0x00B21D38, 0x00B21CF3);
		//IMPORTANT!!!!!!!!!
		WRITE_MEMORY(0x16A467C, void*, CHudSonicStageRemoveCallback2);

	}
}