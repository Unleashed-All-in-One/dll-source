
namespace SUC::ImGuiMenu
{
	bool DebugMenu::s_Visible = false;
	std::string sceneIndexToLoad;
	std::string eventIndexToLoad;
	int playerID;
	const float UI_INDENT = 16;
	void ImGuiInputTextString(const char* label, std::string* str, ImGuiInputTextFlags flags = 0)
	{
		char buffer[256];
		strncpy(buffer, str->c_str(), sizeof(buffer));
		buffer[sizeof(buffer) - 1] = '\0';

		if (ImGui::InputText(label, buffer, sizeof(buffer), flags)) {
			*str = buffer;
		}
	}

	bool triggerStageLoad;
	bool triggerMovieLoad;
	bool parsedTree = false;
	std::string nameTemp;
	hh::math::CVector2 scaleFactor;
	std::string GetSceneToLoad()
	{
		return sceneIndexToLoad;
	}
	void customStageLoad()
	{
		
		//System::StageManager::SetOverrideStageIDProcessor(GetSceneToLoad, true, __FILENAME__);
		//auto message = Sonic::Message::MsgSequenceEvent(2, 0);
		//auto message2 = Sonic::Message::MsgStorySequenceEvent(0, 0);
		//auto test = Sonic::Sequence::Main::GetInstance();
		//void __thiscall StorySeqProcessStorySequenceEvent(int storySequence, CMsgStorySequenceEvent *storySequenceEvent)
		//SequenceHelpers::ResetStorySequence();

		//FUNCTION_PTR(void, __thiscall, StorySeqProcessStorySequenceEvent, 0x00D76980, Sonic::Sequence::Story * StorySeq, Sonic::Message::MsgStorySequenceEvent * a2);
		//auto message2 = Sonic::Message::MsgStorySequenceEvent(1, 1);
		//StorySeqProcessStorySequenceEvent(Sonic::Sequence::Story::GetInstance(), &message2);
		//Sonic::Sequence::Main::ProcessMessage(&message);
	}
	std::string m_Moduletest;
	void drawStageTreeNode(SUC::Project::DebugStageTree::STreeNode node)
	{
		
		ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
		if (ImGui::TreeNodeEx(node.Name.c_str(), ImGuiTreeNodeFlags_None))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0, 0 });
			ImGui::Indent(16);
			for (size_t x = 0; x < node.TreeEntry.size(); x++)
			{
				ImGui::BeginGroup();
				ImGui::Text("X");
				ImGui::SameLine();
				if (ImGui::Button(node.TreeEntry[x].DisplayName.c_str()))
				{
					sceneIndexToLoad = node.TreeEntry[x].Stage;
					eventIndexToLoad = node.TreeEntry[x].EventID;
					DebugMenu::s_Visible = false;
					std::string temp = sceneIndexToLoad.c_str();
					if(node.TreeEntry[x].Type == Project::DebugStageTree::STreeNode::SNodeEntry::eNodeType_Cutscene)
					{
						System::StageManager::ConfigureNextEvent(sceneIndexToLoad.c_str(), eventIndexToLoad.c_str());
					}
					else
						System::StageManager::ConfigureNextStage(sceneIndexToLoad.c_str(), node.TreeEntry[x].IsNight ? System::SLoadInfo::WEREHOG : System::SLoadInfo::MODERN, false);
					System::StageManager::TriggerStageLoad();
					//if (!eventIndexToLoad.empty())
					//	SUC::System::StageManager::ForcePlayCutscene(eventIndexToLoad, sceneIndexToLoad, false, 0);
				}
				ImGui::EndGroup();
			}
			ImGui::Unindent(16);
			for (size_t i = 0; i < node.Children.size(); i++)
			{
				drawStageTreeNode(node.Children[i]);
			}
			ImGui::PopStyleVar();
			ImGui::TreePop();
		}
	}
	void DebugMenu::Draw()
	{
		const bool end = s_Visible;
		if (!parsedTree)
		{
			scaleFactor = hh::math::CVector2((ImGui::GetIO().DisplaySize.x / 1920), (ImGui::GetIO().DisplaySize.y / 1080));
			parsedTree = true;
		}
		ImGui::SetNextWindowBgAlpha(1);
		ImGui::SetNextWindowSize({ 1700 * scaleFactor.x(), 956 * scaleFactor.y() }, ImGuiCond_Always);
		ImGui::SetNextWindowPos({ 110 * scaleFactor.x(), 62 * scaleFactor.y() }, ImGuiCond_Always);
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		if (s_Visible && ImGui::Begin("Sonic Unleashed Conversion", &s_Visible, flags))
		{

			ImGui::TextUnformatted("Sonic Unleashed Conversion");
			ImGuiInputTextString("ModuleName", &m_Moduletest);
			if (ImGui::Button("Test module"))
			{

				System::SequenceHelpers::ChangeModuleByName(m_Moduletest.c_str());
			}
			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);

			if (ImGui::TreeNodeEx("StageArchives", ImGuiTreeNodeFlags_None))
			{

				for (size_t i = 0; i < SUC::Project::s_DebugStageTree.TreeNodes.size(); i++)
				{
					drawStageTreeNode(SUC::Project::s_DebugStageTree.TreeNodes[i]);
				}
				ImGui::Unindent(16);
				ImGui::TreePop();
			}
			ImGui::End();

		}
	}


}