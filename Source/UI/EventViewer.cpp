#include <rapidxml/rapidxml_utils.hpp>

using namespace rapidxml;

class Resource {
public:
	int id;
	std::string type;
	std::string fileName;
	std::string cueName;

	Resource(int id, const std::string& type, const std::string& fileName, const std::string& cueName)
		: id(id), type(type), fileName(fileName), cueName(cueName) {}
};

class Trigger {
public:
	float resourceID;
	float startFrame;
	float endFrame;
	bool hasBeenUsed = false;

	Trigger(float resourceID, float startFrame, float endFrame)
		: resourceID(resourceID), startFrame(startFrame), endFrame(endFrame) {}
};
struct InspireResourceTriggerSort
{
	inline bool operator() (const Trigger& struct1, const Trigger& struct2)
	{
		return (struct1.startFrame < struct2.startFrame);
	}
};
class InspireResource {
public:
	std::string fileVersion;
	int startFrame;
	int endFrame;
	bool isFixFrame;
	std::string matrix;
	std::vector<Resource> resources;
	std::vector<Trigger> triggers;

	InspireResource(const char* filePath) 
	{
		
		// Load the XML file
		file<> xmlFile(filePath);
		xml_document<> doc;
		doc.parse<0>(xmlFile.data());

		// Access the root node
		xml_node<>* root = doc.first_node("InspireResource");

		// Parse VersionInfo
		xml_node<>* versionInfoNode = root->first_node("VersionInfo");
		fileVersion = versionInfoNode->first_node("FileVersion")->value();

		// Parse ProjectInfo
		xml_node<>* projectInfoNode = root->first_node("ProjectInfo");
		startFrame = std::stoi(projectInfoNode->first_node("StartFrame")->value());
		endFrame = std::stoi(projectInfoNode->first_node("EndFrame")->value());
		isFixFrame = std::string(projectInfoNode->first_node("IsFixFrame")->value()) == "True";

		// Parse TerrainInfo
		xml_node<>* terrainInfoNode = root->first_node("TerrainInfo");
		matrix = terrainInfoNode->first_node("Matrix")->value();

		// Parse ResourceInfo
		xml_node<>* resourceInfoNode = root->first_node("ResourceInfo");
		for (xml_node<>* resourceNode = resourceInfoNode->first_node("Resource"); resourceNode; resourceNode = resourceNode->next_sibling()) {
			int id = std::stoi(resourceNode->first_node("ID")->value());
			std::string type = resourceNode->first_node("Type")->value();
			std::string fileName = resourceNode->first_node("Param")->first_node("FileName")->value();
			std::string cueName = resourceNode->first_node("Param")->first_node("CueName")->value();
			resources.emplace_back(id, type, fileName, cueName);
		}

		// Parse TriggerInfo
		xml_node<>* triggerInfoNode = root->first_node("TriggerInfo");
		for (xml_node<>* triggerNode = triggerInfoNode->first_node("Trigger"); triggerNode; triggerNode = triggerNode->next_sibling()) {
			float resourceID = std::stof(triggerNode->first_node("ResourceID")->value());
			float start = std::stof(triggerNode->first_node("Frame")->first_node("Start")->value());
			float end = std::stof(triggerNode->first_node("Frame")->first_node("End")->value());
			triggers.emplace_back(resourceID, start, end);
		}
		std::sort(triggers.begin(), triggers.end(), InspireResourceTriggerSort());
	}
	Trigger findTriggersForResource(int resourceID) 
	{
		for (const auto& trigger : triggers) {
			if (trigger.resourceID == resourceID) {
				return trigger;
			}
		}
		return Trigger(-1, -1, -1);
	}
};










Chao::CSD::RCPtr<Chao::CSD::CProject> rcNewUi;
Chao::CSD::RCPtr<Chao::CSD::CScene> basicSceneTemplate;
boost::shared_ptr<Sonic::CGameObjectCSD> spNewUi;
DWORD* objecttt;
bool isGoingToSkip;
bool isInEventViewer;
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

void EventViewer::update()
{
	DebugDrawText::log(std::to_string(LevelLoadingManager::LastSavedQueueIndex).c_str(), 0);
	if (isInEventViewer)
	{
		if (isGoingToSkip)
		{
			//if (UnleashedHUD_API::IsLoadingFadeoutCompleted())
			//{
				//LevelLoadingManager::setCorrectStage();
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
HOOK(char, __fastcall, sub_10FE9E0, 0x10FE9E0, void* This, void* Edx, DWORD* a1, int a2)
{
	isInEventViewer = true;
	return originalsub_10FE9E0(This, Edx, a1,a2);
}
float frame = 0;
bool doCount = true;
InspireResource* resource;
Trigger* nextTrigger;
AudioHandle soundhandle;
int currentEventIndex = 0;
int startFrameTime;
HOOK(void, __fastcall, EventUpdate, 0x00B24A40, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	doCount = !doCount;
	if (doCount)
		frame += (1.0f / in_rUpdateInfo.DeltaTime) / 60.0f;
	if (resource == nullptr)
	{
		return originalEventUpdate(This, Edx, in_rUpdateInfo);
	}
	for (size_t i = currentEventIndex; i < resource->triggers.size(); i++)
	{
		if (frame >= resource->triggers[i].startFrame && !resource->triggers[i].hasBeenUsed)
		{
			MiniAudioHelper::playSound(soundhandle, 0, resource->resources[i].cueName, false, true);
			DebugDrawText::log(std::format("PLAYSOUND: {0}", resource->resources[i].cueName).c_str(), 5);
			const char* in_Name = resource->resources[i].cueName.c_str();
			resource->triggers[i].hasBeenUsed = true;
		}
	}
	
	DebugDrawText::log(std::format("EVENT_FRAME CUSTOM: {0}", frame).c_str(), 0);
	return originalEventUpdate(This, Edx, in_rUpdateInfo);
}
HOOK(long, __fastcall, sub_B1ECF0, 0xB1ECF0, int This, void* Edx, int a2, int a3, int _38)
{
	frame = 0;
	doCount = true;
	auto string = std::format("{0}\\disk\\EventSceneResource\\{1}_voice_English.inspire_resource.xml", Project::modPath, LevelLoadingManager::getEventID());
	if (!std::filesystem::exists(string))
	{
		printf("\n[SonicUnleashedConversion] Missing InspireResource file.");
	}
	else
	{
		resource = new InspireResource(string.c_str());
	}
	//currentEventIndex = 0;
	return originalsub_B1ECF0(This, Edx, a2, a3, _38);
}
//
//Sonic::CEventScene::UpdateSerial(_DWORD *this, float *a2)
//char __thiscall sub_10FE9E0(void *this, DWORD *a1, int a2)
void EventViewer::applyPatches()
{
	INSTALL_HOOK(CreateBTNSKip);
	INSTALL_HOOK(EventUpdate);
	INSTALL_HOOK(sub_B1ECF0);
	INSTALL_HOOK(sub_10fe500);
	INSTALL_HOOK(sub_10FE9E0);
	//WRITE_JUMP(0x00B21AD0, InterceptProject);
	//WRITE_JUMP(0x00B21A33, InterceptGameObject);

	WRITE_JUMP(0x00B21D38, 0x00B21CF3);
	//IMPORTANT!!!!!!!!!
	WRITE_MEMORY(0x16A467C, void*, CHudSonicStageRemoveCallback2);

}
