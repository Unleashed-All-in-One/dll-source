#pragma once

#include <BlueBlur.inl>

namespace hk2010_2_0_Conv
{
    // Typedefs - Taken from Havok header-only SDK
    //--------------------------------------------
    typedef unsigned short  hkUint16;
    typedef unsigned __w64 hk_size_t;

    // Forward declarations (only one so far)
    //---------------------------------------
    class hkpRigidBody;

    // Memory management
    //----------------------
    class hkMemoryRouter
    {
    public:
        BB_INSERT_PADDING(44);
        class Heap
        {
        public:
            BB_INSERT_PADDING(0x04);

            virtual ~Heap();

            /// Allocate numBytes.
            virtual void* blockAlloc(int numBytes) = 0;

            /// Free numBytes.
            virtual void blockFree(void* p, int numBytes) = 0;

        };

        Heap* m_heap;
        // Havok SDK method syntax
        Heap& heap() { return *m_heap; }
    };
    class hkMemoryTracker
    {
    protected:
        ~hkMemoryTracker() = default;

    public:
        /// New hkReferenced derived object
        virtual void onNewReferencedObject(const char* typeName, hk_size_t size, void* ptr) = 0;
        /// Delete hkReferenced derived object
        virtual void onDeleteReferencedObject(void* ptr) = 0;
    };

    inline hkMemoryRouter* getMemoryRouter()
    {
        uint32_t m_slotID = *(int*)0x01CFCBE0;
        return (hkMemoryRouter*)(unsigned long)TlsGetValue(m_slotID);
    }
    inline hkMemoryTracker* getTrackerInstancePtr()
    {
        BB_FUNCTION_PTR(void*, __cdecl, GET, 0x00898640);
        return (hkMemoryTracker*)GET();
    }


    // Basic Types
    //----------------------
    class hkReferencedObject
    {
        __int16 m_memSizeAndFlags{};
        __int16 m_referenceCount{};

    public:
        virtual ~hkReferencedObject() {}

        __forceinline static const char* getTypeIdentifier() { return __FUNCTION__; }

        __forceinline void* __cdecl operator new(hk_size_t nbytes)
        {
            hkReferencedObject* b = static_cast<hkReferencedObject*>(getMemoryRouter()->heap().blockAlloc(static_cast<int>(nbytes)));
            b->m_memSizeAndFlags = static_cast<hkUint16>(nbytes);
            if (hkMemoryTracker* _trk = getTrackerInstancePtr())
                _trk->onNewReferencedObject(getTypeIdentifier(), nbytes, b);
            return b;
        }

        __forceinline void __cdecl operator delete(void* p)
        {
            hkReferencedObject* b = static_cast<hkReferencedObject*>(p);
            if (hkMemoryTracker* _trk = getTrackerInstancePtr())
                _trk->onDeleteReferencedObject(b);
            getMemoryRouter()->heap().blockFree(p, b->m_memSizeAndFlags);
        }

        __forceinline void* __cdecl operator new(hk_size_t, void* p) { return p; }
        __forceinline void* __cdecl operator new[](hk_size_t, void* p)
        {
            __asm { int 3 };
            return p;
        }
        __forceinline void __cdecl operator delete(void*, void*)
        {
        }
        __forceinline void __cdecl operator delete[](void*, void*)
        {
            __asm { int 3 };
        }

        void addReference()
        {
            BB_FUNCTION_PTR(void, __thiscall, func, 0x008720D0, hkReferencedObject * This);
            func(this);
        }

        void removeReference()
        {
            BB_FUNCTION_PTR(void, __thiscall, func, 0x00872170, hkReferencedObject * This);
            func(this);
        }
    };

    template <typename T>
    class hkArray
    {
    public:
        T* m_ptr;
        uint32_t m_numElements;
        uint32_t m_maxElements;

        template <typename E>
        T* GetIndex(E i)
        {
            return (T*)((uint32_t)m_ptr + ((int)i * sizeof(T)));
        }
    };

    class hkFinishLoadedObjectFlag
    {
        //+hk.hkMemoryTracker(ignore=True)
    public:
        hkFinishLoadedObjectFlag() : m_finishing(0) {}
        int m_finishing;
    };

