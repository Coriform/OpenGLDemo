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
		COMPONENT_MAP["Light"] = new Light();
		COMPONENT_MAP["Model"] = new Model();
		COMPONENT_MAP["Player"] = new Player();
		COMPONENT_MAP["Transform"] = new Transform();
	}

	Factory::~Factory()
	{
		RemoveAllEntities();
	}

	void Factory::Update(float dt)
	{
		for( unsigned i = 0; i < DELETION_LIST.size(); ++i )
			RemoveEntity( DELETION_LIST[i] );

		DELETION_LIST.clear();
	}

	Entity* Factory::AddEntity(std::string path)
	{
		return GetSystem(Factory)->AddEntityNS(path);
	}

	Entity* Factory::AddTempEntity(std::string path)
	{
		return GetSystem(Factory)->AddTempEntityNS(path);
	}

	void Factory::Destroy(Entity* e)
	{
		GetSystem(Factory)->DestroyNS(e);
	}

	Entity* Factory::AddEntityNS(std::string path)
	{
		Entity* entity = CreateEntity("Assets/Objects/"+path);

		if( entity == nullptr )
			return nullptr;		

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
		entity->Temp = false;

		entity->Initialize();

		return entity;
	}

	Entity* Factory::AddTempEntityNS(std::string path)
	{
		Entity* entity = AddEntityNS(path);

		if( entity == nullptr )
			return nullptr;	

		entity->Temp = true;

		entity->Initialize();

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
		}
		
		return entity;
	}

	void Factory::DestroyNS(Entity* e)
	{
		DELETION_LIST.push_back(e);
	}

	void Factory::RemoveEntity(Entity* e)
	{
		auto it_s = ENTITY_LIST.begin();
		auto it_e = ENTITY_LIST.end();
		for( ; it_s != it_e; ++it_s )
		{
			if( *it_s == e )
			{
				ENTITY_LIST.erase(it_s);
				delete e;
				return;
			}
		}
	}

	void Factory::RemoveAllEntities()
	{
		for( unsigned i = 0; i < ENTITY_LIST.size(); ++i )
		{
			RemoveEntity( ENTITY_LIST[i] );
		}
	}
}