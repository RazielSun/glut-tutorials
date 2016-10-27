
#include "skybox_program.h"

SkyBoxProgram::SkyBoxProgram()
{
	m_uWVP = 0;
	m_texture = 0;
}

bool SkyBoxProgram::Init()
{
	return ShaderProgram::Init();
}

void SkyBoxProgram::Link()
{
	m_uWVP = GetUniformLocation("WVP");
    m_texture = GetUniformLocation("sampler");
}