#pragma once

#include <Hedgehog/Universe/Engine/hhMessageActor.h>
#include <Sonic/System/ApplicationDocument.h>
namespace Sonic::Sequence
{
    class Main : public Hedgehog::Universe::CMessageActor
    {
    public:
        static Hedgehog::Universe::CMessageActor* GetInstance()
        {
            return *((Hedgehog::Universe::CMessageActor**)Sonic::CApplicationDocument::GetInstance()->m_pMember + 90);
        };
        static void ProcessMessage(Hedgehog::Universe::Message* message)
        {
            BB_FUNCTION_PTR(int*, __thiscall, ProcessMSG, 0x00D770D0, Hedgehog::Universe::CMessageActor * MainSequenceImpl, Hedgehog::Universe::Message * a1);
            ProcessMSG(GetInstance(), message);
        }
    };    
}