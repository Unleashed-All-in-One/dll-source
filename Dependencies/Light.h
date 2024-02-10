#pragma once
#include <BlueBlur.h>

class LightRGB {
public:
    float r;
    float g;
    float b;
    float scalar = 1.0f;

    LightRGB(float _r = 0.0f, float _g = 0.0f, float _b = 0.0f, float _s = 0.0f) {
        r = _r;
        g = _g;
        b = _b;
        scalar = _s;
    }
};

class SpawnableLight {
private:
    LightRGB currentColor = LightRGB();
    float currentFadeSpeed = 0.0f;
    float currentLifeTime = -69.0f;
    float currentStartDelay = -69.0f;

    float lerp(float a, float b, float f)
    {
        return a * (1.0f - f) + (b * f);
    }

    enum LightState {
        StateDelay,
        StateLifeTime,
        StateStop
    };

public:

    Hedgehog::Math::CVector Position;

    LightRGB Color = LightRGB();

    float Range = 0.0f;

    bool Enabled = true;

    hh::base::CRefPtr<Sonic::CLocalLight> Light = nullptr;
    
    float FadeOutSpeed = 0.0f;
    float FadeInSpeed = 0.0f;

    float LifeTime = 0.0f;
   
    float StartDelay = 0.0f;

    LightState CurrentState;

    LightRGB LerpColor(LightRGB a, LightRGB b, float f) {
        return LightRGB(lerp(a.r, b.r, f), lerp(a.g, b.g, f), lerp(a.b, b.b, f), lerp(a.scalar, b.scalar, f));
    }

    Hedgehog::Math::CVector playerLocalPos(Eigen::Vector3f vec) {
        Sonic::Player::CPlayerSpeedContext* sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();

        if (!sonic)
            return Hedgehog::Math::CVector::Zero();

        Hedgehog::Math::CVector playerRight = sonic->m_spMatrixNode->m_Transform.m_Rotation * Hedgehog::Math::CVector::UnitX();
        Hedgehog::Math::CVector playerUp = sonic->m_spMatrixNode->m_Transform.m_Rotation * Hedgehog::Math::CVector::UnitY();
        Hedgehog::Math::CVector playerForward = sonic->m_spMatrixNode->m_Transform.m_Rotation * Hedgehog::Math::CVector::UnitZ();

        Hedgehog::Math::CVector offsetVector = playerRight * vec[0] +
            playerUp * vec[1] +
            playerForward * vec[2];

        return sonic->m_spMatrixNode->m_Transform.m_Position + offsetVector;
    }

    void Play() {
        Hedgehog::Base::TSynchronizedPtr gameDoc = Sonic::CGameDocument::GetInstance();
        
        if (!gameDoc)
            return;

        Enabled = true;

        currentColor = LightRGB();
        currentFadeSpeed = FadeInSpeed;
        currentStartDelay = StartDelay;

        CurrentState = StateDelay;
        
        if (Light == nullptr)
            Light = gameDoc->m_pMember->m_spLightManager->AddLocalLight(Position, { currentColor.r, currentColor.g, currentColor.b, 1.0f }, { 0.0f, 0.0f, 0.0f, Range });
    }

    void Stop(bool instant = false) {
        Enabled = false;
        currentFadeSpeed = FadeOutSpeed;
        CurrentState = StateStop;
       
        if (instant) {
            currentColor = LightRGB();
            Light = nullptr;
        }
    }

    void StateUpdate(float delta) {
        switch (CurrentState) {
            case StateLifeTime:
                if (currentLifeTime > 0.0f)
                    currentLifeTime -= delta;
                else if (currentLifeTime <= 0.0f)
                    Stop();
                break;
            
            case StateDelay:
                if (currentStartDelay > 0.0f)
                    currentStartDelay -= delta;
                else if (currentStartDelay <= 0.0f) {
                    currentLifeTime = LifeTime;
                    CurrentState = StateLifeTime;
                }
                break;
            
            default:
                break;
        }
    }

    void Update(float delta) {
        if (Light == nullptr)
            return;

        Light->SetPosition(Position);
        Light->m_spLight->m_Color = Hedgehog::Math::CVector4(currentColor.r, currentColor.g, currentColor.b, 1.0f);

        if (CurrentState != StateDelay)
            currentColor = LerpColor(currentColor, (Enabled ? Color : LightRGB()), delta * currentFadeSpeed);
        
        StateUpdate(delta);
    }
};
