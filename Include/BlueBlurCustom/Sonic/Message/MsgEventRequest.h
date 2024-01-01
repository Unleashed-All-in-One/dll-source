#pragma once

#include <Hedgehog/Universe/Engine/hhMessage.h>

namespace Sonic::Message
{
    class MsgEventRequest : public Hedgehog::Universe::MessageTypeSet
    {
    public:
        HH_FND_MSG_MAKE_TYPE(0x01681C04);

        Hedgehog::Base::CSharedString m_EventName;
		size_t m_Unknown;

        MsgEventRequest
        (
            Hedgehog::Base::CSharedString in_EventName,
            size_t in_UnknownFlag
        ) 
            : m_EventName(in_EventName)
            , m_Unknown(in_UnknownFlag)
        {}
    };
}