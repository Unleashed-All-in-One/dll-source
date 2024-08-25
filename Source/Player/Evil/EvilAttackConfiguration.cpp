#include "EvilAttackConfiguration.h"

namespace SUC::Player::Evil
{
	bool m_Initialized;
	bool Is(const char* w1, const char* w2)
	{
		int i = 0;
		int j = 0;


		while (w1[i] != '\0') {
			if (w1[i] == w2[j])
			{
				int init = i;
				while (w1[i] == w2[j] && w2[j] != '\0')
				{
					j++;
					i++;
				}
				if (w2[j] == '\0') {
					return true;
				}
				j = 0;
			}
			i++;
		}
		return false;
	}
	//bool AssignFloatIfCorrect(const float& in_ValueToSet, const char* w1, rapidxml::xml_node<>* in_Node)
	//{
	//	std::string sW1 = std::string(w1);
	//	std::string sW2 = std::string(in_Node->name());
	//	if(sw1)
	//	//int i = 0;
	//	//int j = 0;
	//	//while (w1[i] != '\0') {
	//	//	if (w1[i] == w2[j])
	//	//	{
	//	//		int init = i;
	//	//		while (w1[i] == w2[j] && w2[j] != '\0')
	//	//		{
	//	//			j++;
	//	//			i++;
	//	//		}
	//	//		if (w2[j] == '\0') {
	//	//			return true;
	//	//		}
	//	//		j = 0;
	//	//	}
	//	//	i++;
	//	//}
	//	//return false;
	//}
	std::string toCheck;
	std::string alreadyChecked;
	std::vector<Motion> EvilAttackConfiguration::s_AnimationTable;
	std::vector<std::string> alreadyRegistered;
	std::vector<Motion> localMotionFileList;
	std::vector<WerehogAttackNew> EvilAttackConfiguration::s_Attacks;
	std::vector<WerehogAttackNew> EvilAttackConfiguration::s_StartAttacks;
	////RHand,
	//LHand,
	//RLeg,
	//LLeg,
	//MiddleHand,
	//Head,
	//Hips,
	//MiddleLeg
	std::vector<std::string> collisionBoneNames = { "None","RHand", "LHand", "RLeg", "LLeg", "MiddleHand", "Head", "Hips", "MiddleLeg" };
	WerehogAttackNew ParseActionNode(rapidxml::xml_node<>* node, rapidxml::xml_node<>* parent)
	{
		WerehogAttackNew attack = WerehogAttackNew();
		for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling())
		{
			const char* name = child->name();
			if (Is(name, "ActionName"))				attack.ActionName = std::string(child->value());
			if (Is(name, "MotionName"))				attack.MotionName = std::string(child->value());
			if (Is(name, "ValidLevel_Min"))			attack.ValidLevel_Min = reinterpret_cast<int>(child->value());
			if (Is(name, "ValidLevel_Max"))			attack.ValidLevel_Max = reinterpret_cast<int>(child->value());
			if (Is(name, "ValidCommon"))			attack.ValidCommon = std::string(child->value()) == "true";
			if (Is(name, "ValidBerserker"))			attack.ValidBerserker = std::string(child->value()) == "true";
			if (Is(name, "KEY__YDown"))				attack.KEY__YDown = std::string(child->value());
			if (Is(name, "KEY__XDown"))				attack.KEY__XDown = std::string(child->value());
			if (Is(name, "KEY__ADown"))				attack.KEY__ADown = std::string(child->value());
			if (Is(name, "KEY__Land"))				attack.KEY__Land = std::string(child->value());
			if (Is(name, "KEY__AirCombo"))			attack.KEY__AirCombo = std::string(child->value());
			if (Is(name, "KEY__End"))				attack.KEY__End = std::string(child->value());
			if (Is(name, "Guard"))					attack.Guard = std::string(child->value()) == "true";
			if (Is(name, "Avoid"))					attack.Avoid = std::string(child->value()) == "true";
			if (Is(name, "KEY__StartFrame"))		attack.KEY__StartFrame = reinterpret_cast<int>(child->value());
			if (Is(name, "KEY__EndFrame"))			attack.KEY__EndFrame = reinterpret_cast<int>(child->value());
			if (Is(name, "WaitEndMotionEndFrame"))	attack.WaitEndMotionEndFrame = reinterpret_cast<int>(child->value());
			if (Is(name, "WaitEndMotionSpeed"))		attack.WaitEndMotionSpeed = reinterpret_cast<int>(child->value());
			if (Is(name, "EndMotionSpeed"))			attack.EndMotionSpeed = reinterpret_cast<int>(child->value());
			if (Is(name, "LandStartFrame"))			attack.LandStartFrame = reinterpret_cast<int>(child->value());
			if (Is(name, "ActionValidHeightMin"))	attack.ActionValidHeightMin = reinterpret_cast<int>(child->value());
			if (Is(name, "IsGravity"))				attack.IsGravity = std::string(child->value()) == "true";
		}
		return attack;
	}

	Motion ParseMotionNode(rapidxml::xml_node<>* in_MotionNode)
	{
		Motion returned = Motion();
		returned.MotionMoveSpeedRatio_H = std::vector<MoveRatioHelper>(0);
		returned.Collision.BoneInfo = std::vector<CollisionParam>(12);

		returned.MotionName				= in_MotionNode->first_node("MotionName")->value();
		returned.FileName				= in_MotionNode->first_node("FileName")->value();
		returned.MotionBlendTimeS		= std::stof(in_MotionNode->first_node("MotionBlendTimeS")->value());
		returned.MotionBlendTimeE		= std::stof(in_MotionNode->first_node("MotionBlendTimeE")->value());
		returned.MotionBlendTimeEIdle   = std::stof(in_MotionNode->first_node("MotionBlendTimeEIdle")->value());
		returned.MotionMoveSpeedRatio   = std::stof(in_MotionNode->first_node("MotionMoveSpeedRatio")->value());
		returned.MotionMoveSpeedRatio_Y = std::stof(in_MotionNode->first_node("MotionMoveSpeedRatio_Y")->value());

		returned.MotionFirstSpeed = std::stof(in_MotionNode->first_node("MotionFirstSpeed")->value());
		returned.MiddleSpeed = std::stof(in_MotionNode->first_node("MiddleSpeed")->value());
		returned.EndSpeed = std::stof(in_MotionNode->first_node("EndSpeed")->value());
		returned.IsGravity = std::string(in_MotionNode->first_node("IsGravity")->value()) == "true";
		returned.MotionSpeed_FirstFrame = std::stof(in_MotionNode->first_node("MotionSpeed_FirstFrame")->value());
		returned.MotionSpeed_MiddleFrame = std::stof(in_MotionNode->first_node("MotionSpeed_MiddleFrame")->value());

		returned.MoveType = in_MotionNode->first_node("MoveType")->value();
		//Maximum is 21 since in Unleashed the maximum is 21, for whatever reason
		for (size_t i = 0; i < 20; i++)
		{
			int m_IndexOffset = i + 1;

			auto m_NodeFrameStart = in_MotionNode->first_node(SUC::Format("MotionMoveSpeedRatioFrameStart_%d", m_IndexOffset));
			auto m_NodeFrame = in_MotionNode->first_node(std::format("MotionMoveSpeedRatioFrame_{0}", m_IndexOffset).c_str());
			auto m_NodeFrameY = in_MotionNode->first_node(std::format("MotionMoveSpeedRatioFrameY_{0}", m_IndexOffset).c_str());

			//This is a custom struct, it stores the StartFrame, the value, and the Y value for every MotionMoveSpeedRatio
			if (!m_NodeFrameStart && !m_NodeFrame && !m_NodeFrameY)
				break;
			if (returned.MotionMoveSpeedRatio_H.size() <= i) returned.MotionMoveSpeedRatio_H.push_back({});

			if (m_NodeFrameStart) returned.MotionMoveSpeedRatio_H.at(i).FrameStart = std::stof(m_NodeFrameStart->value());
			if (m_NodeFrame)      returned.MotionMoveSpeedRatio_H.at(i).FrameValue = std::stof(m_NodeFrame->value());
			if (m_NodeFrameY)     returned.MotionMoveSpeedRatio_H.at(i).FrameValue_Y = std::stof(m_NodeFrameY->value());
		}
		
		if(in_MotionNode->first_node("Effect"))
		{
			auto parentNode = in_MotionNode->first_node("Effect");
			auto m_NodeREffectName = parentNode->first_node("REffect_Name1");
			auto m_NodeLEffectName = parentNode->first_node("LEffect_Name1");
			auto m_NodeRClawStart1 = parentNode->first_node("RClaw_Start1");
			auto m_NodeLClawStart1 = parentNode->first_node("LClaw_Start1");

			//Effect to play on right hand
			if (m_NodeREffectName)
				returned.Effect.REffect_Name1 = m_NodeREffectName->value();

			//Effect to play on left hand
			if (m_NodeLEffectName)
				returned.Effect.LEffect_Name1 = m_NodeLEffectName->value();

			//Time to start ClawStrip on right hand
			if (m_NodeRClawStart1)
				returned.Effect.REffect_Name1 = PARTICLE_CLAW;

			//Time to start ClawStrip on left hand
			if (m_NodeLClawStart1)
				returned.Effect.LEffect_Name1 = PARTICLE_CLAW;
		}
		//Get collision data for attack hitboxes
		if (in_MotionNode->first_node("Collision"))
		{
			auto m_CollisionNode = in_MotionNode->first_node("Collision");
			for (int i = 1; i < 13; ++i)
			{
				int adjustedIndex = i - 1;
				//Size of hitbox
				auto m_Size = m_CollisionNode->first_node(std::format("Size{0}", i).c_str());
				//Unknown, I guess it could be additional over time
				auto m_SizeDelta = m_CollisionNode->first_node(std::format("SizeDelta{0}", i).c_str());
				//Loop through all possible bones with an index, as nodes are structured as follows
				//RHandStart1, MiddleLegEnd13
				for (size_t x = 0; x < collisionBoneNames.size(); x++)
				{
					//Start and end frame for bone collision
					auto m_NodeStart = m_CollisionNode->first_node(std::format("{0}Start{1}", collisionBoneNames[x], i).c_str());
					auto m_NodeEnd = m_CollisionNode->first_node(std::format("{0}End{1}", collisionBoneNames[x], i).c_str());

					//Cant have hitboxes without the above, so ignore if they dont exist.
					if(m_NodeStart && m_NodeEnd)
					{
						returned.Collision.BoneInfo[adjustedIndex].BoneType = static_cast<CollisionBoneType>(x);

						returned.Collision.BoneInfo[adjustedIndex].Size = std::stof(m_Size->value());
						returned.Collision.BoneInfo[adjustedIndex].SizeDelta = std::stof(m_SizeDelta->value());

						returned.Collision.BoneInfo[adjustedIndex].StartFrame = std::stof(m_NodeStart->value());						
						returned.Collision.BoneInfo[adjustedIndex].EndFrame = std::stof(m_NodeEnd->value());
					}
				}
			}			
		}
		return returned;
	}
	void RegisterAnims(std::vector<Motion>& vec, rapidxml::xml_node<>* nodeMotion)
	{
		/// Fun fact!
		/// Some attacks cannot be converted to the simple evilsonic_attack(name) format, because some have custom
		/// attack names but not custom animation names.
		/// At the same time, not every attack is in the MotionFile
		/// so you have to do both at the same time!!!!
		for (rapidxml::xml_node<>* child = nodeMotion->first_node(); child; child = child->next_sibling())
		{
			auto motionName = child->first_node();
			auto fileName = child->last_node();
			Motion m = Motion();
			m.MotionName = motionName->value();
			m.FileName = fileName->value();
			localMotionFileList.push_back(m);
		}
		for (size_t i = 0; i < vec.size(); i++)
		{
			auto attack = vec[i];
			bool foundInMotionList = false;
			for (size_t i = 0; i < localMotionFileList.size(); i++)
			{
				if (localMotionFileList[i].MotionName == attack.MotionName)
				{
					foundInMotionList = true;
					attack.FileName = localMotionFileList[i].FileName;
					break;
				}
				if (localMotionFileList[i].FileName == attack.MotionName)
				{
					foundInMotionList = true;
					attack.FileName = localMotionFileList[i].FileName;
					break;
				}

			}
			std::string file = attack.FileName;
			if (!foundInMotionList)
			{
				std::string file = std::format("evilsonic_attack{0}", attack.FileName);
				std::string fileU = std::format("Evilsonic_attack{0}", attack.FileName);
				SUC::System::AnimationSetPatcher::RegisterClassicAnimation(fileU, file);
				vec[i].FileName = file;
			}
			else
			{
				vec[i].FileName = attack.FileName;
				SUC::System::AnimationSetPatcher::RegisterClassicAnimation(file, attack.FileName, 1, false);
			}
			if (std::find(alreadyRegistered.begin(), alreadyRegistered.end(), attack.MotionName) == alreadyRegistered.end())
			{
				alreadyRegistered.push_back(vec[i].MotionName);
			}

		}
	}
	boost::shared_ptr<hh::db::CDatabase> database;

	void RegisterResources(const char* path)
	{
		//std::string modDir = std::string(path);
		//size_t pos = modDir.find_last_of("\\/");
		//if (pos != std::string::npos)
		//{
		//	modDir.erase(pos + 1);
		//}
		for (size_t i = 0; i < EvilAttackConfiguration::s_AnimationTable.size(); i++)
		{
			int attackPos = i;
			auto fileName = _strdup(std::format("{0}.tbres.xml", EvilAttackConfiguration::s_AnimationTable.at(i).FileName).c_str());
			auto m_TbResource = database->GetRawData(fileName);

			if (m_TbResource == nullptr)
			{
				SUC::LogMessage("Error while parsing attack file");
				continue;
			}

			unsigned char* m_TbResourceData = m_TbResource->m_spData.get();
			std::string m_Action(m_TbResourceData, m_TbResourceData + m_TbResource->m_DataSize / sizeof m_TbResourceData[0]);
			/*std::string filepath = std::format("{0}attack/{1}.tbres.xml", modDir, EvilAttackConfiguration::s_AnimationTable.at(i).FileName);
			FILE* file = fopen(filepath.c_str(), "rb");
			if (file == nullptr)
				continue;
			fseek(file, 0, SEEK_END);
			long fileSize = ftell(file);
			fseek(file, 0, SEEK_SET);*/

			char* cstr2 = new char[m_Action.size() + 1];
			strcpy(cstr2, m_Action.c_str());
			rapidxml::xml_document<> doc;
			doc.parse<0>(cstr2);
			auto resourceNode = doc.first_node();
			int id = 0;
			printf(std::format("\nParsing {0}...", EvilAttackConfiguration::s_AnimationTable.at(i).FileName).c_str());
			//Add trigger functionality too once everything else is done
			for (rapidxml::xml_node<>* TriggerBinderResource_Child = resourceNode->first_node(); TriggerBinderResource_Child; TriggerBinderResource_Child = TriggerBinderResource_Child->next_sibling())
			{
				if (Is(TriggerBinderResource_Child->name(), "ResourceInfo"))
				{
					auto resource = TriggerBinderResource_Child->first_node();
					Resource res;

					for (rapidxml::xml_node<>* ResourceInfo_Child = resource->first_node(); ResourceInfo_Child; ResourceInfo_Child = ResourceInfo_Child->next_sibling())
					{
						auto name = ResourceInfo_Child->name();
						if (Is(name, "ID"))
							res.ID = id;
						if (Is(name, "Type"))
							res.Type = Is(ResourceInfo_Child->value(), "CSB") ? ResourceType::CSB : ResourceType::Effect;
						if (Is(name, "Param"))
						{
							Param p;
							p.FileName = ResourceInfo_Child->first_node()->value();
							if (ResourceInfo_Child->first_node()->next_sibling() != nullptr)
								p.Cue = ResourceInfo_Child->first_node()->next_sibling()->value();
							res.Params = p;

						}
					}
					EvilAttackConfiguration::s_AnimationTable.at(attackPos).ResourceInfos.Resources.push_back(res);
				}
				else if (Is(TriggerBinderResource_Child->name(), "TriggerInfo"))
				{
					int indexChildTemp = 0;
					for (rapidxml::xml_node<>* TriggerInfo_Child = TriggerBinderResource_Child->first_node(); TriggerInfo_Child; TriggerInfo_Child = TriggerInfo_Child->next_sibling())
					{
						indexChildTemp++;
						Trigger trig;
						for (rapidxml::xml_node<>* TriggerInfo_Trigger_Child = TriggerInfo_Child->first_node(); TriggerInfo_Trigger_Child; TriggerInfo_Trigger_Child = TriggerInfo_Trigger_Child->next_sibling())
						{
							auto name = TriggerInfo_Trigger_Child->name();
							if (Is(name, "ResourceID"))
							{
								trig.ResourceID = std::stoi(TriggerInfo_Trigger_Child->value());
								continue;
							}
							if (Is(name, "Frame"))
							{
								for (rapidxml::xml_node<>* child3 = TriggerInfo_Trigger_Child->first_node(); child3; child3 = child3->next_sibling())
								{
									auto name2 = child3->name();
									if (Is(name2, "Type"))
									{
										trig.Frame.Type = std::stoi(child3->value());
										continue;
									}
									if (Is(name2, "Start"))
									{
										trig.Frame.Start = std::stoi(child3->value());
										continue;
									}
									if (Is(name2, "End"))
									{
										trig.Frame.End = std::stoi(child3->value());
										continue;
									}
								}
							}
							if (Is(name, "IsFollowNode"))
							{
								trig.IsFollowNode = TriggerInfo_Trigger_Child->value() == "true";
								continue;
							}
							if (Is(name, "IsInheritPositionOnly"))
							{
								trig.IsInheritPositionOnly = TriggerInfo_Trigger_Child->value() == "true";
								continue;
							}
							if (Is(name, "NodeName"))
							{
								trig.NodeName = TriggerInfo_Trigger_Child->value();
								continue;
							}
						}
						EvilAttackConfiguration::s_AnimationTable.at(attackPos).TriggerInfos.Resources.push_back(trig);
					}
				}
				id++;
			}
		}
	}
	void ParseAttackAction_Motion(std::string& in_AttackAction1, std::string& in_AttackMotion)
	{
		char* cstr = new char[in_AttackAction1.size() + 1];
		strcpy(cstr, in_AttackAction1.c_str());
		char* cstr2 = new char[in_AttackMotion.size() + 1];
		strcpy(cstr2, in_AttackMotion.c_str());
		rapidxml::xml_document<> m_ActionXml;
		rapidxml::xml_document<> m_MotionXml;
		in_AttackAction1.push_back('\0');
		in_AttackMotion.push_back('\0');


		m_ActionXml.parse<0>(cstr);
		m_MotionXml.parse<0>(cstr2);

		auto motionNode = m_ActionXml.first_node()->first_node()->next_sibling()->next_sibling();
		for (rapidxml::xml_node<>* child = motionNode->first_node(); child; child = child->next_sibling())
		{
			EvilAttackConfiguration::s_AnimationTable.push_back(ParseMotionNode(child));
		}
		auto actionNode = m_ActionXml.first_node()->last_node();
		for (rapidxml::xml_node<>* child = actionNode->first_node(); child; child = child->next_sibling())
		{
			auto attack = ParseActionNode(child, actionNode);
			if (attack.ActionName.find("Start_") != std::string::npos)
				EvilAttackConfiguration::s_StartAttacks.push_back(attack);
			else
				EvilAttackConfiguration::s_Attacks.push_back(attack);
		}
		auto motionStartpoint = m_MotionXml.first_node()->first_node();
		RegisterAnims(EvilAttackConfiguration::s_AnimationTable, motionStartpoint);

		RegisterResources(nullptr);
	}
	void Test()
	{
		m_Initialized = true;
		//auto m_spAttackActionXml = database->GetRawData("EvilAttackAction1.xml");
		//auto m_spAttackMotionXml = database->GetRawData("EvilAttackMotionFile.xml");
		//
		//unsigned char* m_ActionData = m_spAttackActionXml->m_spData.get();
		//unsigned char* m_MotionData = m_spAttackMotionXml->m_spData.get();
		//
		//std::string m_Action(m_ActionData, m_ActionData + m_spAttackActionXml->m_DataSize / sizeof m_ActionData[0]);
		//std::string m_Motion(m_MotionData, m_MotionData + m_spAttackMotionXml->m_DataSize / sizeof m_MotionData[0]);
		////There seems to be garbage in the first 3 letters
		//m_Action.erase(0, 3);
		//m_Motion.erase(0, 3);
		//

		RegisterResources(nullptr);
		printf("\nParsing Complete");
	}
	HOOK(boost::shared_ptr<void>, __stdcall, sub_D0B4F0, 0xD0B4F0, void* This)
	{
		auto retval = originalsub_D0B4F0(This);
		if (!m_Initialized)
		{
			database = hh::db::CDatabase::CreateDatabase();
			auto& databaseLoader = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spDatabaseLoader;
			Hedgehog::Database::SArchiveParam param = Hedgehog::Database::SArchiveParam(100, 0);
			param.AfterArchiveLoadCallback = Test;
			databaseLoader->CreateArchiveList("#EvilSonic.ar", "#EvilSonic.arl", { 100, 0 });
			databaseLoader->LoadArchiveList(database, "#EvilSonic.arl");
			databaseLoader->LoadArchive(database, "#EvilSonic.ar", param, false, true);
		}
		return retval;
	}
	//0x00D00EE0
	HOOK(void*, __fastcall, CGameplayFlowStageActStart, 0x00D00EE0,void* This, void* Edx)
	{
		if(!m_Initialized)
		{
			database = hh::db::CDatabase::CreateDatabase();
			auto& databaseLoader = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spDatabaseLoader;
			Hedgehog::Database::SArchiveParam param = Hedgehog::Database::SArchiveParam(100, 0);
			param.AfterArchiveLoadCallback = Test;
			databaseLoader->CreateArchiveList("#EvilSonic.ar", "#EvilSonic.arl", { 100, 0 });
			databaseLoader->LoadArchiveList(database, "#EvilSonic.arl");
			databaseLoader->LoadArchive(database, "#EvilSonic.ar", param, false, true);
		}
		return originalCGameplayFlowStageActStart(This, Edx);
	}
	void EvilAttackConfiguration::RegisterHooks(const char* path)
	{
		INSTALL_HOOK(CGameplayFlowStageActStart);
		std::ifstream myfile("EvilAttackAction1.xml");
		std::ifstream myfile2("EvilAttackMotionFile.xml");
		rapidxml::xml_document<> doc;
		rapidxml::xml_document<> doc2;

		std::vector<char> buffer((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());
		std::vector<char> buffer2((std::istreambuf_iterator<char>(myfile2)), std::istreambuf_iterator<char>());

		buffer.push_back('\0');
		buffer2.push_back('\0');


		doc.parse<0>(&buffer[0]);
		doc2.parse<0>(&buffer2[0]);

		auto motionNode = doc.first_node()->first_node()->next_sibling()->next_sibling();
		for (rapidxml::xml_node<>* child = motionNode->first_node(); child; child = child->next_sibling())
		{
			s_AnimationTable.push_back(ParseMotionNode(child));
		}
		auto actionNode = doc.first_node()->last_node();
		for (rapidxml::xml_node<>* child = actionNode->first_node(); child; child = child->next_sibling())
		{
			auto attack = ParseActionNode(child, actionNode);
			if (attack.ActionName.find("Start_") != std::string::npos)
				s_StartAttacks.push_back(attack);
			else
				s_Attacks.push_back(attack);
		}
		auto motionStartpoint = doc2.first_node()->first_node();
		RegisterAnims(s_AnimationTable, motionStartpoint);

		printf("\nParsing Complete 1/2");
		
		
		

		
			
	}
}
