
// The other macros are your object's class name, not XML name.
BB_SET_OBJECT_MAKE_HOOK(WerehogPole);

// Whatever your entrypoint is, you'll need this:
void WerehogPole::Install()
{
    BB_INSTALL_SET_OBJECT_MAKE_HOOK(WerehogPole)
}