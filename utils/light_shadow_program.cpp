
#include "light_shadow_program.h"

LightShadowProgram::LightShadowProgram()
{
	m_shaderProgram = 0;
}

bool LightShadowProgram::Init()
{
	if (!LightProgram::Init()) {
		return false;
	}
	
	return true;
}

void LightShadowProgram::Link()
{
	LightProgram::Link();

    m_uLightWVP = GetUniformLocation("lightWVP");
    m_shadowMap = GetUniformLocation("shadowMap");
}

void LightShadowProgram::SetLightWVP(const Matrix4f& lightWVP)
{
    glUniformMatrix4fv(m_uLightWVP, 1, GL_TRUE, (const GLfloat*)lightWVP.m);
}

void LightShadowProgram::SetShadowMapTextureUnit(unsigned int textureUnit)
{
    glUniform1i(m_shadowMap, textureUnit);
}
