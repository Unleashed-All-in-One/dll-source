#pragma once

#include <Hedgehog/Base/Container/hhVector.h>
#include <Hedgehog/MirageCore/Renderable/hhSingleElementEffect.h>

namespace Hedgehog::Motion
{
    class CSingleElementEffectMotionAll2 : public CSingleElementEffectMotionAll
    {
    public:
        void AddTexcoordMotion(boost::shared_ptr<hh::mr::CModelData> const& in_ModelData, boost::shared_ptr<Hedgehog::Motion::CTexcoordAnimationData> const& in_TexcoordData)
        {
            BB_FUNCTION_PTR(void, __thiscall, fpCreateUVAnim, 0x7537E0,
                Hedgehog::Motion::CSingleElementEffectMotionAll2 * This,
                boost::shared_ptr<hh::mr::CModelData> const& modelData,
                boost::shared_ptr<Hedgehog::Motion::CTexcoordAnimationData> const& texCoordAnimData
            );
            fpCreateUVAnim(this, in_ModelData, in_TexcoordData);
        }
        void Update(float in_DeltaTime)
        {
            BB_FUNCTION_PTR(void, __thiscall, fpUpdateMotionAll, 0x752F00, Hedgehog::Motion::CSingleElementEffectMotionAll2 * This, float dt);
            fpUpdateMotionAll(this, in_DeltaTime);
        }
    };

    BB_ASSERT_OFFSETOF(CSingleElementEffectMotionAll, m_IsInForeground, 0x40);
    BB_ASSERT_OFFSETOF(CSingleElementEffectMotionAll, m_TexcoordMotionList, 0x44);
    BB_ASSERT_OFFSETOF(CSingleElementEffectMotionAll, m_MaterialMotionList, 0x54);
    BB_ASSERT_OFFSETOF(CSingleElementEffectMotionAll, m_TexpatternMotionList, 0x64);
    BB_ASSERT_OFFSETOF(CSingleElementEffectMotionAll, m_VisibilityMotionList, 0x74);
    BB_ASSERT_OFFSETOF(CSingleElementEffectMotionAll, m_MorphMotionList, 0x84);
    BB_ASSERT_SIZEOF(CSingleElementEffectMotionAll, 0xA0);
}