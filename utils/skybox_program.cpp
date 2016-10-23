
#include "skybox_program.h"

bool SkyBoxProgram::Init()
{
	return ShaderProgram::Init();
}

void SkyBoxProgram::Link()
{
	m_uWVP = GetUniformLocation("WVP");
    m_texture = GetUniformLocation("sampler");
}