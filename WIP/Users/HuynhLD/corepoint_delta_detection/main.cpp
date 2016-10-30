#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <iostream>


const int IMAGE_HEIGHT = 288;
const int IMAGE_WIDTH = 256;


int ImageData[IMAGE_WIDTH][IMAGE_HEIGHT];

double O[IMAGE_WIDTH][IMAGE_HEIGHT];
// Todo
double R[IMAGE_WIDTH][IMAGE_WIDTH];

const int angleLimit = 5;
const int distanceLimit = 25;
const int minuNumberLimit = 14; 
double image_mean = 50;
double image_variance = 300;

#define PI 3.1415926535897931

using namespace cv;

void normalize(int N){
    int m,n;
    m = IMAGE_HEIGHT;
    n = IMAGE_WIDTH;

    for(int i = 0; i< m; ++i){
        for(int j = 0; j<n; ++j){
            R[i][j] = 255;
        }
    }

    for(int i = 0; i< m-N; i+= N){
        for(int j = 0; j<n-N; j+=N){
            
            double mean = 0;
            int count = 0;

            for(int k = i; k<=i+N-1; ++k){
                for(int h = j; h<=j+N-1; ++h){
                    mean += ImageData[k][h];
                    count ++;
                }
            }

            mean /= count;

        }
    }

}

void orientation(int widthSqare)
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
            O[j][i] = 0;
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

            O[x][y] = PI / 2 - 0.5*atan2(2 * Gxy, Gxx - Gyy);
        }
    }
    //return O;
    
}

void LoadImageData(cv::Mat im)
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
        }
    }
    //out << (int)im.at<uchar>((1, 0)) << " " << (int)im.at<uchar>(Point(1, 0)) << endl;
    image_mean = static_cast<double>(temp_mean) / static_cast<double>(IMAGE_HEIGHT*IMAGE_WIDTH);
    //cout << image_mean << endl;
    orientation(4);
    
    // int WindowSize = 20;
    // for (int i = 1; i < IMAGE_WIDTH - 1 -WindowSize; i+=WindowSize) {
    //     for (int j = 1; j < IMAGE_HEIGHT -1-WindowSize; j+=WindowSize) {
    //         cv::Rect roi(i, j, WindowSize, WindowSize);

    //         cv::Mat roiImage = im(roi);
    //         //double direction = calculateDirectionForWindow(roiImage);

    //         // TODO: Refactor direction drawing into own function
    //         // direction = i % 360; // for testing
    //         double xDir = std::cos((O[i][j] * 180 / PI)/180*M_PI);
    //         double yDir = -1 * std::sin((O[i][j] * 180 / PI)/180*M_PI); // y-Axis is inverted because
    //                                                          // in math, +y is typically
    //                                                          // considered to go in the top
    //                                                          // direction whereas in the image
    //                                                          // it goes towards the bottom

    //         cv::Point p1(
    //                      WindowSize/2 + ((WindowSize/4)*xDir),
    //                      WindowSize/2 + ((WindowSize/4)*yDir));
    //         cv::Point p2(
    //                      WindowSize/2 - ((WindowSize/4)*xDir),
    //                      WindowSize/2 - ((WindowSize/4)*yDir));
    //         //cv::Scalar colorScalar = cv::Scalar(0, 0, 255);
    //         cv::line(roiImage, p1, p2,  1);
    //     }
    // }
    // imshow("Direction Field", im); waitKey(0);
}

/*
    Ham tinh chi so point care de xet diem Singlarity tai diem (i,j)
    O: truong dinh huong cua anh => double O[IMAGE_WIDTH][IMAGE_HEIGHT]
    N(le): kich thuoc "duong cong so"(curve) xem xet
*/

double pointcare( int N, int i, int j)
{
    int a = (N-1)/2;
    double d = 0.0;
    double d0 = O[i-a][j-a];
    double delta ;

    for(int k = j - a + 1; k <= j + a ; ++k){
        delta = O[i-a][k] - d0;
        
        if(delta < -PI/2) 
            delta = PI + delta;
        else if(delta > PI/2) 
            delta = delta - PI;

        d = d+delta;
        d0 = O[i-a][k];

    }

    for(int h = i-a+1; h<= i+a; ++h){
        delta = O[h][j+a] - d0;

        if(delta < -PI/2)
            delta=PI + delta;
        else if(delta > PI/2)
            delta = delta - PI;

        d = d + delta;
        d0 = O[h][j+a];

    }

    for(int k = j+a-1; j-a <= k; --k){
        delta = O[i+a][k] -d0;

        if(delta < -PI/2)
            delta = PI + delta;
        else if(delta > PI/2)
            delta = delta - PI;

        d = d+delta;
        d0 = O[i+a][k];
    }

    for(int h = i+a-1; i-a<= h; --h){
        delta = O[h][j-a] - d0;

        if(delta < -PI/2)
            delta=PI + delta;
        else if(delta > PI/2)
            delta = delta - PI;

        d = d + delta;
        d0 = O[h][j-a];

    }
    double P = round(d*180/PI);
    return P;
}


