
#include "skinning_program.h"
#include "shader_program.h"
#include <cstdio>

SkinningProgram::SkinningProgram()
{
	//
}

bool SkinningProgram::Init()
{
	if (!LightProgram::Init()) {
		return false;
	}
	return true;
}

void SkinningProgram::Link()
{
	LightProgram::Link();

	for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_bones); i++) {
		char name[128];
        memset(name, 0, sizeof(name));
        sprintf(name, "Bones[%d]", i);
        m_bones[i] = GetUniformLocation(name);
	}
}

void SkinningProgram::BindAttrs()
{
	ShaderProgram::BindAttrs();

	glBindAttribLocation(m_shaderProgram, 3, "BoneIDs");
    glBindAttribLocation(m_shaderProgram, 4, "Weights");
}

void SkinningProgram::SetBoneTransform(uint Index, const Matrix4f& Transform)
{
	// assert(Index < MAX_BONES);
 //    //Transform.Print();
 //    glUniformMatrix4fv(m_boneLocation[Index], 1, GL_TRUE, (const GLfloat*)Transform);  
}