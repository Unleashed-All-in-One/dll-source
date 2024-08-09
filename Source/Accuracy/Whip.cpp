#include "Whip.h"
namespace SUC::Accuracy
{
    float m_LastTimeAnim;
    bool m_ExecutingStartAnim;
    bool m_ExecutingResultAnim;
    std::string m_RankAnim, m_RankUVAnim;
    boost::shared_ptr< Hedgehog::Database::CDatabase> m_Database;
	boost::shared_ptr<hh::mr::CModelData> m_ModelData;


    bool Whip::SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase)
    {
        const char* assetName = "WhipRoot";
        ObjectUtility::CreateModel(assetName, in_spDatabase, m_spSpawnedModel);
        hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
        boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
        m_spSpawnedModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
        m_AnimatorPose = boost::make_shared<Hedgehog::Animation::CAnimationPose>(in_spDatabase, assetName);
        auto animations = std::vector<SUC::NewAnimationData>();
        animations.push_back(NewAnimationData("StartD", "whip_start_sonicD", 1, false, nullptr));
        animations.push_back(NewAnimationData("StartI", "whip_start_sonicI", 1, false, nullptr));
        animations.push_back(NewAnimationData("StartEvil", "whip_start_evil", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultGenericS", "whip_result_sonic_S", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultGenericA", "whip_result_sonic_A", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultGenericB", "whip_result_sonic_B", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultGenericC", "whip_result_sonic_C", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultGenericD", "whip_result_sonic_D", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultGenericE", "whip_result_sonic_E", 1, false, nullptr));

        animations.push_back(NewAnimationData("ResultEvilS", "whip_result_evil_S", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultEvilA", "whip_result_evil_A", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultEvilB", "whip_result_evil_B", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultEvilC", "whip_result_evil_C", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultEvilD", "whip_result_evil_D", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultEvilE", "whip_result_evil_E", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultLook", "whip_result_evil_E", 1, false, nullptr));
        animations.push_back(NewAnimationData("Look", "whip_result_look", 1, false, nullptr));
        animations.push_back(NewAnimationData("ResultS", "whip_resultS", 1, false, nullptr));

        this->SetContext(this); //why?
        ObjectUtility::RegisterAnimations(m_AnimatorPose, animations, m_spSpawnedModel, this);
        m_spEffectMotionAll = boost::make_shared<Hedgehog::Motion::CSingleElementEffectMotionAll>();
        m_spSpawnedModel->BindMatrixNode(m_spMatrixNodeTransform);
        m_spSpawnedModel->BindEffect(m_spEffectMotionAll);
        
        Sonic::CGameObject::AddRenderable("Object", m_spSpawnedModel, true);
        m_Database = in_spDatabase;
        m_ModelData = spModelData;
        m_spMatrixNodeTransform->m_Transform.SetPosition(s_SpawnPosition);
        m_spMatrixNodeTransform->m_Transform.SetRotation(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_spMatrixNode->m_Transform.m_Rotation);
        this->ChangeState(s_StartMode == Stand ? "StartI" : "StartD");
        m_ExecutingStartAnim = true;
        ObjectUtility::SetTransformScale(m_spMatrixNodeTransform->m_Transform, Hedgehog::Math::CVector(1, 1, 1));
        return true;
    }
    bool Whip::ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag)
    {
        if (in_Flag)
        {
            if(in_rMsg.IsOfType("MsgRestart"))
            {
                m_spMatrixNodeTransform->m_Transform.SetPosition(s_SpawnPosition);
                m_spMatrixNodeTransform->m_Transform.SetRotation(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_spMatrixNode->m_Transform.m_Rotation);
                ChangeState(s_StartMode == Stand ? "StartI" : "StartD");
                m_ExecutingStartAnim = true;
                m_ExecutingResultAnim = false;
                m_LastTimeAnim = 0;
                return true;
            }
        }
        return CObjectBase::ProcessMessage(in_rMsg, in_Flag);
    };

    bool Whip::SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase)
    {
        return true;
    }
    Hedgehog::Animation::CAnimationPose* Whip::GetAnimationPose()
    {
        return m_AnimatorPose.get();
    }
    Hedgehog::Math::CVector Whip::GetVelocityForAnimationSpeed()
    {
        return Hedgehog::Math::CVector(1.0f, 1.0f, 1.0f);
    }
    Hedgehog::Math::CVector Whip::GetVelocityForAnimationChange()
    {
        return Hedgehog::Math::CVector(1.0f, 1.0f, 1.0f);
    }
    void Whip::UpdateParallel(const Hedgehog::Universe::SUpdateInfo& in_rUpdateInfo)
	{
		m_AnimatorPose->Update(in_rUpdateInfo.DeltaTime);
        FUNCTION_PTR(void, __thiscall, fpUpdateMotionAll, 0x752F00, Hedgehog::Motion::CSingleElementEffectMotionAll * This, float dt);

        fpUpdateMotionAll(m_spEffectMotionAll.get(), in_rUpdateInfo.DeltaTime);
		float m_AnimTime = ObjectUtility::GetAnimTime(this);
		DebugDrawText::log(SUC::Format("CHIP: %.3f", m_AnimTime), 0);
        if(m_ExecutingStartAnim)
        {
	        if(m_LastTimeAnim >= m_AnimTime)
	        {
                m_ExecutingStartAnim = false;
                this->SetVisible(false);
	        }
        }
        if(m_ExecutingResultAnim)
        {
            m_spMatrixNodeTransform->m_Transform.SetRotation(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_spMatrixNode->m_Transform.m_Rotation);
        	m_spMatrixNodeTransform->m_Transform.SetPosition(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_spMatrixNode->m_Transform.m_Position);
            m_spMatrixNodeTransform->NotifyChanged();
            
            if (m_AnimTime >= 3) // 100 frames
            {
                ObjectUtility::AssignUVAnimation(SUC::Format("%s_eyeL", m_RankUVAnim.c_str()), m_Database, m_ModelData, Whip::m_spEffectMotionAll);
                ObjectUtility::AssignUVAnimation(SUC::Format("%s_eyeR", m_RankUVAnim.c_str()), m_Database, m_ModelData, Whip::m_spEffectMotionAll);
                m_ExecutingResultAnim = false;
                ChangeState(m_RankAnim.c_str());
                return;
            }
	        
        }
		m_LastTimeAnim = m_AnimTime;
	}
    void Whip::SetupResult()
	{
        ChangeState("Look");
        m_ExecutingStartAnim = false;
        m_ExecutingResultAnim = true;
        this->SetVisible(true);
	}
	HOOK(void*, __fastcall, SpawnPlayer, 0x00D96110, void* This)
    {
        auto returnVal =  originalSpawnPlayer(This);
        Whip::s_SingletonWhip = boost::make_shared<Whip>();
        Sonic::CGameDocument::GetInstance()->AddGameObject(Whip::s_SingletonWhip);
        return returnVal;
    }
    //void __thiscall sub_CFABF0(CTempState *this)
    HOOK(void, __fastcall,Whip_CGameplayFlowStage_CStateGoalBegin, 0xCFABF0, void* This)
    {
        originalWhip_CGameplayFlowStage_CStateGoalBegin(This);
        Whip::s_SingletonWhip->SetupResult();
    }
    HOOK(void, __fastcall, MsgChangeResultState, 0xE692C0, void* This, void* Edx, uint32_t a2)
    {
        uint32_t const state = *(uint32_t*)(a2 + 16);
        if (state == 2)
        {        
        }
        if (state == 3)
        {
            if (Sonic::Player::CSonicContext::GetInstance())
            {
                // Play only when it's not Super Sonic
                if (!Sonic::Player::CSonicSpContext::GetInstance())
                {
                    Whip::s_SingletonWhip->SetupResult();
                    int rank = *(int*)(a2 + 20);
                    std::string m_RankString = "E";
                    // Force rank animation code
                    switch (rank)
                    {
					case 4: m_RankString = "S"; break;
					case 3: m_RankString = "A"; break;
					case 2: m_RankString = "B"; break;
					case 1: m_RankString = "C"; break;
					case 0: m_RankString = "D"; break;
					default: break; // 0x07
                    }

                    // E-Rank Generations support
                    if (rank == 4 && *(uint8_t*)0x15EFE9D == 0x45 || rank == -1)
                    {
                        m_RankString = "E";
                    }
                    auto m_PlayerType = Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID == 0;
                    m_RankAnim = std::string(SUC::Format("Result%s%s", m_PlayerType ? "Generic" : "Evil", m_RankString.c_str()));
                    m_RankUVAnim = std::string(SUC::Format("whip_result_%s_%s", m_PlayerType ? "sonic" : "evil", m_RankString.c_str()));
                	DebugDrawText::log(m_RankAnim.c_str());
                }
            }
        }

        originalMsgChangeResultState(This, Edx, a2);
    }
    //Reverse the location of these strings and replace this in the future.
    HOOK(uint32_t, __stdcall, ParseStageStgXml, 0x11D27A0, void* a1, void* a2, char* in_XmlData, const size_t in_XmlDataSize)
    {
        std::string str(in_XmlData, in_XmlDataSize);

        // Get starting mode
        if (str.find("<Mode>Stand</Mode>") != std::string::npos)
        {
            Whip::s_StartMode = Whip::Stand;
            DebugDrawText::log("Start Mode = Stand");
        }
        else if (str.find("<Mode>CrouchingStartFront</Mode>") != std::string::npos)
        {
            Whip::s_StartMode = Whip::CrouchingStartFront;
            DebugDrawText::log("Start Mode = CrouchingStartFront");
        }
        else
        {
            Whip::s_StartMode = Whip::Invalid;
            DebugDrawText::log("Start Mode = Others");
        }
        // Get Sonic's position (in string)
        size_t xPosStart = str.find("<x>");
        size_t xPposEnd = str.find("</x>");
        size_t yPosStart = str.find("<y>");
        size_t yPposEnd = str.find("</y>");
        size_t zPosStart = str.find("<z>");
        size_t zPposEnd = str.find("</z>");
        if (xPosStart != std::string::npos && xPposEnd != std::string::npos
            && yPosStart != std::string::npos && yPposEnd != std::string::npos
            && zPosStart != std::string::npos && zPposEnd != std::string::npos)
        {
            Whip::s_SpawnPosition.x() = std::stof(str.substr(xPosStart + 3, xPposEnd - xPosStart - 3));
            Whip::s_SpawnPosition.y() = std::stof(str.substr(yPosStart + 3, yPposEnd - yPosStart - 3));
            Whip::s_SpawnPosition.z() = std::stof(str.substr(zPosStart + 3, zPposEnd - zPosStart - 3));
            DebugDrawText::log(format("Sonic's pos = {%.3f, %.3f, %.3f}", Whip::s_SpawnPosition.x(), Whip::s_SpawnPosition.y(), Whip::s_SpawnPosition.z()));
        }
        else
        {
            Whip::s_StartMode = Whip::Invalid;
            DebugDrawText::log("ERROR reading Sonic's position");
        }

        return originalParseStageStgXml(a1, a2, in_XmlData, in_XmlDataSize);
    }
    void Whip::RegisterHooks()
    {
        //Set camera parameters for goal camera
        WRITE_MEMORY(0x15AF4C3, uint8_t, 0x75);
        WRITE_MEMORY(0x15D1EBB, uint8_t, 0x75);
        WRITE_MEMORY(0x15D293B, uint8_t, 0x75);
        GOALCAMERA_OFFSET_RIGHT = -0.6053f;

        INSTALL_HOOK(SpawnPlayer);
        INSTALL_HOOK(ParseStageStgXml);
        INSTALL_HOOK(MsgChangeResultState);
        INSTALL_HOOK(Whip_CGameplayFlowStage_CStateGoalBegin);
    }


}
