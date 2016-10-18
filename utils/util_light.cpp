
#include <cstdio>
#include <iostream>
#include "util_light.h"

LightProgram::LightProgram()
{
	m_shaderProgram = 0;
}

bool LightProgram::Init()
{
	if (!ShaderProgram::Init()) {
		return false;
	}
	
	return true;
}

void LightProgram::Link()
{
	ShaderProgram::Link();

	m_directionLight.Color = GetUniformLocation("directionLight.Base.Color");
    m_directionLight.AmbientIntensity = GetUniformLocation("directionLight.Base.AmbientIntensity");
    m_directionLight.DiffuseIntensity = GetUniformLocation("directionLight.Base.DiffuseIntensity");
    m_directionLight.Direction = GetUniformLocation("directionLight.Direction");
    
    m_pointsCount = GetUniformLocation("numPointLights");

    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_points) ; i++) {
        char name[128];
        memset(name, 0, sizeof(name));

        sprintf(name, "pointLight[%d].Base.Color", i);
        m_points[i].Color = GetUniformLocation(name);

        sprintf(name, "pointLight[%d].Base.AmbientIntensity", i);
        m_points[i].AmbientIntensity = GetUniformLocation(name);

        sprintf(name, "pointLight[%d].Base.DiffuseIntensity", i);
        m_points[i].DiffuseIntensity = GetUniformLocation(name);

        sprintf(name, "pointLight[%d].Position", i);
        m_points[i].Position = GetUniformLocation(name);

        sprintf(name, "pointLight[%d].Atten.Constant", i);
        m_points[i].Attenuation.Constant = GetUniformLocation(name);

        sprintf(name, "pointLight[%d].Atten.Linear", i);
        m_points[i].Attenuation.Linear = GetUniformLocation(name);

        sprintf(name, "pointLight[%d].Atten.Exp", i);
        m_points[i].Attenuation.Exp = GetUniformLocation(name);
    }
}

void LightProgram::SetDirectionLight(const DirectionLight& light)
{
	glUniform3f(m_directionLight.Color, light.Color.x, light.Color.y, light.Color.z);
    glUniform1f(m_directionLight.AmbientIntensity, light.AmbientIntensity);
    Vector3f Direction = light.Direction;
    Direction.Normalize();
    glUniform3f(m_directionLight.Direction, Direction.x, Direction.y, Direction.z);
    glUniform1f(m_directionLight.DiffuseIntensity, light.DiffuseIntensity);
}

void LightProgram::SetPointLights(unsigned int numLights, const PointLight* pLights)
{
	glUniform1i(m_pointsCount, numLights);

	for (unsigned int i = 0; i < numLights; i++) {
		glUniform3f(m_points[i].Color, pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z);
		glUniform1f(m_points[i].AmbientIntensity, pLights[i].AmbientIntensity);
		glUniform1f(m_points[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
		glUniform3f(m_points[i].Position, pLights[i].Position.x, pLights[i].Position.y, pLights[i].Position.z);
		glUniform1f(m_points[i].Attenuation.Constant, pLights[i].Attenuation.Constant);
		glUniform1f(m_points[i].Attenuation.Linear, pLights[i].Attenuation.Linear);
		glUniform1f(m_points[i].Attenuation.Exp, pLights[i].Attenuation.Exp);
    }
}