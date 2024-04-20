#pragma once
using namespace hh::math;

class ETFStageGate :public Sonic::CObjectBase, public Sonic::CSetObjectListener
{
public:
    BB_SET_OBJECT_MAKE("ETFStageGate")
    boost::shared_ptr<hh::mr::CSingleElement> m_spExampleElement;

    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;

    boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;   
    bool m_playerInsideCollider;
    float timerGate;

    bool activateGate;
    bool disagreed;
    uint32_t stageID, actIndex, worldIndex, stageType;
    static ETFStageGateUIContainer* containerUI;
    SharedPtrTypeless handle1;
    void fpAddParticle2(Sonic::CParticleManager* manager, SharedPtrTypeless& handle, void* node, const hh::base::CSharedString& name, uint32_t flag)
    {
        uint32_t func = 0x00E8F8A0;
        __asm
        {
            push flag
            push name
            push node
            push handle
            mov eax, manager
            call func
        };
    };
    DWORD* GetServiceGameplay2(Hedgehog::Base::TSynchronizedPtr<Sonic::CApplicationDocument> doc)
    {
        uint32_t func = 0x0040EBD0;
        DWORD* result;
        __asm
        {
            mov     edi, doc
            add     edi, 34h
            call func
            mov     result, eax
        };
    };
    bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        if (stageType != GetServiceGameplay2(Sonic::CApplicationDocument::GetInstance())[1])
        {
            DebugDrawText::log(std::format("[SonicUnleashedConversion] StageType doesn't match ServiceGameplay PlayerClass. {0}, {1}", stageType, GetServiceGameplay2(Sonic::CApplicationDocument::GetInstance())[1]).c_str(), 10);
            return false;
        }
        const char* assetName = "cmn_obj_km_etfdoor_NAC";
        hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
        boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
        m_spExampleElement = boost::make_shared<hh::mr::CSingleElement>(spModelData);

        m_spExampleElement->BindMatrixNode(m_spMatrixNodeTransform);
        const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
        auto node = m_spExampleElement->GetNode("ETFdoor_Mark_00");
        const char* particleName = "stage1";
        switch (stageType)
        {
        case 0:
        {
            particleName = "stage1";
            break;
        }
        case 1:
        {
            particleName = "stage_night";
            break;
        }
        case 2:
        {
            particleName = "stage1";
            break;
        }
        }
        //fpAddParticle2(Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager.get(), handle1, &node, "ef_ch_sng_yh1_spinattack", 1);
        Common::fCGlitterCreate(playerContext, handle1, &node, particleName, 0);
        AddRenderable("Object", m_spExampleElement, true);
        DebugDrawText::log("I EXIST!!", 10);
        return true;
    }
    bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
    {
        if (in_Flag)
        {
            if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
            {
                const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
                if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
                {
                    disagreed = false;
                    activateGate = false;
                    timerGate = 0;
                    m_playerInsideCollider = true;
                }
                return true;
            }
            if (std::strstr(in_rMsg.GetType(), "MsgLeaveEventCollision") != nullptr)
            {
                const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
                if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
                {
                    activateGate = false;
                    m_playerInsideCollider = false;
                }
                return true;
            }
            if (std::strstr(in_rMsg.GetType(), "MsgRestartStage") != nullptr
                || std::strstr(in_rMsg.GetType(), "MsgStageClear") != nullptr)
            {
                containerUI->Kill();
                containerUI = nullptr;
                return true;
            }
        }
        return Sonic::CObjectBase::ProcessMessage(in_rMsg, in_Flag);
    };
    bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        if (stageType != GetServiceGameplay2(Sonic::CApplicationDocument::GetInstance())[1])
        {
            DebugDrawText::log(std::format("[SonicUnleashedConversion] StageType doesn't match ServiceGameplay PlayerClass. {0}, {1}", stageType, GetServiceGameplay2(Sonic::CApplicationDocument::GetInstance())[1]).c_str(), 10);
            return false;
        }
        m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
        m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0,0,0));
        m_spNodeEventCollision->NotifyChanged();
        m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());

        hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(1, 6, 1);
        AddEventCollision("Object", shapeEventTrigger1, *pColID_PlayerEvent, true, m_spNodeEventCollision);
        return true;
    }  
    CQuaternion EulerToQuaternion(CVector euler)
    {
        CQuaternion q;
        q = Eigen::AngleAxisf(euler.x(), Eigen::Vector3f::UnitX())
            * Eigen::AngleAxisf(euler.y(), Eigen::Vector3f::UnitY())
            * Eigen::AngleAxisf(euler.z(), Eigen::Vector3f::UnitZ());
        return q;
    }
    CQuaternion rotation;
    void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
    {
        auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
        const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
        if (m_playerInsideCollider && !disagreed)
        {
            if (timerGate <= 2)
                timerGate += in_rUpdateInfo.DeltaTime;
        }
        if (!disagreed && timerGate >= 2)
        {
            if (!activateGate)
            {
                activateGate = true;
                if (containerUI == nullptr)
                    containerUI = ETFStageGateUIContainer::Generate(playerContext->m_pPlayer, false, false);
                containerUI->UpdateState(SequenceHelpers::getCurrentStageName(true) + std::to_string(stageID), actIndex, worldIndex);
                containerUI->Show();
                playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_IgnorePadInput] = false;

                playerContext->m_pPlayer->m_PostureStateMachine.ChangeState("Stop");
                playerContext->m_spMatrixNode->m_Transform.m_Position = m_spMatrixNodeTransform->m_Transform.m_Position;
                rotation = (EulerToQuaternion(Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera()->m_MyCamera.m_Direction));
            }
            else
            {
                if (inputPtr->IsTapped(Sonic::eKeyState_A))
                {
                    //example: ghz + stageID(200) = ghz200
                    std::string stageToLoadS = (SequenceHelpers::getCurrentStageName(true) + std::to_string(stageID));
                    uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
                    char** h = (char**)stageTerrainAddress;
                    const char* stageToLoad = stageToLoadS.c_str();

                    LevelLoadingManager::setETFInfo(SequenceHelpers::getCurrentStageName(false));
                    LevelLoadingManager::setGameParameters(SequenceHelpers::getCurrentStageName(true) + std::to_string(stageID), "");
                    TitleWorldMap::LoadingReplacementEnabled = true;
                    LevelLoadingManager::WhiteWorldEnabled = false;
                    SequenceHelpers::loadStage((SequenceHelpers::getCurrentStageName(true) + std::to_string(stageID)).c_str(), 0);
                    strcpy(*(char**)stageTerrainAddress, stageToLoad);
                }
                if (inputPtr->IsTapped(Sonic::eKeyState_B))
                {
                    containerUI->Hide();
                    disagreed = true;
                }
            }
        }
    }
    void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
    {
        in_rEditParam.SetInt(&stageID, "StageIDNumber");
        in_rEditParam.SetInt(&worldIndex, "WorldIndex");
        in_rEditParam.SetInt(&actIndex, "ActIndex");
        in_rEditParam.SetInt(&stageType, "StageType");
    }

    static void registerObject();
};