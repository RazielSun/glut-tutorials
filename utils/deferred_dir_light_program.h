#ifndef DEFERRED_DIR_LIGHT_PROGRAM_H
#define DEFERRED_DIR_LIGHT_PROGRAM_H

#include <GL/glew.h>

#include "utils.h"
#include "lights_common.h"
#include "deferred_light_program.h"

class DeferredDirLightProgram : public DeferredLightProgram {
public:

	DeferredDirLightProgram();

	void SetDirectionLight(const DirectionLight& light);

	virtual bool Init();
	virtual void Link();

private:

	DirectionLightGL m_directionLight;
};

#endif /* DEFERRED_DIR_LIGHT_PROGRAM_H */