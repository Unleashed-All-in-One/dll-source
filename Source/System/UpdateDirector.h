#pragma once
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
        float m_FieldD0;
        BB_INSERT_PADDING(0x4);
        Hedgehog::Base::CSharedString m_FieldD8;
    };
    BB_INSERT_PADDING(0x4);
    CMember* m_pMember;
    void AddUpdateCommand(float elapsedTime, const char* name) const;
    void AddRenderCommand(float elapsedTime, const char* name) const;
    void AddUpdateAndRenderCommand(float elapsedTime, const char* name);
    void CallUnknownFunction(float elapsedTime) const;
    void* FinishUpdater() const;
    void* FinishRenderer() const;
};
BB_ASSERT_OFFSETOF(CUpdateDirector, m_pMember, 0x4);
BB_ASSERT_OFFSETOF(CUpdateDirector::CMember, m_Field24, 0x24);
BB_ASSERT_OFFSETOF(CUpdateDirector::CMember, m_Field34, 0x34);
BB_ASSERT_OFFSETOF(CUpdateDirector::CMember, m_Field38, 0x38);
BB_ASSERT_OFFSETOF(CUpdateDirector::CMember, m_FieldD0, 0xD0);
BB_ASSERT_OFFSETOF(CUpdateDirector::CMember, m_FieldD8, 0xD8);
