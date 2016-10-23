
#include "pipeline.h"

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

void Pipeline::SetPerspectiveProj(const PersProjInfo& info)
{
    m_projInfo = info;
}

void Pipeline::SetCamera(const Vector3f& pos, const Vector3f& target, const Vector3f& up)
{
    m_cameraPos = pos;
    m_cameraDir = target;
    m_cameraUp = up;
}

void Pipeline::SetCamera(const Camera& camera)
{
    SetCamera(camera.GetPos(), camera.GetTarget(), camera.GetUp());
}

const Matrix4f& Pipeline::GetWorldTrans()
{
    Matrix4f ScaleTrans, RotateTrans, TranslationTrans;

    ScaleTrans.InitScaleTransform(m_scale.x, m_scale.y, m_scale.z);
    RotateTrans.InitRotateTransform(m_rotate.x, m_rotate.y, m_rotate.z);
    TranslationTrans.InitTranslationTransform(m_pos);

    m_World = TranslationTrans * RotateTrans * ScaleTrans;

    return m_World;
}

const Matrix4f& Pipeline::GetViewTrans()
{
    Matrix4f CameraRotateTrans, CameraTranslationTrans;

    CameraTranslationTrans.InitTranslationTransform(-m_cameraPos.x, -m_cameraPos.y, -m_cameraPos.z);
    CameraRotateTrans.InitCameraTransform(m_cameraDir, m_cameraUp);

    m_View = CameraRotateTrans * CameraTranslationTrans;

    return m_View;
}

const Matrix4f& Pipeline::GetProjTrans()
{
    Matrix4f PerspProjTrans;

    PerspProjTrans.InitPerspectiveProj(m_projInfo.FOV, m_projInfo.Width, m_projInfo.Height, m_projInfo.zNear, m_projInfo.zFar);

    m_Proj = PerspProjTrans;

    return m_Proj;
}

const Matrix4f& Pipeline::GetTrans()
{
	GetWorldTrans();
    GetViewTrans();
    GetProjTrans();

    m_WVP = m_Proj * m_View * m_World;
    return m_WVP;
}
