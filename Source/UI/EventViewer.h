#pragma once
#include <rapidxml/rapidxml_utils.hpp>

namespace SUC::UI::Event
{
	class EventViewer
	{

	public:
		class Resource
		{
		public:
			int id;
			std::string type;
			std::string fileName;
			std::string cueName;
			float volume;

			Resource(int id, const std::string& type, const std::string& fileName, const std::string& cueName, float volume)
				: id(id), type(type), fileName(fileName), cueName(cueName), volume(volume) {}
		};

		class Trigger
		{
		public:
			float resourceID;
			float startFrame;
			float endFrame;
			bool hasBeenUsed = false;
			bool notPlayingAnymore = false;

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
		class InspireResource
		{
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
				rapidxml::file<> xmlFile(filePath);
				rapidxml::xml_document<> doc;
				doc.parse<0>(xmlFile.data());

				// Access the root node
				rapidxml::xml_node<>* root = doc.first_node("InspireResource");

				// Parse VersionInfo
				rapidxml::xml_node<>* versionInfoNode = root->first_node("VersionInfo");
				fileVersion = versionInfoNode->first_node("FileVersion")->value();

				// Parse ProjectInfo
				rapidxml::xml_node<>* projectInfoNode = root->first_node("ProjectInfo");
				startFrame = std::stoi(projectInfoNode->first_node("StartFrame")->value());
				endFrame = std::stoi(projectInfoNode->first_node("EndFrame")->value());
				isFixFrame = std::string(projectInfoNode->first_node("IsFixFrame")->value()) == "True";

				// Parse TerrainInfo
				rapidxml::xml_node<>* terrainInfoNode = root->first_node("TerrainInfo");
				matrix = terrainInfoNode->first_node("Matrix")->value();

				// Parse ResourceInfo
				rapidxml::xml_node<>* resourceInfoNode = root->first_node("ResourceInfo");
				for (rapidxml::xml_node<>* resourceNode = resourceInfoNode->first_node("Resource"); resourceNode; resourceNode = resourceNode->next_sibling())
				{
					std::string type = resourceNode->first_node("Type")->value();
					if (type != "CriAudioData")
						continue;
					int id = std::stoi(resourceNode->first_node("ID")->value());
					std::string fileName = resourceNode->first_node("Param")->first_node("FileName")->value();
					std::string cueName = resourceNode->first_node("Param")->first_node("CueName")->value();
					float volume = std::stof(resourceNode->first_node("Param")->first_node("MainVolume")->value());
					resources.emplace_back(id, type, fileName, cueName, volume);
				}

				// Parse TriggerInfo
				rapidxml::xml_node<>* triggerInfoNode = root->first_node("TriggerInfo");
				for (rapidxml::xml_node<>* triggerNode = triggerInfoNode->first_node("Trigger"); triggerNode; triggerNode = triggerNode->next_sibling())
				{
					int resourceID = std::stoi(triggerNode->first_node("ResourceID")->value());
					if (!DoesResourceExist(resourceID))
						continue;
					float start = std::stof(triggerNode->first_node("Frame")->first_node("Start")->value());
					float end = std::stof(triggerNode->first_node("Frame")->first_node("End")->value());
					triggers.emplace_back(resourceID, start, end);
				}
				std::sort(triggers.begin(), triggers.end(), InspireResourceTriggerSort());
			}
			bool DoesResourceExist(int triggerID)
			{
				for (const auto& resource : resources) 
				{
					if (resource.id == triggerID) {
						return true;
					}
				}
				return false;
			}
			Trigger FindTriggersForResource(int resourceID)
			{
				for (const auto& trigger : triggers) 
				{
					if (trigger.resourceID == resourceID) 
					{
						return trigger;
					}
				}
				return Trigger(-1, -1, -1);
			}
		};
		class SoundHandleEvent
		{
		public:
			SharedPtrTypeless soundHandle;
			std::string cueName;
			int triggerID;
			SoundHandleEvent(std::string in_CueName, int in_TriggerID, float in_Volume)
			{
				Common::PlaySoundStaticCueName(soundHandle, in_CueName.c_str());
				Hedgehog::Sound::CSoundHandleCri* handleTemp2 = (Hedgehog::Sound::CSoundHandleCri*)soundHandle.get();
				DebugDrawText::log(std::format("PLAYER: {0}", (uint32_t)handleTemp2->GetSoundPlayer()).c_str(), 10);
				auto playerCri = (CSoundPlayerCri*)handleTemp2->GetSoundPlayer();

				FUNCTION_PTR(void*, __thiscall, SoundPlayerSetVolume, 0x00765850, void* _SoundPlayer, float _Volume);
				SoundPlayerSetVolume(playerCri, in_Volume);
				cueName = in_CueName;
				triggerID = in_TriggerID;
			}
			void Kill()
			{
				soundHandle.reset();
			}
		};
		static void RegisterHooks();
		static void Update();
	};
}
