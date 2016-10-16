
#include <SDL2/SDL.h>
#include "util_camera.h"

const static float STEP_SIZE = 1.0f;
const static int MARGIN = 10;

Camera::Camera(int width, int height)
{
	m_windowWidth = width;
	m_windowHeight = height;
	m_pos = Vector3f(0.0f, 0.0f, 0.0f);

	m_target = Vector3f(0.0f, 0.0f, 1.0f);
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

const Vector3f& Camera::GetPos()
{
	return m_pos;
}

const Vector3f& Camera::GetTarget()
{
	return m_target;
}

const Vector3f& Camera::GetUp()
{
	return m_up;
}
