#pragma once

#include <Hedgehog/Base/Thread/hhSynchronizedObject.h>
#include <Hedgehog/Base/Thread/hhSynchronizedPtr.h>
#include <Hedgehog/Universe/Engine/hhMessageManager.h>

namespace Sonic2::Message
{
	struct SRequestChangeModuleInfo;
}

namespace Hedgehog2::Database
{
    class CDatabase;
    class CDatabaseLoader;
}

namespace Hedgehog2::Mirage
{
    class CRenderingInfrastructure;
}

namespace Hedgehog2::Universe
{
    class CService;
}

namespace Sonic2::Sequence
{
    class CSequenceMainImpl;
}

namespace Sonic2
{
    class CApplication;
    class CParameterEditor;
    class CGameParameter;
    class CModuleManager : public Hedgehog::Universe::CMessageActor
    {
    public:
        struct SModuleConstructor
        {
            int Index;
            void* pFunction;
        };
        int m_ExtraActorID;
        SModuleConstructor* m_ModuleConstructorAddress;
    };
    
    class CGameplayFlowManager : public Hedgehog::Universe::TStateMachine<CGameplayFlowManager>
    {
    public:
        class CMember
        {
        public:
            Sonic::Message::SRequestChangeModuleInfo* field0;
            CModuleManager* m_pModuleManager;
            int m_CurrentModuleIndex;
            Sonic::Message::SRequestChangeModuleInfo* m_CurrentModuleInfo;
            BB_INSERT_PADDING(68 - 16);
            int m_BitmaskWeird44;
        };
        BB_INSERT_PADDING(0xDC - sizeof(Hedgehog::Universe::TStateMachine<CGameplayFlowManager>));
        CMember* test;
        
    };
    BB_ASSERT_OFFSETOF(CGameplayFlowManager::CMember, m_pModuleManager, 0x4);
    BB_ASSERT_OFFSETOF(CGameplayFlowManager::CMember, m_BitmaskWeird44, 0x44);
    BB_ASSERT_OFFSETOF(CGameplayFlowManager, test, 0xDC);
    class CApplicationDocument : public Hedgehog::Base::CSynchronizedObject
    {
    public:
        class CMember
        {
        public:
            BB_INSERT_PADDING(0x10);
            CApplication* m_pApplication;
            BB_INSERT_PADDING(0x8);
            Hedgehog::Universe::CMessageManager* m_pMessageManager;
            BB_INSERT_PADDING(0x10);
            CGameplayFlowManager* m_pGameplayFlowManager;
            BB_INSERT_PADDING(0x4);
            boost::shared_ptr<Hedgehog::Mirage::CRenderingInfrastructure> m_spRenderingInfrastructure;
            BB_INSERT_PADDING(0x3C);
            boost::shared_ptr<Hedgehog::Database::CDatabase> m_spApplicationDatabase;
            boost::shared_ptr<Hedgehog::Database::CDatabase> m_spShaderDatabase;
            BB_INSERT_PADDING(0x10);
            boost::shared_ptr<Hedgehog::Database::CDatabase> m_spDatabase;
            BB_INSERT_PADDING(0x20);
            boost::shared_ptr<Hedgehog::Database::CDatabaseLoader> m_spDatabaseLoader;
            BB_INSERT_PADDING(0x18);
            boost::shared_ptr<Hedgehog::Mirage::CMatrixNode> m_spMatrixNodeRoot;
            BB_INSERT_PADDING(0x78);
            boost::shared_ptr<Sonic::Sequence::CSequenceMainImpl> m_spSequenceMain;
            BB_INSERT_PADDING(0x44);
            boost::shared_ptr<Sonic::CGameParameter> m_spGameParameter;
            BB_INSERT_PADDING(0x48);
            boost::shared_ptr<CParameterEditor> m_spParameterEditor;
            BB_INSERT_PADDING(0x24);

            virtual ~CMember() = default;
        };

        static constexpr Hedgehog::Base::TSynchronizedPtr<CApplicationDocument>* ms_pInstance = (Hedgehog::Base::TSynchronizedPtr<CApplicationDocument>*)0x1E66B34;

        CMember* m_pMember;
        BB_INSERT_PADDING(0x2C);
        hh::map<uint32_t, boost::shared_ptr<Hedgehog::Universe::CService>> m_ServiceMap;
    };

    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_pApplication, 0x14);
    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_pMessageManager, 0x20);
    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_pGameplayFlowManager, 0x34);
    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_spRenderingInfrastructure, 0x3C);
    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_spApplicationDatabase, 0x80);
    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_spShaderDatabase, 0x88);
    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_spDatabase, 0xA0);
    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_spDatabaseLoader, 0xC8);
    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_spMatrixNodeRoot, 0xE8);
    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_spSequenceMain, 0x168);
    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_spGameParameter, 0x1B4);
    BB_ASSERT_OFFSETOF(CApplicationDocument::CMember, m_spParameterEditor, 0x204);
    BB_ASSERT_SIZEOF(CApplicationDocument::CMember, 0x230);

    BB_ASSERT_OFFSETOF(CApplicationDocument, m_pMember, 0x4);
    BB_ASSERT_OFFSETOF(CApplicationDocument, m_ServiceMap, 0x34);
    BB_ASSERT_SIZEOF(CApplicationDocument, 0x40);
}