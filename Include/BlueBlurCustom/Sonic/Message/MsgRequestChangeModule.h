#pragma once

#include <Hedgehog/Universe/Engine/hhMessage.h>

namespace Sonic::Message
{
    class MsgRequestChangeModule : public Hedgehog::Universe::MessageTypeSet
    {
    public:
        HH_FND_MSG_MAKE_TYPE(0x0168120C);

		size_t m_ModuleIndex;

        MsgRequestChangeModule
        (
            size_t in_ModuleIndex
        ) 
            : m_ModuleIndex(in_ModuleIndex)
        {}
    };
}