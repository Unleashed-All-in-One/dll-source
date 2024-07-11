#pragma once

#include <Hedgehog/Universe/Engine/hhMessage.h>

namespace Sonic::Message
{
    struct SRequestChangeModuleInfo
    {
    public:
        Hedgehog::base::CSharedString m_Field00;
        Hedgehog::base::CSharedString m_Field04;
        int m_Field08;
        Hedgehog::base::CSharedString m_Field12;
        Hedgehog::base::CSharedString m_Field16;
        hh::list < Hedgehog::Base::CSharedString> test;
        int m_Field32;
        int m_Field36;
        Hedgehog::base::CSharedString m_Field40;
        Hedgehog::base::CSharedString m_Field44;
        float m_Field48;
        int m_Field52;
    };
    BB_ASSERT_OFFSETOF(SRequestChangeModuleInfo, m_Field48, 48);
    class MsgRequestChangeModule : public Hedgehog::Universe::MessageTypeSet
    {
    public:
        HH_FND_MSG_MAKE_TYPE(0x0168120C);

		size_t m_ModuleIndex;
        SRequestChangeModuleInfo moduleInfo;

       
    };
}