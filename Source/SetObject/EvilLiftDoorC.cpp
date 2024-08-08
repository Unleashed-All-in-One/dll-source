#include "EvilLiftDoor.h"
namespace SUC::SetObject
{
    BB_SET_OBJECT_MAKE_HOOK(EvilLiftDoor);

    EvilQTEUI* EvilLiftDoor::m_ItemBoxUI;
    void EvilLiftDoor::RegisterObject()
    {
        BB_INSTALL_SET_OBJECT_MAKE_HOOK(EvilLiftDoor)
    }
}