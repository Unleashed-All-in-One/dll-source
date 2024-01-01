#pragma once

#include <Hedgehog/Universe/Engine/hhMessage.h>

namespace Sonic::Message
{
    class MsgSequenceEvent : public Hedgehog::Universe::MessageTypeSet
    {
    public:
        HH_FND_MSG_MAKE_TYPE(0x0167F11C);

        size_t m_EventType;
		size_t m_Unknown;

        // the same order as sub_4F9E90
        MsgSequenceEvent
        (
            size_t in_EventType,
            bool in_UnknownFlag
        ) 
            : m_EventType(in_EventType)
            , m_Unknown(in_UnknownFlag)
        {}
    };
}