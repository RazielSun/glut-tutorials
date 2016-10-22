#ifndef SHADOW_MAP_FBO_H
#define SHADOW_MAP_FBO_H

#include <GL/glew.h>

class ShadowMapFBO
{
public:
	ShadowMapFBO();
	~ShadowMapFBO();

	bool Init(unsigned int width, unsigned int height);

	void BindForWriting();
	void BindForReading(GLenum textureUnit);

private:
	GLuint m_fbo;
	GLuint m_rbo;
	GLuint m_shadowMap;
};

#endif /* SHADOW_MAP_FBO_H */