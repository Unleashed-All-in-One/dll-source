BB_SET_OBJECT_MAKE_HOOK(EvilLiftDoor);

EvilQTEUI* EvilLiftDoor::m_ItemBoxUI;
void EvilLiftDoor::registerObject()
{
    BB_INSTALL_SET_OBJECT_MAKE_HOOK(EvilLiftDoor)
}