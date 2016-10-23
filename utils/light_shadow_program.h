
#ifndef LIGHT_SHADOW_PROGRAM_H
#define LIGHT_SHADOW_PROGRAM_H

#include "utils.h"
#include "light_program.h"

class LightShadowProgram : public LightProgram {
public:
	LightShadowProgram();

	virtual bool Init();
	virtual void Link();

	void SetLightWVP(const Matrix4f& lightWVP);
    void SetShadowMapTextureUnit(unsigned int textureUnit);

private:
	GLuint m_uLightWVP;
	GLuint m_shadowMap;
};


#endif /* LIGHT_SHADOW_PROGRAM_H */