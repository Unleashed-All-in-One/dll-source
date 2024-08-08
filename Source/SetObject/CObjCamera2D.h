namespace Sonic::Message
{
    class MsgGetCameraDefaultFovy : public Hedgehog::Universe::MessageTypeGet
    {
    public:
        HH_FND_MSG_MAKE_TYPE(0x01682250);
        float m_DefaultFOV;
    };
}
class CObjCamera2D : public CObjCamera
{
public:
    BB_SET_OBJECT_MAKE("ObjCamera2D");

    struct SCamParams2D
    {
        char IsBaseSpacePlayer;
        char IsPositionBasePlayer;
        char field_152;
        char field_153;
        float BaseSpacePathPosition;
        int m_Field158;
        int m_Field15C;
        Hedgehog::Math::CVector DistancceRotationYZ;
        float TargetUpOffset;
        float TargetFrontOffsetBias;
        float TargetFrontOffsetSpeedScale;
        int TargetFrontOffsetMax;
    };

    CObjCamera2D()
    {
        // VTable assignments.
        int* This = (int*)this;
        This[0] = 0x016AB2FC;
        This[0x28 / 4] = 0x016AB368;
        This[0xAC / 4] = 0x016AB37C;
        This[0xF4 / 4] = 0x016AB38C;

        // This is what SU does, so just copy it over.

        m_Params.IsBaseSpacePlayer = 1;
        m_Params.BaseSpacePathPosition = 24.0f;
        m_Params.IsPositionBasePlayer = 1;
        m_Params.DistancceRotationYZ.x() = 8.0f;
        m_Params.DistancceRotationYZ.y() = 0.0f;
        m_Params.DistancceRotationYZ.z() = -0.031399999f;
        *(m_Params.DistancceRotationYZ.data() + 1) = 0.0; // can probably set this cleaner...
        m_Params.TargetUpOffset = 2.0f;
        m_Params.TargetFrontOffsetBias = 1.8f;
        m_Params.TargetFrontOffsetSpeedScale = 0.1f;
        m_Params.TargetFrontOffsetMax = 4.0f;

        m_spMsgGetCameraDefaultFovy = boost::make_shared<Sonic::Message::MsgGetCameraDefaultFovy>();
        m_spMsgGetCameraDefaultFovy->m_DefaultFOV = 45.0f;
    }

    boost::shared_ptr<Hedgehog::Mirage::CSingleElement> m_spElementDebug;
    int m_Field130;
    int m_Field134;
    int m_Field138;
    boost::shared_ptr<Sonic::Message::MsgGetCameraDefaultFovy> m_spMsgGetCameraDefaultFovy;
    SCamParams2D m_Params;
    static void RegisterObject();
};
BB_SET_OBJECT_MAKE_HOOK(CObjCamera2D)
BB_ASSERT_OFFSETOF(CObjCamera2D, m_Params, 0x150);