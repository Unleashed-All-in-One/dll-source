
#include "WerehogMenu.h"

#include "../../Player/Evil/EvilGlobal.h"

namespace SUC::ImGuiMenu
{
	bool WerehogMenu::s_Visible = false;
	
	void WerehogMenu::Draw()
	{
		ImGui::SetNextWindowBgAlpha(1);
		if (s_Visible && ImGui::Begin("Werehog", &s_Visible))
		{
			auto e = Player::Evil::EvilGlobal::GetMotionFromName(Player::Evil::EvilGlobal::s_LatestAttackName);
			s_MotionDisplay = &e;
			if(s_MotionDisplay == nullptr)
			{
				ImGui::End();
				return;
			}
			ImGui::TextUnformatted(SUC::Format("Current attack: %s", s_MotionDisplay->FileName.c_str()));
			ImGui::TextUnformatted(SUC::Format("Move Type: %s", s_MotionDisplay->MoveType.c_str()));
			ImGui::InputFloat3("Applied Velocity", s_AppliedVelocity.data());
			ImGui::Separator();																 
			ImGui::TextUnformatted(SUC::Format("MotionMoveSpeedRatio: %f", s_MotionDisplay->MotionMoveSpeedRatio));
			ImGui::TextUnformatted(SUC::Format("MotionMoveSpeedRatio_Y: %f", s_MotionDisplay->MotionMoveSpeedRatio_Y));
			ImGui::TextUnformatted(SUC::Format("MotionFirstSpeed: %f", s_MotionDisplay->MotionFirstSpeed));
			ImGui::TextUnformatted(SUC::Format("MiddleSpeed: %f", s_MotionDisplay->MiddleSpeed));
			ImGui::TextUnformatted(SUC::Format("EndSpeed: %f", s_MotionDisplay->EndSpeed));
			ImGui::TextUnformatted(SUC::Format("MotionSpeed_FirstFrame: %f", s_MotionDisplay->MotionSpeed_FirstFrame));
			ImGui::TextUnformatted(SUC::Format("MotionSpeed_MiddleFrame: %f", s_MotionDisplay->MotionSpeed_MiddleFrame));
			ImGui::TextUnformatted(SUC::Format("MotionSpeed_EndFrame: %f", s_MotionDisplay->MotionSpeed_EndFrame));
			ImGui::Separator();
			if(ImGui::CollapsingHeader("MotionMoveSpeedRatio_H"))
			{
				for (auto ratioHelper : s_MotionDisplay->MotionMoveSpeedRatio_H)
				{
					ImGui::Indent(16);
					ImGui::TextUnformatted(SUC::Format("MoveSpeedRatio_Start: %f", ratioHelper.FrameStart));
					ImGui::TextUnformatted(SUC::Format("MoveSpeedRatio_Value: %f", ratioHelper.FrameValue));
					ImGui::TextUnformatted(SUC::Format("MoveSpeedRatio_ValueY: %f", ratioHelper.FrameValue_Y));
					ImGui::Unindent(16);
				}				
			}
			if (ImGui::CollapsingHeader("MotionMoveSpeedRatio_H_Y"))
			{
				for (auto ratioHelper : s_MotionDisplay->MotionMoveSpeedRatio_H_Y)
				{
					ImGui::Indent(16);
					ImGui::TextUnformatted(SUC::Format("MoveSpeedRatio_Start: %f", ratioHelper.FrameStart));
					ImGui::TextUnformatted(SUC::Format("MoveSpeedRatio_Value: %f", ratioHelper.FrameValue));
					ImGui::TextUnformatted(SUC::Format("MoveSpeedRatio_ValueY: %f", ratioHelper.FrameValue_Y));
					ImGui::Unindent(16);
				}
			}
		
			ImGui::End();
		}
	}


}
