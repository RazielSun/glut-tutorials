
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <iostream>

#include "utils/utils.h"
#include "utils/skybox.h"
#include "utils/camera.h"
#include "utils/pipeline.h"
#include "utils/lights_common.h"
#include "utils/skinning_program.h"
#include "utils/skinned_only_mesh.h"
#include "utils/skeletal_animation.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 768

const char* WINDOW_NAME = "Tutorial 40";

PersProjInfo projInfo;
Camera* camera = NULL;

DirectionLight directionLight;

SkinningProgram* program = NULL;

SkinnedOnlyMesh* mesh = NULL;
SkeletalAnimation* anims[4];
std::string AnimNames[] = {
	"content/assault_combat_idle.FBX",
	"content/assault_combat_run.FBX",
	"content/assault_combat_shoot_burst.FBX",
	"content/assault_combat_shoot.FBX"
};

//Analog joystick dead zone
// const int JOYSTICK_DEAD_ZONE = 8000;
//Game Controller 1 handler
// SDL_Joystick* gameController = NULL;

#define MAX_CONTROLLERS 4
SDL_GameController *ControllerHandles[MAX_CONTROLLERS];

static float scale = 0.0f;
static long long m_startTime = 0;
static uint animIndex = 0;

float GetRunningTime()
{
    float RunningTime = (float)((double)GetCurrentTimeMillis() - (double)m_startTime) / 1000.0f;
    return RunningTime;
}

void Render ()
{
	camera->OnRender();

	scale += 0.1f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float RunningTime = GetRunningTime();

	std::vector<Matrix4f> Transforms;

	anims[animIndex]->BoneTransform(RunningTime, mesh, Transforms);

	// printf("set Bones: %f %lu\n", RunningTime, Transforms.size());

	for (uint i = 0 ; i < Transforms.size() ; i++) {
        program->SetBoneTransform(i, Transforms[i]);
    }

	Pipeline p;
	p.Pos(0.0f, 0.0f, 6.0f);
	p.Scale(0.1f, 0.1f, 0.1f);
	p.Rotate(0.0f, 90.0f, 0.0f);
	p.SetCamera(*camera);
    p.SetPerspectiveProj(projInfo); 
	
	program->SetWVP(p.GetTrans());
	program->SetWorld(p.GetWorldTrans());

	mesh->Render();
}

void createProgram ()
{
	program = new SkinningProgram();
	if (!program->Init()) {
		exit(1);
	}
	program->AddShader(GL_VERTEX_SHADER, "shaders/skinning_shader.vs");
	program->AddShader(GL_FRAGMENT_SHADER, "shaders/skinning_shader.fs");
	program->Compile();
	program->Link();
	program->SetDirectionLight(directionLight);
    program->Enable();
}

SDL_Window *window;

void Init()
{
	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER ) < 0 )
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

	//Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
        printf( "Warning: Linear texture filtering not enabled!" );
    }

    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");

    int MaxJoysticks = SDL_NumJoysticks();

	int ControllerIndex = 0;
	for(int JoystickIndex=0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
	{
	    if (!SDL_IsGameController(JoystickIndex))
	    {
	    	printf( "! SDL_IsGameController %d\n", JoystickIndex );
	        continue;
	    }
	    if (ControllerIndex >= MAX_CONTROLLERS)
	    {
	        break;
	    }
	    ControllerHandles[ControllerIndex] = SDL_GameControllerOpen( JoystickIndex );
	    ControllerIndex++;
	}
}

void InitLights ()
{
	directionLight.AmbientIntensity = 0.55f;
	directionLight.Color = COLOR_YELLOW;
	directionLight.DiffuseIntensity = 0.9f;
	directionLight.Direction = Vector3f(1.0f, 0.0f, 0.0f);
}

void AddController ( int id )
{
    if ( id < MAX_CONTROLLERS && SDL_IsGameController( id ) ) {
        ControllerHandles[id] = SDL_GameControllerOpen( id );

        if ( ControllerHandles[id] ) {
            SDL_Joystick *joy = SDL_GameControllerGetJoystick( ControllerHandles[id] );
            int instanceID = SDL_JoystickInstanceID( joy );

            printf( "add controller: %d = %d\n", id, instanceID );
        }
    }
}

void RemoveController ( int id )
{
    SDL_GameController *pad = ControllerHandles[id];
    SDL_GameControllerClose( pad );
    printf( "remove controller: %d\n", id );
}

int main (int argc, char *argv[])
{
	Init();
	InitLights();

	Vector3f Pos(0.0f, 3.0f, -1.0f);
    Vector3f Target(0.0f, 0.0f, 1.0f);
    Vector3f Up(0.0, 1.0f, 0.0f);

	camera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);
    projInfo.Width = WINDOW_WIDTH;
    projInfo.Height = WINDOW_HEIGHT;
	
	createProgram();

	mesh = new SkinnedOnlyMesh();
	if (!mesh->LoadMesh("content/ToonSoldier.FBX")) {
		return 1;
	}

	for (uint i = 0; i < 4; i++) {
		anims[i] = new SkeletalAnimation();
		anims[i]->Load(AnimNames[i]);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    m_startTime = GetCurrentTimeMillis();
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
				// case SDL_JOYAXISMOTION: // sticks
				// 	
				// 	printf("axis: %d value: %d which: %d\n", event.jaxis.axis, event.jaxis.value, event.jbutton.which);
				// break;
				// case SDL_JOYBUTTONDOWN: // all buttons
				// case SDL_JOYBUTTONUP:
				// 	printf("joy button: %d state: %d which: %d\n", event.jbutton.button, event.jbutton.state, event.jbutton.which);
				// break;
				// case SDL_JOYHATMOTION: // cross
				// 	
				// 	printf("hat: %d value: %d which: %d\n", event.jhat.hat, event.jhat.value, event.jbutton.which);
				// break;
				case SDL_CONTROLLERDEVICEADDED:
					AddController( event.cdevice.which );
					// printf("controller added %d\n", event.cdevice.which);
					break;
				case SDL_CONTROLLERDEVICEREMOVED:
					RemoveController( event.cdevice.which );
					// printf("controller removed %d\n", event.cdevice.which);
					break;
				case SDL_CONTROLLERBUTTONDOWN:
				case SDL_CONTROLLERBUTTONUP:
					switch(event.jbutton.button)
					{
						case 0:
						case 1:
						case 2:
						case 3:
							if (event.cbutton.state == 1)
							{
								animIndex++;
								if (animIndex >= 4)
									animIndex = 0;
							}
							break;
						case 11:
						case 12:
						case 13:
						case 14:
							if (event.cbutton.state == 1)
								camera->OnJoyHat(event.jbutton.button);
							break;
					}
					printf("controller button which:%d button:%d state:%d\n", event.cbutton.which, event.cbutton.button, event.cbutton.state);
					break;
				case SDL_CONTROLLERAXISMOTION:
					camera->OnJoyAxis(event.caxis.axis, event.caxis.value);
					printf("controller axis which:%d axis:%d value:%d\n", event.caxis.which, event.caxis.axis, event.caxis.value);
					break;
			}
		}
		
		camera->OnUpdate();
		Render();
		SDL_GL_SwapWindow(window);
	}

	for(int ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
	{
	   if (ControllerHandles[ControllerIndex])
	   {
	       SDL_GameControllerClose(ControllerHandles[ControllerIndex]);
	   }
	}
	// SDL_JoystickClose( gameController );
 //    gameController = NULL;

	SDL_Quit();

	return 0;
}
