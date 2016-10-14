
#ifndef UTIL_3D
#define UTIL_3D

#include <math.h>

#define ToRadian(x) ((x) * M_PI / 180.0f)
#define ToDegree(x) ((x) * 180.0f / M_PI)

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
};

class Matrix4f {
public:
	float m[4][4];

	Matrix4f()
	{
	}

	inline Matrix4f operator* (const Matrix4f& right)
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
};

class Pipeline {
public:
	Pipeline()
	{
		m_near = 1.0f;
		m_far = 1000.0f;
		m_width = 640;
		m_height = 480;
		m_FOV = 60.0f;
		m_scale = Vector3f(1.0f, 1.0f, 1.0f);
		m_rotate = Vector3f(0.0f, 0.0f, 0.0f);
		m_pos = Vector3f(0.0f, 0.0f, 0.0f);
	}

	void Scale(float scaleX, float scaleY, float scaleZ);
	void Pos(float x, float y, float z);
	void Rotate(float rotateX, float rotateY, float rotateZ);
	void SetPerspectiveProj(float fov, int width, int height, float near, float far);
	const Matrix4f* GetTrans();

private:
	void InitScaleTransform(Matrix4f& matrix);
    void InitRotateTransform(Matrix4f& matrix);
    void InitTranslationTransform(Matrix4f& matrix);
    void InitPerspectiveProj(Matrix4f& matrix);

    float m_near;
    float m_far;
    int m_width;
    int m_height;
    float m_FOV;
	Vector3f m_scale;
	Vector3f m_pos;
	Vector3f m_rotate;
	Matrix4f m_transformation;
};

#endif /* UTIL_3D */