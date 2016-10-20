
#include <cstdio>
#include <math.h>
#include <iostream>
#include "light.h"

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
    m_spotsCount = GetUniformLocation("numSpotLights");

    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_points) ; i++) {
        char name[128];
        memset(name, 0, sizeof(name));

        sprintf(name, "pointLights[%d].Base.Color", i);
        m_points[i].Color = GetUniformLocation(name);

        sprintf(name, "pointLights[%d].Base.AmbientIntensity", i);
        m_points[i].AmbientIntensity = GetUniformLocation(name);

        sprintf(name, "pointLights[%d].Base.DiffuseIntensity", i);
        m_points[i].DiffuseIntensity = GetUniformLocation(name);

        sprintf(name, "pointLights[%d].Position", i);
        m_points[i].Position = GetUniformLocation(name);

        sprintf(name, "pointLights[%d].Atten.Constant", i);
        m_points[i].Attenuation.Constant = GetUniformLocation(name);

        sprintf(name, "pointLights[%d].Atten.Linear", i);
        m_points[i].Attenuation.Linear = GetUniformLocation(name);

        sprintf(name, "pointLights[%d].Atten.Exp", i);
        m_points[i].Attenuation.Exp = GetUniformLocation(name);
    }

    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_spots) ; i++) {
        char name[128];
        memset(name, 0, sizeof(name));

        sprintf(name, "spotLights[%d].Base.Base.Color", i);
        m_spots[i].Color = GetUniformLocation(name);

        sprintf(name, "spotLights[%d].Base.Base.AmbientIntensity", i);
        m_spots[i].AmbientIntensity = GetUniformLocation(name);

        sprintf(name, "spotLights[%d].Base.Base.DiffuseIntensity", i);
        m_spots[i].DiffuseIntensity = GetUniformLocation(name);

        sprintf(name, "spotLights[%d].Base.Position", i);
        m_spots[i].Position = GetUniformLocation(name);

        sprintf(name, "spotLights[%d].Base.Atten.Constant", i);
        m_spots[i].Attenuation.Constant = GetUniformLocation(name);

        sprintf(name, "spotLights[%d].Base.Atten.Linear", i);
        m_spots[i].Attenuation.Linear = GetUniformLocation(name);

        sprintf(name, "spotLights[%d].Base.Atten.Exp", i);
        m_spots[i].Attenuation.Exp = GetUniformLocation(name);

        sprintf(name, "spotLights[%d].Direction", i);
        m_spots[i].Direction = GetUniformLocation(name);

        sprintf(name, "spotLights[%d].Cutoff", i);
        m_spots[i].Cutoff = GetUniformLocation(name);
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

void LightProgram::SetSpotLights(unsigned int numLights, const SpotLight* sLights)
{
    glUniform1i(m_spotsCount, numLights);

    for (unsigned int i = 0; i < numLights; i++) {
        glUniform3f(m_spots[i].Color, sLights[i].Color.x, sLights[i].Color.y, sLights[i].Color.z);
        glUniform1f(m_spots[i].AmbientIntensity, sLights[i].AmbientIntensity);
        glUniform1f(m_spots[i].DiffuseIntensity, sLights[i].DiffuseIntensity);
        glUniform3f(m_spots[i].Position, sLights[i].Position.x, sLights[i].Position.y, sLights[i].Position.z);
        glUniform1f(m_spots[i].Attenuation.Constant, sLights[i].Attenuation.Constant);
        glUniform1f(m_spots[i].Attenuation.Linear, sLights[i].Attenuation.Linear);
        glUniform1f(m_spots[i].Attenuation.Exp, sLights[i].Attenuation.Exp);
        Vector3f Direction = sLights[i].Direction;
        Direction.Normalize();
        glUniform3f(m_spots[i].Direction, Direction.x, Direction.y, Direction.z);
        glUniform1f(m_spots[i].Cutoff, cosf(ToRadian(sLights[i].Cutoff)));
    }
}
