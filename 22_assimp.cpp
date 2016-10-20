
#include <assert.h>
#include <math.h>
#include "utils/utils.h"
#include "utils/camera.h"
#include "utils/pipeline.h"
#include "utils/light.h"
#include "utils/mesh.h"
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

const char* WINDOW_NAME = "Tutorial 22";

DirectionLight directionLight;

Camera* camera = NULL;
LightProgram* program = NULL;
Mesh* mesh = NULL;

static std::string vertex_shader = ""
"attribute vec3 position;"
"attribute vec2 texCoord;"
"attribute vec3 normal;"
"uniform mat4 WVP;"
"uniform mat4 world;"
"varying vec2 uvCoord;"
"varying vec3 normal0;"
"varying vec3 worldPos0;"
"void main()"
"{"
"   gl_Position = WVP * vec4(position, 1.0);"
"	uvCoord = texCoord;"
"	normal0 = (world * vec4(normal, 0.0)).xyz;"
"	worldPos0 = (world * vec4(position, 1.0)).xyz;"
"}";

static std::string fragment_shader = ""
"const int MAX_LIGHTS = 2;"
"struct BaseLight"
"{"
"	vec3 Color;"
"	float AmbientIntensity;"
"	float DiffuseIntensity;"
"};"
"struct DirectionalLight"
"{"
"	BaseLight Base;"
"	vec3 Direction;"
"};"
"struct Attenuation"
"{"
"	float Constant;"
"	float Linear;"
"	float Exp;"
"};"
"struct PointLight"
"{"
"	BaseLight Base;"
"	vec3 Position;"
"	Attenuation Atten;"
"};"
"struct SpotLight"
"{"
"	PointLight Base;"
"	vec3 Direction;"
"	float Cutoff;"
"};"
"uniform DirectionalLight directionLight;"
"uniform PointLight pointLights[MAX_LIGHTS];"
"uniform SpotLight spotLights[MAX_LIGHTS];"
"uniform int numPointLights;"
"uniform int numSpotLights;"
"uniform sampler2D sampler;"
"varying vec2 uvCoord;"
"varying vec3 normal0;"
"varying vec3 worldPos0;"
"vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)"
"{"
"    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0);"
"    float DiffuseFactor = dot(Normal, -LightDirection);"
"    vec4 DiffuseColor  = vec4(0, 0, 0, 0);"
"    if (DiffuseFactor > 0.0) {"
"        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0);"
"    }"
"    return (AmbientColor + DiffuseColor);"
"}"
"vec4 CalcDirectionalLight(vec3 Normal)"
"{"
"	return CalcLightInternal(directionLight.Base, directionLight.Direction, Normal);"
"}"
"vec4 CalcPointLight(PointLight l, vec3 Normal)"
"{"
"	vec3 LightDirection = worldPos0 - l.Position;"
"	float Distance = length(LightDirection);"
"   LightDirection = normalize(LightDirection);"
"   vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal);"
"   float Atn = l.Atten.Constant + l.Atten.Linear * Distance + l.Atten.Exp * Distance * Distance;"
"	return Color / Atn;"
"}"
"vec4 CalcSpotLight(SpotLight l, vec3 Normal)"
"{"
"	vec3 LightToPixel = normalize(worldPos0 - l.Base.Position);"
"	float SpotFactor = dot(LightToPixel, l.Direction);"
"	if (SpotFactor > l.Cutoff) {"
"		vec4 Color = CalcPointLight(l.Base, Normal);"
"       return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));"
"   } else {"
"       return vec4(0.0, 0.0, 0.0, 0.0);"
"   }"
"}"
"void main()"
"{"
"    vec3 Normal = normalize(normal0);"
"    vec4 TotalLight = CalcDirectionalLight(Normal);"
"    for (int i = 0; i < numPointLights; i++) {"
"        TotalLight += CalcPointLight(pointLights[i], Normal);"
"    }"
"	 for (int i = 0; i < numSpotLights; i++) {"
"	     TotalLight += CalcSpotLight(spotLights[i], Normal);"
"	 }"
"    gl_FragColor = texture2D(sampler, uvCoord) * TotalLight;"
"}";

void render ()
{
	camera->OnRender();

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	static float scale = 0.0f;
	scale += 0.1f;

	Pipeline p;
	p.Scale(0.1f, 0.1f, 0.1f);
	p.Rotate(0.0f, scale, 0.0f);
	p.Pos(0.0f, 0.0f, 10.0f);
	p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);
	p.SetCamera(*camera);
	
	program->SetWVP(p.GetTrans());
	program->SetWorld(p.GetWorldTrans());
	program->SetDirectionLight(directionLight);
	program->SetPointLights(0, NULL);
	program->SetSpotLights(0, NULL);

	mesh->Render();
}

void createShaderProgram ()
{
	program = new LightProgram();
	if (!program->Init()) {
		exit(1);
	}
	program->AddShader(GL_VERTEX_SHADER, vertex_shader.c_str());
	program->AddShader(GL_FRAGMENT_SHADER, fragment_shader.c_str());
	program->Compile();
	program->Link();
	glUniform1i(program->GetUniformLocation("sampler"), 0);
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

	Vector3f pos(3.0f, 7.0f, -10.0f);
	Vector3f target(0.0f, -0.2f, 1.0f);
	Vector3f up(0.0f, 1.0f, 0.0f);

	camera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, pos, target, up);

	mesh = new Mesh();
	mesh->LoadMesh("content/phoenix_ugv.md2");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    directionLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    directionLight.AmbientIntensity = 1.0f; 
    directionLight.DiffuseIntensity = 0.01f;
    directionLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);
    
	createShaderProgram();

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
