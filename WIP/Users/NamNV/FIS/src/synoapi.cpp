#include "../header/synoapi.h"
#include "../header/utility.h"


#define BAUD_RATE B57600
#define DATA_BUF_SIZE 10000
#define IMAGE_X 256
#define IMAGE_Y 288
const int   MAX_PACKAGE_SIZE  =  350;
const int   HEAD_LENGTH       =  3; 
const unsigned char     CMD      =  0x01;
const unsigned char     DATA     =  0x02;
const unsigned char     ENDDATA  =  0x08;
const unsigned char     RESPONSE =  0x07;

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
	int count = 0;
	unsigned char headerBuf[10]; //
	int packageNum = 0;
	while(true) {
		ret = 0;
	    /* set timeout value within input loop */
	    Timeout.tv_usec = 0;  /* milliseconds */
	    Timeout.tv_sec  = 2;  /* seconds */
		fd_set readfs;
		FD_SET(tty_fd, &readfs);  /* set testing for source 1 */
		ret = select(tty_fd + 1, &readfs, NULL, NULL, &Timeout);
		if (ret==0) {
			break;
		}	
		
		while((read(tty_fd, &ImgData[i], 1)) > 0) {
			i++;
		}
		packageNum++;
	}
	std::cout << "packageNum " << packageNum << std::endl;
	//PSImgData2BMP(ImgData,"/home/namte/Desktop/NamTe/port2C/namte");
	
	size = i;
	return ret;
}


int SynoApi::GetPackage(unsigned char *pData) {
	unsigned char headerBuf[10];
	unsigned char recvBuf[MAX_PACKAGE_SIZE];
	unsigned char cChar;
	int iSuccess = 0;
	int i;
	int ret = 0;
	struct timeval Timeout;
	int nCount;
	unsigned char* decodedBuf;
	int nDecodedLen;
	decodedBuf = pData;
	for(i=0;i<10;i++){
		headerBuf[i]=0;
	}

	int RecNum=0;
	
	while(true) {
		/* set timeout value within input loop */
	    Timeout.tv_usec = 0;  /* milliseconds */
	    Timeout.tv_sec  = 2;  /* seconds */
		fd_set readfs;
		FD_SET(tty_fd, &readfs);  /* set testing for source 1 */
		select(tty_fd + 1, &readfs, NULL, NULL, &Timeout);
		
		iSuccess = read(tty_fd, &cChar, 1);
		if(iSuccess) {
			for(i=0; i<8; i++){
				headerBuf[i]=headerBuf[i+1];
				
			}
			headerBuf[8]=cChar;
			
			if((headerBuf[0]==0xef)&&(headerBuf[1]==0x01))
				break;
		}
		RecNum++;
	}

	int PacketLen=0;
	for(nCount=0;nCount<9;nCount++){
		recvBuf[nCount]=headerBuf[nCount];

	}
	PacketLen=(recvBuf[7]<<8)+recvBuf[8];
	std::cout << PacketLen << std::endl;
	while(true) {
		ret = 0;
		/* set timeout value within input loop */
	    Timeout.tv_usec = 0;  /* milliseconds */
	    Timeout.tv_sec  = 2;  /* seconds */
		fd_set readfs;
		FD_SET(tty_fd, &readfs);  /* set testing for source 1 */
		ret = select(tty_fd + 1, &readfs, NULL, NULL, &Timeout);
		if(ret == 0) {
			return -1;
		}

		while((read(tty_fd, &cChar, 1)) > 0) {
			recvBuf[nCount++] = cChar;
			if((PacketLen>0) && (nCount>=PacketLen+9)){
				goto end_of_function;
			}
		}

	}
end_of_function:
	if ( !DeCode(recvBuf, nCount, decodedBuf, &nDecodedLen) ){
		return -1;
	}
	return 0;
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
		if (ret == 0 || i >= size) {
			break;
		}
		while((ret = read(tty_fd, &result[i], 1)) > 0) {
			i++;
			if(i == size) {
				break;
			}
		}
	}
	return result[9];
}

