#include "Config.h"

Config* Config::instance = NULL;
const string Config::config_file = "config.ini";

Config* Config::getInstance()
{
	if(!instance) {
		instance = new Config();
	}
	return instance;
}

Config::Config()
{
	
}


string Config::get_value(string config_key)
{
	std::ifstream is_file(Config::config_file.c_str());
	string value = "";
	string line;
	string key;
	while( std::getline(is_file, line) )
	{
		std::istringstream is_line(line);
		if(std::getline(is_line, key, '=') )
		{
			if(!config_key.compare(key))
			{
				if( std::getline(is_line, value) ) {
					std::size_t found = value.find_last_of("\r\n");
					if(found == string::npos)
		  				return value;
		  			return value.substr(0,found);
				}
			}
			
		}
		value = "";
	}
	return value;
}


