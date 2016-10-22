#ifndef SHADOW_PROGRAM_H
#define SHADOW_PROGRAM_H

#include "utils.h"
#include "shader_program.h"

class ShadowProgram : public ShaderProgram {
public:
    ShadowProgram();

    virtual bool Init();
    virtual void Link();
};

#endif /* SHADOW_PROGRAM_H */