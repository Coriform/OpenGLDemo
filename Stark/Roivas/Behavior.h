#pragma once

#include "Component.h"

namespace Roivas
{
	class Behavior : public Component
	{
		public:
			Behavior();
			Behavior(const Behavior& b);
			~Behavior();
			Behavior* Clone();
			void Initialize();
			void Deserialize(FileIO& fio, Json::Value& root);

		// Data

	};
}