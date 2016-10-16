
#include "util_pipeline.h"

void Pipeline::Scale(float scaleX, float scaleY, float scaleZ)
{
	m_scale.x = scaleX;
	m_scale.y = scaleY;
	m_scale.z = scaleZ;
}

void Pipeline::Pos(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}

void Pipeline::Rotate(float rotateX, float rotateY, float rotateZ)
{
	m_rotate.x = rotateX;
	m_rotate.y = rotateY;
	m_rotate.z = rotateZ;
}

void Pipeline::SetPerspectiveProj(float fov, int width, int height, float near, float far)
{
    m_projInfo.FOV = fov;
    m_projInfo.width = width;
    m_projInfo.height = height;
    m_projInfo.near = near;
    m_projInfo.far = far;
}

void Pipeline::SetCamera(const Camera& camera)
{
	m_camera = camera;
}

const Matrix4f* Pipeline::GetTrans()
{
	Matrix4f ScaleTrans, RotateTrans, TranslationTrans, PerspProjTrans, CameraRotateTrans, CameraTranslationTrans;
    ScaleTrans.InitScaleTransform(m_scale.x, m_scale.y, m_scale.z);
    RotateTrans.InitRotateTransform(m_rotate.x, m_rotate.y, m_rotate.z);
    TranslationTrans.InitTranslationTransform(m_pos);
    PerspProjTrans.InitPerspectiveProj(m_projInfo.FOV, m_projInfo.width, m_projInfo.height, m_projInfo.near, m_projInfo.far);
    CameraTranslationTrans.InitTranslationTransform(m_camera.GetPos());
    CameraRotateTrans.InitCameraTransform(m_camera.GetTarget(), m_camera.GetUp());
    m_transformation = PerspProjTrans * CameraRotateTrans * CameraTranslationTrans * TranslationTrans * RotateTrans * ScaleTrans;
    return &m_transformation;
}
