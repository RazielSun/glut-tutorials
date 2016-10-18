
#include <assert.h>
#include <math.h>
#include "utils/util_3d.h"
#include "utils/util_camera.h"
#include "utils/util_pipeline.h"
#include "utils/util_texture.h"
#include "utils/util_light.h"
#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define PLANE_WIDTH 5.0f
#define PLANE_HEIGHT 5.0f

const char* WINDOW_NAME = "Tutorial 20";

GLuint ibo;
GLuint vbo;
DirectionLight directionLight;
PointLight pointLights[2];

Camera *camera = NULL;
Texture *texture = NULL;
LightProgram *program = NULL;

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
"const int MAX_POINT_LIGHTS = 2;"
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
"uniform DirectionalLight directionLight;"
"uniform PointLight pointLight[MAX_POINT_LIGHTS];"
"uniform int numPointLights;"
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
"vec4 CalcPointLight(int Index, vec3 Normal)"
"{"
"	vec3 LightDirection = worldPos0 - pointLight[Index].Position;"
"	float Distance = length(LightDirection);"
"   LightDirection = normalize(LightDirection);"
"   vec4 Color = CalcLightInternal(pointLight[Index].Base, LightDirection, Normal);"
"   float Atn = pointLight[Index].Atten.Constant + pointLight[Index].Atten.Linear * Distance + pointLight[Index].Atten.Exp * Distance * Distance;"
"	return Color / Atn;"
"}"
"void main()"
"{"
"    vec3 Normal = normalize(normal0);"
"    vec4 TotalLight = CalcDirectionalLight(Normal);"
"    for (int i = 0; i < numPointLights; i++) {"
"        TotalLight += CalcPointLight(i, Normal);"
"    }"
"    gl_FragColor = texture2D(sampler, uvCoord) * TotalLight;"
"}";

void render ()
{
	camera->OnRender();

	glClear(GL_COLOR_BUFFER_BIT);

	static float scale = 0.0f;
	scale += 0.1f;

	Pipeline p;

	// p.Rotate(0.0f, scale, 0.0f);
	p.Pos(0.0f, 0.0f, 3.0f);
	p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);
	p.SetCamera(*camera);
	
	program->SetWVP(p.GetTrans());
	program->SetWorld(p.GetWorldTrans());
	program->SetDirectionLight(directionLight);
	program->SetPointLights(2, pointLights);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	texture->Bind(GL_TEXTURE0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void createContext ()
{
	unsigned int indices[] = { 0, 1, 2,
                               0, 2, 3 };

	unsigned int indexCount = ARRAY_SIZE_IN_ELEMENTS(indices);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	Vector3f Normal(0.0f, 1.0f, 0.0f);

	Vertex vertices[4] = { Vertex(Vector3f(-PLANE_WIDTH, 0.0f, -PLANE_HEIGHT), 	Vector2f(0.0f, 0.0f), Normal),
                           Vertex(Vector3f(-PLANE_WIDTH, 0.0f, PLANE_HEIGHT), 	Vector2f(0.0f, 1.0f), Normal),
                           Vertex(Vector3f(PLANE_WIDTH, 0.0f, PLANE_HEIGHT),  	Vector2f(1.0f, 1.0f), Normal),
                           Vertex(Vector3f(PLANE_WIDTH, 0.0f, -PLANE_HEIGHT),	Vector2f(1.0f, 0.0f), Normal) };

	unsigned int vertexCount = ARRAY_SIZE_IN_ELEMENTS(vertices);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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

	camera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);
	camera->SetPos(0.0f, -2.0f, 2.0f);
	camera->LookAt(0.0f, 0.0f, 3.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    directionLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    directionLight.AmbientIntensity = 0.1f; 
    directionLight.DiffuseIntensity = 0.1f;
    directionLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);

    pointLights[0].DiffuseIntensity = 20.0f;
    pointLights[0].Color = Vector3f(1.0f, 0.5f, 0.0f);
    pointLights[0].Position = Vector3f(1.0f, 0.5f, 1.0f);
    pointLights[0].Attenuation.Constant = 0.01f;
    pointLights[0].Attenuation.Linear = 0.1f;
    pointLights[0].Attenuation.Exp = 0.001f;
    pointLights[1].DiffuseIntensity = 20.0f;
    pointLights[1].Color = Vector3f(0.0f, 0.5f, 1.0f);
    pointLights[1].Position = Vector3f(-1.0f, 0.5f, 1.0f);
    pointLights[1].Attenuation.Constant = 0.01f;
    pointLights[1].Attenuation.Linear = 0.1f;
    pointLights[1].Attenuation.Exp = 0.001f;

	createContext();
	createShaderProgram();

	texture = new Texture(GL_TEXTURE_2D, "content/uvLayoutGrid.jpg");
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
