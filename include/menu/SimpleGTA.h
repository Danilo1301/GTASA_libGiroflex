#pragma once

class CVector {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    CVector(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    CVector operator+(const CVector right)
    {
        CVector result(this->x, this->y, this->z);
        result.x += right.x;
        result.y += right.y;
        result.z += right.z;
        return result;
    }
};

class CVector2D {
public:
    float x = 0.0f;
    float y = 0.0f;

    CVector2D(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    CVector2D operator+(const CVector2D vec)
    {
        CVector2D result(this->x, this->y);
        result.x += vec.x;
        result.y += vec.y;
        return result;
    }

    CVector2D operator-(const CVector2D vec)
    {
        CVector2D result(this->x, this->y);
        result.x -= vec.x;
        result.y -= vec.y;
        return result;
    }
};

class CRGBA {
public:
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;

    CRGBA(unsigned char r, unsigned char g, unsigned char b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = 255;
    }

    CRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
};

class CRect {
public:
    float left = 0;          // x1
    float bottom = 0;        // y1
    float right = 0;         // x2
    float top = 0;           // y2

    CRect(float left, float bottom, float right, float top)
    {
        this->left = left;
        this->bottom = bottom;
        this->right = right;
        this->top = top;
    }
};

class CSprite2d {
public:
	// class variables
	struct RwTexture *m_pTexture;
	// static variables
	//static unsigned int& nextBufferIndex;
	//static unsigned int& nextBufferVertex;
	//static float& NearScreenZ;
	//static float& RecipNearClip;
	// count: 8
	//static struct RwD3D9Vertex *maVertices;
	// class functions
	//CSprite2d();
	//~CSprite2d();
};

enum eFontAlignment : unsigned char {
    ALIGN_CENTER,
    ALIGN_LEFT,
    ALIGN_RIGHT
};

enum eFontStyle : unsigned char {
    FONT_GOTHIC,
    FONT_SUBTITLES,
    FONT_MENU,
    FONT_PRICEDOWN
};