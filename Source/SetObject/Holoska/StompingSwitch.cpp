#include "StompingSwitch.h"
namespace SUC::SetObject
{
	BB_SET_OBJECT_MAKE_HOOK(StompingSwitch);

	void StompingSwitch::RegisterObject()
	{
		BB_INSTALL_SET_OBJECT_MAKE_HOOK(StompingSwitch)
	}
}