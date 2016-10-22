#include "shadow_program.h"

ShadowProgram::ShadowProgram()
{
}

bool ShadowProgram::Init()
{
	return ShaderProgram::Init();
}

void ShadowProgram::Link()
{
	m_uWVP = GetUniformLocation("WVP");
    m_texture = GetUniformLocation("sampler");
}