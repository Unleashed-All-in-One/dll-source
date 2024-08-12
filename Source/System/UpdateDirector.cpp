#include "UpdateDirector.h"

FUNCTION_PTR(void*, __thiscall, fun662010, 0x662010, void* This, Hedgehog::Base::CSharedString* value);
FUNCTION_PTR(void*, __thiscall, fun77AC20, 0x77AC20, void* This, uint32_t a2);
FUNCTION_PTR(void*, __thiscall, fun77ACC0, 0x77ACC0, void* This, uint32_t a2);

FUNCTION_PTR(void*, __thiscall, fun6F8B10, 0x6F8B10, void* This);
FUNCTION_PTR(void*, __thiscall, fun6F8FB0, 0x6F8FB0, void* This);

FUNCTION_PTR(void*, __thiscall, funD0AEE0, 0xD0AEE0, void* This);
FUNCTION_PTR(void*, __thiscall, fun6F8AF0, 0x6F8AF0, void* This);

uint32_t funD67820FunctionAddress = 0xD67820;
uint32_t funD66F90FunctionAddress = 0xD66F90;

class CUpdateDirector
{
public:
    class CMember
    {
    public:
        BB_INSERT_PADDING(0x24);
        void* m_Field24;
        BB_INSERT_PADDING(0xC);
        void* m_Field34;
        void* m_Field38;
        BB_INSERT_PADDING(0x94);
        void* m_FieldD0;
        BB_INSERT_PADDING(0x4);
        void* m_FieldD8; 
    };
    BB_INSERT_PADDING(0x4);
    CMember* m_pMember;
};
BB_ASSERT_OFFSETOF(CUpdateDirector, m_pMember, 0x4);
BB_ASSERT_OFFSETOF(CUpdateDirector::CMember, m_Field24, 0x24);
BB_ASSERT_OFFSETOF(CUpdateDirector::CMember, m_Field34, 0x34);
BB_ASSERT_OFFSETOF(CUpdateDirector::CMember, m_Field38, 0x38);
BB_ASSERT_OFFSETOF(CUpdateDirector::CMember, m_FieldD0, 0xD0);
BB_ASSERT_OFFSETOF(CUpdateDirector::CMember, m_FieldD8, 0xD8);
void UpdateDirector::AddUpdateCommand(void* context, const float elapsedTime, const char* name)
{
    
    Hedgehog::Base::CSharedString string(name);

    fun662010((void*)(*(uint32_t*)((uint32_t)context + 4) + 216), &string);
    *(float*)(*(uint32_t*)((uint32_t)context + 4) + 208) = elapsedTime;
    uint32_t v3 = *(uint32_t*)((uint32_t)context + 4);
    if (*(uint32_t*)(v3 + 36))
        fun77AC20(*(uint32_t**)(v3 + 36), v3 + 208);
}

void UpdateDirector::AddRenderCommand(void* context, const float elapsedTime, const char* name)
{
    Hedgehog::Base::CSharedString string(name);

    fun662010((void*)(*(uint32_t*)((uint32_t)context + 4) + 216), &string);
    *(float*)(*(uint32_t*)((uint32_t)context + 4) + 208) = elapsedTime;
    uint32_t v3 = *(uint32_t*)((uint32_t)context + 4);
    if (*(uint32_t*)(v3 + 36))
        fun77ACC0(*(uint32_t**)(v3 + 36), v3 + 208);
}

void UpdateDirector::AddUpdateAndRenderCommand(void* context, const float elapsedTime, const char* name)
{
    UpdateDirector::AddUpdateCommand(context, elapsedTime, name);
    UpdateDirector::AddRenderCommand(context, elapsedTime, name);
}

void UpdateDirector::CallUnknownFunction(void* context, float elapsedTime)
{
    funD0AEE0(*(uint32_t**)(*(uint32_t*)((uint32_t)context + 4) + 52));

    __asm
    {
        mov eax, context
        push elapsedTime
        call [funD66F90FunctionAddress]
    }

    fun6F8AF0(*(uint32_t**)(*(uint32_t*)((uint32_t)context + 4) + 56));
}

void* UpdateDirector::FinishUpdater(void* context)
{
    return fun6F8B10(*(uint32_t**)(*(uint32_t*)((uint32_t)context + 4) + 56));
}

void* UpdateDirector::FinishRenderer(void* context)
{
    __asm
    {
        mov esi, context
        call [funD67820FunctionAddress]
    }

    if (*(bool*)0x1A430EC)
        return fun6F8FB0(*(uint32_t**)(*(uint32_t*)((uint32_t)context + 4) + 56));

    return nullptr;
}