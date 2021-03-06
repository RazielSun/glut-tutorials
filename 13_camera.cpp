
#include <assert.h>
#include <math.h>
#include "util_3d.h"
#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

GLuint ibo;
GLuint vbo;
GLuint uWorldLocation;

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const char* WINDOW_NAME = "Tutorial 13";

static const GLfloat vertices[] = {
   -1.0f, -1.0f, 0.0f,
   0.0f, -1.0f, 1.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

static const unsigned int indices[] = {
	0, 3, 1,
	1, 3, 2,
	2, 3, 0,
	0, 1, 2,
};

static std::string vertex_shader = ""
"attribute vec3 position;"
"uniform mat4 world;"
"varying vec4 color;"
"void main()"
"{"
"   gl_Position = world * vec4(position, 1.0);"
"	color = vec4(clamp(position, 0.0, 1.0), 1.0);"
"}";

static std::string fragment_shader = ""
"varying vec4 color;"
"void main()"
"{"
"    gl_FragColor = color;"
"}";

void render ()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float scale = 0.0f;
	scale += 0.1f;

	Pipeline p;

	p.Rotate(0.0f, scale, 0.0f);
	p.Pos(0.0f, 0.0f, 3.0f);
	p.SetPerspectiveProj(30.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 1000.0f);

	Vector3f cameraPos(0.0f, 0.5f, 5.0f); // Inverse position for camera
	Vector3f cameraTarget(0.0f, 0.0f, 2.0f);
	Vector3f cameraUp(0.0f, 1.0f, 0.0f);
	p.SetCamera(cameraPos, cameraTarget, cameraUp);

	glUniformMatrix4fv(uWorldLocation, 1, GL_TRUE, (const GLfloat*)p.GetTrans());

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
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

void createContext ()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glGenVertexArrays(1, &vbo); // may use another name variable instead vbo
	glBindVertexArray(vbo);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

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

    uWorldLocation = glGetUniformLocation(program, "world");
	assert(uWorldLocation != 0xFFFFFFFF);
}

int main (int argc, char *argv[])
{
	init();
	createContext();
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

				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running = false;
						break;
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
