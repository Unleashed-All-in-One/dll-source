#pragma once
class FallCam
{
public:
    /// <summary>
    /// Installs the mid-ASM hooks.
    /// </summary>
    static void Install();

    static float AngleBetween(const Hedgehog::Math::CVector& a, const Hedgehog::Math::CVector& b) {
        float cosTheta = a.dot(b) / (a.norm() * b.norm());

        return acos(cosTheta);
    }
};