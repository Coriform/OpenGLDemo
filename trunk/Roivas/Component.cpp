#include "CommonLibs.h"
#include "Component.h"
#include "Entity.h"
#include "Core.h"

namespace Roivas
{
	Component::Component() : Type(CT_NONE), Owner(nullptr)
	{
		
	}

	Component::Component(ComponentType type) : Type(type), Owner(nullptr)
	{
	
	}

	Component::Component(const Component& t) : Type(CT_NONE), Owner(nullptr)
	{

	}

	Body* Component::GetBody()
	{
		return (Body*)(Owner->Components[CT_Body]);
	}

	Model* Component::GetModel()
	{
		return (Model*)(Owner->Components[CT_Model]);
	}

	Player* Component::GetPlayer()
	{
		return (Player*)(Owner->Components[CT_Player]);
	}

	Transform* Component::GetTransform()
	{
		return (Transform*)(Owner->Components[CT_Transform]);
	}
}