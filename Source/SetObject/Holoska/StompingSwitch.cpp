#include "StompingSwitch.h"
BB_SET_OBJECT_MAKE_HOOK(StompingSwitch);

void StompingSwitch::registerObject() {
	BB_INSTALL_SET_OBJECT_MAKE_HOOK(StompingSwitch)
}