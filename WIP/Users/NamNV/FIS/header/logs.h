#ifndef LOGS_H_
#define LOGS_H_
#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
using namespace std;

class logs
{
	private:
		logs();
		static logs* log_instance;
	public:
		static string IN_STATE;
		static string OUT_STATE;
		static logs* getInstance();
		static void write_logs(string function_name, string class_name,  string state, string description = "No description");
};

#endif