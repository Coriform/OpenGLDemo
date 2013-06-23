#pragma once
#include "Enums.h"
#include "Math3D.h"

namespace Roivas
{
	class System
	{
		public:
			static void CreateInstance();
			static void FreeInstance();
			static System* GetInstance();

			virtual void Initialize() = 0;
			virtual void Start() {}
			virtual void Update(float dt) = 0;		
			virtual ~System() {};

			int GetType() { return s_type; }
			std::string GetName() { return s_name; }
			virtual void AddComponent(class Component* c) {}
			virtual void RemoveComponent(class Component* c) {}

			System* SYSTEM;

		protected:
			System(SystemType _type, std::string _name) : s_type(_type), s_name(_name) {}
			SystemType s_type;			
			std::string s_name;
	};
}	