#include "CommonLibs.h"
#include "Core.h"

namespace Roivas
{
	Input* Input::INPUT_SINGLETON = nullptr;

	Input::Input()
	{
		Keystate.clear();
		MouseButtons.clear();
	}

	void Input::Update(float dt)
	{
		// Update controllers

	}

	void Input::KeyboardEvent(SDL_Keycode key, bool pressed)
	{
		switch( key )
		{
			case SDLK_ESCAPE:
				Core::GetInstance()->SetState(-1);
			break;

			case SDLK_p:
			{
				if( pressed )
				{
					Core::GetInstance()->ToggleStepping();
					std::cout << "Toggle Stepping" << std::endl;
				}
			}
			break;

			case SDLK_SPACE:
			{
				if( pressed )
				{
					if( Core::GetInstance()->Stepping() )
					{
						Core::GetInstance()->StepThrough();
						//std::cout << "Step Through" << std::endl;
					}
				}
			}
			break;

			case SDLK_LEFT:
				GetSystem(Graphics)->CameraYaw(-1.0f);
			break;

			case SDLK_RIGHT:
				GetSystem(Graphics)->CameraYaw(1.0f);
			break;

			case SDLK_UP:
				GetSystem(Graphics)->CameraPitch(1.0f);
			break;

			case SDLK_DOWN:
				GetSystem(Graphics)->CameraPitch(-1.0f);
			break;

			default:
				break;
		}
		
		Keystate[key] = pressed;
	}

	void Input::MouseButtonEvent(Uint8 mousebutton, bool pressed, Uint16 x, Uint16 y)
	{
		switch( mousebutton )
		{
			case SDL_BUTTON_LEFT:
			break;

			case SDL_BUTTON_MIDDLE:
			break;

			case SDL_BUTTON_RIGHT:
			break;

			default:
				break;
		}

		MouseButtons[mousebutton] = pressed;
	}

	bool Input::GetKey(SDL_Keycode key) 
	{ 
		if( Keystate.find(key) == Keystate.end() )
			Keystate[key] = false;

		return Keystate.at(key); 
	}

	bool Input::GetMouseButton(Uint8 button)
	{
		if( MouseButtons.find(button) == MouseButtons.end() )
			MouseButtons[button] = false;

		return MouseButtons.at(button); 
	}

	void Input::CreateInstance()
	{
		if( INPUT_SINGLETON == nullptr )
			INPUT_SINGLETON = new Input();
	}

	Input* Input::GetInstance()
	{
		return INPUT_SINGLETON;
	}

	void Input::FreeInstance() 
	{
		if( INPUT_SINGLETON != nullptr ) 
		{
			delete INPUT_SINGLETON;
			INPUT_SINGLETON = nullptr;
		}
	}

	Input::~Input()
	{

	}
}