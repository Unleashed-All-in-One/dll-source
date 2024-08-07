#include "ETFTimeSwitch.h"
BB_SET_OBJECT_MAKE_HOOK(ETFTimeSwitch);

void __declspec(naked) GetCServiceGameplayMidAsmHook()
{
    __asm
    {
        mov eax, [esi + 0x10]
        mov[eax + 4], 0
        pop esi
        add esp, 8
        retn
    }
}
void ETFTimeSwitch::initialize()
{
	WRITE_MEMORY(0xD00E6F, uint8_t, 0xEB);
	WRITE_NOP(0xD94BAF, 6);

	// crash fixes
	WRITE_MEMORY(0xD9767A, uint8_t, 0x30, 0xC0);
	WRITE_MEMORY(0x53AA38, uint8_t, 0x0D);
	WRITE_MEMORY(0x53AEB8, uint8_t, 0x0D);

	WRITE_JUMP(0x40EC09, GetCServiceGameplayMidAsmHook);


}
void ETFTimeSwitch::registerObject()
{
    BB_INSTALL_SET_OBJECT_MAKE_HOOK(ETFTimeSwitch)
}