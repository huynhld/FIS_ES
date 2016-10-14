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
#include "header/synoapi.h"
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
	//cout << image_mean << endl;
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

	//imshow("Before", img); waitKey(0);
	//Mat img = sourceImage.clone();
	LoadImageData(img);
	//normalization(img, 50, 300);
	//imshow("After", img); waitKey(0);
	//cv::cvtColor(img, img, CV_RGB2GRAY);
	//imshow("After", img); waitKey(0);
	localThreshold::binarisation(img, 25, 28);
	//binarisation(img);
	//imshow("binarisation", img); waitKey(0);
	cv::threshold(img, img, 0, 255, cv::THRESH_BINARY);
	Mat binImg = img.clone();
	ideka::binOptimisation(img);


	//skeletionizing
	cv::bitwise_not(img, img);    //Inverse for bit-operations
	GuoHall::thinning(img);
	cv::bitwise_not(img, img);


	crossingNumber::getMinutiae(img, minutiae, 30, directMatrix);
	//cout << "Anzahl gefundener Minutien: " << minutiae.size() << endl;

	//Minutiae-filtering
	Filter::filterMinutiae(minutiae);
	//std::cout << "After filter: " << minutiae.size() << std::endl;



    // Mat minutImg2 = img.clone();
    // cvtColor(img, minutImg2, CV_GRAY2RGB);
    // for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){
    //     //add an transparent square at each minutiae-location
    //     int squareSize = 5;     //has to be uneven
    //     Mat roi = minutImg2(Rect(minutiae[i].getLocX()-squareSize/2, minutiae[i].getLocY()-squareSize/2, squareSize, squareSize));
    //     double alpha = 0.3;
    //     if(minutiae[i].getType() == Minutiae::RIDGEENDING){
    //         Mat color(roi.size(), CV_8UC3, cv::Scalar(255,0,0));    //blue square for ridgeending
    //         addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
    //         //iRIDGEENDING++;
    //     }else if(minutiae[i].getType() == Minutiae::BIFURCATION){
    //         Mat color(roi.size(), CV_8UC3, cv::Scalar(0,0,255));    //red square for bifurcation
    //         addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
    //         //iBIFURCATION++;
    //     }

    // }
    //  namedWindow( "Minutien gefiltert", WINDOW_AUTOSIZE );     // Create a window for display.
    // imshow( "Minutien gefiltert", minutImg2 );                 //
	return 0;
}



void show_vector(vector<Minutiae> v) {
	std::cout << "==============Start Vector=======================" << std::endl;
	for(int i = 0; i < v.size(); i++) {
		std::cout << v[i].getLocX() << " : " << v[i].getLocY() << " : " <<  v[i].getDirect() << std::endl;
	}
	std::cout << "==============End Vector=========================" << std::endl;
}

