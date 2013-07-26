#include "CommonLibs.h"
#include "FileIO.h"

namespace Roivas
{
	FileIO::FileIO()
	{
		//Root = nullptr;
	}

	bool FileIO::LoadFile( std::string& path)
	{
		//std::ifstream jsonFile( "Assets/Objects/test2.json" );
		std::ifstream jsonFile( path );
		std::stringstream jsonData;
		jsonData << jsonFile.rdbuf();  
		jsonFile.close();
		const std::string &json_example = jsonData.str();

		if( !reader.parse(json_example, Root, false) )
		{
			std::cout << "Didn't parse the JSON correctly" << std::endl;
			std::cout << reader.getFormatedErrorMessages() << std:: endl;
			return false;
		}
				
		iter	= Root.begin();
		RootKey = Root.begin().key().asString();

		return true;
	}

	Json::Value FileIO::GetChildren(Json::Value& root)
	{
		return *(root.begin());
	}

	void FileIO::Read(Json::Value& value, bool& data)
	{
		if( value.isNull() || value.size() != 0 )		return;

		data = value.asBool();
	}

	void FileIO::Read(Json::Value& value, GLuint& data)
	{
		if( value.isNull() || value.size() != 0 )		return;
		
		data = value.asUInt();
	}

	void FileIO::Read(Json::Value& value, GLint& data)
	{
		if( value.isNull() || value.size() != 0 )		return;

		data = value.asInt();
	}

	void FileIO::Read(Json::Value& value, float& data)
	{
		if( value.isNull() || value.size() != 0 )		return;

		data = (float)value.asDouble();
	}

	void FileIO::Read(Json::Value& value, std::string& data)
	{
		if( value.isNull() || value.size() != 0 )		return;

		data = value.asString();
	}

	void FileIO::Read(Json::Value& value, vec3& data)
	{
		if( value.isNull() || value.size() != 3 )		return;

		data.x = (float)value[0u].asDouble();
		data.y = (float)value[1u].asDouble();
		data.z = (float)value[2u].asDouble();
	}

	void FileIO::Read(Json::Value& value, vec4& data)
	{
		if( value.isNull() ||  value.size() != 4 )		return;

		data.x = (float)value[0u].asDouble();
		data.y = (float)value[1u].asDouble();
		data.z = (float)value[2u].asDouble();
		data.w = (float)value[3u].asDouble();
	}

	void FileIO::Read(Json::Value& value, quat& data)
	{
		if( value.isNull() || value.size() != 4 )		return;

		data.x = (float)value[0u].asDouble();
		data.y = (float)value[1u].asDouble();
		data.z = (float)value[2u].asDouble();
		data.w = (float)value[3u].asDouble();
	}
}