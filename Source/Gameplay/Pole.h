#pragma once
class Pole
{
public:
    static void applyPatches();

    static inline SharedPtrTypeless swingHandle;
    static inline bool locusPlayed = false;
    static inline bool trailPlayed = false;

    static inline const char* GetPoleSwingParticle() {
        Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();
        
        if (!sonic)
            return "";

        if (sonic->m_Is2DMode)
            return *pSuperSonicContext ? "ef_ch_sps_lms_bar_trace02" : "ef_ch_sng_lms_bar_trace02";
        else
            return *pSuperSonicContext ? "ef_ch_sps_lms_bar_trace01" : "ef_ch_sng_lms_bar_trace01";
    }

    static inline void LocusUpdate() {
        if (IsSwingJump() && !locusPlayed) {
            locusPlayed = true;
            Common::SonicContextRequestLocusEffect();
        }
        else if (!IsSwingJump() && locusPlayed)
            locusPlayed = false;
    }

    static inline void PlayTrail() {
        Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();

        if (!sonic)
            return;

        if (!Common::CheckPlayerNodeExist("Foot_L"))
            return;

        boost::shared_ptr<Hedgehog::Mirage::CMatrixNodeSingleElementNode> FootNode = sonic->m_pPlayer->m_spCharacterModel->GetNode("Foot_L");
        Common::fCGlitterCreate(sonic, swingHandle, &FootNode, GetPoleSwingParticle(), 1);
    }

    static inline void TrailUpdate() {
        Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();
        
        if (!sonic)
            return;

        if (!Common::CheckPlayerNodeExist("Foot_L"))
            return;
       
        if (Common::SonicContextIsAnimation("PoleSpinLoop") && !trailPlayed) {
            PlayTrail();
            trailPlayed = true;
        }
        else if (!Common::SonicContextIsAnimation("PoleSpinLoop") && trailPlayed) {
            Common::fCGlitterEnd(sonic, swingHandle, true);
            trailPlayed = false;
        }
    }

    static inline bool IsSwingJump() {
        return strstr(Common::SonicContextGetAnimationName(), "PoleSpinJump");
    }
};