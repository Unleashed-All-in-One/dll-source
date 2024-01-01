#pragma once

#include <Hedgehog/Universe/Engine/hhMessageActor.h>
#include <Sonic/System/ApplicationDocument.h>
namespace Sonic::Event
{
    class EventQueue : public Hedgehog::Universe::CMessageActor
    {
    public:
        static Hedgehog::Universe::CMessageActor* GetInstance()
        {
            return *((Hedgehog::Universe::CMessageActor**)Sonic::CApplicationDocument::GetInstance()->m_pMember + 94);
        }
        static void ProcessMessage(Hedgehog::Universe::Message* message)
        {
            BB_FUNCTION_PTR(void, __thiscall, ProcessMsgEventQueue, 0x10FE9E0, Hedgehog::Universe::CMessageActor * Th, Hedgehog::Universe::Message * a1, int a2);

            ProcessMsgEventQueue(GetInstance(), message, 1);
        }
    };
}