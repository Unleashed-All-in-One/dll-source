#pragma once
class ShortJump
{
public:
    /// <summary>
    /// Installs the mid-ASM hooks.
    /// </summary>
    static void Install();

    static inline float hurdleTime = 0.75f;
    static inline float jumpTime = 0.15f;
};