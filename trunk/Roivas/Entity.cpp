#include "CommonLibs.h"
#include "Entity.h"
#include "Core.h"

namespace Roivas
{
	Entity::Entity() : Components(), Temp(true), ID(-1)
	{

	}

	Entity::Entity(const Entity& e) : Components(), Temp(true), ID(-1)
	{
		for( unsigned i = 0; i < CT_Total; ++i )
		{
			if( e.Components[i] == nullptr )
				continue;

			Components[i] = e.Components[i]->Clone();
			Components[i]->Owner = this;
		}
	}

	Entity* Entity::Clone()
	{
		return new Entity(*this);
	}

	void Entity::Initialize()
	{
		for( unsigned i = 0; i < CT_Total; ++i )
		{
			if( Components[i] != nullptr )
				Components[i]->Initialize();
		}
	}

	void Entity::Destroy()
	{
		Factory::Destroy(this);
	}

	void Entity::AddComponent(Component* comp)
	{
		Components[comp->Type] = comp;
	}
}