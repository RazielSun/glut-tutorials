
#include <assert.h>
#include <math.h>
#include "utils/util_3d.h"
#include "utils/util_camera.h"
#include "utils/util_pipeline.h"
#include "utils/util_texture.h"
#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

const char* WINDOW_NAME = "Tutorial 18";

GLuint ibo;
GLuint vbo;

// UNIFORMS FOR SHADERS
GLuint uWVP;
GLuint uWorld;
GLuint uSampler;
GLuint uLightColor;
GLuint uLightDirection;
GLuint uAmbientIntensity;
GLuint uDiffuseIntensity;

Camera *camera = NULL;
Texture *texture = NULL;

static std::string vertex_shader = ""
"attribute vec3 position;"
"attribute vec2 texCoord;"
"attribute vec3 normal;"
"uniform mat4 WVP;"
"uniform mat4 world;"
"uniform vec3 lightDirection;"
"varying vec2 uvCoord;"
"varying float diffuseFactor;"
"void main()"
"{"
"   gl_Position = WVP * vec4(position, 1.0);"
"	uvCoord = texCoord;"
"	vec3 vNormal = (world * vec4(normal, 0.0)).xyz;"
"	diffuseFactor = dot(normalize(vNormal), -lightDirection);"
"}";

static std::string fragment_shader = ""
"uniform sampler2D sampler;"
"uniform vec3 lightColor;"
"uniform float ambientIntensity;"
"uniform float diffuseIntensity;"
"varying vec2 uvCoord;"
"varying float diffuseFactor;"
"void main()"
"{"
"	vec4 ambient = vec4(lightColor * ambientIntensity, 1.0);"
"	vec4 diffuse;"
"	if (diffuseFactor > 0.0) {"
"		diffuse = vec4(lightColor * diffuseIntensity * diffuseFactor, 1.0);"
"	} else {"
"		diffuse = vec4(0.0, 0.0, 0.0, 0.0);"
"	}"
"	gl_FragColor = texture2D(sampler, uvCoord) * (ambient + diffuse);"
"}";

static float ambientIntensity = 0.5f;
static float diffuseIntensity = 0.5f;

void render ()
{
	camera->OnRender();

	glClear(GL_COLOR_BUFFER_BIT);

	static float scale = 0.0f;
	scale += 0.1f;

	Pipeline p;

	p.Rotate(0.0f, scale, 0.0f);
	p.Pos(0.0f, 0.0f, 3.0f);
	p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

	p.SetCamera(*camera);

	glUniform3f(uLightColor, 1.0f, 1.0f, 1.0f);
	glUniform3f(uLightDirection, 1.0f, 0.0f, 0.0f);
	glUniform1f(uAmbientIntensity, ambientIntensity);
	glUniform1f(uDiffuseIntensity, diffuseIntensity);
	glUniformMatrix4fv(uWorld, 1, GL_TRUE, (const GLfloat*)p.GetWorldTrans());
	glUniformMatrix4fv(uWVP, 1, GL_TRUE, (const GLfloat*)p.GetTrans());
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	texture->Bind(GL_TEXTURE0);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void CalcNormals(const unsigned int* pIndices, unsigned int IndexCount, Vertex* pVertices, unsigned int VertexCount)
{
    for (unsigned int i = 0 ; i < IndexCount ; i += 3) {
        unsigned int Index0 = pIndices[i];
        unsigned int Index1 = pIndices[i + 1];
        unsigned int Index2 = pIndices[i + 2];
        Vector3f v1 = pVertices[Index1].pos - pVertices[Index0].pos;
        Vector3f v2 = pVertices[Index2].pos - pVertices[Index0].pos;
        Vector3f Normal = v1.Cross(v2);
        Normal.Normalize();

        pVertices[Index0].normal += Normal;
        pVertices[Index1].normal += Normal;
        pVertices[Index2].normal += Normal;
    }

    for (unsigned int i = 0 ; i < VertexCount ; i++) {
        pVertices[i].normal.Normalize();
    }
}

void createContext ()
{
	unsigned int indices[] = { 0, 3, 1,
                               1, 3, 2,
                               2, 3, 0,
                               0, 1, 2 };

	unsigned int indexCount = ARRAY_SIZE_IN_ELEMENTS(indices);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	Vertex vertices[4] = { Vertex(Vector3f(-1.0f, -1.0f, 0.5773f), Vector2f(0.0f, 0.0f)),
                           Vertex(Vector3f(0.0f, -1.0f, -1.15475f), Vector2f(0.5f, 0.0f)),
                           Vertex(Vector3f(1.0f, -1.0f, 0.5773f),  Vector2f(1.0f, 0.0f)),
                           Vertex(Vector3f(0.0f, 1.0f, 0.0f),      Vector2f(0.5f, 1.0f)) };

	unsigned int vertexCount = ARRAY_SIZE_IN_ELEMENTS(vertices);

    CalcNormals(indices, indexCount, vertices, vertexCount);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void addShader (GLuint program, const char* shaderText, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &shaderText, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", shaderType, InfoLog);
        exit(1);
    }

    glAttachShader(program, shader);
}

void createShaderProgram ()
{
	GLuint program = glCreateProgram();

	addShader(program, vertex_shader.c_str(), GL_VERTEX_SHADER);
	addShader(program, fragment_shader.c_str(), GL_FRAGMENT_SHADER);

	GLint success = 0;

	glBindAttribLocation(program, 0, "position");
    glBindAttribLocation(program, 1, "texCoord");
    glBindAttribLocation(program, 2, "normal");

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    GLchar ErrorLog[1024] = { 0 };

	if (success == 0)
	{
		glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	}

    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);

    if (!success) 
   	{
        glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(program);

    uWVP = glGetUniformLocation(program, "WVP");
	assert(uWVP != 0xFFFFFFFF);
	uWorld = glGetUniformLocation(program, "world");
	assert(uWorld != 0xFFFFFFFF);
	uSampler = glGetUniformLocation(program, "sampler");
	assert(uSampler != 0xFFFFFFFF);
	uLightColor = glGetUniformLocation(program, "lightColor");
	assert(uLightColor != 0xFFFFFFFF);
	uAmbientIntensity = glGetUniformLocation(program, "ambientIntensity");
	assert(uAmbientIntensity != 0xFFFFFFFF);
	uLightDirection = glGetUniformLocation(program, "lightDirection");
	assert(uLightDirection != 0xFFFFFFFF);
	uDiffuseIntensity = glGetUniformLocation(program, "diffuseIntensity");
	assert(uDiffuseIntensity != 0xFFFFFFFF);
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

	createContext();
	createShaderProgram();
    glUniform1i(uSampler, 0);

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
						case SDLK_s:
							ambientIntensity += 0.1f;
							break;
						case SDLK_a:
							ambientIntensity -= 0.1f;
							break;
						case SDLK_x:
							diffuseIntensity += 0.1f;
							break;
						case SDLK_z:
							diffuseIntensity -= 0.1f;
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
