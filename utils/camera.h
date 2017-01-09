
#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"

class Camera {
public:
	Camera() {}
	
	Camera(int width, int height);
	Camera(int width, int height, Vector3f& pos, Vector3f& target, Vector3f& up);
	void SetPos(float x, float y, float z);
	void LookAt(float x, float y, float z);
	void OnMouse(int x, int y);
	bool OnKeyboard(int key);
	void OnJoyAxis(int axis, int value);
	void OnJoyButton(int button);
	void OnJoyHat(int value);
	void OnUpdate();
	void OnRender();

	const Vector3f& GetPos() const;
	const Vector3f& GetTarget() const;
	const Vector3f& GetUp() const;

private:
	void Init();
	void Update();

	int m_windowWidth;
	int m_windowHeight;

	float m_AngleH;
	float m_AngleV;

	float m_AxisValue;
	bool m_RotateV;
	bool m_RotateH;

	bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

	Vector2i m_mousePos;

	Vector3f m_pos;
	Vector3f m_target;
	Vector3f m_up;
};

#endif /* CAMERA_H */