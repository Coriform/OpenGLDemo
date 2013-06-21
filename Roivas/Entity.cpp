#include "CommonLibs.h"
#include "Entity.h"
#include "Core.h"

namespace Roivas
{
	Entity::Entity() : components()
	{

	}

	void Entity::AddComponent(Component* comp)
	{
		components[comp->Type] = comp;
	}
}