/*
    Ham tim cac diem Singularity tren anh van tay
    core: vi tri cua diem core
    delta: vi tri cua diem delta so voi diem core - toa do decac
    R: region mask - mat na vung van tay
    O: truong dinh huong cua van tay    double O[][] 
    N1(le): kich thuoc " duong con so " - curve xem xet (theo phuong phap poicare) de tim diem core
    N2(le): kich thuoc " duong con so " - curve xem xet (theo phuong phap poicare) de tim diem delta
*/
void singularity(int N1, int N2)
{
    int m = IMAGE_HEIGHT;
    int n = IMAGE_WIDTH;

    double S1[m][n], S2[m][n];

    for(int i = 0; i< m; ++i){
        for(int j = 0; j<n; ++j){
            S1[i][j] = 0;
            S2[i][j] = 0;
        }
    }

    // Tim tat ca cac diem singularity ( co the co dien loi )

    for(int i = N1+1; i<=m-N1; ++i){
        for(int j = N1+1; j<=n-N1; ++j){
            if( ( R[i-N1][j] == 0 ) and ( R[i+N1][j] == 0) and ( R[i][j-N1] == 0) and ( R[i][j+N1] == 0) ){
                double P = pointcare(N1, i, j);
                if( (P == 360) or (P == 180) ){
                    // Whorl or loop
                    S1[i][j] = 1;
                }
            }
        }
    }

    for(int i = N2+1; i<=m-N2; ++i){
        for(int j = N2+1; j<=n-N2; ++j){
            if( ( R[i-N2][j] == 0 ) and ( R[i+N2][j] == 0) and ( R[i][j-N2] == 0) and ( R[i][j+N2] == 0) ){
                double P = pointcare(N2, i, j);
                if( P == -180 ){
                    // Delta
                    S2[i][j] = 1;
                }
            }
        }
    }

    // tim diem core
    double core = 0.4;
    double i_core = 0;
    double j_core = 0;
    int c_core = 0;

    for(int i = (N1 + 1)/2 ; i<= m-(N1-1)/2 ; ++i){
        for(int j = (N1 + 1)/2 ; j<= n-(N1-1)/2 ; ++j){
            double mean = 0;
            int count = 0;
            
            for(int k = i-(N1-1)/2; k <= i+(N1-1)/2; ++k){
                for(int h = j-(N1-1)/2; h<= j+(N1-1)/2; ++h){
                    mean += S1[k][h];
                    count += 1;
                }
            }

            mean = mean / count;

            if(mean >= core){
                i_core += i;
                j_core += j;
                c_core += 1;
            }
        }
    }

    i_core = round(i_core / c_core);
    j_core = round(j_core / c_core);

    // Tim diem delta ben phai (neu co)
    double delta = 0.5;
    double i_delta = 0;
    double j_delta = 0;

    for(int i = i_core + (N2 +1)/2 ;i <=m-(N2-1)/2; ++i){
        for(int j = j_core + (N2 +1)/2 ;j <=n-(N2-1)/2; ++j){
            double mean = 0;
            int count = 0;

            for(int k = i-(N2-1)/2; k <= i+(N2-1)/2; ++k){
                for(int h = j-(N2-1)/2; h<= j+(N2-1)/2; ++h){
                    mean += S2[k][h];
                    count += 1;
                }
            }

            mean = mean / count;

            if(mean > delta){
                i_delta = i;
                j_delta = j;
            }
        }
    }

    double i_delta_right = 300; // 300 mean not existing.
    double j_delta_right = 300;

    if( (i_delta !=0) and (j_delta != 0) ){
        i_delta_right = i_delta - i_core;
        j_delta_right = j_delta - j_core;

    }

    // Tim diem delta ben trai (neu co)
    delta = 0.5;
    i_delta = 0;
    j_delta = 0;

    for(int i = i_core + (N2 +1)/2 ;i <=m-(N2-1)/2; ++i){
        for(int j = (N2 +1)/2 ;j <=j_core-(N2-1)/2; ++j){
            double mean = 0;
            int count = 0;

            for(int k = i-(N2-1)/2; k <= i+(N2-1)/2; ++k){
                for(int h = j-(N2-1)/2; h<= j+(N2-1)/2; ++h){
                    mean += S2[k][h];
                    count += 1;
                }
            }

            mean = mean / count;

            if(mean > delta){
                i_delta = i;
                j_delta = j;
            }
        }
    }

    double i_delta_left = 300; // 300 mean not existing.
    double j_delta_left = 300;

    if( (i_delta !=0) and (j_delta != 0) ){
        i_delta_left = i_delta - i_core;
        j_delta_left = j_delta - j_core;

    }
}

int main()
{
    Mat img = imread("/home/huynhld/FIS_ES/WIP/Users/HuynhLD/corepoint_delta_detection/img/img_l_1_2");
    LoadImageData(img);
}