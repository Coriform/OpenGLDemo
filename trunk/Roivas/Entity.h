#pragma once

#include "Component.h"

namespace Roivas
{
	class Entity
	{
		public:
			Entity();
			Entity(const Entity& e);
			Entity* Clone();
			void Initialize();
			void Destroy();
			void AddComponent(Component* comp);	
			class Body*			GetBody()		{ return (class Body*)Components[CT_Body]; }
			class Model*		GetModel()		{ return (class Model*)Components[CT_Model]; }
			class Player*		GetPlayer()		{ return (class Player*)Components[CT_Player]; }
			class Transform*	GetTransform()	{ return (class Transform*)Components[CT_Transform]; }

		// Data
			Component* Components[CT_Total];
			GLint ID;
			bool Static;
			bool Temp;
	};
}