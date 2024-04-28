

bool ImguiManager::visible = true;
std::string sceneIndexToLoad;
std::string eventIndexToLoad;
int playerID;
const float UI_INDENT = 16;
void ImGuiInputTextString(const char* label, std::string& str, ImGuiInputTextFlags flags = 0) 
{
	char buffer[256];
	strncpy(buffer, str.c_str(), sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';

	if (ImGui::InputText(label, buffer, sizeof(buffer), flags)) {
		str = buffer;
	}
}
void ImguiManager::update()
{
	const bool end = visible;

	ImGui::SetNextWindowBgAlpha(1);
	if (visible && ImGui::Begin("Sonic Unleashed Conversion", &visible, ImGuiWindowFlags_MenuBar))
	{
		ImGuiInputTextString("Scene ID", sceneIndexToLoad);
		ImGuiInputTextString("Event ID", eventIndexToLoad);
		ImGui::InputInt("Player ID (0: GENERIC, 1: EVIL)", &playerID);
		if (ImGui::Button("Load as stage"))
		{
			SequenceHelpers::loadStage(sceneIndexToLoad.c_str());
			SequenceHelpers::setPlayerType(PlayerType::CLASSIC_SONIC);
		}
		if(ImGui::Button("Load as cutscene"))
		{
			SequenceHelpers::loadStage(sceneIndexToLoad.c_str());
			StageManager::forcePlayCutscene(eventIndexToLoad, sceneIndexToLoad, false, 0);
		}
		
	}
	if (end)
		ImGui::End();
}