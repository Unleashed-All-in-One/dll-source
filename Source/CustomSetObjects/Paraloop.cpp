
BB_SET_OBJECT_MAKE_HOOK(Paraloop);

void Paraloop::registerObject()
{
    WRITE_STRING(0x01610F5C, "Gensloop");
    BB_INSTALL_SET_OBJECT_MAKE_HOOK(Paraloop)
}