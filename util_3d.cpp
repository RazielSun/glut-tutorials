
#include <math.h>
#include <stdio.h>
#include "util_3d.h"

Vector3f Vector3f::Cross(const Vector3f& v)
{
	const float _x = y * v.z - z * v.y;
    const float _y = z * v.x - x * v.z;
    const float _z = x * v.y - y * v.x;

    return Vector3f(_x, _y, _z);
}

Vector3f& Vector3f::Normalize()
{
	const float Length = sqrtf(x * x + y * y + z * z);

    x /= Length;
    y /= Length;
    z /= Length;

    return *this;
}

void Matrix4f::InitScaleTransform(float scaleX, float scaleY, float scaleZ)
{
	m[0][0] = scaleX; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
	m[1][0] = 0.0f; m[1][1] = scaleY; m[1][2] = 0.0f; m[1][3] = 0.0f;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = scaleZ; m[2][3] = 0.0f;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}

void Matrix4f::InitRotateTransform(float rotateX, float rotateY, float rotateZ)
{
	Matrix4f rx, ry, rz;

    const float x = ToRadian(rotateX);
    const float y = ToRadian(rotateY);
    const float z = ToRadian(rotateZ);

    rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f   ; rx.m[0][2] = 0.0f    ; rx.m[0][3] = 0.0f;
    rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(x); rx.m[1][2] = -sinf(x); rx.m[1][3] = 0.0f;
    rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(x); rx.m[2][2] = cosf(x) ; rx.m[2][3] = 0.0f;
    rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f   ; rx.m[3][2] = 0.0f    ; rx.m[3][3] = 1.0f;

    ry.m[0][0] = cosf(y); ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(y); ry.m[0][3] = 0.0f;
    ry.m[1][0] = 0.0f   ; ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f    ; ry.m[1][3] = 0.0f;
    ry.m[2][0] = sinf(y); ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(y) ; ry.m[2][3] = 0.0f;
    ry.m[3][0] = 0.0f   ; ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f    ; ry.m[3][3] = 1.0f;

    rz.m[0][0] = cosf(z); rz.m[0][1] = -sinf(z); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
    rz.m[1][0] = sinf(z); rz.m[1][1] = cosf(z) ; rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
    rz.m[2][0] = 0.0f   ; rz.m[2][1] = 0.0f    ; rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
    rz.m[3][0] = 0.0f   ; rz.m[3][1] = 0.0f    ; rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

    *this = rx * ry * rz;
}

void Matrix4f::InitTranslationTransform(float x, float y, float z)
{
	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}

void Matrix4f::InitPerspectiveProj(float fov, int width, int height, float near, float far)
{
    const float ar = (float)width / (float)height;
    const float zNear = near;
    const float zFar = far;
    const float zRange = zNear - zFar;
    const float tanHalfFOV = tanf(ToRadian(fov / 2.0));

    m[0][0] = 1.0f / (tanHalfFOV * ar); 
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = 1.0f / tanHalfFOV; 
    m[1][2] = 0.0f; 
    m[1][3] = 0.0f;

    m[2][0] = 0.0f; 
    m[2][1] = 0.0f; 
    m[2][2] = (-zNear - zFar) / zRange; 
    m[2][3] = 2.0f * zFar * zNear / zRange;

    m[3][0] = 0.0f;
    m[3][1] = 0.0f; 
    m[3][2] = 1.0f; 
    m[3][3] = 0.0f;
}

void Matrix4f::InitCameraTransform(const Vector3f& target, const Vector3f& up)
{
	Vector3f N = target;
	N.Normalize();
	Vector3f U = up;
	U.Normalize();
	U = U.Cross(target);
	Vector3f V = N.Cross(U);

	m[0][0] = U.x; m[0][1] = U.y; m[0][2] = U.z; m[0][3] = 0.0f;
    m[1][0] = V.x; m[1][1] = V.y; m[1][2] = V.z; m[1][3] = 0.0f;
    m[2][0] = N.x; m[2][1] = N.y; m[2][2] = N.z; m[2][3] = 0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}

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

void Pipeline::SetCamera(const Vector3f& pos, const Vector3f& target, const Vector3f& up)
{
	m_camera.pos = pos;
	m_camera.target = target;
	m_camera.up = up;
}

const Matrix4f* Pipeline::GetTrans()
{
	Matrix4f ScaleTrans, RotateTrans, TranslationTrans, PerspProjTrans, CameraRotateTrans, CameraTranslationTrans;
    ScaleTrans.InitScaleTransform(m_scale.x, m_scale.y, m_scale.z);
    RotateTrans.InitRotateTransform(m_rotate.x, m_rotate.y, m_rotate.z);
    TranslationTrans.InitTranslationTransform(m_pos.x, m_pos.y, m_pos.z);
    PerspProjTrans.InitPerspectiveProj(m_projInfo.FOV, m_projInfo.width, m_projInfo.height, m_projInfo.near, m_projInfo.far);
    CameraTranslationTrans.InitTranslationTransform(m_camera.pos.x, m_camera.pos.y, m_camera.pos.z);
    CameraRotateTrans.InitCameraTransform(m_camera.target, m_camera.up);
    m_transformation = PerspProjTrans * CameraRotateTrans * CameraTranslationTrans * TranslationTrans * RotateTrans * ScaleTrans;
    return &m_transformation;
}