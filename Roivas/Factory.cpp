#include "CommonLibs.h"
#include "Factory.h"
#include "Core.h"
#include "FileIO.h"

#include "Body.h"
#include "Model.h"
#include "Player.h"
#include "Transform.h"



namespace Roivas
{

	Factory::Factory() 
		:	System(SYS_Factory, "Factory")
	{
		COMPONENT_MAP["Body"] = new Body();
		COMPONENT_MAP["Model"] = new Model();
		COMPONENT_MAP["Player"] = new Player();
		COMPONENT_MAP["Transform"] = new Transform();
	}

	Entity* Factory::AddEntity(std::string path)
	{
		return GetSystem(Factory)->AddEntityNS(path);
	}

	Entity* Factory::AddEntityNS(std::string path)
	{
		Entity* entity = CreateEntity(path);

		if( entity == nullptr )
			return nullptr;


		////
		ENTITY_LIST.resize(10);

		Entity* e = new Entity();
		ENTITY_LIST[0] = e;
		Entity* f = new Entity();
		ENTITY_LIST[1] = f;
		Entity* r = new Entity();
		ENTITY_LIST[2] = r;

		Entity* g = new Entity();
		ENTITY_LIST[4] = g;
		Entity* h = new Entity();
		ENTITY_LIST[9] = g;
		
		////
		

		GLuint id = ENTITY_LIST.size();
		for( unsigned i = 0; i < ENTITY_LIST.size(); ++i )
		{
			if( ENTITY_LIST.at(i) == nullptr )
			{
				id = i;
				break;
			}
		}

		if( id == ENTITY_LIST.size() )
			ENTITY_LIST.push_back(entity);
		else
			ENTITY_LIST[id] = entity;

		entity->ID = id;

		return entity;
	}

	Entity* Factory::CreateEntity(std::string path)
	{
		// If exists as archetype
		if( ARCHETYPE_MAP.find(path) != ARCHETYPE_MAP.end() )
		{
			return ARCHETYPE_MAP.at(path)->Clone();
		}

		Entity* entity = BuildArchetype(path);

		if( entity == nullptr )
			return nullptr;

		ARCHETYPE_MAP[path] = entity;

		return ARCHETYPE_MAP.at(path)->Clone();
	}

	Entity* Factory::BuildArchetype(std::string path)
	{
		FileIO fio;
		
		if( !fio.LoadFile(path) || fio.RootKey != "Entity" )
			return nullptr;

		Json::Value components = fio.GetChildren(fio.Root);

		Entity* entity = new Entity();

		for( Json::ValueIterator itr = components.begin(); itr != components.end(); ++itr )
		{
			std::string key = itr.key().asString();

			if( COMPONENT_MAP.find(key) == COMPONENT_MAP.end() )
				continue;

			Component* comp = COMPONENT_MAP.at(key)->Clone();

			comp->Deserialize(fio, *itr);

			entity->Components[comp->Type] = comp;
			
			// Deserialize data
			//Json::Value value = *itr;
		}
		
		return entity;
	}
}