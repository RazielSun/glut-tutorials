
#include <assert.h>
#include <math.h>
#include "utils/utils.h"
#include "utils/camera.h"
#include "utils/pipeline.h"
#include "utils/light_shadow_program.h"
#include "utils/shadow_program.h"
#include "utils/mesh.h"
#include "utils/shadow_map_fbo.h"
#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define PLANE_WIDTH 5.0f
#define PLANE_HEIGHT 5.0f
#define MAX_LIGHTS 2

const char* WINDOW_NAME = "Tutorial 23";

PersProjInfo projInfo;

DirectionLight directionLight;
SpotLight spotLight[MAX_LIGHTS];

Camera* camera = NULL;

ShadowMapFBO shadowMapFBO;
ShadowProgram* shadowProgram = NULL;
LightShadowProgram* lightProgram = NULL;

Texture* ground = NULL;

Mesh* mesh = NULL;
Mesh* quad = NULL;

static float scale = 0.0f;

void RenderShadowMapPass ()
{
	shadowMapFBO.BindForWriting();

	glClear(GL_DEPTH_BUFFER_BIT);

	shadowProgram->Enable();

	Pipeline p;
    p.Scale(0.1f, 0.1f, 0.1f);
    p.Rotate(0.0f, scale, 0.0f);
    p.Pos(0.0f, 0.0f, 3.0f);
    p.SetCamera(spotLight[0].Position, spotLight[0].Direction, Vector3f(0.0f, 1.0f, 0.0f));
    p.SetPerspectiveProj(projInfo);
    shadowProgram->SetWVP(p.GetTrans());

    mesh->Render();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass ()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	lightProgram->Enable();

    shadowMapFBO.BindForReading(GL_TEXTURE1);

    Pipeline p;
    p.SetPerspectiveProj(projInfo);

    p.Scale(10.0f, 10.0f, 10.0f);
    p.Pos(0.0f, 0.0f, 1.0f);
    p.Rotate(90.0f, 0.0f, 0.0f);

    p.SetCamera(camera->GetPos(), camera->GetTarget(), camera->GetUp());
    lightProgram->SetWVP(p.GetTrans());
    lightProgram->SetWorld(p.GetWorldTrans());

    p.SetCamera(spotLight[0].Position, spotLight[0].Direction, Vector3f(0.0f, 1.0f, 0.0f));
    lightProgram->SetLightWVP(p.GetTrans());

    ground->Bind(GL_TEXTURE0);
    quad->Render();

    p.Scale(0.1f, 0.1f, 0.1f);
    p.Rotate(0.0f, scale, 0.0f);
    p.Pos(0.0f, 0.0f, 3.0f);

    p.SetCamera(camera->GetPos(), camera->GetTarget(), camera->GetUp());
    lightProgram->SetWVP(p.GetTrans());
    lightProgram->SetWorld(p.GetWorldTrans());

    p.SetCamera(spotLight[0].Position, spotLight[0].Direction, Vector3f(0.0f, 1.0f, 0.0f));
    lightProgram->SetLightWVP(p.GetTrans());

    mesh->Render();
}

void Render ()
{
	camera->OnRender();
	scale += 0.05f;

	RenderShadowMapPass();
	RenderPass();
}

void createShadowProgram ()
{
	shadowProgram = new ShadowProgram();
	if (!shadowProgram->Init()) {
		exit(1);
	}

	shadowProgram->AddShader(GL_VERTEX_SHADER, "shaders/shadow_shader.vs");
	shadowProgram->AddShader(GL_FRAGMENT_SHADER, "shaders/shadow_shader.fs");

	shadowProgram->Compile();
	shadowProgram->Link();
}

void createLightProgram ()
{
	lightProgram = new LightShadowProgram();
	if (!lightProgram->Init()) {
		exit(1);
	}

	lightProgram->AddShader(GL_VERTEX_SHADER, "shaders/light_shadow_shader.vs");
	lightProgram->AddShader(GL_FRAGMENT_SHADER, "shaders/light_shadow_shader.fs");

	lightProgram->Compile();
	lightProgram->Link();

	// lightProgram->SetDirectionLight(directionLight);
	lightProgram->SetSpotLights(1, spotLight);
    lightProgram->SetTextureUnit(0);
    lightProgram->SetShadowMapTextureUnit(1);
    lightProgram->Enable();
}

SDL_Window *window;

void init()
{
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Unable to init SDL, error: '%s'\n", SDL_GetError());
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

	window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	if (window == NULL)
	{
		fprintf(stderr, "Unable to create SDL Context");
		exit(1);
	}

	glewExperimental = GL_TRUE;
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
	    fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
	    exit(1);
	}
}

int main (int argc, char *argv[])
{
	init();

	Vector3f Pos(3.0f, 8.0f, -10.0f);
    Vector3f Target(0.0f, -0.2f, 1.0f);
    Vector3f Up(0.0, 1.0f, 0.0f);

	camera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

	spotLight[0].AmbientIntensity = 0.1f;
    spotLight[0].DiffuseIntensity = 0.9f;
    spotLight[0].Color = Vector3f(1.0f, 1.0f, 1.0f);
    spotLight[0].Attenuation.Linear = 0.01f;
    spotLight[0].Position  = Vector3f(-20.0, 20.0, 5.0f);
    spotLight[0].Direction = Vector3f(1.0f, -1.0f, 0.0f);
    spotLight[0].Cutoff =  20.0f;

    directionLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    directionLight.AmbientIntensity = 0.1f; 
    directionLight.DiffuseIntensity = 0.1f;
    directionLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);

    projInfo.Width = WINDOW_WIDTH;
    projInfo.Height = WINDOW_HEIGHT;

    createLightProgram();

    shadowMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT);
	createShadowProgram();

    ground = new Texture(GL_TEXTURE_2D, "content/uvLayoutGrid.jpg");
    ground->Load();

	mesh = new Mesh();
	mesh->LoadMesh("content/phoenix_ugv.md2");

	quad = new Mesh();
	quad->LoadMesh("content/quad.obj");

	bool running = true;

	while(running)
	{
		SDL_Event event;

		while ( SDL_PollEvent(&event) )
		{
			switch(event.type)
			{
				case SDL_QUIT:
					running = false;
					break;
				case SDL_MOUSEMOTION:
					camera->OnMouse(event.motion.x, event.motion.y);
					break;

				case SDL_KEYDOWN:
				{
					camera->OnKeyboard(event.key.keysym.sym);
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running = false;
							break;
					}
				}
				break;
			}
		}
		
		Render();
		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();
	return 0;
}
