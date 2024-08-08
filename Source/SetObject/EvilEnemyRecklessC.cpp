#include "EvilEnemyReckless.h"
namespace SUC::SetObject
{
    BB_SET_OBJECT_MAKE_HOOK(EvilEnemyReckless);
    void EvilEnemyReckless::RegisterObject()
    {
        BB_INSTALL_SET_OBJECT_MAKE_HOOK(EvilEnemyReckless)
    }
}