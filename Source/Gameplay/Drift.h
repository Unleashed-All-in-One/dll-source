#pragma once
class Drift
{
public:
    static void applyPatches();

    static bool checkForBDrift() {
        Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();
        
        if (!sonic)
            return false;
        
        return abs(sonic->m_Velocity.norm()) >= 25.0f && !sonic->m_Is2DMode && !Common::IsPlayerInForwardPath() && !Common::IsPlayerControlLocked() && !Common::IsPlayerGrinding() && !Common::IsPlayerOnBoard();
    }
};