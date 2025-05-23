#pragma once
#include "../UI/HelpCaptionUI.h"
using namespace hh::math;
namespace SUC::SetObject
{
    class Hintring :public Sonic::CObjectBase, public Sonic::CSetObjectListener, public Sonic::IAnimationContext, public Sonic::CAnimationStateMachine
    {
    public:
        BB_SET_OBJECT_MAKE("Hintring")
            boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;
        boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
        boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;

        boost::shared_ptr<Hedgehog::Animation::CAnimationPose> m_AnimatorPose;
        static HintDataList* hintData;
        bool m_playerInsideCollider;
        static HelpCaptionUIContainer* m_HintUI;
        bool m_IsExiting;

        //from Set data
        int m_HintDataIndex = 0;
        bool m_PlayerStop = true;
        bool m_NeedInput = true;
        float m_TimerWaitReset;
        bool m_Cooldown = false;
        std::vector<SUC::NewAnimationData> animations;
        boost::shared_ptr<Sonic::CAnimationStateMachine> animationStateMachine;

        Sonic::CParamTypeList* test;
        bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
        {
            const char* assetName = "cmn_obj_sk2_hintring";
            hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
            boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
            m_spSpawnedModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
            m_AnimatorPose = boost::make_shared<Hedgehog::Animation::CAnimationPose>(in_spDatabase, assetName);
            animations = std::vector<SUC::NewAnimationData>();
            animations.push_back(SUC::NewAnimationData("Idle", "cmn_obj_sk2_hintring", 1, true, nullptr));
            animations.push_back(SUC::NewAnimationData("Appear", "cmn_obj_sk2_hintring_appear", 1, false, nullptr));
            animations.push_back(SUC::NewAnimationData("Touch", "cmn_obj_sk2_hintring_touch", 1, false, "Idle"));

            this->SetContext(this); //why?
            ObjectUtility::RegisterAnimations(m_AnimatorPose, animations, m_spSpawnedModel, this);
            m_spSpawnedModel->BindMatrixNode(m_spMatrixNodeTransform);
            Sonic::CGameObject::AddRenderable("Object", m_spSpawnedModel, true);

            this->ChangeState("Idle");
            return true;
        }
        Hedgehog::Animation::CAnimationPose* GetAnimationPose()
        {
            return m_AnimatorPose.get();
        }
        Hedgehog::Math::CVector GetVelocityForAnimationSpeed()
        {
            return Hedgehog::Math::CVector(1.0f, 1.0f, 1.0f);
        }
        Hedgehog::Math::CVector GetVelocityForAnimationChange()
        {
            return Hedgehog::Math::CVector(1.0f, 1.0f, 1.0f);
        }
        //virtual  = 0;
        //virtual Hedgehog::Math::CVector GetVelocityForAnimationChange() = 0;
        bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
        {
            if (in_Flag)
            {
                //OnCollisionEnter
                if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
                {
                    const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
                    if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID && hintData != nullptr)
                    {
                        m_playerInsideCollider = true;
                        if (m_HintUI == nullptr)
                        {
                            //may not be a good idea to parent this to the player
                            m_HintUI = HelpCaptionUIContainer::Generate(playerContext->m_pPlayer);
                        }
                        if (m_PlayerStop)
                        {
                            const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
                            if (playerContext->m_pPlayer->m_PostureStateMachine.GetCurrentState()->m_Name == "Standard")
                                playerContext->m_pPlayer->m_PostureStateMachine.ChangeState("Stop");

                            playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_OutOfControl] = true;
                        }
                        m_IsExiting = false;
                        m_HintUI->Reset();
                        m_HintUI->strings = hintData->hintData[m_HintDataIndex];
                        m_HintUI->Show();
                        m_Cooldown = true;
                        this->ChangeState("Touch");
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

                }
            }
            return Sonic::CObjectBase::ProcessMessage(in_rMsg, in_Flag);
        };
        bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
        {
            m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
            m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
            m_spNodeEventCollision->NotifyChanged();
            m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());
            hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(1, 6, 1);
            AddEventCollision("Object", shapeEventTrigger1, CollisionLayerID::PlayerEvent, true, m_spNodeEventCollision);
            return true;
        }


        void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
        {
            if (m_Cooldown && !m_playerInsideCollider)
            {
                m_TimerWaitReset += in_rUpdateInfo.DeltaTime;
                if (m_TimerWaitReset > 5)
                {
                    m_Cooldown = false;
                    m_TimerWaitReset = 0;

                    this->ChangeState("Appear");
                }
            }
            m_AnimatorPose->Update(in_rUpdateInfo.DeltaTime);
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
        uint32_t temp;
        Sonic::CParamTypeList* CreateParamString(uint32_t* pValue, const Hedgehog::Base::CSharedString& in_rName,
                                                                      const Hedgehog::Base::CSharedString& in_rDescription, Sonic::CEditParam& editParam)
        {
	        Sonic::CParamTypeList* pParamTypeList = Sonic::CParamTypeList::Create(pValue, in_rDescription);
            pParamTypeList->AddRef();
            fCEditParamAddParamTypeList(&in_rName, &editParam, pParamTypeList);

            return pParamTypeList;
        }
        void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
        {
            in_rEditParam.CreateParamBool(&m_NeedInput, "NeedInput");
            in_rEditParam.CreateParamBool(&m_PlayerStop, "PlayerStop");
            in_rEditParam.CreateParamInt(&m_HintDataIndex, "HintDataIndex");
        }
        static void RegisterObject();
    };
}