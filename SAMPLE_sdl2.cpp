#include <SDL2/SDL.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

#include <iostream>
using namespace std;

SDL_Window *window;

const int width = 640;
const int height = 480;

void init()
{
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		cout << "Unable to init SDL, error: " << SDL_GetError() << endl;
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

	window = SDL_CreateWindow("Cube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	SDL_GLContext glcontext = SDL_GL_CreateContext(window); // создаем контекст OpenGL

	if (window == NULL)
	{
		exit(1);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	// glShadeModel(GL_SMOOTH);
	// glMatrixMode(GL_PROJECTION);
	// glLoadIdentity();
	// gluPerspective(45.0f, (float) width / (float) height, 0.1f, 100.0f);
	// glMatrixMode(GL_MODELVIEW);
}

GLuint vao;
GLuint vbo;

static const GLfloat vertices[] = {
	   -1.0f, -1.0f, 0.0f,
	   1.0f, -1.0f, 0.0f,
	   0.0f,  1.0f, 0.0f,
	};

void createContext ()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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
	glDrawArrays(GL_POINTS, 0, 3); //GL_TRIANGLES
	glDisableVertexAttribArray(0);

	glFlush();
}

int main (int argc, char *argv[])
{
	init();
	createContext();

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