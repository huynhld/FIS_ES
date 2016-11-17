#ifndef PINETWORK_H_
#define PINETWORK_H_

#include <iostream>
#include <cstring>
#include <cstdlib>
#include "Config.h"
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <unistd.h> /* read, write, close */
#include <fstream>
#include <iterator>
#include <vector>

using namespace std;

class PiNetwork{
	private:
		Config* config;
		static PiNetwork *p_instance;
		PiNetwork();
	public:
		static PiNetwork* getInstance();
		bool send_register_request(string minutiae_one,
					string minutiae_two, string minutiae_three);
		string construct_body(string minutiae_one,
					string minutiae_two, string minutiae_three);
		bool send(string message, string &response);
		bool send_file();
		bool send_file_deviceId(string file_path);
};

#endif
