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
#include "header/MaskGabor.h"
#include <ctime>
#include <stdio.h>
#include <sstream>
using namespace cv;
using namespace std;



int ImageData[IMAGE_WIDTH][IMAGE_HEIGHT];
//int ImageData[IMAGE_WIDTH][IMAGE_HEIGHT];
double directMatrix[IMAGE_WIDTH][IMAGE_HEIGHT];
const int maskNumber = 32;
const int angleLimit = 90;
const int distanceLimit = 30;
const int minuNumberLimit = 14;
const int f = 7;
const int fi = 3; 
double image_mean = 100;
double image_variance = 100;

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
			ImageData[x][y] = temp;
		}
	}
	//out << (int)im.at<uchar>((1, 0)) << " " << (int)im.at<uchar>(Point(1, 0)) << endl;
	image_mean = static_cast<double>(temp_mean) / static_cast<double>(IMAGE_HEIGHT*IMAGE_WIDTH);
	//cout << image_mean << endl;
	GetDirectionMatrix(4);
	// int WindowSize = 9;
	// for (int i = 1; i < IMAGE_WIDTH - 1 -WindowSize; i+=WindowSize) {
 //        for (int j = 1; j < IMAGE_HEIGHT -1-WindowSize; j+=WindowSize) {
 //            cv::Rect roi(i, j, WindowSize, WindowSize);

 //            cv::Mat roiImage = im(roi);
 //            //double direction = calculateDirectionForWindow(roiImage);

 //            // TODO: Refactor direction drawing into own function
 //            // direction = i % 360; // for testing
 //            double xDir = std::cos((directMatrix[i][j] * 180 / PI)/180*M_PI);
 //            double yDir = -1 * std::sin((directMatrix[i][j] * 180 / PI)/180*M_PI); // y-Axis is inverted because
 //                                                             // in math, +y is typically
 //                                                             // considered to go in the top
 //                                                             // direction whereas in the image
 //                                                             // it goes towards the bottom

 //            cv::Point p1(
 //                         WindowSize/2 + ((WindowSize/4)*xDir),
 //                         WindowSize/2 + ((WindowSize/4)*yDir));
 //            cv::Point p2(
 //                         WindowSize/2 - ((WindowSize/4)*xDir),
 //                         WindowSize/2 - ((WindowSize/4)*yDir));
 //            //cv::Scalar colorScalar = cv::Scalar(0, 0, 255);
 //            cv::line(roiImage, p1, p2,  1);
 //        }
 //    }
 //    imshow("Direction Field", im); waitKey(0);
}




double** GetMaskFilter(double filterDirect, std::vector<MaskGabor> MaskGaborCollection)
{
		
	MaskGabor mask;
	for(int i=0;i<maskNumber;i++)
	{
		if(filterDirect>=i*PI/maskNumber&&filterDirect<(i+1)*PI/maskNumber)
		{
			mask = (MaskGabor) MaskGaborCollection[i];
			return mask.get_mask();
		}
	}
	mask = (MaskGabor)MaskGaborCollection[maskNumber-1];
	return mask.get_mask();

}
double** GetMaskFilter(double filterDirect,int widthSquare,double f,int fi)
{			
	double** rtMask = new double*[2*widthSquare+1];
	for(int x=0;x<2*widthSquare+1;x++){
		rtMask[x] = new double[2*widthSquare+1];
		for(int y=0;y<2*widthSquare+1;y++)
		{
			double x1 = sin(filterDirect)*(x-widthSquare) + cos(filterDirect)*(y-widthSquare);
			double y1 = sin(filterDirect)*(y-widthSquare) - cos(filterDirect)*(x-widthSquare);
			rtMask[x][y] = exp(-0.5*(pow(x1,2)/pow(fi,2) + pow(y1,2)/pow(fi,2)))*cos(2*PI*f*x1);
		}
	}
		
	return rtMask;
}



