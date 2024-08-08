#include "Pelican.h"
namespace SUC::SetObject
{
    BB_SET_OBJECT_MAKE_HOOK(Pelican);
    void Pelican::RegisterObject()
    {
        BB_INSTALL_SET_OBJECT_MAKE_HOOK(Pelican)
    }
}