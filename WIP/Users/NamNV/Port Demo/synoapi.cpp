#include "synoapi.h"
#include "utility.h"


#define BAUD_RATE B57600
#define DATA_BUF_SIZE 10000
SynoApi::SynoApi() {

    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;

    tty_fd = open("/dev/ttyUSB0", O_RDWR | O_NONBLOCK);
	if (tty_fd == -1)
	{
	/*
	* Could not open the port.
	*/
		std::cout << "open_port: Unable to open /dev/ttyUSB0 - " << tty_fd << std::endl;
	}
	else {
		std::cout << "open_port: Success to open /dev/ttyUSB0 - " << tty_fd << std::endl;
		//fcntl(tty_fd, F_SETFL, O_NONBLOCK);
		 tcflush(tty_fd, TCIFLUSH);
	}
    
    cfsetospeed(&tio, BAUD_RATE);            // 57600 baud
    cfsetispeed(&tio, BAUD_RATE);            // 57600 baud

    tcsetattr(tty_fd,TCSANOW,&tio);

    
}

SynoApi::~SynoApi() {
	close(tty_fd);
}

bool SynoApi::is_opened() {
	return tty_fd != -1;
}

void SynoApi::show_message(int ret) {
	switch(ret) {
		case -1 : {
			std::cout << "Internal error" << std::endl;
			break;
		}
		case 0: {
			std::cout << "commad execution complete" << std::endl;
			break;
		}
		case 1: {
			std::cout << "error when receiving data package"<< std::endl;
			break;
		}
		case 2: {
			std::cout << "no finger on the sensor" << std::endl;
			break;
		}
		default: {
			std::cout << "not really error" << std::endl;
			break;
		}
	}
}




//Read ImageBuffer data
int SynoApi::read_image_buffer(int &size) {
	sleep(1);
	struct timeval Timeout;
	int i = 0;
	int ret = 0;
	memset (ImgData,'\0',256*288);	
	int fd_1 = open("/home/namte/Desktop/NamTe/port2C/namte", O_RDWR | O_NONBLOCK);
	while(true) {
		ret = 0;
	    /* set timeout value within input loop */
	    Timeout.tv_usec = 0;  /* milliseconds */
	    Timeout.tv_sec  = 5;  /* seconds */
		fd_set readfs;
		FD_SET(tty_fd, &readfs);  /* set testing for source 1 */
		ret = select(tty_fd + 1, &readfs, NULL, NULL, &Timeout);
		if (ret==0) {
			break;
		}	
		
		while((read(tty_fd, &ImgData[i], 1)) > 0) {
			write(fd_1,&ImgData[i], 1);
			i++;
		}
	}
	close(fd_1);
	size = i;
	return ret;
}



//Read 12 byte data | Acknowledge package format
int SynoApi::read_return_package(int size) {
	//sleep(1);
	struct timeval Timeout;
	int ret = 0;
	int i = 0;
	int bytes_available = 0;
	unsigned char result[20] = {'\0'};
	while(true) {
		/* set timeout value within input loop */
	    Timeout.tv_usec = 0;  /* milliseconds */
	    Timeout.tv_sec  = 2;  /* seconds */
		fd_set readfs;
		FD_SET(tty_fd, &readfs);  /* set testing for source 1 */
		ret = select(tty_fd + 1, &readfs, NULL, NULL, &Timeout);
		if (ret == 0 && i != 12) {
			printf("read_return_package %d = %x \n", 9, result[9]);
			return -1;
		}else if(ret == 0) {
			break;
		}
		
		while((ret = read(tty_fd, &result[i], 1)) > 0) {
			printf("read_return_package %d = %x \n", i, result[i]);
			i++;
		}
	}
	return result[9];
}



int SynoApi::write_package(char* package, int size) {
	int i = 0;
	int ret = 0;
	for(i = 0; i < size; i++) {
		ret = write(tty_fd, &package[i], 1);
		if(ret < 0) {
			std::cout <<  "Can not write to serial port" << std::endl;
			return -1;
		}
	}
	
	return ret;
}

int SynoApi::get_img() {
	std::cout << "get_img start" << std::endl;
	int size = 0;
	int return_code = 0;
	return_code = write_package(gen_img, sizeof(gen_img)/sizeof(char));
	if(return_code < 0) {
		std::cout << "write_package get_img error" << std::endl;
		return -1;
	}
 	return_code = read_return_package(12);
	return return_code;
}

int SynoApi::upload_img() {
	std::cout << "upload_img start : " << std::endl;
	int size = 0;
	int return_code = 0;
	return_code = write_package(up_img, 12);
	if(return_code < 0) {
		std::cout << "write_package upload_img error" << std::endl;
		return -1;
	}

	return_code = this->read_image_buffer(size);
	std::cout << "Size of image: " << size*sizeof(char) << "Return code " << return_code << std::endl;
	
	return return_code;
}

