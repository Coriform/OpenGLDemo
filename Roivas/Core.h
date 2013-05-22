#pragma once
#pragma warning( disable : 4351 )

#include "Window.h"
#include "Graphics.h"
#include "System.h"
#include "Input.h"

namespace Roivas
{
	struct CoreParams
	{
		int window_width;
		int window_height;
		bool full_screen;
		bool audio;
		bool borderless;
		int level;
		unsigned systems : SYS_TOTAL;
	};

	class Core
	{
		public:			
			static void CreateInstance(CoreParams& params);
			static void FreeInstance();
			static Core* GetInstance();

			System* AccessSystem(SystemType type);

			void Run();
			void Initialize();
			void UpdateSystems(float dt);
			void SetState(int _state) { state = _state; }

			bool Stepping() { return stepping; }
			void ToggleStepping();
			void StepThrough();

		private:
			Core(CoreParams& params);
			Core& operator=(const Core& c);
			~Core();

			void AddSystem(System* sys);

			System* systems[SYS_TOTAL];
			int state;
			int sys_i;
			int current_time, previous_time;
			bool stepping, step_through;

			static Core* ENGINE;
	};

	#define GetSystem(type) \
		static_cast<type*>(Core::GetInstance()->AccessSystem(SYS_##type))
}