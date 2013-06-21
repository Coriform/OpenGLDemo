#pragma once

namespace Roivas
{
	class Component
	{
		public:
			Component();
			Component(ComponentType type);
			class Body*			GetBody();
			class Model*		GetModel();
			class Player*		GetPlayer();
			class Transform*	GetTransform();
			class Entity* Owner;
			ComponentType Type;
	};
}