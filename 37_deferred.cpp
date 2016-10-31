
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <iostream>

#include "utils/utils.h"
#include "utils/skybox.h"
#include "utils/camera.h"
#include "utils/pipeline.h"
#include "utils/lights_common.h"
#include "utils/deferred_dir_light_program.h"
#include "utils/deferred_point_light_program.h"
#include "utils/mesh.h"
#include "utils/gbuffer.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 768

const char* WINDOW_NAME = "Tutorial 37";

PersProjInfo projInfo;
Camera* camera = NULL;

DirectionLight directionLight;
PointLight pointLight[3];

DeferredDirLightProgram* dirLightProgram = NULL;
DeferredPointLightProgram* pointLightProgram = NULL;
ShaderProgram* nullProgram = NULL;
ShaderProgram* geomProgram = NULL;

GBuffer* gbuffer = NULL;

Mesh* quad = NULL;
Mesh* box = NULL;
Mesh* bsphere = NULL;

Vector3f boxPositions[5];

static float scale = 0.0f;

void GeometryPass ()
{
	geomProgram->Enable();

	gbuffer->BindForGeomPass();

	// Only the geometry pass updates the depth buffer
    glDepthMask(GL_TRUE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);

	Pipeline p;
    p.Rotate(0.0f, scale, 0.0f);
	p.SetPerspectiveProj(projInfo);
	p.SetCamera(*camera);

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(boxPositions); i++)
	{
		p.Pos(boxPositions[i].x, boxPositions[i].y, boxPositions[i].z);
		geomProgram->SetWVP(p.GetTrans());
		geomProgram->SetWorld(p.GetWorldTrans());
		box->Render();
	}

	// When we get here the depth buffer is already populated and the stencil pass
    // depends on it, but it does not write to it.
    glDepthMask(GL_FALSE);

    glDisable(GL_DEPTH_TEST);
}

float CalcPointLightBSphere(const PointLight& Light)
{
    float MaxChannel = fmax(fmax(Light.Color.x, Light.Color.y), Light.Color.z);
    
    float ret = (-Light.Attenuation.Linear + sqrtf(Light.Attenuation.Linear * Light.Attenuation.Linear - 4 * Light.Attenuation.Exp * (Light.Attenuation.Exp - 256 * MaxChannel * Light.DiffuseIntensity))) 
                /
                (2 * Light.Attenuation.Exp);
    return ret;
}

void StencilPass (unsigned int i)
{
	nullProgram->Enable();

	// Disable color/depth write and enable stencil
	gbuffer->BindForStencilPass();
	glEnable(GL_DEPTH_TEST);

    glDisable(GL_CULL_FACE);

	glClear(GL_STENCIL_BUFFER_BIT);

	// We need the stencil test to be enabled but we want it
	// to succeed always. Only the depth test matters.
	glStencilFunc(GL_ALWAYS, 0, 0);

	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	Pipeline p;
	p.Pos(pointLight[i].Position.x, pointLight[i].Position.y, pointLight[i].Position.z);
    float BBoxScale = CalcPointLightBSphere(pointLight[i]);
	p.Scale(BBoxScale, BBoxScale, BBoxScale);		
    p.SetCamera(*camera);
    p.SetPerspectiveProj(projInfo);

	nullProgram->SetWVP(p.GetTrans());
	bsphere->Render();  
}

void PointLightPass (unsigned int i)
{
	gbuffer->BindForLightPass();

    pointLightProgram->Enable();

	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
  		
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    Pipeline p;
    p.Pos(pointLight[i].Position.x, pointLight[i].Position.y, pointLight[i].Position.z);
    float BBoxScale = CalcPointLightBSphere(pointLight[i]);        
	p.Scale(BBoxScale, BBoxScale, BBoxScale);		
    p.SetCamera(*camera);
    p.SetPerspectiveProj(projInfo);  

    pointLightProgram->SetWVP(p.GetTrans());
    pointLightProgram->SetPointLight(pointLight[i]);
    bsphere->Render(); 
    glCullFace(GL_BACK);
   
	glDisable(GL_BLEND);
}

void DirectionalLightPass ()
{
	gbuffer->BindForLightPass();

    dirLightProgram->Enable();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

    quad->Render();  

	glDisable(GL_BLEND);
}

void FinalPass ()
{
	gbuffer->BindForFinalPass();
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                      0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Render ()
{
	scale += 0.05f;
	camera->OnRender();

	gbuffer->StartFrame();
	GeometryPass();

	// We need stencil to be enabled in the stencil pass to get the stencil buffer
	// updated and we also need it in the light pass because we render the light
	// only if the stencil passes.
	glEnable(GL_STENCIL_TEST);

	for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(pointLight); i++) {
		StencilPass(i);
		PointLightPass(i);
	}

	// The directional light does not need a stencil test because its volume
	// is unlimited and the final pass simply copies the texture.
	glDisable(GL_STENCIL_TEST);

	DirectionalLightPass();

	FinalPass();
}

