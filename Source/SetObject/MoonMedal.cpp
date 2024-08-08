#include "MoonMedal.h"
namespace SUC::SetObject
{
    BB_SET_OBJECT_MAKE_HOOK(MoonMedal);

    void MoonMedal::RegisterObject()
    {
        BB_INSTALL_SET_OBJECT_MAKE_HOOK(MoonMedal)
    }
}