    class hkStringPtr
    {
    public:

        ///
        enum hkStringFlags
        {
            /// If we own it, we free it.
            OWNED_FLAG = 0x1,
        };

        const char* cString() const
        {
            return reinterpret_cast<char*>(unsigned long(m_stringAndFlag) & ~OWNED_FLAG);
        }

        /// m_stringAndFlag&~1 is the real pointer, m_stringAndFlag&1 is the deallocate flag.
        const char* m_stringAndFlag{};
    };


    // Animation
    //--------------------------

    class hkTransform
    {
    public:
        Hedgehog::Math::CVector     m_translation;
        Hedgehog::Math::CQuaternion m_rotation;
        Hedgehog::Math::CVector     m_scale;
    };

    class hkaBone
    {
    public:
        static const struct InternalClassMember Members[];

        __forceinline hkaBone() : m_lockTranslation(false) {}
        __forceinline hkaBone(const hkaBone& other)
        {
            *this = other;
        }
        __forceinline hkaBone& operator=(const hkaBone& other)
        {
            if (this == &other)
            {
                return *this;
            }
            m_name = other.m_name;
            m_lockTranslation = other.m_lockTranslation;
            return *this;
        }

        /// The name of the bone from the toolchain
        hkStringPtr m_name;

        /// Whether or not this bone can move relative to its parent
        int m_lockTranslation;
    };

    class hkaSkeleton
    {
    public:

        virtual ~hkaSkeleton() = default;

        unsigned short m_memSizeAndFlags;

        /// Reference count. Note that if m_memSizeAndFlags == 0,
        /// reference counting is disabled for this object.
        mutable signed short m_referenceCount;

        /// Get the local frame that is attached to a bone. Returns HK_NULL if there isn't one.
        void* getLocalFrameForBone(int boneIndex) const;

        //
        // Members
        //

            /// A user name to aid in identifying the skeleton
        hkStringPtr m_name;

        /// Parent relationship
        hkArray<signed short> m_parentIndices;

        /// Bones for this skeleton
        hkArray<hkaBone> m_bones;
    };

    // Required thing
    class hkaAnimationControlListener
    {
        //+hk.hkMemoryTracker(ignore=True)
    public:

        /// Called whenever the control is destroyed
        virtual void controlDeletedCallback(class hkaAnimationControl* control) {}

        /// Virtual destructor for derived objects
        virtual ~hkaAnimationControlListener() {}
    };

    class hkaAnimatedSkeleton : public hkReferencedObject, protected hkaAnimationControlListener
    {
    public:
        hkaAnimatedSkeleton(const hkaSkeleton* skeleton);

    private:
        // Array of controls. TODO: Investigate & implement their types.
        __declspec(align(16)) hkArray<void*> m_animationControls;

    public:
        /// The skeleton associated with all animations/controls.
        const hkaSkeleton* m_skeleton;

        inline const hkaSkeleton* getSkeleton()
        {
            return m_skeleton;
        }

        /// The fill threshold below which the reference pose will be blending in when the sum of the control weights is small.
        float m_referencePoseWeightThreshold;

        /// An hkaAnimatedSkeleton consisting of only hkaQuantizedAnimation can be executed more efficiently. Keep track of the number here.
        signed int m_numQuantizedAnimations;

    };

