
#ifndef UTIL_SHADER_PROGRAM
#define UTIL_SHADER_PROGRAM

#include "util_3d.h"
#include <GL/glew.h>

class ShaderProgram {
public:
	ShaderProgram();

	virtual bool Init();
	virtual void Link();
	void Compile();

	void AddShader(GLenum shaderType, const char* shader);

	void SetWVP(const Matrix4f* wvp);
	void SetWorld(const Matrix4f* world);
	GLuint GetUniformLocation(const char* name);

protected:
	GLuint m_shaderProgram;
	GLuint m_uWorld;
	GLuint m_uWVP;
};


#endif /* UTIL_SHADER_PROGRAM */