#pragma once

#include <Hedgehog/Universe/Engine/hhMessage.h>

namespace Sonic::Message
{
    class MsgSequenceEvent : public Hedgehog::Universe::MessageTypeSet
    {
    public:
        HH_FND_MSG_MAKE_TYPE(0x0167F11C);

        size_t m_GameplayFlow;
		size_t m_Module;

        // the same order as sub_4F9E90
        MsgSequenceEvent
        (
            size_t in_GameplayFlow,
            bool in_Module
        ) 
            : m_GameplayFlow(in_GameplayFlow)
            , m_Module(in_Module)
        {}
    };
    
}
namespace Sonic::Message
{
    class MsgChangeStageMode : public Hedgehog::Universe::MessageTypeSet
    {
    public:
        HH_FND_MSG_MAKE_TYPE(0x016819E8);

        uint32_t m_EventType;

        // the same order as sub_4F9E90
        MsgChangeStageMode(uint32_t in_EventType)
        {
            m_EventType = in_EventType;
        }
    };
}
namespace Sonic::Message
{
    class MsgStorySequenceEvent : public Hedgehog::Universe::MessageTypeSet
    {
    public:
        HH_FND_MSG_MAKE_TYPE(0x0167F130);

        size_t m_GameplayFlow;
        size_t m_Module;

        // the same order as sub_4F9E90
        MsgStorySequenceEvent
        (
            size_t in_GameplayFlow,
            bool in_Module
        )
            : m_GameplayFlow(in_GameplayFlow)
            , m_Module(in_Module)
        {}
    };
}