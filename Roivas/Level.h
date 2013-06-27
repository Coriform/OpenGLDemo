#pragma once

namespace Roivas
{
	class Level 
	{
		public:
			Level();
			Level(std::string path);
			bool Load();
			bool Unload() {}
			bool ReadLevelFile(std::string path);
			

		private:
			std::vector<std::pair<std::string,Json::Value>> STATIC_LIST;
			std::vector<std::pair<std::string,Json::Value>> NONSTATIC_LIST;
			bool Loaded;
	};
}