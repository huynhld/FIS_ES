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

using namespace cv;
using namespace std;



int ImageData[IMAGE_WIDTH][IMAGE_HEIGHT];
double directMatrix[IMAGE_WIDTH][IMAGE_HEIGHT];


void GetDirectionMatrix(int widthSqare)
{
	int i, j, x, y;
	int Ax, Ay, Axy;
	int Gxx, Gyy, Gxy;
	int Bx, By;


	for (j = 0; j < IMAGE_WIDTH; j++)
		for (i = 0; i < IMAGE_HEIGHT; i++)
			directMatrix[j][i] = 0;
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
	for (int x = 0; x < IMAGE_WIDTH; x++)
	{
		for (int y = 0; y < IMAGE_HEIGHT; y++)
		{
			Vec3b  intensity = im.at<Vec3b>(Point(x, y));
			uchar blue = intensity.val[0];
			uchar green = intensity.val[1];
			uchar red = intensity.val[2];
			int temp = (int)round(red*0.299 + red*0.587 + blue*0.114);
			ImageData[x][y] = temp;
		}
	}
	GetDirectionMatrix(4);
}


int getMinutiae(std::vector<Minutiae>& minutiae, std::string imagePath)
{
	// First argv is always the binary being executed
	//"C:\\Users\\NamTe\\Desktop\\FingerPrint\\101_1.bmp"
	Mat sourceImage = cv::imread(imagePath, IMREAD_UNCHANGED);

	if (!sourceImage.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << endl;
		return -1;
	}

	Mat img = sourceImage.clone();
	LoadImageData(img);
	//cv::cvtColor(img, img, CV_RGB2GRAY);
	localThreshold::binarisation(img, 25, 28);
	cv::threshold(img, img, 50, 255, cv::THRESH_BINARY);
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

	return 0;
}

int main()
{
	vector<Minutiae> minutiaeOne;
	vector<Minutiae> minutiaeTwo;
	getMinutiae(minutiaeOne, "/home/namte/Desktop/Untitled Folder 2/namte.bmp");
	getMinutiae(minutiaeTwo, "/home/namte/Desktop/Untitled Folder 2/namte.bmp");
	
	int angleStart = -30;
	int angleUnit = 3;
	int angleFinish = 30;
	int anglesCount = 21;
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

	double scaleSet[] = { 0.8,0.9,1.0,1.1,1.2 };
	Minutiae minuResult = Functions::GetMinutiaeChanging_UseHoughTransform(minutiaeOne, minutiaeTwo, angleSet, deltaXSet, deltaYSet, 5 * PI / 180, 388 / 2, 377 / 2);
	int count = Functions::CountMinuMatching(minutiaeOne, minutiaeTwo, minuResult, 5, 5 * PI / 180);
	cout << "Count : " << count << endl;
	if (count >= 10)
		cout << "Hai vân tay trùng khớp" << endl;
	else
		cout << "Hai vân tay không trùng khớp" << endl;

	return 0;

}
