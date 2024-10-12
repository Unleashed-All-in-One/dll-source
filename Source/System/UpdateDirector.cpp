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


void CUpdateDirector::AddUpdateCommand(const float elapsedTime, const char* name) const
{
    Hedgehog::Base::CSharedString string(name);
    this->m_pMember->m_FieldD8 = string;
    this->m_pMember->m_FieldD0 = elapsedTime;
    //fun662010((void*)(*(uint32_t*)((uint32_t)context + 4) + 216), &string);
    //*(float*)(*(uint32_t*)((uint32_t)context + 4) + 208) = elapsedTime;
    //uint32_t v3 = *(uint32_t*)((uint32_t)context + 4);
    if (this->m_pMember->m_Field24)
        fun77AC20(this->m_pMember->m_Field24, this->m_pMember->m_FieldD0);
}

void CUpdateDirector::AddRenderCommand(const float elapsedTime, const char* name) const
{
    Hedgehog::Base::CSharedString string(name);
    this->m_pMember->m_FieldD8 = string;
    this->m_pMember->m_FieldD0 = elapsedTime;
    if (this->m_pMember->m_Field24)
        fun77ACC0(this->m_pMember->m_Field24, this->m_pMember->m_FieldD0);
}

void CUpdateDirector::AddUpdateAndRenderCommand(const float elapsedTime,
	const char* name)

{
    AddUpdateCommand(elapsedTime, name);
    AddRenderCommand(elapsedTime, name);
}

void CUpdateDirector::CallUnknownFunction(float elapsedTime) const
{
	funD0AEE0(this->m_pMember->m_Field34);

	__asm
		{
		mov eax, this
		push elapsedTime
		call[funD66F90FunctionAddress]
		}

	fun6F8AF0(this->m_pMember->m_Field38);
}

void* CUpdateDirector::FinishUpdater() const
{
    return fun6F8B10(this->m_pMember->m_Field38);
}

void* CUpdateDirector::FinishRenderer() const
{
    __asm
    {
        mov esi, this
        call[funD67820FunctionAddress]
    }

    if (ms_IsUpdateForRender)
        return fun6F8FB0(this->m_pMember->m_Field38);

    return nullptr;
}