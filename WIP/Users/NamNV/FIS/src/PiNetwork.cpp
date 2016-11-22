#include "../header/PiNetwork.h"


PiNetwork* PiNetwork::p_instance = NULL;

PiNetwork* PiNetwork::getInstance()
{
	if(!p_instance) {
		p_instance = new PiNetwork();
	}
	return p_instance;
}


PiNetwork::PiNetwork()
{
	config = Config::getInstance();
}


bool PiNetwork::send_register_request(string minutiae_one,
					string minutiae_two, string minutiae_three)
{
	string CRLF = "\r\n";
	string header = "POST ";
	
	string url = config->get_value(BACK_UP);
	header.append(url);
	header.append(" HTTP/1.0");
	header.append(CRLF);
	
	header.append("Content-Type: application/x-www-form-urlencoded");
	header.append(CRLF);
	
	string body = construct_body(minutiae_one, minutiae_two, minutiae_three);
	header.append("Content-Length: ");
	std::stringstream ss;
	ss << body.length();
	header.append(ss.str());
	header.append(CRLF);
	header.append(CRLF);
	header.append(body);
	
	//std::cout << header << std::endl;
	int size = 0;
	string response ="";
	//cout << header << endl;
	return send(header, response, size);
}

bool PiNetwork::send_file()
{
	string CRLF = "\r\n";
	string header = "POST ";
	
	string url = config->get_value(SEND_FINGER);
	header.append("/Admin/TestPostMethod");
	header.append(" HTTP/1.0");
	header.append(CRLF);
	
	header.append("Content-Type: multipart/form-data; boundary=---------------------------10588207912018");
	header.append(CRLF);

	std::ifstream input( "4.bmp", std::ios::binary );
    // copies all data into buffer
    std::vector<char> buffer((
            std::istreambuf_iterator<char>(input)), 
            (std::istreambuf_iterator<char>()));
    string body = "-----------------------------10588207912018";
    body.append(CRLF);
    body.append("Content-Disposition: form-data; name=\"file\"; filename=\"result.bmp\"");
    body.append(CRLF);
    body.append("Content-Type: image/bmp");
    body.append(CRLF);
    body.append(CRLF);
    std::string str(buffer.begin(), buffer.end());
    body.append(str);
    body.append(CRLF);
    body.append("-----------------------------10588207912018--");
	body.append(CRLF);

    header.append("Content-Length: ");
	std::stringstream ss;
	ss << body.length();
	header.append(ss.str());
	header.append(CRLF);
	header.append(CRLF);
	header.append(body);

	string response = "";
	int size = 0;
	//cout << header << endl;
	return send(header, response, size);
}

vector<string> split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

bool PiNetwork::send_file_deviceId(string file_path, string &userid, string &username)
{
	string CRLF = "\r\n";
	string header = "POST ";
	
	string deviceId = config->get_value(API_KEY);
	string url = config->get_value(SEND_FINGER);
	header.append(url);
	header.append(" HTTP/1.0");
	header.append(CRLF);
	
	header.append("Content-Type: multipart/form-data; boundary=---------------------------10588207912018");
	header.append(CRLF);

	std::ifstream input(file_path.c_str(), std::ios::binary);
    // copies all data into buffer
    std::vector<char> buffer((
            std::istreambuf_iterator<char>(input)), 
            (std::istreambuf_iterator<char>()));
    string body = "-----------------------------10588207912018";
    body.append(CRLF);
    body.append("Content-Disposition: form-data; name=\"file\"; filename=\"IFP001.bmp\"");
    body.append(CRLF);
    body.append("Content-Type: image/bmp");
    body.append(CRLF);
    body.append(CRLF);
    std::string str(buffer.begin(), buffer.end());
    body.append(str);
    body.append(CRLF);
    body.append(CRLF);
    body.append("-----------------------------10588207912018");
    body.append(CRLF);
    body.append("Content-Disposition: form-data; name=\"deviceID\"");
    body.append(CRLF);
    body.append(CRLF);
    body.append(deviceId);
    body.append(CRLF);
    body.append("-----------------------------10588207912018--");
	body.append(CRLF);

    header.append("Content-Length: ");
	std::stringstream ss;
	ss << body.length();
	header.append(ss.str());
	header.append(CRLF);
	header.append(CRLF);
	header.append(body);
	string response = "";
	//cout << header << endl;
	int size = 0;
	bool result = send(header, response, size);
	if(!result) return result;
	string s = response.substr(0, size);
	std::vector<std::string> x = split(s, '_');
	if(x[0].compare("Oke") == 0){
		userid = x[1];
		username = x[2];
		return true;
	} 
	return false;
}


