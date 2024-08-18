#pragma once

#include "stdint.h"

#include "rw/rwplcore.h"
#include "rw/rpworld.h"
#include "menu/MenuStructs.h"
#include "menu/SimpleGTA.h"

class CMatrix
{
public:
    union
    {
        float f[4][4];
        struct
        {
            float rx, ry, rz, rw;
            float fx, fy, fz, fw;
            float ux, uy, uz, uw;
            float px, py, pz, pw;
        };
        struct
        {
            CVector      right;
            unsigned int flags;
            CVector      up;
            unsigned int pad1;
            CVector      at;
            unsigned int pad2;
            CVector      pos;
            unsigned int pad3;
        };
        struct // RwV3d style
        {
            CVector      m_right;
            unsigned int m_flags;
            CVector      m_forward;
            unsigned int m_pad1;
            CVector      m_up;
            unsigned int m_pad2;
            CVector      m_pos;
            unsigned int m_pad3;
        };
    };

    void* m_pAttachedMatrix;
    bool         m_bOwnsAttachedMatrix;
    char         matrixpad[3];
};


// Simple entities
class CSimpleTransform
{
public:
    CVector pos;
    float   heading;
};
class CPlaceable
{
protected:
    CPlaceable();
public:
    char padding[4];
    CSimpleTransform m_placement;
    CMatrix *m_matrix;

    CSimpleTransform* GetPlacement()
    {
        return (CSimpleTransform*)((uintptr_t)this + 4);
    }
    CMatrix* GetMatrix()
    {
        return *(CMatrix**)((uintptr_t)this + 20);
    }

    CVector* GetPosSA()
    {
        auto mat = *(CMatrix**)((uintptr_t)this + 20);
        if (mat)
        {
            return &mat->pos;
        }
        return &((CSimpleTransform*)((uintptr_t)this + 4))->pos;
    }
    CVector* GetPosVC()
    {
        return (CVector*)((uintptr_t)this + 52);
    }
    CMatrix* GetMatSA()
    {
        return *(CMatrix**)((uintptr_t)this + 20);
    }
    CMatrix* GetMatVC()
    {
        return (CMatrix*)((uintptr_t)this + 4);
    }
    CMatrix* GetCamMatVC()
    {
        return (CMatrix*)this;
    }
};

class CEntity : public CPlaceable {
protected:
    CEntity();  
public:
    union {
        struct RwObject *m_pRwObject;
        struct RpClump *m_pRwClump;
        struct RpAtomic *m_pRwAtomic;
    };
};

class CCamera : public CPlaceable {};
class CPed : public CPlaceable {};
class CVehicle : public CEntity {};
class CObject : public CPlaceable {};

static RwFrame* GetObjectParent(RwObject *obj) {
    return reinterpret_cast<RwFrame *>(rwObjectGetParent(obj));
}
