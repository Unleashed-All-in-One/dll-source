#pragma once

#include <Hedgehog/Universe/Engine/hhMessageActor.h>
#include <Sonic/System/ApplicationDocument.h>
namespace Sonic::Sequence
{
    class Story : public Hedgehog::Universe::CMessageActor
    {
    public:
        static Hedgehog::Universe::CMessageActor* GetInstance()
        {
            return *((Hedgehog::Universe::CMessageActor**)Sonic::CApplicationDocument::GetInstance()->m_pMember + 45);
        };
    };
}