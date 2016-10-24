
#include "skybox.h"

SkyBox::SkyBox(const Camera* camera, const PersProjInfo& info)
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
	m_program = new SkyBoxProgram();
	if (!m_program->Init()) {
		exit(1);
	}

	m_program->AddShader(GL_VERTEX_SHADER, "shaders/skybox_shader.vs");
	m_program->AddShader(GL_FRAGMENT_SHADER, "shaders/skybox_shader.fs");
	m_program->Compile();
	m_program->Link();
	m_program->Enable();
	m_program->SetTextureUnit(0);

	m_cubemap = new CubemapTexture(dir, posXf, negXf, posYf, negYf, posZf, negZf);
	if (!m_cubemap->Load()) {
		return false;
	}

	m_mesh = new Mesh();

	return m_mesh->LoadMesh("content/sphere.obj");
}

void SkyBox::Render()
{
	m_program->Enable();

	GLint OldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
	GLint OldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	Pipeline p;
	p.Scale(20.0f, 20.0f, 20.0f);
	p.Rotate(0.0f, 0.0f, 0.0f);
	p.Pos(m_camera->GetPos().x, m_camera->GetPos().y, m_camera->GetPos().z);
	p.SetCamera(m_camera->GetPos(), m_camera->GetTarget(), m_camera->GetUp());
	p.SetPerspectiveProj(m_info);

	m_program->SetWVP(p.GetTrans());
	
	m_cubemap->Bind(GL_TEXTURE0);
	m_mesh->Render();  

	glCullFace(OldCullFaceMode);
	glDepthFunc(OldDepthFuncMode);
}