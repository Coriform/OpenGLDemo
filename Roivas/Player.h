#pragma once

#include "Component.h"

namespace Roivas
{
	class Player : public Component
	{
		public:
			Player();
			Player(const Player& p);
			Player* Clone();
			void Deserialize(FileIO& fio, Json::Value& root) {}
	};
}