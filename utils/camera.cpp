
#include <SDL2/SDL.h>
#include "camera.h"

const static float STEP_SIZE = 1.0f;
const static int MARGIN = 10;

Camera::Camera(int width, int height)
{
	m_windowWidth = width;
	m_windowHeight = height;
	m_pos = Vector3f(0.0f, 0.0f, 0.0f);
	m_target = Vector3f(0.0f, 0.0f, 1.0f);
    m_target.Normalize();
	m_up = Vector3f(0.0f, 1.0f, 0.0f);

	Init();
}

Camera::Camera(int width, int height, Vector3f& pos, Vector3f& target, Vector3f& up)
{
	m_windowWidth = width;
	m_windowHeight = height;
	m_pos = pos;

	m_target = target;
	m_target.Normalize();
	m_up = up;
	m_up.Normalize();

	Init();
}

void Camera::SetPos(float x, float y, float z)
{
    Vector3f pos(x, y, z);
    m_pos = pos;
}

void Camera::LookAt(float x, float y, float z)
{
    Vector3f lookAt(x, y, z);
    Vector3f target = lookAt - m_pos;
    target.Normalize();
    Vector3f left(m_pos.x + 1.0f, m_pos.y, m_pos.z);
    left.Normalize();
    Vector3f up = target.Cross(left);
    up.Normalize();

    m_target = target;
    m_up = up;
}

void Camera::Init()
{
	Vector3f HTarget(m_target.x, 0.0, m_target.z);
    HTarget.Normalize();

    if (HTarget.z >= 0.0f)
    {
        if (HTarget.x >= 0.0f)
        {
            m_AngleH = 360.0f - ToDegree(asin(HTarget.z));
        }
        else
        {
            m_AngleH = 180.0f + ToDegree(asin(HTarget.z));
        }
    }
    else
    {
        if (HTarget.x >= 0.0f)
        {
            m_AngleH = ToDegree(asin(-HTarget.z));
        }
        else
        {
            m_AngleH = 90.0f + ToDegree(asin(-HTarget.z));
        }
    }

    m_AngleV = -ToDegree(asin(m_target.y));

    m_OnUpperEdge = false;
    m_OnLowerEdge = false;
    m_OnLeftEdge = false;
    m_OnRightEdge = false;
    m_mousePos.x = m_windowWidth / 2;
    m_mousePos.y = m_windowHeight / 2;
}

void Camera::OnUpdate()
{
    bool ShouldUpdate = false;

    if (m_RotateV) {
        m_AngleV += m_AxisValue;
        ShouldUpdate = true;
    }
    if (m_RotateH) {
        m_AngleH += m_AxisValue;
        ShouldUpdate = true;
    }

    if (ShouldUpdate) {
        Update();
    }
}

void Camera::Update()
{
	Vector3f vaxis(0.0f, 1.0f, 0.0f);

    // Rotate the view vector by the horizontal angle around the vertical axis
    Vector3f view(1.0f, 0.0f, 0.0f);
    view.Rotate(m_AngleH, vaxis);
    view.Normalize();

    // Rotate the view vector by the vertical angle around the horizontal axis
    Vector3f haxis = vaxis.Cross(view);
    haxis.Normalize();
    view.Rotate(m_AngleV, haxis);
    view.Normalize();

    m_target = view;
    m_target.Normalize();

    m_up = m_target.Cross(haxis);
    m_up.Normalize();
}

void Camera::OnRender()
{
    bool ShouldUpdate = false;

    if (m_OnLeftEdge) {
        m_AngleH -= 0.1f;
        ShouldUpdate = true;
    }
    else if (m_OnRightEdge) {
        m_AngleH += 0.1f;
        ShouldUpdate = true;
    }

    if (m_OnUpperEdge) {
        if (m_AngleV > -90.0f) {
            m_AngleV -= 0.1f;
            ShouldUpdate = true;
        }
    }
    else if (m_OnLowerEdge) {
        if (m_AngleV < 90.0f) {
            m_AngleV += 0.1f;
            ShouldUpdate = true;
        }
    }

    if (ShouldUpdate) {
        Update();
    }
}

void Camera::OnMouse(int x, int y)
{
    const int DeltaX = x - m_mousePos.x;
    const int DeltaY = y - m_mousePos.y;

    m_mousePos.x = x;
    m_mousePos.y = y;

    m_AngleH += (float)DeltaX / 20.0f;
    m_AngleV += (float)DeltaY / 20.0f;

    if (DeltaX == 0) {
        if (x <= MARGIN) {
            m_OnLeftEdge = true;
        }
        else if (x >= (m_windowWidth - MARGIN)) {
            m_OnRightEdge = true;
        }
    }
    else {
        m_OnLeftEdge = false;
        m_OnRightEdge = false;
    }

    if (DeltaY == 0) {
        if (y <= MARGIN) {
            m_OnUpperEdge = true;
        }
        else if (y >= (m_windowHeight - MARGIN)) {
            m_OnLowerEdge = true;
        }
    }
    else {
        m_OnUpperEdge = false;
        m_OnLowerEdge = false;
    }

    Update();
}

bool Camera::OnKeyboard(int key)
{
	bool value = false;

	switch(key)
	{
		case SDLK_UP:
		{
			m_pos += (m_target * STEP_SIZE);
			value = true;
		}
		break;
		case SDLK_DOWN:
		{
			m_pos -= (m_target * STEP_SIZE);
			value = true;
		}
		break;
		case SDLK_LEFT:
		{
			Vector3f right = m_up.Cross(m_target);
	        right.Normalize();
	        right *= STEP_SIZE;
	        m_pos += right;
			value = true;
		}
		break;
		case SDLK_RIGHT:
		{
			Vector3f left = m_target.Cross(m_up);
	        left.Normalize();
	        left *= STEP_SIZE;
	        m_pos += left;
			value = true;
		}
		break;
	}

	return value;
}

void Camera::OnJoyAxis(int axis, int value)
{
    int delta = value - 128;

    if (delta == 0)
    {
        m_AxisValue = 0.0f;

        switch(axis)
        {
            case 0:
            case 2:
                m_RotateH = false;
            break;
            case 1:
            case 3:
                m_RotateV = false;
            break;
        }
    }
    else
    {
        m_AxisValue = (float) delta / 32768.0f;

        switch(axis)
        {
            case 0:
            case 2:
                m_RotateH = true;
            break;
            case 1:
            case 3:
                m_RotateV = true;
                m_AxisValue *= -1;
            break;
        }
    }

    Update();
}

void Camera::OnJoyButton(int button)
{
    //
}

void Camera::OnJoyHat(int value)
{
    switch(value)
    {
        case 11:
        {
            m_pos += (m_target * STEP_SIZE);
        }
        break;
        case 12:
        {
            m_pos -= (m_target * STEP_SIZE);
        }
        break;
        case 14:
        {
            Vector3f right = m_up.Cross(m_target);
            right.Normalize();
            right *= STEP_SIZE;
            m_pos += right;
        }
        break;
        case 13:
        {
            Vector3f left = m_target.Cross(m_up);
            left.Normalize();
            left *= STEP_SIZE;
            m_pos += left;
        }
        break;
    }
}

const Vector3f& Camera::GetPos() const
{
	return m_pos;
}

const Vector3f& Camera::GetTarget() const
{
	return m_target;
}

const Vector3f& Camera::GetUp() const
{
	return m_up;
}
