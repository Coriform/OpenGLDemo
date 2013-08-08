#pragma once

#include "Component.h"

namespace Roivas
{
	class Body : public Component
	{
		public:
			Body();
			Body(const Body& b);
			Body* Clone();
			void Initialize();
			void Deserialize(FileIO& fio, Json::Value& root) {}
	};
}