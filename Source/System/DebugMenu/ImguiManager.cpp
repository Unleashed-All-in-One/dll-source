

bool ImguiManager::visible = false;
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
void customStageLoad()
{
	std::string temp = sceneIndexToLoad.c_str();
	//auto message = Sonic::Message::MsgSequenceEvent(2, 0);
	//auto message2 = Sonic::Message::MsgStorySequenceEvent(0, 0);
	//auto test = Sonic::Sequence::Main::GetInstance();
	//void __thiscall StorySeqProcessStorySequenceEvent(int storySequence, CMsgStorySequenceEvent *storySequenceEvent)
	//SequenceHelpers::resetStorySequence();

	//FUNCTION_PTR(void, __thiscall, StorySeqProcessStorySequenceEvent, 0x00D76980, Sonic::Sequence::Story * StorySeq, Sonic::Message::MsgStorySequenceEvent * a2);
	//auto message2 = Sonic::Message::MsgStorySequenceEvent(1, 1);
	//StorySeqProcessStorySequenceEvent(Sonic::Sequence::Story::GetInstance(), &message2);
	//Sonic::Sequence::Main::ProcessMessage(&message);
	SequenceHelpers::loadStage(_strdup(temp.c_str()), 0, false);
	//uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	//char** h = (char**)stageTerrainAddress;
	//const char* terr = *h;
	//*h = _strdup(temp.c_str()); // Use strdup to duplicate the string
	StageManager::nextStageID = temp;
}
void drawStageTreeNode(SUC::Project::DebugStageTree::DebugStageTreeNode node)
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::TreeNodeEx(node.name.c_str(), ImGuiTreeNodeFlags_None))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0, 0 });
		ImGui::Indent(16);
		for (size_t x = 0; x < node.treeEntries.size(); x++)
		{
			ImGui::BeginGroup();
			ImGui::Text("X");
			ImGui::SameLine();
			if (ImGui::Button(node.treeEntries[x].displayName.c_str()))
			{
				sceneIndexToLoad = node.treeEntries[x].stage;
				eventIndexToLoad = node.treeEntries[x].cutsceneID;
				ImguiManager::visible = false;
				customStageLoad();
				if (!eventIndexToLoad.empty())
					StageManager::forcePlayCutscene(eventIndexToLoad, sceneIndexToLoad, false, 0);
			}
			ImGui::EndGroup();
		}
		ImGui::Unindent(16);
		for (size_t i = 0; i < node.children.size(); i++)
		{
			drawStageTreeNode(node.children[i]);
		}
		ImGui::PopStyleVar();
		ImGui::TreePop();
	}
}
void ImguiManager::update()
{
	const bool end = visible;
	if (!parsedTree)
	{
		scaleFactor = hh::math::CVector2((ImGui::GetIO().DisplaySize.x / 1920), (ImGui::GetIO().DisplaySize.y / 1080));
		parsedTree = true;
		SUC::Project::GetDebugTree();
	}
	ImGui::SetNextWindowBgAlpha(1);
	ImGui::SetNextWindowSize({ 1700 * scaleFactor.x(), 956 * scaleFactor.y()}, ImGuiCond_Always);
	ImGui::SetNextWindowPos({ 110 * scaleFactor.x(), 62 * scaleFactor.y() }, ImGuiCond_Always);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	if (visible && ImGui::Begin("Sonic Unleashed Conversion", &visible, flags))
	{
		
        ImGui::TextUnformatted("Sonic Unleashed Conversion");
        ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
        
		if (ImGui::TreeNodeEx("StageArchives", ImGuiTreeNodeFlags_None))
		{
			
			for (size_t i = 0; i < SUC::Project::s_DebugStageTree.treeNodes.size(); i++)
			{
				drawStageTreeNode(SUC::Project::s_DebugStageTree.treeNodes[i]);
			}
			ImGui::Unindent(16);
			ImGui::TreePop();
		}
        ImGui::End();
		
	}
}