    // Collision
    // -----------------------------
    class hkpShape : public hkReferencedObject
    {
    public:
        enum ShapeType
        {
            HK_SHAPE_INVALID = 0x0,
            HK_SHAPE_SPHERE = 0x1,
            HK_FIRST_SHAPE_TYPE = 0x1,
            HK_SHAPE_CYLINDER = 0x2,
            HK_SHAPE_TRIANGLE = 0x3,
            HK_SHAPE_BOX = 0x4,
            HK_SHAPE_CAPSULE = 0x5,
            HK_SHAPE_CONVEX_VERTICES = 0x6,
            HK_SHAPE_COLLECTION = 0x7,
            HK_SHAPE_BV_TREE = 0x8,
            HK_SHAPE_LIST = 0x9,
            HK_SHAPE_MOPP = 0xA,
            HK_SHAPE_CONVEX_TRANSLATE = 0xB,
            HK_SHAPE_CONVEX_TRANSFORM = 0xC,
            HK_SHAPE_SAMPLED_HEIGHT_FIELD = 0xD,
            HK_SHAPE_EXTENDED_MESH = 0xE,
            HK_SHAPE_TRANSFORM = 0xF,
            HK_SHAPE_COMPRESSED_MESH = 0x10,
            HK_SHAPE_COMPOUND = 0x11,
            HK_SHAPE_MAX_ID_SPU = 0x12,
            HK_SHAPE_CONVEX = 0x13,
            HK_SHAPE_MOPP_EMBEDDED = 0x14,
            HK_SHAPE_CONVEX_PIECE = 0x15,
            HK_SHAPE_MULTI_SPHERE = 0x16,
            HK_SHAPE_CONVEX_LIST = 0x17,
            HK_SHAPE_TRIANGLE_COLLECTION = 0x18,
            HK_SHAPE_MULTI_RAY = 0x19,
            HK_SHAPE_HEIGHT_FIELD = 0x1A,
            HK_SHAPE_SPHERE_REP = 0x1B,
            HK_SHAPE_BV = 0x1C,
            HK_SHAPE_PLANE = 0x1D,
            HK_SHAPE_PHANTOM_CALLBACK = 0x1E,
            HK_SHAPE_USER0 = 0x1F,
            HK_SHAPE_USER1 = 0x20,
            HK_SHAPE_USER2 = 0x21,
            HK_SHAPE_MAX_ID = 0x22,
            HK_SHAPE_ALL = 0xFFFFFFFF,
        };

        uint32_t m_userData = 0;
        ShapeType m_shapeType = HK_SHAPE_INVALID;

    };

    class hkpSphereShape : public hkpShape
    {
    public:
        float m_Radius = 1.0f;
    private:
        int m_Field14{};
        int m_Field18{};
        int m_Field1C{};

    public:

        hkpSphereShape(float radius)
        {
            BB_FUNCTION_PTR(void*, __thiscall, ctor, 0x008C2D30, hkpSphereShape * This, float _radius);
            //m_Unk = 32;
            ctor(this, radius);
        }
    };
    BB_ASSERT_SIZEOF(hkpSphereShape, 0x20);

    class hkpBoxShape : public hkpShape
    {
    public:
        float m_ConvexShapeRadius = 0.050000001f;
        Hedgehog::Math::CVector m_HalfExtents{};

        hkpBoxShape(const Hedgehog::Math::CVector& halfExtents, float radius = 0.050000001f)
        {
            BB_FUNCTION_PTR(void*, __thiscall, ctor, 0x008CC880, hkpBoxShape * This, const Hedgehog::Math::CVector & _halfExtents, float _radius);
            ctor(this, halfExtents, radius);
        }
        hkpBoxShape(float Width, float Height, float Length, float radius = 0.050000001f)
        {
            BB_FUNCTION_PTR(void*, __thiscall, ctor, 0x008CC880, hkpBoxShape * This, const Hedgehog::Math::CVector & _halfExtents, float _radius);
            ctor(this, Hedgehog::Math::CVector(Width * 0.5f, Height * 0.5f, Length * 0.5f), radius);
        }
    };
    BB_ASSERT_SIZEOF(hkpBoxShape, 0x30);
#if defined(__INTEL_COMPILER)
#	define HK_COMPILER_INTEL
#endif
#if !defined(HK_COMPILER_INTEL)
#			define HK_FORCE_INLINE __forceinline
#endif
    class hkBool
    {
        //+hk.MemoryTracker(ignore=True)
    public:

        // used in compile time asserts
        typedef char CompileTimeTrueType;
        typedef int CompileTimeFalseType;

        HK_FORCE_INLINE hkBool()
        {
        }

        HK_FORCE_INLINE hkBool(bool b)
        {
            m_bool = static_cast<char>(b);
        }

        HK_FORCE_INLINE operator bool() const
        {
            return m_bool != 0;
        }

