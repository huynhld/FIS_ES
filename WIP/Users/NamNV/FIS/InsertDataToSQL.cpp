#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "header/AdaptiveLocalThreshold.h"
#include "header/Ideka.h"
#include "header/GuoHall.h"
#include "header/CrossingNumber.h"
#include "header/Filter.h"
#include "header/Minutiae.h"
#include "header/Functions.h"
#include "header/SQL.h"
#include <stdio.h>
using namespace cv;
using namespace std;



int ImageData[IMAGE_WIDTH][IMAGE_HEIGHT];
int SouceImageData[IMAGE_WIDTH][IMAGE_HEIGHT];
double directMatrix[IMAGE_WIDTH][IMAGE_HEIGHT];


const int angleLimit = 5;
const int distanceLimit = 15;
const int minuNumberLimit = 14; 
double image_mean = 50;
double image_variance = 300;

void GetDirectionMatrix(int widthSqare)
{
	int i, j, x, y;
	int Ax, Ay, Axy;
	int Gxx, Gyy, Gxy;
	int Bx, By;
	double temp = 0;

	for (j = 0; j < IMAGE_WIDTH; j++)
	{
		for (i = 0; i < IMAGE_HEIGHT; i++)
		{
			directMatrix[j][i] = 0;
			temp += (ImageData[j][i] - image_mean)*(ImageData[j][i] - image_mean);
		}
	}
	image_variance = temp / (IMAGE_WIDTH*IMAGE_HEIGHT);
	//cout << image_variance << endl;		
	for (x = widthSqare + 1; x < IMAGE_WIDTH - widthSqare - 1; x++)
	{
		for (y = widthSqare + 1; y < IMAGE_HEIGHT - widthSqare - 1; y++)
		{
			Ax = 0; Ay = 0; Axy = 0;
			Gxx = 0; Gyy = 0; Gxy = 0;
			Bx = 0; By = 0;

			for (i = x - widthSqare; i < x + widthSqare - 1; i++)
			{
				for (j = y - widthSqare; j < y + widthSqare - 1; j++)
				{
					Bx = ((ImageData[i + 2][j] + 2 * ImageData[i + 2][j + 1] + ImageData[i + 2][j + 2] - ImageData[i][j] - 2 * ImageData[i][j + 1] - ImageData[i][j + 2]));
					By = ((ImageData[i][j + 2] + 2 * ImageData[i + 1][j + 2] + ImageData[i + 2][j + 2] - ImageData[i][j] - 2 * ImageData[i + 1][j] - ImageData[i + 2][j]));
					Ax += Bx*Bx;
					Ay += By*By;
					Axy += Bx*By;
				}
			}
			Gxx = Ax;
			Gyy = Ay;
			Gxy = Axy;

			directMatrix[x][y] = PI / 2 - 0.5*atan2(2 * Gxy, Gxx - Gyy);
		}
	}
	//return directMatrix;

}

void LoadImageData(Mat im)
{
	int temp_mean = 0;
	//cout << im.channels() << " " << im.cols << endl;
	for (int x = 0; x < im.cols; x++)
	{
		for (int y = 0; y < im.rows; y++)
		{
			int temp = (int)im.at<uchar>(Point(x, y));
			ImageData[x][y] = temp;
			temp_mean += temp;
			SouceImageData[x][y] = temp;
		}
	}
	//out << (int)im.at<uchar>((1, 0)) << " " << (int)im.at<uchar>(Point(1, 0)) << endl;
	image_mean = static_cast<double>(temp_mean) / static_cast<double>(IMAGE_HEIGHT*IMAGE_WIDTH);
	cout << image_mean << endl;
	GetDirectionMatrix(4);
}


int getMinutiae(std::vector<Minutiae>& minutiae, std::string imagePath)
{
	// First argv is always the binary being executed
	//"C:\\Users\\NamTe\\Desktop\\FingerPrint\\101_1.bmp"
	Mat img = cv::imread(imagePath, 0);

	if (!img.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << endl;
		return -1;
	}

	
	//Mat img = sourceImage.clone();
	LoadImageData(img);
	//normalization(img, 50, 300);
	
	localThreshold::binarisation(img, 25, 28);
	//binarisation(img);
	
	cv::threshold(img, img, 0, 255, cv::THRESH_BINARY);
	Mat binImg = img.clone();
	ideka::binOptimisation(img);


	//skeletionizing
	cv::bitwise_not(img, img);    //Inverse for bit-operations
	GuoHall::thinning(img);
	cv::bitwise_not(img, img);


	crossingNumber::getMinutiae(img, minutiae, 30, directMatrix);
	cout << "Anzahl gefundener Minutien: " << minutiae.size() << endl;

	//Minutiae-filtering
	Filter::filterMinutiae(minutiae);
	std::cout << "After filter: " << minutiae.size() << std::endl;
	return 0;
}


int main(int argc, const char** argv)
{
	
	if(argc != 2) {
        std::cout << "Please provide a target file as the parameter..." << std::endl;
        exit(1);
    }

	vector<Minutiae> minutiaeOne;
	std::string imgPath(argv[1]);
	getMinutiae(minutiaeOne, imgPath);
	SQL sql;
	sql.create_table();
	// void create_table();
	// 	void get_all_database();
	// 	const string minutiae_database_name = "Minutiae.db";
	// 	std::vector<Minutiae> get_all();
	// 	void insert_minutiae(std::vector<Minutiae> v, int fingerprint_id);
	// 	int insert_fingerprint();
	int personid = sql.insert_fingerprint();
	sql.insert_minutiae(minutiaeOne, personid);

	// int max_count = 0;
	// int count = 0;
	// int finger_id_exist = -1;
	// SQL sql;
 //    sql.create_table();   
 //    map<int, vector<Minutiae> > map_data = sql.get_all_database();
 //    if(map_data.size() == 0) {
 //    	cout << "Not found" << endl;
 //    }else {
 //    	std::map<int, vector<Minutiae> >::iterator iterator;
	// 	for(iterator = map_data.begin(); iterator != map_data.end(); iterator++) {
	// 	    // iterator->first = key
	// 	    // iterator->second = value
	// 	    vector<Minutiae> v = static_cast<vector<Minutiae> > (iterator->second);
	// 	    show_vector(v);
	// ---Init data---///
	
	return 0;

}
