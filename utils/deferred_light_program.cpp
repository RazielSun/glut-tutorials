
#include <stdio.h>
#include "deferred_light_program.h"

DeferredLightProgram::DeferredLightProgram()
{
	//
}

bool DeferredLightProgram::Init()
{
	return ShaderProgram::Init();
}

void DeferredLightProgram::Link()
{
	ShaderProgram::Link();

	m_posTexture = GetUniformLocation("posMap");
	m_normalTexture = GetUniformLocation("normalMap");
	// m_eyeWorldPos = GetUniformLocation("eyeWorldPos");
	m_screenSize = GetUniformLocation("screenSize");

	printf("Link DeferredLightProgram: %d %d %d \n", m_posTexture, m_normalTexture, m_screenSize);

	// if (m_posTexture == INVALID_UNIFORM_LOCATION ||
	// 	m_normalTexture == INVALID_UNIFORM_LOCATION ||
	// 	// m_eyeWorldPos == INVALID_UNIFORM_LOCATION ||
	// 	m_screenSize == INVALID_UNIFORM_LOCATION) {
		
	// }
}

void DeferredLightProgram::SetPositionMapUnit(unsigned int textureUnit)
{
	glUniform1i(m_posTexture, textureUnit);
}

void DeferredLightProgram::SetNormalMapUnit(unsigned int textureUnit)
{
	glUniform1i(m_normalTexture, textureUnit);
}

void DeferredLightProgram::SetEyeWorldPos(const Vector3f& EyePos)
{
	glUniform3f(m_eyeWorldPos, EyePos.x, EyePos.y, EyePos.z);
}

void DeferredLightProgram::SetScreenSize(unsigned int Width, unsigned int Height)
{
	glUniform2f(m_screenSize, (float)Width, (float)Height);
}

