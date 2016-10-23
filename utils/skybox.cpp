
#include "skybox.h"

SkyBox::SkyBox(const Camera& camera, const PersProjInfo& info)
{
	m_camera = camera;
	m_info = info;

	m_program = NULL;
	m_cubemap = NULL;
	m_mesh = NULL;
}

SkyBox::~SkyBox()
{
	SAFE_DELETE(m_program);
    SAFE_DELETE(m_cubemap);
    SAFE_DELETE(m_mesh);
}

bool SkyBox::Init(const std::string& dir,
	const std::string& posXf,
	const std::string& negXf,
	const std::string& posYf,
	const std::string& negYf,
	const std::string& posZf,
	const std::string& negZf)
{
	m_program = new LightProgram();
	if (!m_program->Init()) {
		exit(1);
	}

	m_program->AddShader(GL_VERTEX_SHADER, "../shaders/skybox_shader.vs");
	m_program->AddShader(GL_FRAGMENT_SHADER, "../shaders/skybox_shader.fs");
	m_program->Compile();
	m_program->Link();
	m_program->Enable()
	m_program->SetTextureUnit(0);

	m_cubemap = CubemapTexture(dir, posXf, negXf, posYf, negYf, posZf, negZf);
	if (!m_cubemap->Init()) {
		return false;
	}

	m_mesh = new Mesh();

	return m_mesh->LoadMesh("../content/sphere.obj");
}

void SkyBox::Render()
{
	//
}