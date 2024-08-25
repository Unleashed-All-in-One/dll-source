#pragma once
#include "../UI/ETFStageGateUI.h"
using namespace hh::math;
namespace SUC::SetObject
{
    class ETFStageGate :public Sonic::CObjectBase, public Sonic::CSetObjectListener
    {
    public:
        BB_SET_OBJECT_MAKE("ETFStageGate")
            boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;

        boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;

        boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;
        bool m_playerInsideCollider;
        float timerGate;

        bool activateGate;
        bool disagreed;
        int stageID, actIndex, worldIndex;
        bool stageType;
        int subStageIndex = -1;
        static ETFStageGateUIContainer* containerUI;
        std::string countryNames[9] = { "Mykonos", "Europe","China","Africa","Snow","Petra","Beach","NewYork","Eggmanland" };
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
        bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
        {
            if (stageType != Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID)
            {
                DebugDrawText::log(std::format("[SonicUnleashedConversion] StageType doesn't match ServiceGameplay PlayerClass. {0}, {1}", stageType, Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID).c_str(), 10);
                return false;
            }
            const char* assetName = "cmn_obj_km_etfdoor_NAC";
            hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
            boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
            m_spSpawnedModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);

            m_spSpawnedModel->BindMatrixNode(m_spMatrixNodeTransform);
            const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
            auto node = m_spSpawnedModel->GetNode("ETFdoor_Mark_00");
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
            //case 2:
            //{
            //    particleName = "stage1";
            //    break;
            //}
            }
            //fpAddParticle2(Sonic::CGameDocument::GetInstance()->m_pMember->m_spParticleManager.get(), handle1, &node, "ef_ch_sng_yh1_spinattack", 1);
            Common::fCGlitterCreate(playerContext, handle1, &node, particleName, 0);
            Sonic::CGameObject::AddRenderable("Object", m_spSpawnedModel, true);
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
            if (stageType != Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID)
            {
                DebugDrawText::log(SUC::Format("[StageGate] StageType(%d) doesn't match ServiceGameplay PlayerClass(%d).", stageType, Sonic::CApplicationDocument::GetInstance()->GetService<Sonic::CServiceGamePlay>()->m_PlayerID), 10, 0, TEXT_RED);
                return false;
            }
            m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
            m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
            m_spNodeEventCollision->NotifyChanged();
            m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());

            hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(1, 6, 1);
            AddEventCollision("Object", shapeEventTrigger1, CollisionLayerID::PlayerEvent, true, m_spNodeEventCollision);
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
            playerContext->m_spMatrixNode->m_Transform.m_Rotation = rotation;
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

                    std::string stageIDN = std::format("{0}{1}_{2}", stageType ? "D" : "N", countryNames[worldIndex], actIndex);
                    containerUI->UpdateState(stageIDN, actIndex, subStageIndex, worldIndex);
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
                        std::string stageToLoadS = (SUC::System::SequenceHelpers::GetCurrentStageName(true) + std::to_string(stageID));
                        const char* stageToLoad = stageToLoadS.c_str();

                        std::string stageIDN = std::format("{0}{1}_{2}", stageType ? "D" : "N", countryNames[worldIndex], actIndex);
                        if (subStageIndex != -1)
                        {
                            stageIDN = stageIDN + std::format("-{0}", subStageIndex);
                        }
                        SUC::System::StageManager::SetETFInfo(SUC::System::SequenceHelpers::GetCurrentStageName(false));
                        SUC::System::StageManager::SetGameParameters(stageIDN, "");
                        SUC::System::StageManager::s_LoadingReplacementEnabled = true;
                        SUC::System::StageManager::s_HubModeEnabled = false;
                        SUC::System::SequenceHelpers::LoadStage(stageIDN.c_str(), 0);
                        SUC::System::StageManager::SetGameParameters(stageIDN, "Stage");
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
            in_rEditParam.CreateParamInt(&stageID, "StageIDNumber");
            in_rEditParam.CreateParamInt(&worldIndex, "WorldIndex");
            in_rEditParam.CreateParamInt(&actIndex, "ActIndex");
            in_rEditParam.CreateParamInt(&subStageIndex, "SubStage");
            in_rEditParam.CreateParamBool(&stageType, "IsDay");
        }

        static void RegisterObject();
    };
}