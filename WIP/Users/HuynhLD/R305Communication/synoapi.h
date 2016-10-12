/*
 * synoapi.h
 *
 *  Created on: Sep 16, 2016
 *      Author: huynhld
 */

#ifndef SYNOAPI_H_
#define SYNOAPI_H_

#include <cstring>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>

#define PS_OK                0x00
#define PS_COMM_ERR          0x01
#define PS_NO_FINGER         0x02
#define PS_GET_IMG_ERR       0x03
#define PS_FP_TOO_DRY        0x04
#define PS_FP_TOO_WET        0x05
#define PS_FP_DISORDER       0x06
#define PS_LITTLE_FEATURE    0x07
#define PS_NOT_MATCH         0x08
#define PS_NOT_SEARCHED      0x09
#define PS_MERGE_ERR         0x0a
#define PS_ADDRESS_OVER      0x0b
#define PS_READ_ERR          0x0c
#define PS_UP_TEMP_ERR       0x0d
#define PS_RECV_ERR          0x0e
#define PS_UP_IMG_ERR        0x0f
#define PS_DEL_TEMP_ERR      0x10
#define PS_CLEAR_TEMP_ERR    0x11
#define PS_SLEEP_ERR         0x12
#define PS_INVALID_PASSWORD  0x13
#define PS_RESET_ERR         0x14
#define PS_INVALID_IMAGE     0x15
#define PS_HANGOVER_UNREMOVE 0X17

class SynoApi {

private:
	int tty_fd;
	struct termios tio;
	int write_package(unsigned char* package, int size);
	int read_image_buffer(int &size);
	int read_return_package(int size);
	unsigned char ImgData[256 * 288];
	int GetPackage(unsigned char *pData);
	int PSImgData2BMP(unsigned char* pImgData, const char* pImageFile);
	int WriteBMP(char* file, unsigned char* Input);
	bool DeCode(unsigned char* pSource, int iSourceLength, unsigned char* pDestination, int* iDestinationLength);
	int GetPackageContentLength(unsigned char* pData);
	int GetPackageLength(unsigned char* pData);
	int VerifyResponsePackage(unsigned char nPackageType, unsigned char* pData);

public:
	SynoApi();
	~SynoApi();
	int get_img();
	int upload_img();
	void show_message(int ret);
	bool is_opened();

};

#endif /* SYNOAPI_H_ */
