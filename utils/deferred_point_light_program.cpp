
#include "deferred_point_light_program.h"

DeferredPointLightProgram::DeferredPointLightProgram()
{
	//
}

bool DeferredPointLightProgram::Init()
{
	return DeferredLightProgram::Init();
}

void DeferredPointLightProgram::Link()
{
	DeferredLightProgram::Link();

    m_point.Color = GetUniformLocation("pointLight.Base.Color");
    m_point.AmbientIntensity = GetUniformLocation("pointLight.Base.AmbientIntensity");
    m_point.DiffuseIntensity = GetUniformLocation("pointLight.Base.DiffuseIntensity");
    m_point.Position = GetUniformLocation("pointLight.Position");
    m_point.Attenuation.Constant = GetUniformLocation("pointLight.Atten.Constant");
    m_point.Attenuation.Linear = GetUniformLocation("pointLight.Atten.Linear");
    m_point.Attenuation.Exp = GetUniformLocation("pointLight.Atten.Exp");

    // if (m_point.Color == INVALID_UNIFORM_LOCATION ||
    //     m_point.AmbientIntensity == INVALID_UNIFORM_LOCATION ||
    //     m_point.DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
    //     m_point.Position == INVALID_UNIFORM_LOCATION ||
    //     m_point.Attenuation.Constant == INVALID_UNIFORM_LOCATION ||
    //     m_point.Attenuation.Linear == INVALID_UNIFORM_LOCATION ||
    //     m_point.Attenuation.Exp == INVALID_UNIFORM_LOCATION) {

    // }

    printf("Link DeferredPointLightProgram: %d %d %d %d %d %d %d \n",
            m_point.Color, m_point.AmbientIntensity, m_point.DiffuseIntensity,
            m_point.Position, m_point.Attenuation.Constant,
            m_point.Attenuation.Linear, m_point.Attenuation.Exp);
}

void DeferredPointLightProgram::SetPointLight(const PointLight& light)
{
	glUniform3f(m_point.Color, light.Color.x, light.Color.y, light.Color.z);
    glUniform1f(m_point.AmbientIntensity, light.AmbientIntensity);
    glUniform1f(m_point.DiffuseIntensity, light.DiffuseIntensity);
    glUniform3f(m_point.Position, light.Position.x, light.Position.y, light.Position.z);
    glUniform1f(m_point.Attenuation.Constant, light.Attenuation.Constant);
    glUniform1f(m_point.Attenuation.Linear, light.Attenuation.Linear);
    glUniform1f(m_point.Attenuation.Exp, light.Attenuation.Exp);
}