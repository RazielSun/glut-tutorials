
#ifndef UTIL_CAMERA
#define UTIL_CAMERA

#include "util_3d.h"

class Camera {
public:
	Camera() {}
	
	Camera(int width, int height);
	Camera(int width, int height, Vector3f& pos, Vector3f& target, Vector3f& up);
	void SetPos(float x, float y, float z);
	void LookAt(float x, float y, float z);
	void OnMouse(int x, int y);
	bool OnKeyboard(int key);
	void OnRender();

	const Vector3f& GetPos();
	const Vector3f& GetTarget();
	const Vector3f& GetUp();

private:
	void Init();
	void Update();

	int m_windowWidth;
	int m_windowHeight;

	float m_AngleH;
	float m_AngleV;

	bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

	Vector2i m_mousePos;

	Vector3f m_pos;
	Vector3f m_target;
	Vector3f m_up;
};

#endif /* UTIL_CAMERA */