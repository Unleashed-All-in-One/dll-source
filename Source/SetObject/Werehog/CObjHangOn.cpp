#include "CObjHangOn.h"
namespace SUC::SetObject
{
    BB_SET_OBJECT_MAKE_HOOK(CObjHangOn);


    void CObjHangOn::RegisterObject()
    {
        BB_INSTALL_SET_OBJECT_MAKE_HOOK(CObjHangOn)
    }
}