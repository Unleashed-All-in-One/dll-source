
#include "Paraloop.h"
namespace SUC::SetObject
{
    BB_SET_OBJECT_MAKE_HOOK(Paraloop);

    void Paraloop::RegisterObject()
    {
        WRITE_STRING(0x01610F5C, "Gensloop");
        BB_INSTALL_SET_OBJECT_MAKE_HOOK(Paraloop)
    }
}