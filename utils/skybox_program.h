
#ifndef SKYBOX_PROGRAM_H
#define SKYBOX_PROGRAM_H

#include "shader_program.h"

class SkyBoxProgram : public ShaderProgram {
public:
	SkyBoxProgram();

	virtual bool Init();
	virtual void Link();
};

#endif /* SKYBOX_PROGRAM_H */