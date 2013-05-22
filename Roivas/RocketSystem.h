
#include <Rocket/Core.h>

#pragma once

namespace Roivas
{
	class RocketSystem : public Rocket::Core::SystemInterface
	{
		public:
			RocketSystem();
			~RocketSystem() {}

			virtual float GetElapsedTime();

		private:
			float start_time;

	};
}