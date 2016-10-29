
#include <assert.h>
#include <math.h>
#include <iostream>

#include "utils/utils.h"
#include "utils/skybox.h"
#include "utils/camera.h"
#include "utils/pipeline.h"
#include "utils/light_program.h"
#include "utils/mesh.h"
#include "utils/gbuffer.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

const char* WINDOW_NAME = "Tutorial 35";

PersProjInfo projInfo;
// DirectionLight directionLight;

Camera* camera = NULL;

// LightProgram* lightProgram = NULL;
ShaderProgram* geomProgram = NULL;
Mesh* mesh = NULL;
GBuffer* gbuffer = NULL;

static float scale = 0.0f;

void RenderGeom ()
{
	geomProgram->Enable();

	gbuffer->BindForWriting();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Pipeline p;
	p.Scale(0.1f, 0.1f, 0.1f);
    p.Rotate(0.0f, scale, 0.0f);
    p.Pos(-0.8f, -1.0f, 12.0f);
	p.SetPerspectiveProj(projInfo);
	p.SetCamera(*camera);
	geomProgram->SetWVP(p.GetTrans());
	geomProgram->SetWorld(p.GetWorldTrans());
	mesh->Render();
}

void RenderLight ()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gbuffer->BindForReading();

	GLint HalfWidth = (GLint)(WINDOW_WIDTH / 2.0f);
    GLint HalfHeight = (GLint)(WINDOW_HEIGHT / 2.0f);
    
    gbuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    gbuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, HalfHeight, HalfWidth, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    gbuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, HalfWidth, HalfHeight, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    gbuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, HalfWidth, 0, WINDOW_WIDTH, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Render ()
{
	scale += 0.05f;

	camera->OnRender();

	RenderGeom();
	RenderLight();
}

// void createLightProgram ()
// {
// 	lightProgram = new LightProgram();
// 	if (!lightProgram->Init()) {
// 		exit(1);
// 	}

// 	lightProgram->AddShader(GL_VERTEX_SHADER, "shaders/light_bump_shader.vs");
// 	lightProgram->AddShader(GL_FRAGMENT_SHADER, "shaders/light_bump_shader.fs");

// 	lightProgram->Compile();
// 	lightProgram->Link();

// 	lightProgram->SetDirectionLight(directionLight);
//     // lightProgram->SetTextureUnit(0);
//     // lightProgram->SetNormalMapUnit(1);
//     lightProgram->Enable();
// }

void createGeomProgram ()
{
	geomProgram = new ShaderProgram();
	if (!geomProgram->Init()) {
		exit(1);
	}

	geomProgram->AddShader(GL_VERTEX_SHADER, "shaders/ds_geom_pass.vs");
	geomProgram->AddShader(GL_FRAGMENT_SHADER, "shaders/ds_geom_pass.fs");

	geomProgram->Compile();
	geomProgram->Link();

	geomProgram->SetTextureUnit(0);

    geomProgram->Enable();
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

    projInfo.Width = WINDOW_WIDTH;
    projInfo.Height = WINDOW_HEIGHT;

    // directionLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    // directionLight.AmbientIntensity = 0.2f; 
    // directionLight.DiffuseIntensity = 0.8f;
    // directionLight.Direction = Vector3f(1.0f, 0.0f, 0.0f);
    
	// createLightProgram();
	createGeomProgram();

	gbuffer = new GBuffer();
	gbuffer->Init(WINDOW_WIDTH, WINDOW_HEIGHT);

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
