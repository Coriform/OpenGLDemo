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

			case SDLK_l:
			{
				if( pressed )
				{
					if( GetSystem(Graphics)->current_lighting == SH_LightingSSM )
					{
						GetSystem(Graphics)->current_lighting = SH_LightingDSM;
						std::cout << " >< Deferred Shadow Maps" << std::endl;
					}
					else if( GetSystem(Graphics)->current_lighting == SH_LightingDSM )
					{
						GetSystem(Graphics)->current_lighting = SH_Lighting;
						std::cout << " >< No Shadow Maps" << std::endl;
					}
					else if( GetSystem(Graphics)->current_lighting == SH_Lighting )
					{
						GetSystem(Graphics)->current_lighting = SH_LightingSSM;
						std::cout << " >< Simple Shadow Maps" << std::endl;
					}
				}
			}
			break;

			case SDLK_n:
			{
				if( pressed )
				{
					GetSystem(Graphics)->normal_mapping_enabled = !GetSystem(Graphics)->normal_mapping_enabled ;
				}
			}
			break;

			case SDLK_k:
			{
				if( pressed )
				{
					GetSystem(Graphics)->wireframe_enabled = !GetSystem(Graphics)->wireframe_enabled ;
				}
			}
			break;

			case SDLK_LEFT:
				if( pressed )
				{
					GetSystem(Graphics)->PrevRT();
				}
			break;

			case SDLK_RIGHT:
				if( pressed )
				{
					GetSystem(Graphics)->NextRT();
				}
			break;

			case SDLK_UP:
				//GetSystem(Graphics)->CameraPitch(1.0f);
			break;

			case SDLK_DOWN:
				//GetSystem(Graphics)->CameraPitch(-1.0f);
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
				GetSystem(Graphics)->MouseSelectEntity( float(x), float(y), pressed );
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