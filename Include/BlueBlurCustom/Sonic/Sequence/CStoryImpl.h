#pragma once

#include <Hedgehog/Universe/Engine/hhMessageActor.h>
#include <Sonic/System/ApplicationDocument.h>
namespace Sonic::Sequence
{
    class Story : public Hedgehog::Universe::CMessageActor
    {
    public:
        static Story* GetInstance()
        {
            return *reinterpret_cast<Story**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(Sonic::CApplicationDocument::GetInstance()->m_pMember) + 0x168) + 0x94);
        };
    };
}