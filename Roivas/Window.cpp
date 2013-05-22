#include "CommonLibs.h"
#include "Window.h"
#include "Core.h"

namespace Roivas
{
	Window::Window(int _width = 800, int _height = 600, bool _borderless = false, bool _fullscreen = false) 
		:	System(SYS_Window, "Window"), 
			WINDOW_W(_width),
			WINDOW_H(_height),
			WINDOW_X(100),
			WINDOW_Y(100),
			window(nullptr)
	{
		SDL_Rect rect;// = nullptr;
		rect.w = 0;
		rect.h = 0;
		SDL_GetDisplayBounds(0,&rect);

		if( rect.w > 0 )	WINDOW_X = rect.w / 2 - (WINDOW_W/2);
		if( rect.h > 0 )	WINDOW_Y = rect.h / 2 - (WINDOW_H/2);
 
		Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | (SDL_WINDOW_FULLSCREEN * _fullscreen) | (SDL_WINDOW_BORDERLESS * _borderless);

		window = SDL_CreateWindow( "Roivas v1", WINDOW_X, WINDOW_Y, WINDOW_W, WINDOW_H, flags);
		if( window == nullptr )
		{
			std::cout << SDL_GetError() << std::endl;
			Core::GetInstance()->SetState(-1);
			return;
		}
		
		glcontext = SDL_GL_CreateContext(window);
		
		if( glcontext == nullptr )
		{
			std::cout << SDL_GetError() << std::endl;
			Core::GetInstance()->SetState(-1);
			return;
		}

		glewExperimental = GL_TRUE;
		glewInit();

	}

	void Window::Update(float dt)
	{
		//Event polling
		while (SDL_PollEvent(&e))
		{
			switch( e.type )
			{
				//If user closes the window
				case SDL_QUIT:
					Core::GetInstance()->SetState(-1);
					break;

				case SDL_WINDOWEVENT:	
				{
					if( e.window.event == SDL_WINDOWEVENT_RESIZED )
					{
						WINDOW_W = e.window.data1;
						WINDOW_H = e.window.data2;
						GetSystem(Graphics)->UpdateScreenDims(WINDOW_X, WINDOW_Y, WINDOW_W, WINDOW_H);
						break;
					}
				}
				
				default:
					HandleInput(e, dt);
					break;
			};
		}


		//SDL_RenderClear(renderer);
		//SDL_RenderPresent(renderer);
	}

	void Window::HandleInput(SDL_Event& event, float dt)
	{
		switch( event.type )
		{
			case SDL_MOUSEMOTION:
			{
				if( Input::GetInstance()->GetMouseButton(SDL_BUTTON_RIGHT) )
					GetSystem(Graphics)->UpdateCameraRotation((float)event.motion.xrel*dt*0.3f, (float)event.motion.yrel*dt*0.3f);				
			}
			break;

			case SDL_MOUSEBUTTONDOWN:
				Input::GetInstance()->MouseButtonEvent(event.button.button, true, event.button.x, event.button.y);
			break;

			case SDL_MOUSEBUTTONUP:
				Input::GetInstance()->MouseButtonEvent(event.button.button, false, event.button.x, event.button.y);
			break;

			case SDL_KEYDOWN:
				Input::GetInstance()->KeyboardEvent(event.key.keysym.sym, true);
			break;

			case SDL_KEYUP:
				Input::GetInstance()->KeyboardEvent(event.key.keysym.sym, false);
			break;

			default:
				break;
		}
	}

	Window::~Window()
	{
		SDL_GL_DeleteContext(glcontext);
		SDL_DestroyWindow(window);
	}
}