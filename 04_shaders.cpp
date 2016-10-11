
#include <GL/glew.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

SDL_Window *window;

const int width = 640;
const int height = 480;

GLuint vao;
GLuint vbo;

static const GLfloat vertices[] = {
	   -1.0f, -1.0f, 0.0f,
	   1.0f, -1.0f, 0.0f,
	   0.0f,  1.0f, 0.0f,
	};

static std::string vertex_shader = ""
"attribute vec4 position;"
"void main()"
"{"
"    gl_Position = vec4(0.5 * position.x, 0.5 * position.y, position.z, 1.0);"
"}";

static std::string fragment_shader = ""
"void main()"
"{"
"    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
"}";

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

	window = SDL_CreateWindow("Tutorial 04", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
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

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

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

void createShaders ()
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
}

void render ()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(
	   0,                  	// attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  	// size
	   GL_FLOAT,           	// type
	   GL_FALSE,           	// normalized?
	   0,                  	// stride
	   0            		// array buffer offset
	);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);
}

int main (int argc, char *argv[])
{
	init();
	createContext();
	createShaders();

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
