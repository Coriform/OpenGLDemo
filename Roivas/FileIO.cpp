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
		if( value.size() != 0 )		return;

		data = value.asBool();
	}

	void FileIO::Read(Json::Value& value, GLuint& data)
	{
		if( value.size() != 0 )		return;
		
		data = value.asUInt();
	}

	void FileIO::Read(Json::Value& value, GLint& data)
	{
		if( value.size() != 0 )		return;

		data = value.asInt();
	}

	void FileIO::Read(Json::Value& value, float& data)
	{
		if( value.size() != 0 )		return;

		data = (float)value.asDouble();
	}

	void FileIO::Read(Json::Value& value, std::string& data)
	{
		if( value.size() != 0 )		return;

		data = value.asString();
	}

	void FileIO::Read(Json::Value& value, vec3&data)
	{
		if( value.size() != 3 )		return;

		data.x = (float)value[0u].asDouble();
		data.y = (float)value[1u].asDouble();
		data.z = (float)value[2u].asDouble();
	}

	void FileIO::Read(Json::Value& value, vec4&data)
	{
		if( value.size() != 4 )		return;

		data.x = (float)value[0u].asDouble();
		data.y = (float)value[1u].asDouble();
		data.z = (float)value[2u].asDouble();
		data.w = (float)value[3u].asDouble();
	}

	void FileIO::Test()
	{

		std::ifstream jsonFile( "Assets/Objects/test2.json" );
		std::stringstream jsonData;
		jsonData << jsonFile.rdbuf();  
		jsonFile.close();
		const std::string &json_example = jsonData.str();




		if( !reader.parse(json_example, Root, false) )
		{
			std::cout << "Didn't parse the JSON correctly" << std::endl;
			std::cout << reader.getFormatedErrorMessages() << std:: endl;
			return;
		}

		/*
		std::ifstream jsonFile2( "Assets/Objects/test3.json" );
		std::stringstream jsonData2;
		jsonData2 << jsonFile2.rdbuf();  
		jsonFile2.close();
		const std::string &json_example2 = jsonData2.str();

		if( !reader.parse(json_example2, Root, false) )
		{
			std::cout << "Didn't parse the JSON correctly" << std::endl;
			std::cout << reader.getFormatedErrorMessages() << std:: endl;
			return;
		}
		*/


		/*
		const Json::Value array = Root["Transform"];
		
		for(unsigned int index=0; index<array.size(); ++index)
		{
			std::cout << "Element " << index << " in array: " << array[index].asString() << std::endl;
		}

		const Json::Value notAnArray = Root["not an array"];

		if( !notAnArray.isNull())
		{
			std::cout<<"Not an array: " << notAnArray.asString() << std::endl;
		}
		*/

		std::cout << "Json Example pretty print: " << std::endl << Root.toStyledString() << std::endl;
		
		std::cout << std::endl << std::endl;

		PrintJSONTree(Root);
	}

	void FileIO::PrintJSONValue( Json::Value &val )
	{
		if( val.isString() ) {
			printf( "string(%s)", val.asString().c_str() ); 
		} else if( val.isBool() ) {
			printf( "bool(%d)", val.asBool() ); 
		} else if( val.isInt() ) {
			printf( "int(%d)", val.asInt() ); 
		} else if( val.isUInt() ) {
			printf( "uint(%u)", val.asUInt() ); 
		} else if( val.isDouble() ) {
			printf( "double(%f)", val.asDouble() ); 
		}
		else 
		{
			printf( "unknown type=[%d]", val.type() ); 
		}
	}


	bool FileIO::PrintJSONTree( Json::Value &root, unsigned short depth) 
	{
		depth += 1;
		printf( " {type=[%d], size=%d}", root.type(), root.size() ); 

		//iter = root.begin();

		//iter++;

		if( iter == root.end() )
			int iii = 0;

		if( root.size() > 0 ) {
			printf("\n");
			for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
				// Print depth. 
				for( int tab = 0 ; tab < depth; tab++) {
				   printf("-"); 
				}
				printf(" subvalue(");
				PrintJSONValue(itr.key());
				printf(") -");
				//Json::Value root2 = *itr;
				//Json::Value root4 = *itr;
				PrintJSONTree( *itr, depth); 
			}
			return true;
		} else {
			printf(" ");
			PrintJSONValue(root);
			printf( "\n" ); 
		}
		return true;
	}
}