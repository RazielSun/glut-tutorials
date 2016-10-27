
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

#define COLOR_TEXTURE_UNIT GL_TEXTURE0
#define SHADOW_TEXTURE_UNIT GL_TEXTURE1
#define NORMAL_TEXTURE_UNIT GL_TEXTURE2

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define MAX_LIGHTS 2

const char* WINDOW_NAME = "Tutorial 26";

PersProjInfo projInfo;
DirectionLight directionLight;

Camera* camera = NULL;

LightProgram* lightProgram = NULL;
Texture* texture = NULL;
Mesh* mesh = NULL;

void render ()
{
	camera->OnRender();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static float scale = 0.0f;
	scale += 0.1f;

	Pipeline p;
	p.Rotate(0.0f, scale, 0.0f);
	p.Pos(0.0f, 0.0f, 3.0f);
	p.SetPerspectiveProj(projInfo);
	p.SetCamera(*camera);

	lightProgram->Enable();
	lightProgram->SetWVP(p.GetTrans());
	lightProgram->SetWorld(p.GetWorldTrans());
	lightProgram->SetDirectionLight(directionLight);
	lightProgram->SetPointLights(0, NULL);
	lightProgram->SetSpotLights(0, NULL);

	texture->Bind(COLOR_TEXTURE_UNIT);

	mesh->Render();
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

	lightProgram->SetDirectionLight(directionLight);
    lightProgram->SetTextureUnit(0);
    // lightProgram->SetShadowMapTextureUnit(1);
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

	Vector3f Pos(0.5f, 1.025f, 0.25f);
    Vector3f Target(0.0f, -0.5f, 1.0f);
    Vector3f Up(0.0, 1.0f, 0.0f);

	camera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    projInfo.Width = WINDOW_WIDTH;
    projInfo.Height = WINDOW_HEIGHT;

    directionLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    directionLight.AmbientIntensity = 0.2f; 
    directionLight.DiffuseIntensity = 0.8f;
    directionLight.Direction = Vector3f(1.0f, 0.0f, 0.0f);
    
	createLightProgram();

	mesh = new Mesh();
	mesh->LoadMesh("content/box.obj");

	texture = new Texture(GL_TEXTURE_2D, "content/bricks.jpg");
	if (!texture->Load()) {
		return 1;
	}

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
						case SDLK_q:
							directionLight.AmbientIntensity += 0.1f;
							break;
						case SDLK_w:
							directionLight.AmbientIntensity -= 0.1f;
							break;
						case SDLK_ESCAPE:
							running = false;
							break;
					}
				}
				break;
			}
		}
		
		render();
		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();
	return 0;
}
