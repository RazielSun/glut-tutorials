#ifndef DEFERRED_LIGHT_PROGRAM_H
#define DEFERRED_LIGHT_PROGRAM_H

#include <GL/glew.h>

#include "utils.h"
#include "shader_program.h"

class DeferredLightProgram : public ShaderProgram {
public:

	DeferredLightProgram();

	void SetPositionMapUnit(unsigned int textureUnit);
	void SetNormalMapUnit(unsigned int textureUnit);
	void SetEyeWorldPos(const Vector3f& EyePos);
	void SetScreenSize(unsigned int Width, unsigned int Height);

	virtual bool Init();
	virtual void Link();

private:

	GLuint m_posTexture;
	GLuint m_normalTexture;
	GLuint m_eyeWorldPos;
	GLuint m_screenSize;
};

#endif /* DEFERRED_LIGHT_PROGRAM_H */