#pragma once
using namespace hh::math;

class Hintring :public Sonic::CObjectBase, public Sonic::CSetObjectListener
{
public:
    BB_SET_OBJECT_MAKE("Hintring")
    boost::shared_ptr<hh::mr::CSingleElement> m_spExampleElement;
    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
    boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;

    boost::shared_ptr<Hedgehog::Animation::CAnimationPose> animatorTest;
    static HintDataList* hintData;
    bool m_playerInsideCollider;    
    static SubtitleUIContainer* m_HintUI;
    bool m_IsExiting;

    //from Set data
    uint32_t m_HintDataIndex = 0;
    bool m_PlayerStop = true;
    bool m_NeedInput = true;
    std::vector<NewAnimationData> animations;
    boost::shared_ptr<Sonic::CAnimationStateMachine> animationStateMachine;
    bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        const char* assetName = "cmn_obj_sk2_hintring";
        hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
        boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
        m_spExampleElement = boost::make_shared<hh::mr::CSingleElement>(spModelData);

        animatorTest = boost::make_shared<Hedgehog::Animation::CAnimationPose>(in_spDatabase, assetName);

        animations = std::vector<NewAnimationData>();
        animations.push_back(NewAnimationData("Appear", "cmn_obj_sk2_hintring_appear", 1, true, nullptr));

        CAnimationStateInfo* pEntries = new CAnimationStateInfo[animations.size()];

        pEntries[0].m_Name = animations[0].m_stateName;
        pEntries[0].m_FileName = animations[0].m_fileName;
        pEntries[0].m_Speed = animations[0].m_speed;
        pEntries[0].m_PlaybackType = !animations[0].m_isLoop;
        pEntries[0].field10 = 0;
        pEntries[0].field14 = -1.0f;
        pEntries[0].field18 = -1.0f;
        pEntries[0].field1C = 0;
        pEntries[0].field20 = -1;
        pEntries[0].field24 = -1;
        pEntries[0].field28 = -1;
        pEntries[0].field2C = -1;

        animatorTest->AddAnimationList(pEntries, animations.size());
        //animationStateMachine = boost::make_shared< Sonic::CAnimationStateMachine>();
        //auto test = animationStateMachine->GetContext();
        
        m_spExampleElement->BindMatrixNode(m_spMatrixNodeTransform);
        //m_spExampleElement->BindAnimationPose(animatorTest);
        //ChangeState("Appear");
        AddRenderable("Object", m_spExampleElement, true);
        DebugDrawText::log("I EXIST!!", 10);
        return true;
    }
    bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
    {
        if (in_Flag)
        {
            //OnCollisionEnter
            if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
            {
                const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
                if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
                {
                    m_playerInsideCollider = true;
                    if (m_HintUI == nullptr)
                    {
                        //may not be a good idea to parent this to the player
                        m_HintUI = SubtitleUIContainer::Generate(playerContext->m_pPlayer);
                    }
                    if (m_PlayerStop)
                    {
                        const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
                        if(playerContext->m_pPlayer->m_PostureStateMachine.GetCurrentState()->m_Name == "Standard")
                        playerContext->m_pPlayer->m_PostureStateMachine.ChangeState("Stop");

                        playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_OutOfControl] = true;
                    }
                    m_IsExiting = false;
                    m_HintUI->Reset();
                    m_HintUI->strings = hintData->hintData[m_HintDataIndex];
                    m_HintUI->Show();
                }
                return true;
            }    

            //OnCollisionExit
            if (std::strstr(in_rMsg.GetType(), "MsgLeaveEventCollision") != nullptr)
            {
                const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
                if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
                {
                    m_playerInsideCollider = false;
                }
            }
            if (std::strstr(in_rMsg.GetType(), "MsgGetHomingAttackPosition") != nullptr)
            {

                DWORD* msg = (DWORD*)&in_rMsg;
                *(Hedgehog::Math::CVector*)(msg + 16) = m_spMatrixNodeTransform->m_Transform.m_Position;
                SendMessageImm(in_rMsg.m_SenderActorID, (Hedgehog::Universe::MessageTypeSet*)msg);
            }
        }
        return Sonic::CObjectBase::ProcessMessage(in_rMsg, in_Flag);
    };
    bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
        m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0,0,0));
        m_spNodeEventCollision->NotifyChanged();
        m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());
        hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(1, 6, 1);
        AddEventCollision("Object", shapeEventTrigger1, *pColID_PlayerEvent, true, m_spNodeEventCollision);
        return true;
    }        
    

    void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
    {
        auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
        if (m_playerInsideCollider && !m_IsExiting)
        {            
            if (m_HintUI)
            {
                if (inputPtr->IsTapped(Sonic::eKeyState_A))
                {
                    m_HintUI->Progress();
                }
                if (m_HintUI->isComplete)
                {
                    m_IsExiting = true;
                    if (m_PlayerStop)
                    {
                        const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();

                        if (playerContext->m_pPlayer->m_PostureStateMachine.GetCurrentState()->m_Name == "Stop")
                        playerContext->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
                        playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_OutOfControl] = false;
                    }
                }
            }
        }
    }
    void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
    {
        //...why ceramic... why did you have to name these "set" if they're meant to "get"...
        in_rEditParam.SetBool(&m_NeedInput, "NeedInput");
        in_rEditParam.SetBool(&m_PlayerStop, "PlayerStop");
        in_rEditParam.SetInt(&m_HintDataIndex, "HintDataIndex");
    }
    static void registerObject();
};