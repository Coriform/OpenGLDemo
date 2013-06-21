#pragma once

#include "Component.h"

namespace Roivas
{
	class Entity
	{
		public:
			Entity();
			void AddComponent(Component* comp);
			class Body*			GetBody()		{ return (class Body*)components[CT_Body]; }
			class Model*		GetModel()		{ return (class Model*)components[CT_Model]; }
			class Player*		GetPlayer()		{ return (class Player*)components[CT_Player]; }
			class Transform*	GetTransform()	{ return (class Transform*)components[CT_Transform]; }
			Component* components[CT_Total];
	};
}