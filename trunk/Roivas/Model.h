#pragma once

#include "Component.h"

namespace Roivas
{
	class Model : public Component
	{
		public:
			Model();
			Model(const Model& m);
			Model* Clone();
			void Deserialize(FileIO& fio, Json::Value& root);

		// Data
			std::string MeshName;
			std::string DiffuseName;
			GLuint MeshID;			
			GLuint DiffuseID;
			vec4   Color;
	};
}