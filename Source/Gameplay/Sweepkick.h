#pragma once
class Sweepkick
{
public:
    /// <summary>
    /// Installs the mid-ASM hooks.
    /// </summary>
    static void Install();

    static inline bool transitionToStand = true;
    static inline bool useSupportShockwave = true;
    
    static inline float supportShockwaveDelay = 0.2f;

    static inline float lightInDelay = 0.1f;
    static inline float lightLifeTime = 0.35f;

    static inline float lerpSpeedIn = 10.0f;
    static inline float lerpSpeedOut = 5.0f;

    static inline float lightRange = 1.5f;

    static inline Eigen::Vector3f Offset = Eigen::Vector3f(0.0f, 0.6f, 0.0f);

    static inline LightRGB Color = LightRGB(0.0f, 0.55f, 1.0f, 1.25f);

    static inline LightRGB ColorSuper = LightRGB(1.0f, 0.7f, 0.3f, 1.25f);

    static inline float sweepInputTime = 0.3f;

    static inline bool useLight = true;
};

