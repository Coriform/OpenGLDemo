#include "CommonLibs.h"
#include "Entity.h"
#include "Core.h"

namespace Roivas
{
	Entity::Entity() : Components(), Static(true), Temp(true), ID(-1)
	{

	}

	Entity::Entity(const Entity& e) : Components(), Static(true), Temp(true), ID(-1)
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

	void Entity::LoadFromLevel(Json::Value& root)
	{
		FileIO fio;

		if( root["Body"].size() > 0 )
			Components[CT_Body]->Deserialize( fio, root["Body"] );

		if( root["Light"].size() > 0 )
			Components[CT_Light]->Deserialize( fio, root["Light"] );

		if( root["Model"].size() > 0 )
			Components[CT_Model]->Deserialize( fio, root["Model"] );

		if( root["Player"].size() > 0 )
			Components[CT_Player]->Deserialize( fio, root["Player"] );

		if( root["Transform"].size() > 0 )
			Components[CT_Transform]->Deserialize( fio, root["Transform"] );
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