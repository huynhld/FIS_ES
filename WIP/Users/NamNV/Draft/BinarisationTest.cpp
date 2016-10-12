#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "AdaptiveLocalThreshold.h"
#include "Ideka.h"
#include "GuoHall.h"
#include "CrossingNumber.h"
#include "Filter.h"
#include "Minutiae.h"
#include "Functions.h"
#include "SQL.h"
#include <stdio.h>
using namespace cv;
using namespace std;



int ImageData[IMAGE_WIDTH][IMAGE_HEIGHT];
int SouceImageData[IMAGE_WIDTH][IMAGE_HEIGHT];
double directMatrix[IMAGE_WIDTH][IMAGE_HEIGHT];

int ImageDataRows[IMAGE_ROWS][IMAGE_COLUMNS];
int SouceImageDataRows[IMAGE_ROWS][IMAGE_COLUMNS];
double directMatrixRows[IMAGE_ROWS][IMAGE_COLUMNS];



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


void GetDirectionMatrixRows(int widthSqare)
{
	int i, j, x, y;
	int Ax, Ay, Axy;
	int Gxx, Gyy, Gxy;
	int Bx, By;
	double temp = 0;

	for (j = 0; j < IMAGE_ROWS; j++)
	{
		for (i = 0; i < IMAGE_COLUMNS; i++)
		{
			directMatrixRows[j][i] = 0;
			temp += (ImageDataRows[j][i] - image_mean)*(ImageDataRows[j][i] - image_mean);
		}
	}
	image_variance = temp / (IMAGE_WIDTH*IMAGE_HEIGHT);
	cout << image_variance << endl;		
	for (x = widthSqare + 1; x < IMAGE_ROWS - widthSqare - 1; x++)
	{
		for (y = widthSqare + 1; y < IMAGE_COLUMNS - widthSqare - 1; y++)
		{
			Ax = 0; Ay = 0; Axy = 0;
			Gxx = 0; Gyy = 0; Gxy = 0;
			Bx = 0; By = 0;

			for (i = x - widthSqare; i < x + widthSqare - 1; i++)
			{
				for (j = y - widthSqare; j < y + widthSqare - 1; j++)
				{
					Bx = ((ImageDataRows[i + 2][j] + 2 * ImageDataRows[i + 2][j + 1] + ImageDataRows[i + 2][j + 2] - ImageDataRows[i][j] - 2 * ImageDataRows[i][j + 1] - ImageDataRows[i][j + 2]));
					By = ((ImageDataRows[i][j + 2] + 2 * ImageDataRows[i + 1][j + 2] + ImageDataRows[i + 2][j + 2] - ImageDataRows[i][j] - 2 * ImageDataRows[i + 1][j] - ImageDataRows[i + 2][j]));
					Ax += Bx*Bx;
					Ay += By*By;
					Axy += Bx*By;
				}
			}
			Gxx = Ax;
			Gyy = Ay;
			Gxy = Axy;

			directMatrixRows[x][y] = PI / 2 - 0.5*atan2(2 * Gxy, Gxx - Gyy);
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


void LoadImageDataRows(Mat& im)
{
	int temp_mean = 0;
	cout << im.channels() << " " << im.cols << " " <<  im.size() << endl;
	cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);
	for (int x = 0; x < im.rows; x++)
	{
		for (int y = 0; y < im.cols; y++)
		{
			
			Vec3b intensity = im.at<Vec3b>(x, y);
			uchar blue = intensity.val[0];
			uchar green = intensity.val[1];
			uchar red = intensity.val[2];
			int temp = (int)round(red*0.299 + red*0.587 + blue*0.114);
			ImageDataRows[x][y] = temp;
			temp_mean += temp;
			marker.at<uchar>(y, x) = (uchar)temp;
			//cout << (int)im.at<uchar>(x, y) << " " <<  temp << endl;
			SouceImageDataRows[x][y] = temp;
		  
		}
	}
	imshow("marker",marker);
	//im = marker;
	//cout << (int)im.at<uchar>((1, 0)) << " " << (int)im.at<uchar>(Point(1, 0)) << endl;
	image_mean = static_cast<double>(temp_mean) / static_cast<double>(IMAGE_HEIGHT*IMAGE_WIDTH);
	cout << image_mean << endl;
	GetDirectionMatrixRows(4);
}


void normalizationRows(Mat& img, int MEAN, int VARIANCE)
{	
	for (int i = 0; i<img.rows; i++)
	{
		for (int j = 0; j<img.cols; j++)
		{
			double tempData = static_cast<double>(SouceImageDataRows[i][j]);
			if (tempData>image_mean)
			{
				SouceImageDataRows[i][j] = static_cast<int>(MEAN + sqrt((tempData - image_mean)*(tempData - image_mean)*VARIANCE / image_variance));
			}
			else
			{
				SouceImageDataRows[i][j]  = static_cast<int>(MEAN - sqrt((tempData - image_mean)*(tempData - image_mean)*VARIANCE / image_variance));
			}
			//img.at<uchar>((i, j)) = SouceImageDataRows[i][j];

		}
	}
}


void normalization(Mat& img, int MEAN, int VARIANCE)
{	
	for (int i = 0; i<IMAGE_WIDTH; i++)
	{
		for (int j = 0; j<IMAGE_HEIGHT; j++)
		{
			double tempData = static_cast<double>(SouceImageData[i][j]);
			if (tempData>image_mean)
			{
				SouceImageData[i][j] = static_cast<int>(MEAN + sqrt((tempData - image_mean)*(tempData - image_mean)*VARIANCE / image_variance));
			}
			else
			{
				SouceImageData[i][j]  = static_cast<int>(MEAN - sqrt((tempData - image_mean)*(tempData - image_mean)*VARIANCE / image_variance));
			}
			img.at<uchar>((Point(i, j))) = (uchar) SouceImageData[i][j];

		}
	}
}


void binarisation(Mat& im) {
	for (int x = 0; x < im.cols; x++)
	{
		for (int y = 0; y < im.rows; y++)
		{
			int temp = (int)im.at<uchar>(Point(x, y));
			if(temp >= 136) {
				im.at<uchar>(Point(x, y)) = (uchar)255;
			}else {
				im.at<uchar>(Point(x, y)) = (uchar)0;
			}
		}
	}
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

	imshow("Before", img); waitKey(0);
	//Mat img = sourceImage.clone();
	LoadImageData(img);
	//normalization(img, 50, 300);
	imshow("After", img); waitKey(0);
	//cv::cvtColor(img, img, CV_RGB2GRAY);
	imshow("After", img); waitKey(0);
	localThreshold::binarisation(img, 25, 28);
	//binarisation(img);
	imshow("binarisation", img); waitKey(0);
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



    Mat minutImg2 = img.clone();
    cvtColor(img, minutImg2, CV_GRAY2RGB);
    for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){
        //add an transparent square at each minutiae-location
        int squareSize = 5;     //has to be uneven
        Mat roi = minutImg2(Rect(minutiae[i].getLocX()-squareSize/2, minutiae[i].getLocY()-squareSize/2, squareSize, squareSize));
        double alpha = 0.3;
        if(minutiae[i].getType() == Minutiae::RIDGEENDING){
            Mat color(roi.size(), CV_8UC3, cv::Scalar(255,0,0));    //blue square for ridgeending
            addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
            //iRIDGEENDING++;
        }else if(minutiae[i].getType() == Minutiae::BIFURCATION){
            Mat color(roi.size(), CV_8UC3, cv::Scalar(0,0,255));    //red square for bifurcation
            addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
            //iBIFURCATION++;
        }

    }
     namedWindow( "Minutien gefiltert", WINDOW_AUTOSIZE );     // Create a window for display.
    imshow( "Minutien gefiltert", minutImg2 );                 //
	return 0;
}


int getMinutiaeRows(std::vector<Minutiae>& minutiae, std::string imagePath)
{
	// First argv is always the binary being executed
	//"C:\\Users\\NamTe\\Desktop\\FingerPrint\\101_1.bmp"
	Mat img = cv::imread(imagePath, 0);
	if (!img.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << endl;
		return -1;
	}

	LoadImageDataRows(img);
	imshow("Hello", img); waitKey(0);
	normalization(img, 50, 100);
	
	localThreshold::binarisation(img, 25, 28);
	cv::threshold(img, img, 50, 255, cv::THRESH_BINARY);
	Mat binImg = img.clone();
	ideka::binOptimisation(img);


	//skeletionizing
	cv::bitwise_not(img, img);    //Inverse for bit-operations
	GuoHall::thinning(img);
	cv::bitwise_not(img, img);


	crossingNumber::getMinutiaeRows(img, minutiae, 30, directMatrixRows);
	cout << "Anzahl gefundener Minutien: " << minutiae.size() << endl;

	//Minutiae-filtering
	Filter::filterMinutiae(minutiae);
	std::cout << "After filter: " << minutiae.size() << std::endl;

    return 0;
}


void show_vector(vector<Minutiae> v) {
	std::cout << "==============Start Vector=======================" << std::endl;
	for(int i = 0; i < v.size(); i++) {
		std::cout << v[i].getLocX() << " : " << v[i].getLocY() << " : " <<  v[i].getDirect() << std::endl;
	}
	std::cout << "==============End Vector=========================" << std::endl;
}

int main()
{
	
	vector<Minutiae> minutiaeOne;
	vector<Minutiae> minutiaeTwo;
	getMinutiae(minutiaeOne, "/home/namte/Desktop/Untitled Folder 2/namte.bmp");
	getMinutiae(minutiaeTwo, "/home/namte/Desktop/Untitled Folder 2/namte2.bmp");
	show_vector(minutiaeTwo);
	show_vector(minutiaeOne);
	// sql.create_table();
	// // void create_table();
	// // 	void get_all_database();
	// // 	const string minutiae_database_name = "Minutiae.db";
	// // 	std::vector<Minutiae> get_all();
	// // 	void insert_minutiae(std::vector<Minutiae> v, int fingerprint_id);
	// // 	int insert_fingerprint();
	// int personid = sql.insert_fingerprint();
	// sql.insert_minutiae(minutiaeOne, personid);
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
	Minutiae minuResult = Functions::GetMinutiaeChanging_UseHoughTransform(minutiaeOne ,
		minutiaeTwo, angleSet, deltaXSet,
		deltaYSet, anglesCount, deltaXCount,
		deltaYCount, angleLimit * PI / 180,
		IMAGE_ROWS / 2, IMAGE_COLUMNS / 2);
	vector<Minutiae> sds;
	sds.push_back(minuResult);
	show_vector(sds);
	int count = Functions::CountMinuMatching(minutiaeOne , minutiaeTwo,
		minuResult, distanceLimit, angleLimit * PI / 180);
			// if(count >= max_count) {
			// 	max_count = count;
			// }
			// if (count >= 10) {
			// 	finger_id_exist = iterator->first;
			// }
		// }
  //   }
	if (count >= 10)
		cout << "Hai vân tay trùng khớp ID : " << count << endl;
	else
		cout << "Hai vân tay không trùng khớp : " << count << endl;

	return 0;

}
