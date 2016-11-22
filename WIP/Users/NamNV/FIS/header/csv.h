#ifndef CSV_H_
#define CSV_H_
#include <ctime>
#include <iostream>
#include <fstream>
using namespace std;
class csv {
	private:
		csv();
		static csv *csv_instance;
	public:
		static csv* getInstance();
		void write_csv(string userid, string deviceid, string username);
};

#endif