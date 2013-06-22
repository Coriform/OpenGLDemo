#include "CommonLibs.h"
#include "Core.h"

namespace Roivas
{
	Core* Core::ENGINE = nullptr;

	Core::Core(CoreParams& params) : systems(), state(1), current_time(0), previous_time(0), stepping(false)
	{
		Input::CreateInstance();

		if (params.systems & (1 << SYS_Window)) 
		{
			Window* window = new Window(params.window_width, params.window_height, params.borderless, params.full_screen);
			AddSystem(window);

			
			if (params.systems & (1 << SYS_Graphics)) 
			{			
				Graphics *graphics = new Graphics( window->GetWindowContext(), (float)params.window_width, (float)params.window_height );
				AddSystem(graphics);
			}	

			if (params.systems & (1 << SYS_Factory)) 
			{			
				Factory *factory = new Factory();
				AddSystem(factory);
			}	
			
		}
	}

	void Core::Initialize()
	{
		for( System* sys : systems )
		{
			if( sys )	sys->Initialize();
		}
	}

	void Core::Run()
	{
		previous_time = SDL_GetTicks();

		while( state > 0 )
		{
			current_time = SDL_GetTicks();		
			UpdateSystems( (float)(current_time - previous_time) );
			previous_time = current_time;			
		}
	}

	void Core::UpdateSystems(float dt)
	{
		if( systems[0] )
			systems[0]->Update(dt);

		Input::GetInstance()->Update(dt);

		for( sys_i = 1; sys_i < SYS_TOTAL; ++sys_i )
		{
			if( stepping )
			{
				if( step_through )
				{
					if( systems[sys_i] )	systems[sys_i]->Update(dt);
					step_through = false;
				}
				else
				{
					SDL_Delay(17);
					return;
				}
			}
			else
				if( systems[sys_i] )	systems[sys_i]->Update(dt);
		}
	}

	void Core::CreateInstance(CoreParams& params)
	{
		if( ENGINE == nullptr )
			ENGINE = new Core(params);
	}

	Core* Core::GetInstance()
	{
		return ENGINE;
	}

	void Core::FreeInstance() 
	{
		if( ENGINE != nullptr ) 
		{
			delete ENGINE;
			ENGINE = nullptr;
		}
	}

	void Core::AddSystem(System* system)
	{
		systems[system->GetType()] = system;
		std::cout << system->GetName() << " loaded" << std::endl;
	}

	System* Core::AccessSystem(SystemType type)
	{
		return systems[type];
	}

	void Core::ToggleStepping()
	{
		stepping = !stepping;

		if( stepping )
			step_through = false;
	}

	void Core::StepThrough()
	{
		step_through = true;
	}

	Core::~Core()
	{
		for( System* sys : systems )
		{
			if( sys )	delete sys;
		}
	}
}