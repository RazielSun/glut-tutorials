#ifndef LIGHTS_COMMON_H
#define LIGHTS_COMMON_H

#include <GL/glew.h>

#include "utils.h"

struct BaseLight {
	Vector3f Color;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct PointLight : public BaseLight {
	Vector3f Position;
	struct {
		float Constant;
		float Linear;
		float Exp;
	} Attenuation;
};

struct SpotLight : public PointLight {
	Vector3f Direction;
	float Cutoff;

	SpotLight () {
		Direction = Vector3f(0.0f, 0.0f, 0.0f);
		Cutoff = 0.0f;
	}
};

struct DirectionLight : public BaseLight {
	Vector3f Direction;
};

struct BaseLightGL {
	GLuint Color;
	GLuint AmbientIntensity;
	GLuint DiffuseIntensity;
};

struct PointLightGL : public BaseLightGL {
	GLuint Position;
	struct {
		GLuint Constant;
		GLuint Linear;
		GLuint Exp;
	} Attenuation;
};

struct SpotLightGL : public PointLightGL {
	GLuint Direction;
	GLuint Cutoff;
};

struct DirectionLightGL : public BaseLightGL {
	GLuint Direction;
};

#define COLOR_WHITE Vector3f(1.0f, 1.0f, 1.0f)
#define COLOR_RED Vector3f(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN Vector3f(0.0f, 1.0f, 0.0f)
#define COLOR_CYAN Vector3f(0.0f, 1.0f, 1.0f)
#define COLOR_BLUE Vector3f(0.0f, 0.0f, 1.0f)

#endif /* LIGHTS_COMMON_H */