void createDirLightProgram ()
{
	dirLightProgram = new DeferredDirLightProgram();
	if (!dirLightProgram->Init()) {
		exit(1);
	}
	dirLightProgram->AddShader(GL_VERTEX_SHADER, "shaders/light_pass.vs");
	dirLightProgram->AddShader(GL_FRAGMENT_SHADER, "shaders/dir_light_pass.fs");
	dirLightProgram->Compile();
	dirLightProgram->Link();
	dirLightProgram->SetDirectionLight(directionLight);
	dirLightProgram->SetScreenSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	dirLightProgram->SetPositionMapUnit(GBUFFER_POSITION_TEXTURE_UNIT);
	dirLightProgram->SetTextureUnit(GBUFFER_DIFFUSE_TEXTURE_UNIT);
	dirLightProgram->SetNormalMapUnit(GBUFFER_NORMAL_TEXTURE_UNIT);
    dirLightProgram->Enable();

    Matrix4f WVP;
    WVP.InitIdentity();
    dirLightProgram->SetWVP(WVP);
}

void createPointLightProgram ()
{
	pointLightProgram = new DeferredPointLightProgram();
	if (!pointLightProgram->Init()) {
		exit(1);
	}
	pointLightProgram->AddShader(GL_VERTEX_SHADER, "shaders/light_pass.vs");
	pointLightProgram->AddShader(GL_FRAGMENT_SHADER, "shaders/point_light_pass.fs");
	pointLightProgram->Compile();
	pointLightProgram->Link();
	pointLightProgram->SetScreenSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	pointLightProgram->SetPositionMapUnit(GBUFFER_POSITION_TEXTURE_UNIT);
	pointLightProgram->SetTextureUnit(GBUFFER_DIFFUSE_TEXTURE_UNIT);
	pointLightProgram->SetNormalMapUnit(GBUFFER_NORMAL_TEXTURE_UNIT);
    pointLightProgram->Enable();
}

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

void createNullProgram ()
{
	nullProgram = new ShaderProgram();
	if (!nullProgram->Init()) {
		exit(1);
	}
	nullProgram->AddShader(GL_VERTEX_SHADER, "shaders/null_pass.vs");
	nullProgram->AddShader(GL_FRAGMENT_SHADER, "shaders/null_pass.fs");
	nullProgram->Compile();
	nullProgram->Link();
    nullProgram->Enable();
}

SDL_Window *window;

void Init()
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

void InitLights ()
{
	directionLight.AmbientIntensity = 0.1f;
	directionLight.Color = COLOR_CYAN;
	directionLight.DiffuseIntensity = 0.5f;
	directionLight.Direction = Vector3f(1.0f, 0.0f, 0.0f);

	// pointLight[0].AmbientIntensity = 1.0f;
	pointLight[0].DiffuseIntensity = 0.2f;
	pointLight[0].Color = COLOR_GREEN;
    pointLight[0].Position = Vector3f(0.0f, 1.5f, 5.0f);
	pointLight[0].Attenuation.Constant = 0.0f;
    pointLight[0].Attenuation.Linear = 0.0f;
    pointLight[0].Attenuation.Exp = 0.3f;

    // pointLight[1].AmbientIntensity = 1.0f;
	pointLight[1].DiffuseIntensity = 0.2f;
	pointLight[1].Color = COLOR_RED;
    pointLight[1].Position = Vector3f(2.0f, 0.0f, 5.0f);
	pointLight[1].Attenuation.Constant = 0.0f;
    pointLight[1].Attenuation.Linear = 0.0f;
    pointLight[1].Attenuation.Exp = 0.3f;
    
    // pointLight[2].AmbientIntensity = 1.0f;
	pointLight[2].DiffuseIntensity = 0.2f;
	pointLight[2].Color = COLOR_BLUE;
    pointLight[2].Position = Vector3f(0.0f, 0.0f, 3.0f);
	pointLight[2].Attenuation.Constant = 0.0f;
    pointLight[2].Attenuation.Linear = 0.0f;
    pointLight[2].Attenuation.Exp = 0.3f;
}

void InitBoxes ()
{
	boxPositions[0] = Vector3f(0.0f, 0.0f, 5.0f);
    boxPositions[1] = Vector3f(6.0f, 1.0f, 10.0f);
    boxPositions[2] = Vector3f(-5.0f, -1.0f, 12.0f);
    boxPositions[3] = Vector3f(4.0f, 4.0f, 15.0f);
    boxPositions[4] = Vector3f(-4.0f, 2.0f, 20.0f);
}

int main (int argc, char *argv[])
{
	Init();
	InitLights();
	InitBoxes();

	camera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);
    projInfo.Width = WINDOW_WIDTH;
    projInfo.Height = WINDOW_HEIGHT;
	
	createNullProgram();
	createGeomProgram();
	createDirLightProgram();
	createPointLightProgram();

	gbuffer = new GBuffer();
	gbuffer->Init(WINDOW_WIDTH, WINDOW_HEIGHT);

	quad = new Mesh();
	if (!quad->LoadMesh("content/quad.obj")) {
		return 1;
	}

	box = new Mesh();
	if (!box->LoadMesh("content/box.obj")) {
		return 1;
	}

	bsphere = new Mesh();
	if (!bsphere->LoadMesh("content/sphere.obj")) {
		return 1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

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
