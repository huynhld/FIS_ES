#include "PiNetwork.h"


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
	
	string url = config->get_value(REGISTER_URL);
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
	return send(header);
}

bool PiNetwork::send_file()
{
	string CRLF = "\r\n";
	string header = "POST ";
	
	string url = config->get_value(REGISTER_URL);
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
	
	cout << header << endl;
	return send(header);
}

bool PiNetwork::send(string message)
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
	bytes = 4096;
	FD_SET(sockfd, &readfs);  /* set testing for source 1 */
	select(sockfd, &readfs, NULL, NULL, &Timeout);

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
	Timeout.tv_sec  = 2;  /* seconds */
	std::string output;
	bytes = 4096;
	output.resize(bytes);
	FD_SET(sockfd, &readfs);  /* set testing for source 1 */
	select(sockfd + 1, &readfs, NULL, NULL, &Timeout);

	received = read(sockfd, &output[0], bytes-1);
	if (received < 0){
	cout << "Fail to read" << endl;
		return false;
	}
	cout << output << endl;
	string result =  output.substr(output.find_last_of("\r\n\r\n")+1, output.length());
	if(!result.compare("Oke")) {
		return false;
	}
	return true;
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

int main() 
{
	//Use for fake data purpose
	string minutiae_one = "30+%3A+254+%3A+6.95303e-310%0D%0A32+%3A+255+%3A+1.12548%0D%0A33+%3A+175+%3A+1.30642%0D%0A36+%3A+197+%3A+0.9762%0D%0A36+%3A+220+%3A+0.937967%0D%0A38+%3A+195+%3A+1.02414%0D%0A39+%3A+223+%3A+1.03437%0D%0A41+%3A+154+%3A+1.2267%0D%0A57+%3A+125+%3A+1.08164%0D%0A66+%3A+110+%3A+1.13384%0D%0A67+%3A+175+%3A+0.688086%0D%0A76+%3A+196+%3A+1.03356%0D%0A76+%3A+234+%3A+1.00816%0D%0A77+%3A+90+%3A+1.31128%0D%0A77+%3A+197+%3A+0.645195%0D%0A78+%3A+163+%3A+1.26236%0D%0A78+%3A+235+%3A+0.681545%0D%0A83+%3A+180+%3A+1.12888%0D%0A86+%3A+214+%3A+0.769032%0D%0A89+%3A+195+%3A+1.1389%0D%0A89+%3A+196+%3A+0.91499%0D%0A91+%3A+193+%3A+0.979744%0D%0A91+%3A+198+%3A+0.827629%0D%0A92+%3A+209+%3A+0.8572%0D%0A93+%3A+60+%3A+0.979325%0D%0A94+%3A+180+%3A+0.277635%0D%0A95+%3A+182+%3A+0.741607%0D%0A101+%3A+197+%3A+0.463086%0D%0A103+%3A+195+%3A+0.978895%0D%0A105+%3A+244+%3A+0.846841%0D%0A107+%3A+207+%3A+0.449601%0D%0A107+%3A+246+%3A+0.869312%0D%0A109+%3A+205+%3A+1.45844%0D%0A118+%3A+190+%3A+0.428134%0D%0A131+%3A+45+%3A+0.549657%0D%0A142+%3A+241+%3A+0.385235%0D%0A148+%3A+65+%3A+1.19084%0D%0A148+%3A+68+%3A+0.27109%0D%0A148+%3A+130+%3A+0.123484%0D%0A148+%3A+218+%3A+0.16698%0D%0A151+%3A+237+%3A+1.18835%0D%0A152+%3A+118+%3A+1.16833%0D%0A153+%3A+124+%3A+0.109081%0D%0A158+%3A+215+%3A+0.0253324%0D%0A161+%3A+141+%3A+1.84116%0D%0A161+%3A+143+%3A+3.11614%0D%0A164+%3A+196+%3A+0.000274524%0D%0A172+%3A+51+%3A+1.90555%0D%0A182+%3A+208+%3A+2.82012%0D%0A185+%3A+53+%3A+1.93521%0D%0A188+%3A+251+%3A+2.85322%0D%0A189+%3A+142+%3A+1.39737%0D%0A189+%3A+145+%3A+2.81911%0D%0A190+%3A+154+%3A+2.72058%0D%0A192+%3A+251+%3A+2.3865%0D%0A199+%3A+159+%3A+2.88496%0D%0A204+%3A+162+%3A+2.57647%0D%0A206+%3A+158+%3A+2.86353%0D%0A208+%3A+112+%3A+1.87549%0D%0A208+%3A+117+%3A+2.41491%0D%0A224+%3A+168+%3A+2.89661";
	string minutiae_two = "33+%3A+196+%3A+6.95331e-310%0D%0A37+%3A+205+%3A+1.04327%0D%0A38+%3A+203+%3A+1.24903%0D%0A40+%3A+222+%3A+1.20565%0D%0A42+%3A+220+%3A+1.36261%0D%0A50+%3A+218+%3A+1.2294%0D%0A51+%3A+245+%3A+1.23199%0D%0A52+%3A+222+%3A+0.828249%0D%0A53+%3A+243+%3A+1.08313%0D%0A54+%3A+108+%3A+0.859976%0D%0A61+%3A+146+%3A+1.04014%0D%0A64+%3A+86+%3A+0.74453%0D%0A65+%3A+76+%3A+0.826952%0D%0A68+%3A+225+%3A+1.12565%0D%0A68+%3A+226+%3A+1.20881%0D%0A70+%3A+225+%3A+1.16985%0D%0A73+%3A+236+%3A+1.24861%0D%0A74+%3A+223+%3A+0.849304%0D%0A77+%3A+53+%3A+0.960918%0D%0A78+%3A+228+%3A+0.654447%0D%0A80+%3A+226+%3A+0.621142%0D%0A84+%3A+38+%3A+0.773058%0D%0A88+%3A+231+%3A+1.09411%0D%0A91+%3A+258+%3A+1.16364%0D%0A101+%3A+176+%3A+1.20063%0D%0A102+%3A+258+%3A+1.07693%0D%0A103+%3A+174+%3A+1.00968%0D%0A103+%3A+257+%3A+0.936048%0D%0A104+%3A+193+%3A+1.11211%0D%0A105+%3A+255+%3A+1.05374%0D%0A107+%3A+188+%3A+1.07906%0D%0A108+%3A+143+%3A+1.13435%0D%0A108+%3A+205+%3A+0.987509%0D%0A108+%3A+208+%3A+1.10654%0D%0A109+%3A+164+%3A+0.953926%0D%0A110+%3A+141+%3A+1.31755%0D%0A110+%3A+203+%3A+1.03079%0D%0A112+%3A+167+%3A+0.840997%0D%0A113+%3A+198+%3A+1.48746%0D%0A115+%3A+196+%3A+1.08901%0D%0A117+%3A+210+%3A+0.896044%0D%0A118+%3A+190+%3A+0.600161%0D%0A119+%3A+206+%3A+0.673464%0D%0A120+%3A+175+%3A+1.19758%0D%0A121+%3A+190+%3A+1.21591%0D%0A121+%3A+204+%3A+1.02864%0D%0A123+%3A+146+%3A+0.571616%0D%0A124+%3A+176+%3A+0.816862%0D%0A125+%3A+144+%3A+1.103%0D%0A135+%3A+58+%3A+0.362467%0D%0A135+%3A+255+%3A+0.340505%0D%0A136+%3A+56+%3A+1.28942%0D%0A143+%3A+164+%3A+0.290054%0D%0A144+%3A+225+%3A+0.820976%0D%0A149+%3A+220+%3A+1.08887%0D%0A151+%3A+251+%3A+1.28645%0D%0A155+%3A+247+%3A+0.216248%0D%0A171+%3A+159+%3A+0.114065%0D%0A171+%3A+168+%3A+0.230742%0D%0A175+%3A+214+%3A+0.102537%0D%0A178+%3A+225+%3A+0.201279%0D%0A179+%3A+193+%3A+1.14476%0D%0A184+%3A+211+%3A+3.05229%0D%0A185+%3A+236+%3A+3.10467%0D%0A186+%3A+113+%3A+1.24446%0D%0A189+%3A+237+%3A+1.9945%0D%0A193+%3A+170+%3A+3.0123%0D%0A198+%3A+161+%3A+1.80081%0D%0A200+%3A+163+%3A+2.54685%0D%0A201+%3A+124+%3A+2.72094%0D%0A206+%3A+180+%3A+1.37984%0D%0A208+%3A+35+%3A+2.61551%0D%0A209+%3A+56+%3A+2.75776%0D%0A213+%3A+69+%3A+1.73055%0D%0A213+%3A+86+%3A+2.77072%0D%0A214+%3A+227+%3A+2.69742%0D%0A215+%3A+86+%3A+1.53679%0D%0A216+%3A+46+%3A+2.81578%0D%0A217+%3A+105+%3A+2.40584%0D%0A220+%3A+125+%3A+2.83909%0D%0A223+%3A+231+%3A+2.51459%0D%0A225+%3A+153+%3A+1.39404%0D%0A226+%3A+89+%3A+1.3863";
	string minutiae_three = "31+%3A+234+%3A+6.95331e-310%0D%0A32+%3A+174+%3A+1.16805%0D%0A33+%3A+59+%3A+1.07845%0D%0A33+%3A+253+%3A+1.05396%0D%0A34+%3A+169+%3A+0.989937%0D%0A34+%3A+227+%3A+1.07359%0D%0A35+%3A+251+%3A+1.07151%0D%0A36+%3A+225+%3A+0.94305%0D%0A36+%3A+227+%3A+1.14894%0D%0A39+%3A+213+%3A+1.35966%0D%0A40+%3A+41+%3A+1.25289%0D%0A40+%3A+211+%3A+0.79135%0D%0A43+%3A+177+%3A+1.04944%0D%0A45+%3A+228+%3A+0.869237%0D%0A48+%3A+257+%3A+1.17225%0D%0A49+%3A+221+%3A+1.3921%0D%0A49+%3A+250+%3A+1.35799%0D%0A49+%3A+258+%3A+1.12125%0D%0A50+%3A+189+%3A+1.3711%0D%0A51+%3A+218+%3A+0.994687%0D%0A52+%3A+245+%3A+1.03755%0D%0A53+%3A+216+%3A+1.29018%0D%0A56+%3A+218+%3A+1.11788%0D%0A57+%3A+200+%3A+1.43888%0D%0A57+%3A+202+%3A+1.24844%0D%0A58+%3A+171+%3A+1.17963%0D%0A59+%3A+200+%3A+1.19488%0D%0A59+%3A+201+%3A+1.90581%0D%0A60+%3A+169+%3A+1.95602%0D%0A60+%3A+195+%3A+1.34259%0D%0A62+%3A+218+%3A+1.93266%0D%0A65+%3A+211+%3A+1.30587%0D%0A66+%3A+154+%3A+0.856872%0D%0A66+%3A+236+%3A+1.20653%0D%0A68+%3A+152+%3A+1.28552%0D%0A68+%3A+231+%3A+1.0833%0D%0A69+%3A+203+%3A+1.01264%0D%0A72+%3A+225+%3A+1.37465%0D%0A73+%3A+169+%3A+0.890242%0D%0A73+%3A+190+%3A+1.21215%0D%0A76+%3A+184+%3A+1.15951%0D%0A76+%3A+214+%3A+1.22484%0D%0A76+%3A+219+%3A+1.04359%0D%0A78+%3A+180+%3A+1.14403%0D%0A78+%3A+182+%3A+1.21999%0D%0A78+%3A+212+%3A+1.24556%0D%0A79+%3A+236+%3A+1.20803%0D%0A80+%3A+199+%3A+1.0738%0D%0A80+%3A+207+%3A+1.2255%0D%0A82+%3A+173+%3A+1.31931%0D%0A82+%3A+231+%3A+1.07598%0D%0A83+%3A+230+%3A+1.19799%0D%0A84+%3A+224+%3A+1.27622%0D%0A86+%3A+206+%3A+1.43553%0D%0A86+%3A+222+%3A+1.1909%0D%0A86+%3A+229+%3A+1.26072%0D%0A88+%3A+190+%3A+1.14472%0D%0A94+%3A+175+%3A+0.63484%0D%0A94+%3A+192+%3A+1.35117%0D%0A94+%3A+206+%3A+0.880591%0D%0A94+%3A+228+%3A+1.47797%0D%0A96+%3A+194+%3A+1.07795%0D%0A97+%3A+224+%3A+0.968146%0D%0A98+%3A+170+%3A+0.718387%0D%0A99+%3A+194+%3A+1.11114%0D%0A99+%3A+203+%3A+1.08414%0D%0A99+%3A+205+%3A+1.28498%0D%0A101+%3A+217+%3A+1.20413%0D%0A103+%3A+215+%3A+0.997814%0D%0A104+%3A+175+%3A+1.06364%0D%0A105+%3A+209+%3A+1.06589%0D%0A106+%3A+195+%3A+1.32654%0D%0A106+%3A+251+%3A+0.887837%0D%0A107+%3A+226+%3A+1.06813%0D%0A107+%3A+254+%3A+1.11236%0D%0A109+%3A+203+%3A+1.1137%0D%0A110+%3A+40+%3A+1.12484%0D%0A111+%3A+195+%3A+0.29829%0D%0A111+%3A+219+%3A+0.700276%0D%0A112+%3A+241+%3A+1.11319%0D%0A113+%3A+206+%3A+1.16242%0D%0A114+%3A+239+%3A+1.32343%0D%0A115+%3A+193+%3A+1.19963%0D%0A117+%3A+195+%3A+0.572266%0D%0A118+%3A+210+%3A+1.26423%0D%0A124+%3A+215+%3A+0.240448%0D%0A129+%3A+110+%3A+0.204939%0D%0A129+%3A+112+%3A+0.270705%0D%0A137+%3A+231+%3A+3.13331%0D%0A138+%3A+202+%3A+0.738413%0D%0A139+%3A+229+%3A+0.440435%0D%0A141+%3A+220+%3A+1.30719%0D%0A143+%3A+163+%3A+3.1089%0D%0A143+%3A+165+%3A+3.01113%0D%0A149+%3A+176+%3A+3.10605%0D%0A150+%3A+154+%3A+3.13735%0D%0A152+%3A+164+%3A+2.91533%0D%0A160+%3A+118+%3A+1.98107%0D%0A161+%3A+184+%3A+2.8161%0D%0A163+%3A+112+%3A+3.01619%0D%0A172+%3A+233+%3A+2.95774%0D%0A173+%3A+131+%3A+1.5748%0D%0A181+%3A+233+%3A+2.90111%0D%0A182+%3A+138+%3A+2.85508%0D%0A182+%3A+222+%3A+2.36764%0D%0A183+%3A+58+%3A+1.70039%0D%0A184+%3A+33+%3A+2.86439%0D%0A187+%3A+139+%3A+3.00322%0D%0A190+%3A+136+%3A+1.9608%0D%0A190+%3A+222+%3A+3.12412%0D%0A195+%3A+145+%3A+1.88075%0D%0A199+%3A+227+%3A+2.44921%0D%0A199+%3A+229+%3A+1.40742%0D%0A202+%3A+96+%3A+1.49295%0D%0A203+%3A+94+%3A+2.58053%0D%0A208+%3A+146+%3A+2.57647%0D%0A210+%3A+145+%3A+1.89248%0D%0A213+%3A+77+%3A+2.35542%0D%0A214+%3A+75+%3A+2.35425%0D%0A216+%3A+35+%3A+2.80721%0D%0A216+%3A+37+%3A+2.62221%0D%0A218+%3A+244+%3A+2.53919%0D%0A221+%3A+113+%3A+1.6737%0D%0A224+%3A+51+%3A+1.86829%0D%0A224+%3A+107+%3A+0.127647%0D%0A224+%3A+154+%3A+2.52523%0D%0A225+%3A+125+%3A+1.83583%0D%0A226+%3A+100+%3A+1.84942%0D%0A226+%3A+148+%3A+2.52077%0D%0A226+%3A+154+%3A+2.14074";
	PiNetwork* network = PiNetwork::getInstance();
	//bool result = network->send_register_request(minutiae_one, minutiae_two , minutiae_three);
	bool result = network->send_file();
	if(result == true) {
		std::cout << "Hello" << std::endl;
	}else {
		std::cout << "TEST" << std::endl;
	}
	return 0;
}
