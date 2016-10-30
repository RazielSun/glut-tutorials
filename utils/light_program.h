
#ifndef LIGHT_PROGRAM_H
#define LIGHT_PROGRAM_H

#include "utils.h"
#include "lights_common.h"
#include "shader_program.h"

class LightProgram : public ShaderProgram {
public:
	static const unsigned int MAX_POINT_LIGHTS = 2;
	static const unsigned int MAX_SPOT_LIGHTS = 2;

	LightProgram();

	void SetDirectionLight(const DirectionLight& light);
	void SetPointLights(unsigned int numLights, const PointLight* pLights);
	void SetSpotLights(unsigned int numLights, const SpotLight* sLights);
	void SetNormalMapUnit(unsigned int textureUnit);

	virtual bool Init();
	virtual void Link();

private:
	DirectionLightGL m_directionLight;
	GLuint m_pointsCount;
	GLuint m_spotsCount;
	PointLightGL m_points[MAX_POINT_LIGHTS];
	SpotLightGL m_spots[MAX_SPOT_LIGHTS];
	GLuint m_normalMap;
};


#endif /* LIGHT_PROGRAM_H */