#include "CommonLibs.h"
#include "Component.h"
#include "Entity.h"
#include "Core.h"

namespace Roivas
{
	Component::Component() : Type(CT_NONE)
	{
		
	}

	Component::Component(ComponentType type) : Type(type) 
	{
	
	}

	Body* Component::GetBody()
	{
		return (Body*)(Owner->components[CT_Body]);
	}

	Model* Component::GetModel()
	{
		return (Model*)(Owner->components[CT_Model]);
	}

	Player* Component::GetPlayer()
	{
		return (Player*)(Owner->components[CT_Player]);
	}

	Transform* Component::GetTransform()
	{
		return (Transform*)(Owner->components[CT_Transform]);
	}
}