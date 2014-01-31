#pragma once

#include "SDL.h"
#include "System.h"

namespace Roivas
{
	class Input 
	{
		public:
			static void CreateInstance();
			static void FreeInstance();
			static Input* GetInstance();

			bool GetKey(SDL_Keycode key);
			bool GetMouseButton(Uint8 button);

			void Update(float dt);
			void KeyboardEvent(SDL_Keycode, bool pressed);
			void MouseButtonEvent(Uint8 mousebutton, bool pressed, Uint16 x, Uint16 y);

			

		private:
			Input();
			Input& operator=(const Input& c);
			~Input();			

			//bool Keystate[1000];
			std::map<Uint8, bool> MouseButtons;
			std::map<SDL_Keycode,bool> Keystate;

			static Input* INPUT_SINGLETON;
	};
}