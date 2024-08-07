
const Chao::CSD::CScene* sceneB;
bool IsAnimDone(Chao::CSD::CScene* scene)
{
	if (scene->m_MotionSpeed < 0.0f)
	{		
		if ((scene->m_MotionStartFrame + 0.001000000047497451f) >= scene->m_MotionFrame)
			return true;
	}
	else
	{
		float v9 = scene->m_MotionFrame;
		if (v9 >= (scene->m_MotionEndFrame - 0.001000000047497451f))
			return true;
	}
	return false;
}
//this may cause a very very slow memory leak.
void CSDCommon::CheckSceneAnimation(int i)
{
	if (i != -1)
		sceneB = scenesPlayingBack[i];
	if (!sceneB)
	{
		scenesPlayingBack.erase(scenesPlayingBack.begin() + i);
	}
	else
	{
		Common::ClampFloat(scenesPlayingBack[i]->m_MotionFrame, 0, scenesPlayingBack[i]->m_MotionEndFrame);

		if (IsAnimDone(scenesPlayingBack[i]))
		{
			FreezeMotion(*scenesPlayingBack[i], 0);
			scenesPlayingBack.erase(scenesPlayingBack.begin() + i);
		}
	}
}
std::vector<Chao::CSD::CScene*> CSDCommon::scenesPlayingBack; //idk why i need to do this, symbol errors-
void CSDCommon::Update()
{
	if (CSDCommon::scenesPlayingBack.size() != 0)
	{
		for (size_t i = 0; i < CSDCommon::scenesPlayingBack.size(); i +=3)
		{
			if (i < CSDCommon::scenesPlayingBack.size())
				CSDCommon::CheckSceneAnimation(i);
			if (i +1 < CSDCommon::scenesPlayingBack.size())
				CSDCommon::CheckSceneAnimation(i+1);
			if (i + 2< CSDCommon::scenesPlayingBack.size())
				CSDCommon::CheckSceneAnimation(i+2);
			if (i + 3 < CSDCommon::scenesPlayingBack.size())
				CSDCommon::CheckSceneAnimation(i+3);
			
		}
	}
}

void CSDCommon::SplitTextToSeparateCasts(Chao::CSD::CScene* scene, const char* formatCastName, const char* text, int maxCharacterPerLine, int maxLines, bool blankOutEmpty)
{
	const std::string input = std::string(text);
	std::stringstream ss(input);
	std::string line;
	std::vector<std::string> lines;
	for (std::string line; std::getline(ss, line);)
	{
		lines.push_back(line);
	}

	if (lines.empty())
		lines.emplace_back();

	char buff[32];
	for (size_t i = 0; i < maxLines; i++)
	{
		sprintf(buff, formatCastName, i);
		if (i <= lines.size() - 1)
		{
			scene->GetNode(buff)->SetText(lines.at(i).c_str());
		}
		else
		{
			if (blankOutEmpty)
			{
				scene->GetNode(buff)->SetText("");
			}
		}
	}
}
//void CSDCommon::initialize()
//{
//	INSTALL_HOOK(CheckScenesBack);
//}