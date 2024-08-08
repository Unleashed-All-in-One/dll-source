#include "ETFStageGate.h"
namespace SUC::SetObject
{
    BB_SET_OBJECT_MAKE_HOOK(ETFStageGate);


    ETFStageGateUIContainer* ETFStageGate::containerUI;
    void ETFStageGate::RegisterObject()
    {
        BB_INSTALL_SET_OBJECT_MAKE_HOOK(ETFStageGate)
    }
}