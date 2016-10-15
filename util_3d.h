
#ifndef UTIL_3D
#define UTIL_3D

#include <math.h>

#define ToRadian(x) ((x) * M_PI / 180.0f)
#define ToDegree(x) ((x) * 180.0f / M_PI)

struct ProjInfo {
	float near;
    float far;
    int width;
    int height;
    float FOV;

    ProjInfo()
    {
    	near = 1.0f;
		far = 1000.0f;
		width = 640;
		height = 480;
		FOV = 60.0f;
    }
};

struct Vector3f {
	float x, y, z;

	Vector3f()
	{
	}

	Vector3f(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f Cross(const Vector3f& v);
	Vector3f& Normalize();

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

class Matrix4f {
public:
	float m[4][4];

	Matrix4f()
	{
	}

	Matrix4f operator* (const Matrix4f& right)
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

	void InitScaleTransform(float scaleX, float scaleY, float scaleZ);
    void InitRotateTransform(float rotateX, float rotateY, float rotateZ);
    void InitTranslationTransform(const Vector3f& pos);
    void InitTranslationTransform(float x, float y, float z);
    void InitPerspectiveProj(float fov, int width, int height, float near, float far);
    void InitCameraTransform(const Vector3f& target, const Vector3f& up);
};

class Camera {
public:
	Camera();
	Camera(Vector3f& pos, Vector3f& target, Vector3f& up);
	bool OnKeyboard(int key);
	const Vector3f& GetPos();
	const Vector3f& GetTarget();
	const Vector3f& GetUp();
private:
	Vector3f m_pos;
	Vector3f m_target;
	Vector3f m_up;
};

class Pipeline {
public:
	Pipeline()
	{
		m_scale = Vector3f(1.0f, 1.0f, 1.0f);
		m_rotate = Vector3f(0.0f, 0.0f, 0.0f);
		m_pos = Vector3f(0.0f, 0.0f, 0.0f);
	}

	void Scale(float scaleX, float scaleY, float scaleZ);
	void Pos(float x, float y, float z);
	void Rotate(float rotateX, float rotateY, float rotateZ);
	void SetPerspectiveProj(float fov, int width, int height, float near, float far);
	void SetCamera(const Camera& camera);
	const Matrix4f* GetTrans();

private:
	Vector3f m_scale;
	Vector3f m_pos;
	Vector3f m_rotate;
	ProjInfo m_projInfo;
	Matrix4f m_transformation;
	Camera m_camera;
};

#endif /* UTIL_3D */