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
#include "header/PiNetwork.h"
#include "header/logs.h"
#include "header/csv.h"
#include "header/Config.h"
#include <ctime>
#include <wiringPi.h>
#include <lcd.h>
#include <stdio.h>
#include <sstream>
using namespace cv;
using namespace std;


int ImageData[IMAGE_WIDTH][IMAGE_HEIGHT];
double directMatrix[IMAGE_WIDTH][IMAGE_HEIGHT];
const int maskNumber = 9;
const int angleLimit = 85;
const int distanceLimit = 30;
const int minuNumberLimit = 14;
const int f = 7;
const int fi = 3; 
double image_mean = 100;
double image_variance = 100;
const int angleStart = -30;
const int angleUnit = 3;
const int angleFinish = 30;
const int deltaXStart = -IMAGE_WIDTH;
const int deltaXFinish = IMAGE_WIDTH;
const int deltaXUnit = 2;
const int deltaYStart = -IMAGE_HEIGHT;
const int deltaYFinish = IMAGE_HEIGHT;
const int deltaYUnit = 2;
int lcdHandle;

void initWiringPi()
{
    wiringPiSetup();
    lcdHandle = lcdInit(2,16,8, 11,10, 0,1,2,3,4,5,6,7);
}


// void scrollMessage (int line, int width,const char* message)
// {
//     char buf [32] ;
//     static int position = 0 ;
//     static int timer = 0 ;

//     if (millis () < timer)
//         return ;

//     timer = millis () + 200 ;

//     strncpy (buf, &message [position], width) ;
//     buf [width] = 0 ;
//     lcdPosition (lcdHandle, 0, line) ;
//     lcdPuts     (lcdHandle, buf) ;

//     if (++position == (strlen (message) - width))
//         position = 0 ;
// }



void write_to_lcd(string message_line_one, string message_line_two = "")
{
    lcdClear(lcdHandle);
    lcdPosition (lcdHandle, 0, 0) ; lcdPuts (lcdHandle, message_line_one.c_str()) ;
    if(message_line_two.compare("") != 0)
        lcdPosition (lcdHandle, 0, 1) ; lcdPuts (lcdHandle, message_line_two.c_str()) ;
}



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
    for (int x = 0; x < im.cols; x++)
    {
        for (int y = 0; y < im.rows; y++)
        {
            int temp = (int)im.at<uchar>(Point(x, y));
            ImageData[x][y] = temp;
            temp_mean += temp;
        }
    }
    image_mean = static_cast<double>(temp_mean) / static_cast<double>(IMAGE_HEIGHT*IMAGE_WIDTH);
    GetDirectionMatrix(4);
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
        MaskGabor mask_ca(widthSquare,direct,1.0/f,fi);
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

void change_loc(std::vector<Minutiae>& minutiae, int _locX, int _locY)
{
    for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){
        minutiae[i].setLocX(minutiae[i].getLocX() - _locX);
        minutiae[i].setLocY(minutiae[i].getLocY() - _locY);
    }
}

int getMinutiae(std::vector<Minutiae>& minutiae, std::string imagePath)
{
    Mat img = cv::imread(imagePath, IMREAD_GRAYSCALE);

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
    //normalization(img, 100, 100);
    //imshow("After", img); waitKey(0);
    ToFiltring(img, 4, f, fi);
    //imshow("After ToFiltring", img); waitKey(0);
    //cv::cvtColor(img, img, CV_RGB2GRAY);
    //imshow("After", img); waitKey(0);
    localThreshold::binarisation(img, IMAGE_WIDTH/10, IMAGE_HEIGHT/10);
    //binarisation(img);
    //imshow("After binarisation", img); waitKey(0);
	cv::threshold(img, img, 50, 255, cv::THRESH_BINARY);
    Mat binImg = img.clone();
    ideka::binOptimisation(img);
    //imshow("After binOptimisation", img); waitKey(0);

    //skeletionizing
    cv::bitwise_not(img, img);    //Inverse for bit-operations
    GuoHall::thinning(img);
    cv::bitwise_not(img, img);
    //imshow("After thinning", img); waitKey(0);

	crossingNumber::getMinutiae(img, minutiae, 15, directMatrix);
    cout << "Anzahl gefundener Minutien: " << minutiae.size() << endl;
    int locX = 0, locY = 0;
    //Minutiae-filtering
	Filter::filterMinutiae(minutiae);
    std::cout << "After filter: " << minutiae.size() << std::endl;



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
    //namedWindow( "Minutien gefiltert", WINDOW_AUTOSIZE );     // Create a window for display.
    //imshow( "After get", minutImg2 );   waitKey(0);                //
    // imwrite("testimage.bmp",minutImg2);
    //change_loc(minutiae, locX, locY);
    return 0;
}


