
#include <assert.h>
#include <math.h>
#include "utils/utils.h"
#include "utils/camera.h"
#include "utils/pipeline.h"
#include "utils/light_program.h"
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

SpotLight spotLight;

Camera* camera = NULL;

ShadowMapFBO shadowMapFBO;
ShadowProgram* program = NULL;

Mesh* mesh = NULL;
Mesh* quad = NULL;

static float scale = 0.0f;

void RenderShadowMapPass ()
{
	shadowMapFBO.BindForWriting();

	glClear(GL_DEPTH_BUFFER_BIT);

	Pipeline p;
    p.Scale(0.1f, 0.1f, 0.1f);
    p.Rotate(0.0f, scale, 0.0f);
    p.Pos(0.0f, 0.0f, 5.0f);
    p.SetCamera(spotLight.Position, spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
    p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 60.0f);
    program->SetWVP(p.GetTrans());
    
    mesh->Render();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass ()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program->SetTextureUnit(0);
    shadowMapFBO.BindForReading(GL_TEXTURE0);

    Pipeline p;
    p.Scale(5.0f, 5.0f, 5.0f);
    p.Pos(0.0f, 0.0f, 10.0f);
    p.SetCamera(camera->GetPos(), camera->GetTarget(), camera->GetUp());
    p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 60.0f);
    program->SetWVP(p.GetTrans());

    quad->Render();
}

void Render ()
{
	camera->OnRender();
	scale += 0.05f;

	RenderShadowMapPass ();
	RenderPass ();
}

void createShadowProgram ()
{
	program = new ShadowProgram();
	if (!program->Init()) {
		exit(1);
	}

	program->AddShader(GL_VERTEX_SHADER, "shaders/shadow_shader.vs");
	program->AddShader(GL_FRAGMENT_SHADER, "shaders/shadow_shader.fs");

	program->Compile();
	program->Link();
	program->Enable();
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

	camera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

	shadowMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT);

	createShadowProgram();

	spotLight.AmbientIntensity = 0.0f;
    spotLight.DiffuseIntensity = 0.9f;
    spotLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    spotLight.Attenuation.Linear = 0.01f;
    spotLight.Position  = Vector3f(-20.0, 20.0, 5.0f);
    spotLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);
    spotLight.Cutoff =  20.0f;

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
