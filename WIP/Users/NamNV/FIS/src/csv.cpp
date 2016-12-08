#include "../header/csv.h"

csv* csv::csv_instance = NULL;

csv* csv::getInstance() {
	if(!csv_instance) {
		csv_instance = new csv();
	}
	return csv_instance;
}

csv::csv() {

}

void csv::write_csv(string userid, string deviceid, string username)
{
	string filename = "";
	const string DETER = ",";
	const string CRLT = "\r\n";
	time_t now = time(0);
	struct tm *ti = localtime(&now);
	char* dt = ctime(&now);
	filename.append(std::to_string(ti->tm_mon + 1));
	filename.append("-");
	filename.append(std::to_string(ti->tm_year + 1900));
	filename.append(".csv");
	ofstream myfile;
	myfile.open(filename.c_str(),ios::out | ios::app | ios::binary);
	string csv_line = userid;
	csv_line.append(DETER);
	csv_line.append(username);
	csv_line.append(DETER);
	csv_line.append(deviceid);
	csv_line.append(DETER);
	csv_line.append(dt);
	myfile << csv_line;
	myfile.close();
}