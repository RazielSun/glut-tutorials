
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

struct Vector2i {
	int x, y;
};

struct Vector3f {
	float x, y, z;

	Vector3f()
	{
	}

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
	Camera()
	{
	}
	
	Camera(int width, int height);
	Camera(int width, int height, Vector3f& pos, Vector3f& target, Vector3f& up);
	void OnMouse(int x, int y);
	bool OnKeyboard(int key);
	void OnRender();

	const Vector3f& GetPos();
	const Vector3f& GetTarget();
	const Vector3f& GetUp();
private:
	void Init();
	void Update();

	int m_windowWidth;
	int m_windowHeight;

	float m_AngleH;
	float m_AngleV;

	bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

	Vector2i m_mousePos;

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