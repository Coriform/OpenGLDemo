#pragma once

#include "FileIO.h"

namespace Roivas
{
	class Component
	{
		public:
			Component();
			Component(ComponentType type);
			Component(const Component& c);
			virtual Component* Clone() { return nullptr; }
			virtual void Deserialize(FileIO& fio, Json::Value& root) = 0;
			virtual void Initialize() {}
			class Behavior*		GetBehavior();
			class Body*			GetBody();
			class Light*		GetLight();
			class Model*		GetModel();
			class Player*		GetPlayer();
			class Transform*	GetTransform();

		// Data
			class Entity* Owner;
			ComponentType Type;
	};
}