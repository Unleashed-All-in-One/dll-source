#include "Icicle.h"
namespace SUC::SetObject
{
    BB_SET_OBJECT_MAKE_HOOK(Icicle);

    void Icicle::RegisterObject()
    {
        BB_INSTALL_SET_OBJECT_MAKE_HOOK(Icicle)
    }
}