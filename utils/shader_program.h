
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "utils.h"
#include <GL/glew.h>

class ShaderProgram {
public:
	ShaderProgram();

	virtual bool Init();
	virtual void Link();
	void Compile();

	void Enable();

	bool AddShader(GLenum shaderType, const char* fileName);

	void SetWVP(const Matrix4f& wvp);
	void SetWorld(const Matrix4f& world);
	void SetTextureUnit(unsigned int textureUnit);

protected:
	GLuint GetUniformLocation(const char* name);

	GLuint m_shaderProgram;
	GLuint m_uWorld;
	GLuint m_uWVP;
	GLuint m_texture;
};


#endif /* SHADER_PROGRAM_H */