int main(int argc, const char** argv)
{
	// if(argc != 2) {
 //        std::cout << "Please provide a image file as the parameter..." << std::endl;
 //        exit(1);
 //    }

    SynoApi *api = new SynoApi();
	if(!api->is_opened()) {
		api->show_message(-1);
		return 0;
	}
	int ret = api->get_img();
	if(ret != PS_OK) {
		api->show_message(ret);
		return 0;
	}
	ret = api->upload_img();
	if(ret != PS_OK) {
		api->show_message(ret);
		return 0;
	}

	// ---Init data---///
	double scaleSet[] = { 0.8,0.9,1.0,1.1,1.2 };
	int angleStart = -30;
	int angleUnit = 3;
	int angleFinish = 30;
	int anglesCount = (int)((angleFinish - angleStart) / angleUnit) + 1;
	int angleSet[21];
	int i = 0;
	int angle = angleStart;
	while (i < anglesCount)
	{
		angleSet[i] = angle;
		i++;
		angle += angleUnit;
	}
	//DELTAXSET
	int deltaXStart = -IMAGE_WIDTH;
	int deltaXFinish = IMAGE_WIDTH;
	int deltaXUnit = 2;
	int deltaXCount = (int)((deltaXFinish - deltaXStart) / deltaXUnit) + 1;
	int deltaXSet[deltaXCount];
	i = 0;
	int deltaX = deltaXStart;
	while (i < deltaXCount)
	{
		deltaXSet[i] = deltaX;
		i++;
		deltaX += deltaXUnit;
	}
	//DELTAYSET
	int deltaYStart = -IMAGE_HEIGHT;
	int deltaYFinish = IMAGE_HEIGHT;
	int deltaYUnit = 2;
	int deltaYCount = round((deltaYFinish - deltaYStart) / deltaYUnit) + 1;
	int deltaYSet[deltaYCount];
	i = 0;
	int deltaY = deltaYStart;
	while (i < deltaYCount)
	{
		deltaYSet[i] = deltaY;
		i++;
		deltaY += deltaYUnit;
	}


	//-- End Init Data -- //	
	vector<Minutiae> minutiaeOne;
	vector<Minutiae> minutiaeTwo;
	int result = getMinutiae(minutiaeOne, "./fingerprintimage.bmp");
	if(result == -1) {
		return 0;
	}
	//getMinutiae(minutiaeTwo, "/home/namte/Desktop/Untitled Folder 2/namte.bmp");
	//show_vector(minutiaeTwo);
	//show_vector(minutiaeOne);
	//SQL sql;
	//sql.create_table();
	// void create_table();
	// 	void get_all_database();
	// 	const string minutiae_database_name = "Minutiae.db";
	// 	std::vector<Minutiae> get_all();
	// 	void insert_minutiae(std::vector<Minutiae> v, int fingerprint_id);
	// 	int insert_fingerprint();
	//int personid = sql.insert_fingerprint();
	//sql.insert_minutiae(minutiaeOne, personid);
	int max_count = 0;
	int count = 0;
	int finger_id_exist = -1;
	SQL sql;
    sql.create_table();   
    map<int, vector<Minutiae> > map_data = sql.get_all_database();
    if(map_data.size() == 0 || minutiaeOne.size() == 0) {
    	cout << "Not found" << endl;
    }else {
    	std::map<int, vector<Minutiae> >::iterator iterator;
		for(iterator = map_data.begin(); iterator != map_data.end(); iterator++) {
		    // iterator->first = key
		    // iterator->second = value
		    vector<Minutiae> v = static_cast<vector<Minutiae> > (iterator->second);
		    //show_vector(v);
		    Minutiae minuResult = Functions::GetMinutiaeChanging_UseHoughTransform(minutiaeOne ,
				v, angleSet, deltaXSet,
				deltaYSet, anglesCount, deltaXCount,
				deltaYCount, angleLimit * PI / 180,
				IMAGE_ROWS / 2, IMAGE_COLUMNS / 2);
			int count = Functions::CountMinuMatching(minutiaeOne , v,
				minuResult, distanceLimit, angleLimit * PI / 180);
			if(count >= max_count) {
				max_count = count;
			}
			if (count >= 10) {
				finger_id_exist = iterator->first;
			}
		}
	}
	// Minutiae minuResult = Functions::GetMinutiaeChanging_UseHoughTransform(minutiaeOne ,
	// 	minutiaeTwo, angleSet, deltaXSet,
	// 	deltaYSet, anglesCount, deltaXCount,
	// 	deltaYCount, angleLimit * PI / 180,
	// 	IMAGE_ROWS / 2, IMAGE_COLUMNS / 2);
	// int count = Functions::CountMinuMatching(minutiaeOne , minutiaeTwo,
	// 	minuResult, distanceLimit, angleLimit * PI / 180);
			// if(count >= max_count) {
			// 	max_count = count;
			// }
			// if (count >= 10) {
			// 	finger_id_exist = iterator->first;
			// }
		// }
  //   }
	if (max_count >= 10)
		cout << "Welcome : " << finger_id_exist << endl;
	else
	{
		cout << "User Not found!" << endl;
	}

	if( remove( "./fingerprintimage.bmp" ) != 0 )
    	std::cout << "Error deleting file" << std::endl;
	else
    	std::cout << "File successfully deleted" << std::endl;
	return 0;

}
