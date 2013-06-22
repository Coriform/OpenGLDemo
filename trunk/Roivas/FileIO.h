#pragma once

#include <json/json.h>

namespace Roivas
{
	class FileIO
	{
		public:
			FileIO();
			bool LoadFile(std::string& path);
			Json::Value GetChildren(Json::Value& root);

			void Read(Json::Value& value, bool& data);
			void Read(Json::Value& value, GLuint& data);
			void Read(Json::Value& value, GLint& data);
			void Read(Json::Value& value, float& data);
			void Read(Json::Value& value, std::string& data);
			void Read(Json::Value& value, vec3&data);
			void Read(Json::Value& value, vec4&data);

			void Test();
			void PrintJSONValue(Json::Value &val);
			bool PrintJSONTree(Json::Value &val,unsigned short depth = 0);

		// Data
			std::string RootKey;
			Json::Value Root;	

		private:
			Json::Reader reader;			
			Json::ValueIterator iter;
		
	};
}