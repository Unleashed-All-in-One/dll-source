#include "IrremovableMobMykonos.h"
namespace SUC::SetObject
{
    BB_SET_OBJECT_MAKE_HOOK(IrremovableMobMykonos);
    void IrremovableMobMykonos::RegisterObject()
    {
        BB_INSTALL_SET_OBJECT_MAKE_HOOK(IrremovableMobMykonos)
    }
}