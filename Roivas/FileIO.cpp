#include "CommonLibs.h"
#include "FileIO.h"

namespace Roivas
{
	void FileIO::Test()
	{
		std::string json_example = "{\"array\": \
										[\"item1\", \
										\"item2\"], \
										\"not an array\": \
										\"asdf\" \
									}";

		Json::Value root;
		Json::Reader reader;
		bool parsedSuccess = reader.parse(json_example, root, false);

		if( !parsedSuccess )
		{
			std::cout << "Didn't parse the JSON correctly" << std::endl;
			std::cout << reader.getFormatedErrorMessages() << std:: endl;
			return;
		}

		const Json::Value array = root["array"];

		for(unsigned int index=0; index<array.size(); ++index)
		{
			std::cout << "Element " << index << " in array: " << array[index].asString() << std::endl;
		}

		const Json::Value notAnArray = root["not an array"];

		if( !notAnArray.isNull())
		{
			std::cout<<"Not an array: " << notAnArray.asString() << std::endl;
		}

		std::cout << "Json Example pretty print: " << std::endl << root.toStyledString() << std::endl;
	}
}