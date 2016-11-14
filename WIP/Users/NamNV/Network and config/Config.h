#ifndef CONFIG_H_
#define CONFIG_H_

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>

#define IP "IP"
#define PORT "PORT"
#define API_KEY "API_KEY"
#define REGISTER_URL "REGISTER_URL"

using namespace std;

class Config {
	private:
		static Config *instance;
		static const string config_file;
		Config();
	public:
		static Config* getInstance();
		static string get_value(string key);
};

#endif