void ToFiltring(Mat& img, int widthSquare,int f,int fi)
{
	std::vector<MaskGabor> MaskGaborCollection;
	double direct = 0;
	for(int i=0;i<maskNumber;i++)
	{
		MaskGabor mask_ca(4,direct,1.0/f,fi);
		MaskGaborCollection.push_back(mask_ca);
		direct += PI/maskNumber;
	}	
	cout << "ToFiltring" << endl;
	double pointValue = 0;
	for(int x=0;x<IMAGE_WIDTH-2*widthSquare-1;x++)
	{
		for(int y=0;y<IMAGE_HEIGHT-2*widthSquare-1;y++)
		{
			double** mask;
			if(maskNumber>0)
			{
				mask = GetMaskFilter(directMatrix[x][y],MaskGaborCollection);
			}
			else
			{
				mask = GetMaskFilter(directMatrix[x][y],widthSquare,1.0/f,fi);
			}
			for(int i=0;i<2*widthSquare+1;i++)
			{
				for(int j=0;j<2*widthSquare+1;j++)
				{
					pointValue += mask[i][j]*ImageData[i+x][j+y];
				}
			}
				
			if(pointValue<0)
				pointValue = 0;
			if(pointValue>255)
				pointValue = 255;
			ImageData[x][y] = static_cast<int>(pointValue);
			img.at<uchar>((Point(x, y))) = ImageData[x][y];
		}
	}
}


