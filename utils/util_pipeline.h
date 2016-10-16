
#ifndef UTIL_PIPELINE
#define UTIL_PIPELINE

#include "util_3d.h"
#include "util_camera.h"

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

#endif /* UTIL_PIPELINE */