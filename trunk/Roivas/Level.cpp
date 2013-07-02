#include "CommonLibs.h"
#include "Level.h"
#include "Core.h"
#include "FileIO.h"

namespace Roivas
{
	Level::Level() : Loaded(false)
	{
	}

	Level::Level(std::string path) : Loaded(false)
	{
		ReadLevelFile( path );
	}
	
	bool Level::Load()
	{
		for( unsigned i = 0; i < STATIC_LIST.size(); ++i )
		{
			Entity* e = Factory::AddEntity( STATIC_LIST[i].first );
			Json::Value root = STATIC_LIST[i].second;

			int id = root["Id"].asInt();
			if( id != 0 )
				e->ID = id;

			FileIO fio;
			e->LoadFromLevel(root);
		}

		return true;
	}

	bool Level::ReadLevelFile(std::string path)
	{
		FileIO fio;
		
		if( !fio.LoadFile("Assets/Levels/"+path) || fio.RootKey != "Level" )
			return false;

		Json::Value types = fio.GetChildren(fio.Root);

		for( Json::ValueIterator i = types.begin(); i != types.end(); ++i )
		{
			std::string type = i.key().asString();

			Entity* arch = Factory::BuildArchetype("Assets/Objects/"+type);

			if( arch == nullptr )
				continue;

			Json::Value objects = (*i)["Entities"];
			for( Json::ValueIterator j = objects.begin(); j != objects.end(); ++j )
			{
				Json::Value components = *j;
				for( Json::ValueIterator k = components.begin(); k != components.end(); ++k )
				{
					std::string key = k.key().asString();
			
				}

				// Stores Entity name and Json value with components
				STATIC_LIST.push_back( std::pair<std::string,Json::Value>(type,*j) );
			}			
		}


		return true;
	}
}