void show_vector(vector<Minutiae> v) {
    std::cout << "==============Start Vector=======================" << std::endl;
    for(int i = 0; i < v.size(); i++) {
        std::cout << v[i].getLocX() << " : " << v[i].getLocY() << " : " <<  v[i].getDirect() << std::endl;
    }
    std::cout << "==============End Vector=========================" << std::endl;
}


int matching()
{
    logs::getInstance();
    logs::write_logs("matching","fingerprint_process", logs::IN_STATE);
    // if(api != NULL) {
    //     delete api;
    // }
    std::cout << "Done upload_img" << endl;
  // ---Init data---///

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
        return -1;
    }
    int ret = api->get_img();
    if(ret != PS_OK) {
        api->show_message(ret);
        return -1;
    }
    ret = api->upload_img("fingerprintimage.bmp");
    if(ret != PS_OK) {
        api->show_message(ret);
        return -1;
    }

    vector<Minutiae> minutiaeOne;
    vector<Minutiae> minutiaeTwo;
    int result = getMinutiae(minutiaeOne, "./fingerprintimage.bmp");
    if(result == -1) {
        return -1;
    }
    //show_vector(minutiaeOne);
    //getMinutiae(minutiaeTwo, "/home/namte/Desktop/Untitled Folder 2/namte.bmp");
    //show_vector(minutiaeTwo);
    //show_vector(minutiaeOne);
    //SQL sql;
    //sql.create_table();
    // void create_table();
    //  void get_all_database();
    //  const string minutiae_database_name = "Minutiae.db";
    //  std::vector<Minutiae> get_all();
    //  void insert_minutiae(std::vector<Minutiae> v, int fingerprint_id);
    //  int insert_fingerprint();
    //int personid = sql.insert_fingerprint();
    //sql.insert_minutiae(minutiaeOne, personid);
    int max_count = -1;
    int count = 0;
    string finger_id_exist = "";
    float percent = 1;
    SQL sql;
    sql.create_table();   
    map<std::string, vector<Minutiae> > map_data = sql.get_all_database();
    if(map_data.size() == 0 || minutiaeOne.size() == 0) {
        //cout << "Not found" << endl;
        logs::write_logs("matching","fingerprint_process", logs::OUT_STATE, "map_data.size() == 0 || minutiaeOne.size() == 0");
        return 0;
    } else {
        std::map<std::string, vector<Minutiae> >::iterator iterator;
        for(iterator = map_data.begin(); iterator != map_data.end(); iterator++) {
            vector<Minutiae> v = static_cast<vector<Minutiae> > (iterator->second);
            //show_vector(v);
            Minutiae minuResult = Functions::GetMinutiaeChanging_UseHoughTransform(minutiaeOne ,
                v, angleSet, deltaXSet,
                deltaYSet, anglesCount, deltaXCount,
                deltaYCount, angleLimit * PI / 180,
                128, 144);
            float count = (float)Functions::CountMinuMatching(minutiaeOne , v,
                minuResult, distanceLimit, angleLimit * PI / 180);
            float min_count = (float) v.size() < minutiaeOne.size() ? v.size() : minutiaeOne.size();
            float current_percent = (min_count - count) / min_count;
            cout << "Percent: " << current_percent << " + id: " << static_cast<std::string>(iterator->first) << " Count: " << count << " : " << v.size() << endl;
            if(current_percent < 0.35 && current_percent < percent) {
                finger_id_exist = static_cast<std::string>(iterator->first);
                finger_id_exist = finger_id_exist.substr(0, finger_id_exist.find(":"));
                percent = current_percent;
                max_count = count;
            }
        }
    }

    if (percent < 0.35){
        csv* writecsv = csv::getInstance();
        string username = sql.find_username(finger_id_exist);
        Config* config = Config::getInstance();
        string deviceId = config->get_value(API_KEY);
        writecsv->write_csv(finger_id_exist, username, deviceId);
        username = username.substr(0, 16);
        write_to_lcd("Hello", username);
        PiNetwork* piNet = PiNetwork::getInstance();
        piNet->send_log_deviceId(finger_id_exist);
        cout << "Welcome : " << username << " : " <<  finger_id_exist << " : " << max_count << endl;
        logs::write_logs("matching","fingerprint_process", logs::OUT_STATE, "if (percent < 0.35)");
        
        if(writecsv != NULL){
            delete writecsv;
        }

        if(config != NULL){
            delete config;
        }
        return 1;
    }
    else
    {
        write_to_lcd("Sorry", "Not Match");
        cout << "User Not found!"  << endl;
    }
    if(api != NULL) {
        delete api;
    }
    logs::write_logs("matching","fingerprint_process", logs::OUT_STATE);
    return 0;
}


