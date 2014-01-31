#pragma once

#include "System.h"
#include "Level.h"

namespace Roivas
{
	class Game : public System
	{
		public:
			Game();
			~Game();
			void Initialize() {}
			void Update(float dt);

		private:
			std::map<std::string,Level*> LEVEL_LIST;
	};
}