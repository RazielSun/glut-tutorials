
#ifndef UTIL_LIGHT_PROGRAM
#define UTIL_LIGHT_PROGRAM

#include "util_3d.h"
#include "util_shader_program.h"
#include <GL/glew.h>

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

class LightProgram : public ShaderProgram {
public:
	static const unsigned int MAX_POINT_LIGHTS = 2;
	static const unsigned int MAX_SPOT_LIGHTS = 2;

	LightProgram();

	void SetDirectionLight(const DirectionLight& light);
	void SetPointLights(unsigned int numLights, const PointLight* pLights);
	void SetSpotLights(unsigned int numLights, const SpotLight* sLights);

	virtual bool Init();
	virtual void Link();

private:
	DirectionLightGL m_directionLight;
	GLuint m_pointsCount;
	GLuint m_spotsCount;
	PointLightGL m_points[MAX_POINT_LIGHTS];
	SpotLightGL m_spots[MAX_SPOT_LIGHTS];
};


#endif /* UTIL_LIGHT_PROGRAM */