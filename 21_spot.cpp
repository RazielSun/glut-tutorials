
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
#define MAX_LIGHTS 2

const char* WINDOW_NAME = "Tutorial 20";

GLuint ibo;
GLuint vbo;
DirectionLight directionLight;
SpotLight spotLights[MAX_LIGHTS];

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

	glClear(GL_COLOR_BUFFER_BIT);

	static float scale = 0.0f;
	scale += 0.01f;

	Pipeline p;

	float Z = 3.0f;
	spotLights[0].Position = camera->GetPos();
    spotLights[0].Direction = camera->GetTarget();
	spotLights[1].Position = Vector3f(-2.0f*sinf(scale), 3.0f, Z+2.0f*cosf(scale));

	p.Pos(0.0f, 0.0f, Z);
	p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);
	p.SetCamera(*camera);
	
	program->SetWVP(p.GetTrans());
	program->SetWorld(p.GetWorldTrans());
	program->SetDirectionLight(directionLight);
	program->SetPointLights(0, NULL);
	program->SetSpotLights(2, spotLights);
	
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

void addPointIntensity(float delta)
{
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		spotLights[i].AmbientIntensity += delta;
	    spotLights[i].DiffuseIntensity += delta;
	}
}

int main (int argc, char *argv[])
{
	init();

	camera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);
	camera->SetPos(0.0f, 2.0f, -2.0f);
	// camera->LookAt(0.0f, 0.0f, 3.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    directionLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    directionLight.AmbientIntensity = 0.1f; 
    directionLight.DiffuseIntensity = 0.1f;
    directionLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);

    spotLights[0].DiffuseIntensity = 0.9f;
    spotLights[0].Color = Vector3f(0.0f, 1.0f, 1.0f);
    spotLights[0].Position = camera->GetPos();
    spotLights[0].Direction = camera->GetTarget();
    spotLights[0].Attenuation.Linear = 0.1f;
    spotLights[0].Cutoff = 10.0f;

    spotLights[1].DiffuseIntensity = 0.9f;
    spotLights[1].Color = Vector3f(1.0f, 1.0f, 1.0f);
    // spotLights[1].Position = Vector3f(0.0f, 3.0f, 3.0f);
    spotLights[1].Direction = Vector3f(0.0f, -1.0f, 0.0f);
    spotLights[1].Attenuation.Linear = 0.1f;
    spotLights[1].Cutoff = 20.0f;
    
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
						case SDLK_q:
							directionLight.AmbientIntensity += 0.1f;
							break;
						case SDLK_w:
							directionLight.AmbientIntensity -= 0.1f;
							break;
						case SDLK_a:
							addPointIntensity(0.1f);
							break;
						case SDLK_s:
							addPointIntensity(-0.1f);
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
