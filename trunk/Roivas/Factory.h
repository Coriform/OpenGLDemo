#pragma once

#include "System.h"
#include "Entity.h"

namespace Roivas
{
	class Factory : public System
	{
		public:
			Factory();
			~Factory();
			void Initialize() {}
			void Update(float dt);
			static Entity* AddEntity(std::string path);
			static Entity* AddTempEntity(std::string path);
			static void Destroy(Entity* e);

		private:
			Entity* AddEntityNS(std::string path);
			Entity* AddTempEntityNS(std::string path);
			void DestroyNS(Entity* e);
			void RemoveEntity(Entity* e);
			void RemoveAllEntities();
			Entity* CreateEntity(std::string path);
			Entity* BuildArchetype(std::string path);
			std::map<std::string,Entity*> ARCHETYPE_MAP;
			std::map<std::string,Component*> COMPONENT_MAP;
			std::vector<Entity*> ENTITY_LIST;
			std::vector<Entity*> DELETION_LIST;
	};

}