        HK_FORCE_INLINE hkBool& operator=(bool e)
        {
            m_bool = static_cast<char>(e);
            return *this;
        }

        HK_FORCE_INLINE hkBool operator==(bool e) const
        {
            return (m_bool != 0) == e;
        }

        HK_FORCE_INLINE hkBool operator!=(bool e) const
        {
            return (m_bool != 0) != e;
        }

    private:
        char m_bool;
    };
    template<typename ENUM, typename STORAGE>
    class hkEnum
    {
    public:

        hkEnum()
        {
        }

        hkEnum(ENUM e)
        {
            m_storage = static_cast<STORAGE>(e);
        }

        operator ENUM() const
        {
            return static_cast<ENUM>(m_storage);
        }
        void operator=(ENUM e)
        {
            m_storage = static_cast<STORAGE>(e);
        }
        hkBool operator==(ENUM e) const
        {
            return m_storage == static_cast<STORAGE>(e);
        }
        hkBool operator!=(ENUM e) const
        {
            return m_storage != static_cast<STORAGE>(e);
        }

    public:

        STORAGE m_storage;
    };
    /// Signed 8 bit integer
    typedef signed char		hkChar;
    /// Signed 8 bit integer
    typedef signed char		hkInt8;
    /// Signed 16 bit integer
    typedef signed short	hkInt16;
    /// Signed 32 bit integer
    typedef signed int		hkInt32;

    /// Unsigned 8 bit integer
    typedef unsigned char	hkUchar;
    /// Unsigned 8 bit integer
    typedef unsigned char	hkUint8;
    /// Unsigned 16 bit integer
    typedef unsigned short	hkUint16;
    /// Unsigned 32 bit integer
    typedef unsigned int	hkUint32;
    class hkHalf
    {
        //+hk.MemoryTracker(ignore=True)
    public:
        inline hkHalf() { }

        inline explicit hkHalf(const float& f)
        {
            int t = ((const int*)&f)[0];
            m_value = hkInt16(t >> 16);
        }

        inline hkHalf& operator=(const float& f)
        {
            int t = ((const int*)&f)[0];
            m_value = hkInt16(t >> 16);
            return *this;
        }

        inline void store(float* f) const
        {
            hkUint32* asInt = (hkUint32*)f;
            asInt[0] = m_value << 16;
        }

        inline operator float() const
        {
            union
            {
                int i;
                float f;
            } u;
            u.i = (m_value << 16);
            return u.f;
        }

        inline hkInt16 getInt16() const
        {
            return m_value;
        }

    private:
        hkInt16 m_value;
    };

    class hkpMotion : public hkReferencedObject
    {
    public:
        enum MotionType
        {
            /// 
            MOTION_INVALID,

            /// A fully-simulated, movable rigid body. At construction time the engine checks
            /// the input inertia and selects MOTION_SPHERE_INERTIA or MOTION_BOX_INERTIA as
            /// appropriate.
            MOTION_DYNAMIC,

            /// Simulation is performed using a sphere inertia tensor. (A multiple of the
            /// Identity matrix). The highest value of the diagonal of the rigid body's
            /// inertia tensor is used as the spherical inertia.
            MOTION_SPHERE_INERTIA,

            /// Simulation is performed using a box inertia tensor. The non-diagonal elements
            /// of the inertia tensor are set to zero. This is slower than the
            /// MOTION_SPHERE_INERTIA motions, however it can produce more accurate results,
            /// especially for long thin objects.
            MOTION_BOX_INERTIA,

            /// Simulation is not performed as a normal rigid body. During a simulation step,
            /// the velocity of the rigid body is used to calculate the new position of the
            /// rigid body, however the velocity is NOT updated. The user can keyframe a rigid
            /// body by setting the velocity of the rigid body to produce the desired keyframe
            /// positions. The hkpKeyFrameUtility class can be used to simply apply keyframes
            /// in this way. The velocity of a keyframed rigid body is NOT changed by the
            /// application of impulses or forces. The keyframed rigid body has an infinite
            /// mass when viewed by the rest of the system.
            MOTION_KEYFRAMED,

