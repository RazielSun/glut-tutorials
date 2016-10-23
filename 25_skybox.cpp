
#include <assert.h>
#include <math.h>
#include <iostream>

#include "utils/utils.h"
#include "utils/skybox.h"
#include "utils/camera.h"
#include "utils/pipeline.h"
#include "utils/light_program.h"
#include "utils/mesh.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define PLANE_WIDTH 5.0f
#define PLANE_HEIGHT 5.0f

const char* WINDOW_NAME = "Tutorial 25";

PersProjInfo projInfo;

DirectionLight directionLight;

Camera* camera = NULL;

LightProgram *lightProgram = NULL;
SkyBoxProgram* skyboxProgram = NULL;

Mesh* mesh = NULL;

static float scale = 0.0f;

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

void createLightProgram ()
{
	lightProgram = new LightProgram();
	if (!lightProgram->Init()) {
		exit(1);
	}

	lightProgram->AddShader(GL_VERTEX_SHADER, "shaders/light_shader.vs");
	lightProgram->AddShader(GL_FRAGMENT_SHADER, "shaders/light_shader.fs");

	lightProgram->Compile();
	lightProgram->Link();

	// lightProgram->SetDirectionLight(directionLight);
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

    directionLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    directionLight.AmbientIntensity = 0.1f; 
    directionLight.DiffuseIntensity = 0.1f;
    directionLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);

    projInfo.Width = WINDOW_WIDTH;
    projInfo.Height = WINDOW_HEIGHT;

    createLightProgram();

	mesh = new Mesh();
	mesh->LoadMesh("content/phoenix_ugv.md2");

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
