
#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <assimp/Importer.hpp>

bool ReadFile(const char* fileName, std::string& outFile);
long long GetCurrentTimeMillis();

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

#define ToRadian(x) ((x) * M_PI / 180.0f)
#define ToDegree(x) ((x) * 180.0f / M_PI)

#define ZERO_MEM(a) memset(a, 0, sizeof(a))

#define INVALID_UNIFORM_LOCATION 0xffffffff
#define INVALID_OGL_VALUE 0xffffffff

struct Vector2i {
	int x;
	int y;

	Vector2i() {}
	Vector2i(int _x, int _y)
	{
		x = _x;
		y = _y;
	}
};

struct Vector2f {
	float x;
	float y;

	Vector2f() {}
	Vector2f(float _x, float _y)
	{
		x = _x;
		y = _y;
	}
};

struct Vector3f {
	float x;
	float y;
	float z;

	Vector3f() {}

	Vector3f(float f)
    {
        x = y = z = f;
    }

    Vector3f(const float* pFloat)
    {
        x = pFloat[0];
        y = pFloat[0];
        z = pFloat[0];
    }

	Vector3f(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f Cross(const Vector3f& v);
	Vector3f& Normalize();
	void Rotate(float Angle, const Vector3f& Axis);

	Vector3f& operator+=(const Vector3f& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;

        return *this;
    }

    Vector3f& operator-=(const Vector3f& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;

        return *this;
    }

    Vector3f operator+(const Vector3f& r)
	{
	    Vector3f Ret(x + r.x,
	                 y + r.y,
	                 z + r.z);

	    return Ret;
	}

	Vector3f operator-(const Vector3f& r)
	{
	    Vector3f Ret(x - r.x,
	                 y - r.y,
	                 z - r.z);

	    return Ret;
	}

    Vector3f& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;

        return *this;
    }

    Vector3f operator*(float f)
    {
    	Vector3f v;
    	v.x = x * f;
    	v.y = y * f;
    	v.z = z * f;
    	return v;
    }
};

struct Quaternion
{
    float x, y, z, w;

    Quaternion(float _x, float _y, float _z, float _w);

    void Normalize();

    Quaternion Conjugate();  
    
    Vector3f ToDegrees();

	Quaternion operator*(const Quaternion& r)
	{
	    const float _w = (w * r.w) - (x * r.x) - (y * r.y) - (z * r.z);
	    const float _x = (x * r.w) + (w * r.x) + (y * r.z) - (z * r.y);
	    const float _y = (y * r.w) + (w * r.y) + (z * r.x) - (x * r.z);
	    const float _z = (z * r.w) + (w * r.z) + (x * r.y) - (y * r.x);

	    Quaternion ret(_x, _y, _z, _w);

	    return ret;
	}

	Quaternion operator*(const Vector3f& v)
	{
	    const float _w = - (x * v.x) - (y * v.y) - (z * v.z);
	    const float _x =   (w * v.x) + (y * v.z) - (z * v.y);
	    const float _y =   (w * v.y) + (z * v.x) - (x * v.z);
	    const float _z =   (w * v.z) + (x * v.y) - (y * v.x);

	    Quaternion ret(_x, _y, _z, _w);

	    return ret;
	}
 };

class Matrix4f {
public:
	float m[4][4];

	Matrix4f() {}

	Matrix4f(const aiMatrix4x4& AssimpMatrix)
    {
        m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
        m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
        m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
        m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;
    }
    
    Matrix4f(const aiMatrix3x3& AssimpMatrix)
    {
        m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = 0.0f;
        m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = 0.0f;
        m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = 0.0f;
        m[3][0] = 0.0f           ; m[3][1] = 0.0f           ; m[3][2] = 0.0f           ; m[3][3] = 1.0f;
    }

	Matrix4f(float a00, float a01, float a02, float a03,
             float a10, float a11, float a12, float a13,
             float a20, float a21, float a22, float a23,
             float a30, float a31, float a32, float a33)
    {
        m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
        m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
        m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
        m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
    }

	Matrix4f operator* (const Matrix4f& right) const
	{
		Matrix4f ret;
		for (unsigned int i = 0 ; i < 4 ; i++) {
	       for (unsigned int j = 0 ; j < 4 ; j++) {
	           ret.m[i][j] = m[i][0] * right.m[0][j] +
	                         m[i][1] * right.m[1][j] +
	                         m[i][2] * right.m[2][j] +
	                         m[i][3] * right.m[3][j];
	       }
	    }
		return ret;
	}

	operator const float*() const
    {
        return &(m[0][0]);
    }

	void SetZero()
    {
        ZERO_MEM(m);
    }

	inline void InitIdentity()
	{
	    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
	    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
	    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
	    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	void Print() const
    {
        for (int i = 0 ; i < 4 ; i++) {
            printf("%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
        }       
    }

	float Determinant() const;
	Matrix4f& Inverse();

	void InitScaleTransform(float scaleX, float scaleY, float scaleZ);
    void InitRotateTransform(float rotateX, float rotateY, float rotateZ);
    void InitTranslationTransform(const Vector3f& pos);
    void InitTranslationTransform(float x, float y, float z);
    void InitPerspectiveProj(float fov, int width, int height, float near, float far);
    void InitCameraTransform(const Vector3f& target, const Vector3f& up);
};




#endif /* UTILS_H */