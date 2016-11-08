
#ifndef SKINNING_PROGRAM_H
#define SKINNING_PROGRAM_H

#include "light_program.h"

class SkinningProgram : public LightProgram {
public:

	static const uint MAX_BONES = 100;

	SkinningProgram();

	virtual bool Init();
	virtual void Link();
	virtual void BindAttrs();

	void SetBoneTransform(uint Index, const Matrix4f& Transform);

private:

	GLuint m_bones[MAX_BONES];
};


#endif /* SKINNING_PROGRAM_H */