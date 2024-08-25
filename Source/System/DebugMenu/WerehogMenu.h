#pragma once
#include "../../Player/Evil/EvilAttackConfiguration.h"

namespace SUC::ImGuiMenu
{
    class WerehogMenu
    {
    public:
        static void Draw();
        static bool s_Visible;
        static inline SUC::Player::Evil::Motion* s_MotionDisplay;
        static inline hh::math::CVector s_AppliedVelocity;
        static inline float s_MoveSpeedVelocity;
    };	
}