bool PiNetwork::send_log_deviceId(string userid)
{
	string CRLF = "\r\n";
	string header = "POST ";
	
	string url = config->get_value(SEND_LOG);
	header.append(url);
	header.append(" HTTP/1.0");
	header.append(CRLF);
	
	header.append("Content-Type: application/x-www-form-urlencoded");
	header.append(CRLF);
	
	string body = construct_body(userid);
	header.append("Content-Length: ");
	header.append(std::to_string(body.length()));
	header.append(CRLF);
	header.append(CRLF);
	header.append(body);
	string response = "";
	std::cout << header << std::endl;
	int size = 0;
	bool result = send(header, response, size);
	if(!result) return result;
	string s = response.substr(0, size);
	//cout << s << endl;
	if(s.compare("OK") == 0){
		return true;
	} 
	return false;
}


bool PiNetwork::send(string message, string &response, int &size)
{
	
	
	int portno = atoi(config->get_value(PORT).c_str());
	string host = config->get_value(IP);
	//std::cout << portno << " " << host << std::endl;
	struct hostent *server;
	struct sockaddr_in serv_addr;
	int sockfd, bytes, sent, received, total, message_size;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		cout << "Fail to init" << endl;
		return false;
	}

	server = gethostbyname(host.c_str());
	if (server == NULL) {
		cout << "gethostbyname" << endl;
		return false;
	}

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
		cout << "Fail to connect" << endl;
		return false;
	}

	struct timeval Timeout;
		/* set timeout value within input loop */
	Timeout.tv_usec = 0;  /* milliseconds */
	Timeout.tv_sec  = 5;  /* seconds */
	fd_set readfs;
	FD_SET(sockfd, &readfs);  /* set testing for source 1 */
	int success = select(sockfd, &readfs, NULL, NULL, &Timeout);
	if(success <= 0) return false;
	write(sockfd,message.c_str(),message.length());

	// char response[4069];
	// memset(response,0,sizeof(response));
	// total = sizeof(response)-1;
	// received = 0;
	// do {
	//     bytes = read(sockfd,response+received,total-received);
	//     if (bytes < 0){
	//     	cout << "Fail to read" << endl;
	//         return false;
	//     }
	//     if (bytes == 0)
	//         break;
	//     received+=bytes;
	// } while (received < total);

	// if (received == total)
	//     false;
	
	Timeout.tv_usec = 0;  /* milliseconds */
	Timeout.tv_sec  = 1;  /* seconds */
	std::string output;
	bytes = 400;
	output.resize(bytes,'*');
	FD_SET(sockfd, &readfs);  /* set testing for source 1 */
	select(sockfd + 1, &readfs, NULL, NULL, &Timeout);

	received = read(sockfd, &output[0], bytes-1);
	if (received < 0){
		cout << "Fail to read" << endl;
		return false;
	}
	//cout << output << endl;
	response  =  output.substr(output.find_last_of("\r\n\r\n")+1);
	size = response.find("*");
	return true;
}

string PiNetwork::construct_body(std::string userID)
{
	string body = "userID=";
	string CRLF = "\r\n";
	string apikey = config->get_value(API_KEY);
	body.append(userID);
	body.append("&deviceID=");
	body.append(apikey);

	return body;
}


string PiNetwork::construct_body(string minutiae_one,
					string minutiae_two, string minutiae_three)
{
	string body = "one=";
	string CRLF = "\r\n";
	string apikey = config->get_value(API_KEY);
	body.append(minutiae_one);
	body.append("&two=");
	body.append(minutiae_two);
	body.append("&three=");
	body.append(minutiae_three);
	body.append("&apikey=");
	body.append(apikey);
	return body;
}
