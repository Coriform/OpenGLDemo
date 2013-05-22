#pragma once

#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_ttf.h"
#include "System.h"

namespace Roivas
{
	class Window : public System
	{
		public:
			Window(int width, int height, bool borderless, bool fullscreen);
			~Window();
			void Initialize() {}
			void Update(float dt);
			void HandleInput(SDL_Event& key, float dt);

			SDL_Window* GetWindowContext()		{ return window; }
			SDL_GLContext GetGLContext()		{ return glcontext; }

		private:
			SDL_Window*		window;
			SDL_GLContext	glcontext;
			SDL_Event e;

			int WINDOW_W;
			int WINDOW_H;
			int WINDOW_X;
			int WINDOW_Y;
	};
}