void normalization(Mat& img, int MEAN, int VARIANCE)
{	
	for (int i = 0; i<IMAGE_WIDTH; i++)
	{
		for (int j = 0; j<IMAGE_HEIGHT; j++)
		{
			double tempData = static_cast<double>(ImageData[i][j]);
			if (tempData>image_mean)
			{
				ImageData[i][j] = static_cast<int>(MEAN + sqrt((tempData - image_mean)*(tempData - image_mean)*VARIANCE / image_variance));
			}
			else
			{
				ImageData[i][j]  = static_cast<int>(MEAN - sqrt((tempData - image_mean)*(tempData - image_mean)*VARIANCE / image_variance));
			}
			img.at<uchar>((Point(i, j))) = (uchar) ImageData[i][j];

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
	memset(ImageData, 0, sizeof(ImageData[0][0]) * IMAGE_WIDTH * IMAGE_HEIGHT);
	//memset(ImageData, 0, sizeof(ImageData[0][0]) * IMAGE_WIDTH * IMAGE_HEIGHT);
	memset(directMatrix, 0, sizeof(directMatrix[0][0]) * IMAGE_WIDTH * IMAGE_HEIGHT);
	//imshow("Before", img); waitKey(0);
	//Mat img = sourceImage.clone();
	LoadImageData(img);
	normalization(img, 50, 300);
	//imshow("After", img); waitKey(0);
	ToFiltring(img, 4,f,fi);
	//imshow("After ToFiltring", img); waitKey(0);
	//cv::cvtColor(img, img, CV_RGB2GRAY);
	//imshow("After", img); waitKey(0);
	localThreshold::binarisation(img, 26, 29);
	//binarisation(img);
	//imshow("After binarisation", img); waitKey(0);
	cv::threshold(img, img, 0, 255, cv::THRESH_BINARY);
	Mat binImg = img.clone();
	ideka::binOptimisation(img);
	//imshow("After binOptimisation", img); waitKey(0);

	//skeletionizing
	cv::bitwise_not(img, img);    //Inverse for bit-operations
	GuoHall::thinning(img);
	cv::bitwise_not(img, img);
	//imshow("After thinning", img); waitKey(0);

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
    //namedWindow( "Minutien gefiltert", WINDOW_AUTOSIZE );     // Create a window for display.
    imshow( "After get", minutImg2 );   waitKey(0);                //
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
	

	// ---Init data---///
	double scaleSet[] = { 0.8,0.9,1.0,1.1,1.2 };
	int angleStart = -30;
	int angleUnit = 3;
	int angleFinish = 30;
	int anglesCount = (int)((angleFinish - angleStart) / angleUnit) + 1;
	int angleSet[anglesCount];
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
	ret = api->upload_img("fingerprintimage.bmp");
	if(ret != PS_OK) {
		api->show_message(ret);
		return 0;
	}


	ret = api->get_img();
	if(ret != PS_OK) {
		api->show_message(ret);
		return 0;
	}
	ret = api->upload_img("fingerprintimagetwo.bmp");
	if(ret != PS_OK) {
		api->show_message(ret);
		return 0;
	}

	ret = api->get_img();
	if(ret != PS_OK) {
		api->show_message(ret);
		return 0;
	}
	ret = api->upload_img("fingerprintimagethree.bmp");
	if(ret != PS_OK) {
		api->show_message(ret);
		return 0;
	}

	vector<Minutiae> minutiaeOne;
	vector<Minutiae> minutiaeTwo;
	vector<Minutiae> minutiaeThree;
	std::string imgPath( "./fingerprintimage.bmp");
	std::string imgPathTwo( "./fingerprintimagetwo.bmp");
	std::string imgPathThree( "./fingerprintimagethree.bmp");
	getMinutiae(minutiaeOne, imgPath);
	getMinutiae(minutiaeTwo, imgPathTwo);
	getMinutiae(minutiaeThree, imgPathThree);
	Minutiae minuResult = Functions::GetMinutiaeChanging_UseHoughTransform(minutiaeOne ,
							minutiaeTwo, angleSet, deltaXSet,
							deltaYSet, anglesCount, deltaXCount,
							deltaYCount, angleLimit * PI / 180,
							IMAGE_WIDTH / 2, IMAGE_HEIGHT / 2);

	Minutiae minuResultThree = Functions::GetMinutiaeChanging_UseHoughTransform(minutiaeTwo ,
							minutiaeThree, angleSet, deltaXSet,
							deltaYSet, anglesCount, deltaXCount,
							deltaYCount, angleLimit * PI / 180,
							IMAGE_WIDTH / 2, IMAGE_HEIGHT / 2);

	int countTwo = Functions::CountMinuMatching(minutiaeOne , minutiaeTwo,
					minuResult, distanceLimit, angleLimit * PI / 180);
	int countThree = Functions::CountMinuMatching(minutiaeTwo , minutiaeThree,
					minuResultThree, distanceLimit, angleLimit * PI / 180);
	float current_percent_one = static_cast<float>(minutiaeOne.size() - countTwo)/minutiaeOne.size();
	float current_percent_two = static_cast<float>(minutiaeTwo.size() - countThree)/minutiaeTwo.size();
	if(current_percent_one < 0.60 && current_percent_two < 0.60) {
		SQL sql;
		sql.create_table();
		// void create_table();
		// 	void get_all_database();
		// 	const string minutiae_database_name = "Minutiae.db";
		// 	std::vector<Minutiae> get_all();
		// 	void insert_minutiae(std::vector<Minutiae> v, int fingerprint_id);
		// 	int insert_fingerprint();
		int personid = sql.insert_fingerprint();
		sql.insert_minutiae(minutiaeOne, personid, 0);
		sql.insert_minutiae(minutiaeTwo, personid, 1);
		sql.insert_minutiae(minutiaeThree, personid, 2);
		show_vector(minutiaeOne);
		cout << "Insert sucess user id: " << personid << endl;
		cout << "Two fingerprint match! " << current_percent_one << "-" << countTwo
			 << " : " << current_percent_two << "-" << countThree << endl;
		remove( "./fingerprintimage.bmp" );
		remove( "./fingerprintimagetwo.bmp" );
		remove( "./fingerprintimagethree.bmp" );
	}else {
		cout << "Two fingerprint not match! " << current_percent_one << "-" << countTwo
			 << " : " << current_percent_two << "-" << countThree << endl;
	}
	

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
	
	// if( remove( "./fingerprintimage.bmp" ) != 0 )
 //    	std::cout << "Error deleting file" << std::endl;
	// else
 //    	std::cout << "File successfully deleted" << std::endl;

	return 0;

}
