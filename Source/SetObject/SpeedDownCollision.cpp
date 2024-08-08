
#include "SpeedDownCollision.h"
namespace SUC::SetObject
{
    BB_SET_OBJECT_MAKE_HOOK(SpeedDownCollision);

    ItemboxUI* SpeedDownCollision::m_ItemBoxUI;

    void SpeedDownCollision::RegisterObject()
    {
        WRITE_STRING(0x01610E10, "GensSpeedDown");
        BB_INSTALL_SET_OBJECT_MAKE_HOOK(SpeedDownCollision)
    }
}