int fis_register()
{
    // ---Init data---///

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
        return -1;
    }
    int ret = api->get_img();
    if(ret != PS_OK) {
        api->show_message(ret);
        return -1;
    }
    ret = api->upload_img("fingerprintimage.bmp");
    if(ret != PS_OK) {
        api->show_message(ret);
        return -1;
    }


    ret = api->get_img();
    if(ret != PS_OK) {
        api->show_message(ret);
        return -1;
    }
    ret = api->upload_img("fingerprintimagetwo.bmp");
    if(ret != PS_OK) {
        api->show_message(ret);
        return -1;
    }

    ret = api->get_img();
    if(ret != PS_OK) {
        api->show_message(ret);
        return -1;
    }
    ret = api->upload_img("fingerprintimagethree.bmp");
    if(ret != PS_OK) {
        api->show_message(ret);
        return -1;
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
        128, 144);

    Minutiae minuResultThree = Functions::GetMinutiaeChanging_UseHoughTransform(minutiaeTwo ,
        minutiaeThree, angleSet, deltaXSet,
        deltaYSet, anglesCount, deltaXCount,
        deltaYCount, angleLimit * PI / 180,
        128, 144);

    float countTwo = (float)Functions::CountMinuMatching(minutiaeOne , minutiaeTwo,
        minuResult, distanceLimit, angleLimit * PI / 180);
    float countThree = (float)Functions::CountMinuMatching(minutiaeTwo , minutiaeThree,
        minuResultThree, distanceLimit, angleLimit * PI / 180);
    float min_count_one = (float) minutiaeOne.size() < minutiaeTwo.size() ? minutiaeOne.size() : minutiaeTwo.size();
    float min_count_two = (float) minutiaeTwo.size() < minutiaeThree.size() ? minutiaeTwo.size() : minutiaeThree.size();
    float current_percent_one = (min_count_one  - countTwo) / min_count_one;
    float current_percent_two = (min_count_two - countThree) / min_count_two;
    if(current_percent_one < 0.50 && current_percent_two < 0.50) {
        PiNetwork* piNet = PiNetwork::getInstance();
        string userid = "";
        string username = "";
        bool piNetResult = piNet->send_file_deviceId(imgPathTwo, userid, username);
        if(piNetResult == false) {
            cout << "Error to register" << endl;
            return -1;
        }
        SQL sql;
        sql.create_table();

        // void create_table();
        //  void get_all_database();
        //  const string minutiae_database_name = "Minutiae.db";
        //  std::vector<Minutiae> get_all();
        //  void insert_minutiae(std::vector<Minutiae> v, int fingerprint_id);
        //  int insert_fingerprint();
        int personid = sql.insert_fingerprint(userid, username);
        sql.insert_minutiae(minutiaeOne, userid, 0);
        sql.insert_minutiae(minutiaeTwo, userid, 1);
        sql.insert_minutiae(minutiaeThree, userid, 2);
        show_vector(minutiaeOne);
        cout << "Insert sucess user id: " << userid << endl;
        cout << "Two fingerprint match! " << current_percent_one << "-" << countTwo
        << " : " << current_percent_two << "-" << countThree << endl;
        remove( "./fingerprintimage.bmp" );
        remove( "./fingerprintimagetwo.bmp" );
        remove( "./fingerprintimagethree.bmp" );
        return 1;
    }else {
        cout << "Two fingerprint not match! " << current_percent_one << "-" << countTwo
        << " : " << current_percent_two << "-" << countThree << endl;
        remove( "./fingerprintimage.bmp" );
        remove( "./fingerprintimagetwo.bmp" );
        remove( "./fingerprintimagethree.bmp" );
    }
    if(api != NULL) {
        delete api;
    }
    return 0;
}

int main(int argc, const char** argv)
{
    clock_t tStart = clock();

    initWiringPi();
    //Note :  this for cmd purpose
    if(argc < 2) {
        write_to_lcd("Error!", "Provide parameter");
        std::cout << "Please provide a image file as the parameter..." << std::endl;
        exit(1);
    }
    write_to_lcd("Waiting", "Taking finger!");
    string argument(argv[1]);
    int result = 0;
    if(argument.compare("matching") == 0) {
        result = matching();
    }else if(argument.compare("register") == 0) {
        result = fis_register();
    }
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    return 0;

}
