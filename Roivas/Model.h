#pragma once

#include "Component.h"

namespace Roivas
{
	class Model : public Component
	{
		public:
			Model();
			Model(const Model& m);
			~Model();
			Model* Clone();
			void Initialize();
			void Deserialize(FileIO& fio, Json::Value& root);

		// Data
			std::string MeshName;
			std::string DiffuseName;	
			std::string NormalName;
			std::string SpecName;
			std::string GlowName;
			GLuint DiffuseID;
			GLuint NormalID;
			GLuint SpecID;
			GLuint GlowID;
			vec4 Color;
			vec3 WireColor;

			GLuint VertexBuffer;
			GLuint UVBuffer;
			GLuint NormalBuffer;
			GLuint ElementBuffer;
			std::vector<unsigned short> Indices;
	};
}