int SynoApi::write_package(unsigned char* package, int size) {
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

int SynoApi::upload_img(std::string image_path) {
	unsigned char  iGetData[MAX_PACKAGE_SIZE];
	std::cout << "upload_img start : " << std::endl;
	int size = 0;
	int return_code = 0;
	return_code = write_package(up_img, 12);
	if(return_code < 0) {
		std::cout << "write_package upload_img error" << std::endl;
		return -1;
	}
	return_code = read_return_package(12);
	int length;
	int totalLen = 0;
	int checkResult = 0;
	int PacketNum=0;
	int result = 0;
	do
	{
		
		memset(iGetData, 0, MAX_PACKAGE_SIZE);
		if ( !GetPackage(iGetData) )
			return -2;
		
		length = GetPackageContentLength(iGetData)-2;
		result = VerifyResponsePackage(iGetData[0], iGetData);  //Ð£ÑéÓ¦´ð°ü
		if ( result != 0 )
			return_code = -2;
		
		memcpy((void*)(ImgData+totalLen), (void*)(iGetData+HEAD_LENGTH), length);
		totalLen += length;
		PacketNum++;
	}
	while ( iGetData[0] != ENDDATA );
	PSImgData2BMP(ImgData, image_path.c_str());
	return return_code;
}

int SynoApi::GetPackageContentLength(unsigned char* pData)
{
	
	if (!pData)
		return 0;
	int length = 0;
	length = pData[1]*256 + pData[2];
	return length;
}

int  SynoApi::GetPackageLength(unsigned char* pData)
{
    // |  °ü±êÊ¶   |   °ü³¤¶È	  |   ...{ÄÚÈÝ}     |  Ð£ÑéºÍ    |
    // |  1 byte   |     2 bytes  |	  ...{°ü³¤¶È}   |  2 bytes   |
	
	if (!pData)
		return 0;
	int length = 0;
	length = pData[1]*256 + pData[2] + 1 +2;
	return length;
}


int  SynoApi::VerifyResponsePackage(unsigned char nPackageType, unsigned char* pData)
{
	
	long checkSum = 0;
	
	if (!pData)
		return -3;
	if ( pData[0] != nPackageType )
		return -3;
	
	int iLength;
	
	iLength = GetPackageLength(pData);  //µÃµ½°ü³¤¶È
	
	
	if (nPackageType == RESPONSE)
		return pData[3];  //È·ÈÏÂë
	
	return 0;
}

int SynoApi::WriteBMP(char* file,unsigned char* Input)
{
	
	unsigned char head[1078]={
		/***************************/
		//file header
		0x42,0x4d,//file type 
		0x36,0x6c,0x01,0x00, //file size***
		//0x0,0x0,0x0,0x00, //file size***
		0x00,0x00, //reserved
		0x00,0x00,//reserved
		0x36,0x4,0x00,0x00,//head byte***
		/***************************/
		//infoheader
		0x28,0x00,0x00,0x00,//struct size
			
		0x00,0x01,0x00,0x00,//map width*** 
		//0x00,0x00,0x0,0x00,//map width*** 
		0x68,0x01,0x00,0x00,//map height***
		//0x00,0x00,0x00,0x00,//map height***
			
		0x01,0x00,//must be 1
		0x08,0x00,//color count***
		0x00,0x00,0x00,0x00, //compression
		0x00,0x68,0x01,0x00,//data size***
		//0x00,0x00,0x00,0x00,//data size***
		0x00,0x00,0x00,0x00, //dpix
		0x00,0x00,0x00,0x00, //dpiy
		0x00,0x00,0x00,0x00,//color used
		0x00,0x00,0x00,0x00,//color important
	};
	
	FILE *fh;
	
	if( (fh  = fopen( file, "wb" )) == NULL )
		return 0;
	
	int i,j;
	
	long num;
	num=IMAGE_X; head[18]= num & 0xFF;
	num=num>>8;  head[19]= num & 0xFF;
	num=num>>8;  head[20]= num & 0xFF;
	num=num>>8;  head[21]= num & 0xFF;
	
	
	num=IMAGE_Y; head[22]= num & 0xFF;
	num=num>>8;  head[23]= num & 0xFF;
	num=num>>8;  head[24]= num & 0xFF;
	num=num>>8;  head[25]= num & 0xFF;
	
	j=0;
	for (i=54;i<1078;i=i+4)
	{
		head[i]=head[i+1]=head[i+2]=j; 
		head[i+3]=0;
		j++;
	}	
	
    fwrite(head,sizeof(char),1078,fh);
	
	for(  i=0;i<=IMAGE_Y-1; i++ )
	{

		fseek(fh,1078*sizeof(char)+(IMAGE_Y-1-i)*IMAGE_X,SEEK_SET);	
		fwrite((Input+i*IMAGE_X),sizeof(char),IMAGE_X,fh );
	}
	
	
	fclose(fh);
	
	return 1;
}

int SynoApi::PSImgData2BMP(unsigned char* pImgData,const char* pImageFile)
{
	unsigned char newData[IMAGE_X*IMAGE_Y];

	for(int i=0;i<IMAGE_X*IMAGE_Y/2;i++)
	{
		newData[i*2]=(*(pImgData+i)>>4)*16;
		newData[i*2+1]=(*(pImgData+i)&0x0f)*16;
	}
	bool ret = WriteBMP((char*)pImageFile, newData);
	if ( !ret )
		return -6;

	return 0;
}




bool SynoApi::DeCode(unsigned char* pSource, int iSourceLength, unsigned char* pDestination, int* iDestinationLength)
{
	unsigned char*   pSrc;
	unsigned char*   pDest;
	
	*iDestinationLength = 0;
	if ( !pSource || !pDestination)
		return -1;
    if ( iSourceLength<0 || iSourceLength>350 )
		return -1;
	
	pSrc  = pSource;
	pDest = pDestination;
	
	if(*pSrc!=0xEF || *(pSrc+1)!=0x01) return -1;
	pSrc=pSource+6;
	
	int ChkSum=0;
	
	for(int i=0;i<(iSourceLength-8);i++)
	{
		*(pDest+i)=*(pSrc+i);
		ChkSum+=(unsigned char)(*(pSrc+i));
	}
	
	int ValH,ValL,Val;
	ValH=*(pSource+iSourceLength-2);
	ValL=*(pSource+iSourceLength-1);
    Val=(ValH<<8)+ValL;
	
	if(ChkSum!=Val) 
		return -1; 
	*iDestinationLength = iSourceLength-8;
	
	return 0;
}