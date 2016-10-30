
#include <stdio.h>
#include "deferred_dir_light_program.h"

DeferredDirLightProgram::DeferredDirLightProgram()
{
	//
}

bool DeferredDirLightProgram::Init()
{
	return DeferredLightProgram::Init();
}

void DeferredDirLightProgram::Link()
{
	DeferredLightProgram::Link();

	m_directionLight.Color = GetUniformLocation("directionLight.Base.Color");
    m_directionLight.AmbientIntensity = GetUniformLocation("directionLight.Base.AmbientIntensity");
    m_directionLight.DiffuseIntensity = GetUniformLocation("directionLight.Base.DiffuseIntensity");
    m_directionLight.Direction = GetUniformLocation("directionLight.Direction");

    // if (m_directionLight.Color == INVALID_UNIFORM_LOCATION ||
    //     m_directionLight.AmbientIntensity == INVALID_UNIFORM_LOCATION ||
    //     m_directionLight.DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
    //     m_directionLight.Direction == INVALID_UNIFORM_LOCATION) {

    // }

    printf("Link DeferredDirLightProgram: %d %d %d %d \n",
            m_directionLight.Color, m_directionLight.AmbientIntensity,
            m_directionLight.DiffuseIntensity, m_directionLight.Direction);
}

void DeferredDirLightProgram::SetDirectionLight(const DirectionLight& light)
{
	glUniform3f(m_directionLight.Color, light.Color.x, light.Color.y, light.Color.z);
    glUniform1f(m_directionLight.AmbientIntensity, light.AmbientIntensity);
    Vector3f Direction = light.Direction;
    Direction.Normalize();
    glUniform3f(m_directionLight.Direction, Direction.x, Direction.y, Direction.z);
    glUniform1f(m_directionLight.DiffuseIntensity, light.DiffuseIntensity);
}