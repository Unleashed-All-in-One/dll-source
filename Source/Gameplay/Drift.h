#pragma once
class Drift
{
public:
    /// <summary>
    /// Installs the mid-ASM hooks.
    /// </summary>
    static void Install();

    static bool CheckForBDrift() {
        Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();
        
        if (!sonic)
            return false;
        
        return abs(sonic->m_Velocity.norm()) >= 25.0f && !sonic->m_Is2DMode && !Common::IsPlayerInForwardPath() && !Common::IsPlayerControlLocked() && !Common::IsPlayerGrinding() && !Common::IsPlayerOnBoard();
    }
};