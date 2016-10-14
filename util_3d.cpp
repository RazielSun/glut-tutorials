
#include <math.h>
#include "util_3d.h"

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

void Pipeline::InitScaleTransform(Matrix4f& matrix)
{
	matrix.m[0][0] = m_scale.x; matrix.m[0][1] = 0.0f; matrix.m[0][2] = 0.0f; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = 0.0f; matrix.m[1][1] = m_scale.y; matrix.m[1][2] = 0.0f; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = 0.0f; matrix.m[2][1] = 0.0f; matrix.m[2][2] = m_scale.z; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = 0.0f; matrix.m[3][1] = 0.0f; matrix.m[3][2] = 0.0f; matrix.m[3][3] = 1.0f;
}

void Pipeline::InitRotateTransform(Matrix4f& matrix)
{
	Matrix4f rx, ry, rz;

    const float x = ToRadian(m_rotate.x);
    const float y = ToRadian(m_rotate.y);
    const float z = ToRadian(m_rotate.z);

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

    matrix = rx * ry * rz;
}

void Pipeline::InitTranslationTransform(Matrix4f& matrix)
{
	matrix.m[0][0] = 1.0f; matrix.m[0][1] = 0.0f; matrix.m[0][2] = 0.0f; matrix.m[0][3] = m_pos.x;
	matrix.m[1][0] = 0.0f; matrix.m[1][1] = 1.0f; matrix.m[1][2] = 0.0f; matrix.m[1][3] = m_pos.y;
	matrix.m[2][0] = 0.0f; matrix.m[2][1] = 0.0f; matrix.m[2][2] = 1.0f; matrix.m[2][3] = m_pos.z;
	matrix.m[3][0] = 0.0f; matrix.m[3][1] = 0.0f; matrix.m[3][2] = 0.0f; matrix.m[3][3] = 1.0f;
}

void Pipeline::InitPerspectiveProj(Matrix4f& matrix)
{
    const float ar = (float)m_width / (float)m_height;
    const float zNear = m_near;
    const float zFar = m_far;
    const float zRange = zNear - zFar;
    const float tanHalfFOV = tanf(ToRadian(m_FOV / 2.0));

    matrix.m[0][0] = 1.0f / (tanHalfFOV * ar); 
    matrix.m[0][1] = 0.0f;
    matrix.m[0][2] = 0.0f;
    matrix.m[0][3] = 0.0f;

    matrix.m[1][0] = 0.0f;
    matrix.m[1][1] = 1.0f / tanHalfFOV; 
    matrix.m[1][2] = 0.0f; 
    matrix.m[1][3] = 0.0f;

    matrix.m[2][0] = 0.0f; 
    matrix.m[2][1] = 0.0f; 
    matrix.m[2][2] = (-zNear - zFar) / zRange; 
    matrix.m[2][3] = 2.0f * zFar * zNear / zRange;

    matrix.m[3][0] = 0.0f;
    matrix.m[3][1] = 0.0f; 
    matrix.m[3][2] = 1.0f; 
    matrix.m[3][3] = 0.0f;
}

void Pipeline::SetPerspectiveProj(float fov, int width, int height, float near, float far)
{
    m_FOV = fov;
    m_width = width;
    m_height = height;
    m_near = near;
    m_far = far;
}

const Matrix4f* Pipeline::GetTrans()
{
	Matrix4f ScaleTrans, RotateTrans, TranslationTrans, PerspProjTrans;
    InitScaleTransform(ScaleTrans);
    InitRotateTransform(RotateTrans);
    InitTranslationTransform(TranslationTrans);
    InitPerspectiveProj(PerspProjTrans);
    m_transformation = PerspProjTrans * TranslationTrans * RotateTrans * ScaleTrans;
    return &m_transformation;
}