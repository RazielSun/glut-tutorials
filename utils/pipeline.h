
#ifndef PIPELINE_H
#define PIPELINE_H

#include "utils.h"
#include "camera.h"

struct PersProjInfo {
	float FOV;
	int Width;
	int Height;
	float zNear;
	float zFar;

	PersProjInfo() {
		zNear = 1.0f;
		zFar = 50.0f;
		Width = 0;
		Height = 0;
		FOV = 60.0f;
	}
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
	void SetPerspectiveProj(const PersProjInfo& info);
	void SetCamera(const Vector3f& pos, const Vector3f& target, const Vector3f& up);
	void SetCamera(const Camera& camera);

	const Matrix4f& GetWorldTrans();
	const Matrix4f& GetViewTrans();
	const Matrix4f& GetProjTrans();
	const Matrix4f& GetTrans();

private:
	Vector3f m_scale;
	Vector3f m_pos;
	Vector3f m_rotate;
	
	Matrix4f m_World;
	Matrix4f m_View;
	Matrix4f m_Proj;
	Matrix4f m_WVP;

	PersProjInfo m_projInfo;

	Vector3f m_cameraPos;
	Vector3f m_cameraDir;
	Vector3f m_cameraUp;
};

#endif /* PIPELINE_H */