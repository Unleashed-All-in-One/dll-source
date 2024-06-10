
class CObjCamera;
static constexpr int pCObjCameraCtor = 0x01052640;
//void fCObjCameraCtor(CObjCamera* This)
//{
//    __asm
//    {
//        mov esi, This
//        call[pCObjCameraCtor]
//    }
//}

class CObjCamera : public Sonic::CGameObject3D, public Sonic::CSetObjectListener
{
public:

    CObjCamera(const bb_null_ctor& nil) : CGameObject3D(nil), CSetObjectListener(nil) {}
    CObjCamera() : CGameObject3D(bb_null_ctor{})
    {
        __asm
        {
            mov esi, this
            call[pCObjCameraCtor]
        }
    }

    hh::map<void*, void*> m_CameraReferenceMap;
    int m_Field104;
    int m_Field108;
    int m_Field10C;
    int m_Field110;
    char m_Field114;
    char field_115;
    char IsCameraView;
    char field_117;
    float m_EaseTimeEnter;
    float m_EaseTimeLeave;
    char IsCollision;
    char field_121;
    char field_122;
    char field_123;
    int m_CameraSetObjectID;
};
BB_ASSERT_SIZEOF(CObjCamera, 0x128);