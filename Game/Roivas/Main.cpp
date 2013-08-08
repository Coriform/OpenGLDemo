#include "CommonLibs.h"

#include "SDL.h"
#include "SDL_opengl.h"
#include "Core.h"

using namespace Roivas;
using namespace std;
 
int main(int argc, char** argv)
{
	if( SDL_Init(SDL_INIT_EVERYTHING) == -1 )
	{
		std::cout << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE,		8	);
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,		8	);
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,		8	);
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE,		8	);
	SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE,	32	);
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE,	8	);
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,		24	);
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER,	1	);

	CoreParams engine_parameters;
	engine_parameters.window_width	= 1152;
	engine_parameters.window_height = 864;
	engine_parameters.full_screen	= false;
	engine_parameters.audio			= false;
	engine_parameters.borderless	= false;;
	engine_parameters.level			= 1;
	engine_parameters.systems		= 1 << SYS_Window | 1 << SYS_Graphics | 1 << SYS_Factory | 1 << SYS_Game;

	/*
	engine_parameters.systems = 1 << SYS_Input | 
								 1 << SYS_ViewWindow |
								 1 << SYS_ResourceManager |
								 1 << SYS_Graphics |
								 1 << SYS_GameLogic |
								 1 << SYS_ObjectManager |
								 1 << SYS_Physics|								
								 1 << SYS_GameStateManager |
								 1 << SYS_LevelEditor |
								 1 << SYS_Audio |
								 1 << SYS_MenuManager |
								 1 << SYS_AI;
								 */
	
	Core::CreateInstance(engine_parameters);
	Core* engine = Core::GetInstance();

	engine->Initialize();
	engine->Run();

	Core::FreeInstance();

	SDL_Quit();
 
	return 0;
	
}