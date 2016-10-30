#ifndef DEFERRED_POINTS_LIGHT_PROGRAM_H
#define DEFERRED_POINTS_LIGHT_PROGRAM_H

#include <GL/glew.h>

#include "utils.h"
#include "lights_common.h"
#include "deferred_light_program.h"

class DeferredPointLightProgram : public DeferredLightProgram {
public:

	DeferredPointLightProgram();

	void SetPointLight(const PointLight& light);

	virtual bool Init();
	virtual void Link();
	
private:

	PointLightGL m_point;
};

#endif /* DEFERRED_POINTS_LIGHT_PROGRAM_H */