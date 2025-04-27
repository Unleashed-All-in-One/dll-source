#include "GenericSonic.h"

namespace SUC::Accuracy 
{
    class CScriptManagerImpl : public Hedgehog::Universe::CService
    {
    public:
        HH_FND_MSG_MAKE_SERVICE(0x015B7E60, 8);
        void* m_pLuanneCore;	    
    };
    class CServicePlayerDebug : public Hedgehog::Universe::CService
    {
    public:
        HH_FND_MSG_MAKE_SERVICE(0x015B8118, 15);
        BB_INSERT_PADDING(0xC);
        hh::math::CQuaternion m_Field16;
        int m_Field32;
        Hedgehog::base::CSharedString m_Field36;
        int m_Field40;
        Hedgehog::base::CSharedString m_Field44;
        bool m_Field48;
        bool m_Field49;
        bool m_Field50;

    };
    BB_ASSERT_OFFSETOF(CServicePlayerDebug, m_Field44, 44);
    class CServiceSoundHandle : public Hedgehog::Universe::CService
    {public:
        HH_FND_MSG_MAKE_SERVICE(0x015CC07C, 7);
        int m_PlayerID;
        int m_Field04;
        int m_Field08;	    
    };
    
	HOOK(void*, __fastcall, G_InitializePlayer, 0x00D96110, void* This)
	{
		void* result = originalG_InitializePlayer(This);
		auto context = Sonic::Player::CPlayerSpeedContext::GetInstance();    // Hack: there's a better way to do this but whatever. This writes to the singleton anyway.
        auto p = (uint32_t*)Sonic::CApplicationDocument::GetInstance()->m_pMember;
        auto t = (uint32_t*)(p + 0xC0) + 4;
       //auto f = Sonic::Player::CPlayerSpeedContext::GetInstance()->m_spParameter->Get<float>(Sonic::Player::ePlayerSpeedParameter_BoostTimeDecrease);
		//Sonic::Player::CPlayerSpeedContext::GetInstance()->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_BoostTimeDecrease] = 15.0f;
       //auto debug = Sonic::CApplicationDocument::GetInstance()->GetService<CServicePlayerDebug>();
       //for (auto const& [key, val] : Sonic::CApplicationDocument::GetInstance()->m_ServiceMap)
       //{
       //    if(val->Is<CServicePlayerDebug>())
       //    {
       //        auto valget = (CServicePlayerDebug*)val.get();
       //        printf("");
       //
       //    }
       //}
		return result;
	}
    struct MsgChangePlayerAlt : public hh::fnd::MessageTypeSet
    {
    public:
        HH_FND_MSG_MAKE_TYPE(0x0167F870);

        size_t m_PlayerType;

        MsgChangePlayerAlt(size_t playerType) : m_PlayerType(playerType) {}
    };
    HOOK(void*, __fastcall, G_UpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
    {
        if(GetAsyncKeyState(VK_F10))
        {
            ObjectUtility::SendCategoryMessage("Player", boost::make_shared<MsgChangePlayerAlt>(1));
        }
        return originalG_UpdateApplication(This,Edx,elapsedTime,a3);
    }
	void GenericSonic::RegisterHooks()
	{
        //Patch out check for white world stage id for player changer
        //WRITE_JUMP(0x00D00E65, 0x00D00E76);
        INSTALL_HOOK(G_InitializePlayer);
        INSTALL_HOOK(G_UpdateApplication);
	}

}
