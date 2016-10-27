
#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include "pipeline.h"
#include "camera.h"
#include "skybox_program.h"
#include "cubemap_texture.h"
#include "mesh.h"

class SkyBox {
public:
	SkyBox(const Camera* camera, const PersProjInfo& info);
	~SkyBox();

	bool Init(const std::string& dir,
		const std::string& posXf,
		const std::string& negXf,
		const std::string& posYf,
		const std::string& negYf,
		const std::string& posZf,
		const std::string& negZf);

	void Render();

private:
	SkyBoxProgram* m_program;
	const Camera* m_camera;
	CubemapTexture* m_cubemap;
	Mesh* m_mesh;
	PersProjInfo m_info;
};

#endif /* SKYBOX_H */