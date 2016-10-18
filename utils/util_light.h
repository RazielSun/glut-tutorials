
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

struct DirectionLightGL : public BaseLightGL {
	GLuint Direction;
};

class LightProgram : public ShaderProgram {
public:
	static const unsigned int MAX_POINT_LIGHTS = 2;

	LightProgram();

	void SetDirectionLight(const DirectionLight& light);
	void SetPointLights(unsigned int numLights, const PointLight* pLights);

	virtual bool Init();
	virtual void Link();

private:
	DirectionLightGL m_directionLight;
	GLuint m_pointsCount; 
	PointLightGL m_points[MAX_POINT_LIGHTS];
};


#endif /* UTIL_LIGHT_PROGRAM */