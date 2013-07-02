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
			void LoadFromLevel(Json::Value& root);
			void Destroy();
			void AddComponent(Component* comp);	
			class Behavior*		GetBehavior()	{ return (class Behavior*)Components[CT_Behavior]; }
			class Body*			GetBody()		{ return (class Body*)Components[CT_Body]; }
			class Light*		GetLight()		{ return (class Light*)Components[CT_Light]; }
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