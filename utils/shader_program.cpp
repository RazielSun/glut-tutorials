
#include <iostream>
#include <assert.h>
#include "shader_program.h"

ShaderProgram::ShaderProgram()
{
	m_shaderProgram = 0;
}

bool ShaderProgram::Init()
{
	m_shaderProgram = glCreateProgram();

	if (m_shaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        return false;
    }

    return true;
}

void ShaderProgram::AddShader(GLenum shaderType, const char* shaderText)
{
	GLuint shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &shaderText, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", shaderType, InfoLog);
        exit(1);
    }

    glAttachShader(m_shaderProgram, shader);
}

void ShaderProgram::Compile()
{
	GLint success = 0;

	glBindAttribLocation(m_shaderProgram, 0, "position");
    glBindAttribLocation(m_shaderProgram, 1, "texCoord");
    glBindAttribLocation(m_shaderProgram, 2, "normal");

    glLinkProgram(m_shaderProgram);
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);

    GLchar ErrorLog[1024] = { 0 };

	if (success == 0)
	{
		glGetProgramInfoLog(m_shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	}

    glValidateProgram(m_shaderProgram);
    glGetProgramiv(m_shaderProgram, GL_VALIDATE_STATUS, &success);

    if (!success) 
   	{
        glGetProgramInfoLog(m_shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(m_shaderProgram);
}

void ShaderProgram::Link()
{
	m_uWVP = GetUniformLocation("WVP");
    m_uWorld = GetUniformLocation("world");
}

void ShaderProgram::SetWVP(const Matrix4f* wvp)
{
	glUniformMatrix4fv(m_uWVP, 1, GL_TRUE, (const GLfloat*)wvp);
}

void ShaderProgram::SetWorld(const Matrix4f* world)
{
	glUniformMatrix4fv(m_uWorld, 1, GL_TRUE, (const GLfloat*)world);
}

GLuint ShaderProgram::GetUniformLocation(const char* name)
{
    return glGetUniformLocation(m_shaderProgram, name);
}