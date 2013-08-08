#pragma once

#include "Component.h"

namespace Roivas
{
	class Transform : public Component
	{
		public:
			Transform();
			Transform(const Transform& t);
			Transform* Clone();
			void Initialize();
			void Deserialize(FileIO& fio, Json::Value& root);

		// Data
			vec3 Position;
			vec3 Scale;
			vec3 Rotation;
	};
}