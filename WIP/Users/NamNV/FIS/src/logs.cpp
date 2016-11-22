#include "../header/logs.h"

logs* logs::log_instance = NULL;
string logs::IN_STATE = "IN";
string logs::OUT_STATE = "OUT";

logs::logs(){

}

logs* logs::getInstance()
{
	if(!log_instance) {
			log_instance = new logs();
		}
	return log_instance;
}


void logs::write_logs(string function_name, string class_name, string state, string description)
{
	ofstream myfile;
	myfile.open ("logs.txt",ios::out | ios::app | ios::binary);
	time_t now = time(0);
   	char* dt = ctime(&now);
   	string BANK_LINE = "  :  ";
   	string CRLF = "\r\n";
   	string log_line = "";
   	log_line.append(dt);
   	log_line.append(BANK_LINE);
   	log_line.append(class_name);
   	log_line.append(BANK_LINE);
   	log_line.append(function_name);
   	log_line.append(BANK_LINE);
   	log_line.append(description);
   	log_line.append(BANK_LINE);
   	log_line.append(state);
   	log_line.append(CRLF);
	myfile << log_line;
	myfile.close();
}