            /// This motion type is used for the static elements of a game scene, e.g., the
            /// landscape. Fixed rigid bodies are treated in a special way by the system. They
            /// have the same effect as a rigid body with a motion of type MOTION_KEYFRAMED
            /// and velocity 0, however they are much faster to use, incurring no simulation
            /// overhead, except in collision with moving bodies.
            MOTION_FIXED,

            /// A box inertia motion which is optimized for thin boxes and has less stability problems
            MOTION_THIN_BOX_INERTIA,

            /// A specialized motion used for character controllers
            MOTION_CHARACTER,

            /// 
            MOTION_MAX_ID
        };
    public:
        inline MotionType getType() const { return m_type; }
        hkEnum<MotionType, hkUint8>	m_type;
        hkUint8		m_deactivationIntegrateCounter;

        // the number of frames this object is inactive
        hkUint16	m_deactivationNumInactiveFrames[2];

        void* m_motionState;

        // inverted inertia (xyz) and inverted mass (w)
        Hedgehog::Math::CVector4 m_inertiaAndMassInv;

        // ------------------ 2nd CacheLine64 -------------------------
        // velocity data (used by buildAccumulator)
        Hedgehog::Math::CVector4		m_linearVelocity;
        Hedgehog::Math::CVector4		m_angularVelocity;

        // the reference position for the deactivator. .w component is the max velocity squared since last setting this position
        // to deactivate deactivation, simply set m_deactivationIntegrateCounter to 0xff
        Hedgehog::Math::CVector4 m_deactivationRefPosition[2];

        // the packed m_deactivationRefOrientation (stored in the motionState motion (so that sizeof(hkpEntity) < 512))
        uint32_t m_deactivationRefOrientation[2];

        // Stored _DYNAMIC_ motion
        class hkpMaxSizeMotion* m_savedMotion; //+hk.DataObjectType("hkpMotion")

        // Stored quality type of the rigid body, which refers to its saved _DYNAMIC_ motion.
        hkUint16 m_savedQualityTypeIndex;

        // Scale the world gravity to disable gravity or reverse the direction
        hkHalf m_gravityFactor;
    };
    class hkpKeyframedRigidMotion : public hkpMotion
    {

    };
    enum hkpEntityActivation
    {
        /// Tries to add the body in an inactive state. However if the body overlaps with an
        /// active body, it gets activated
        HK_ENTITY_ACTIVATION_DO_NOT_ACTIVATE,

        /// Always activate this body and overlapping bodies
        HK_ENTITY_ACTIVATION_DO_ACTIVATE
    };
    class hkpMaxSizeMotion : public hkpKeyframedRigidMotion
    {
    public:
        //HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
        ////+vtable(1)
        //HK_DECLARE_REFLECTION();
        //
         //hkpMaxSizeMotion() : hkpKeyframedRigidMotion() {}
         //hkpMaxSizeMotion(class hkFinishLoadedObjectFlag flag) : hkpKeyframedRigidMotion(flag) {}
    private:
        void operator=(const hkpMaxSizeMotion& other);
        hkpMaxSizeMotion(const hkpMaxSizeMotion& other);
    };
    enum hkpUpdateCollisionFilterOnEntityMode
    {
        /// Do a full check
        HK_UPDATE_FILTER_ON_ENTITY_FULL_CHECK,

        /// Recheck filter but only check for disabled entity-entity collisions
        /// which have been enabled before
        HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY
    };
    class hkpRigidBody : public hkReferencedObject
    {
        class hkpCollideable
        {
        public:
            hkpShape* m_Shape{};
            uint32_t m_ShapeKey{};
            const void* m_Motion;
            const hkpCollideable* m_Parent{};
        };

    public:
        int m_Unk1;
        int m_Unk2;
        hkpCollideable m_Collideable;
        BB_INSERT_PADDING(200 - 16);
        hkpMotion m_Motion;
        //hk2010_2_0::hkEnum<hk2010_2_0::hkpMotion::MotionType, hk2010_2_0::hkUint8> m_MotionType;


    };
    BB_ASSERT_OFFSETOF(hkpRigidBody, m_Collideable, 0x10);
    BB_ASSERT_OFFSETOF(hkpRigidBody, m_Motion, 224);
}