#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include "header/Minutiae.h"
#include "FIS.cpp"

// const int IMAGE_HEIGHT = 288;
// const int IMAGE_WIDTH = 256;


int I[IMAGE_WIDTH][IMAGE_HEIGHT]; // image

int O[IMAGE_WIDTH][IMAGE_HEIGHT];    // oriented image
int R[IMAGE_WIDTH][IMAGE_HEIGHT];     // region mask image

// double image_mean = 50;
// double image_variance = 300;

// Core Point
double i_core = 0;
double j_core = 0;

// right delta point
double i_delta_right = 300; 
double j_delta_right = 300;

// left delta point
double i_delta_left = -300;
double j_delta_left = -300;

#define PI 3.1415926535897931

using namespace cv;

void normalization(int MEAN, int VARIANCE)
{  
    for (int i = 0; i<IMAGE_WIDTH; i++){
        for (int j = 0; j<IMAGE_HEIGHT; j++){
            double tempData = static_cast<double>(R[i][j]);
            if (tempData>image_mean){
               R[i][j] = static_cast<int>(MEAN + sqrt((tempData - image_mean)*(tempData - image_mean)*VARIANCE / image_variance));
            }
            else{
               R[i][j]  = static_cast<int>(MEAN - sqrt((tempData - image_mean)*(tempData - image_mean)*VARIANCE / image_variance));
            }
            if(R[i][j] < 0)  R[i][j] = 0 ;
       }
   }
}

void orientation(Mat& im, int widthSqare)
{
    int i, j, x, y;
    int Ax, Ay, Axy;
    int Gxx, Gyy, Gxy;
    int Bx, By;
    double temp = 0;    
    for (i = 0; i <IMAGE_WIDTH ; i++){
        for (j = 0; j < IMAGE_HEIGHT; j++){
            O[i][j] = 0;
            temp += (I[i][j] - image_mean)*(I[i][j] - image_mean);
        }
    }

    image_variance = temp / (IMAGE_WIDTH*IMAGE_HEIGHT);

    for (x = widthSqare + 1; x <IMAGE_WIDTH - widthSqare - 1; x++)
    {
        for (y = widthSqare + 1; y <IMAGE_HEIGHT   - widthSqare - 1; y++)
        {
            Ax = 0; Ay = 0; Axy = 0;
            Gxx = 0; Gyy = 0; Gxy = 0;
            Bx = 0; By = 0;

            for (i = x - widthSqare; i < x + widthSqare - 1; i++)
            {
                for (j = y - widthSqare; j < y + widthSqare - 1; j++)
                {
                    Bx = ((I[i + 2][j] + 2 * I[i + 2][j + 1] + I[i + 2][j + 2] - I[i][j] - 2 * I[i][j + 1] - I[i][j + 2]));
                    By = ((I[i][j + 2] + 2 * I[i + 1][j + 2] + I[i + 2][j + 2] - I[i][j] - 2 * I[i + 1][j] - I[i + 2][j]));
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
    // std::cout<<"image_mean " << image_mean << std::endl;
    // std::cout<<"image_variance " << image_variance << std::endl;
    // int WindowSize = 4;
    // for (int i = 0; i <IMAGE_WIDTH  -WindowSize; i+=WindowSize) {
    //     for (int j = 0; j <IMAGE_HEIGHT -WindowSize; j+=WindowSize) {
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

int pointcare( int N, int i, int j)
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
    int P = round(d*180/PI);
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
    int m = IMAGE_WIDTH;
    int n = IMAGE_HEIGHT;
    

    double S1[m][n], S2[m][n];

    for(int i = 0; i< m; ++i){
        for(int j = 0; j<n; ++j){
            S1[i][j] = 0;
            S2[i][j] = 0;
        }
    }

    // Tim tat ca cac diem singularity ( co the co dien loi )

    for(int i = N1+1; i<m-N1; ++i){
        for(int j = N1+1; j<n-N1; ++j){
            //if( ( R[i-N1][j] == 0 ) and ( R[i+N1][j] == 0) and ( R[i][j-N1] == 0) and ( R[i][j+N1] == 0) ){
                int P = pointcare(N1, i, j);
                //std::cout<< "pointcare " << P << std::endl;
                if( (P == 360) or (P == 180) ){
                    // Whorl or loop
                    S1[i][j] = 1;
                }
            //}
        }
    }
    for(int i = N2+1; i<m-N2; ++i){
        for(int j = N2+1; j<n-N2; ++j){
            //if( ( R[i-N2][j] == 0 ) and ( R[i+N2][j] == 0) and ( R[i][j-N2] == 0) and ( R[i][j+N2] == 0) ){
                int P = pointcare(N2, i, j);
                //std::cout<< "pointcare " << P << std::endl;
                if( P == -180 ){
                    // Delta
                    S2[i][j] = 1;
                }
            //}
        }
    }

    // tim diem core
    double core = 0.4; 
    int c_core = 0;

    for(int i = (N1 + 1)/2 ; i< m-(N1-1)/2 ; ++i){
        for(int j = (N1 + 1)/2 ; j< n-(N1-1)/2 ; ++j){
            double mean = 0.0;
            int count = 0;
            
            for(int k = i-(N1-1)/2; k < i+(N1-1)/2; ++k){
                for(int h = j-(N1-1)/2; h< j+(N1-1)/2; ++h){
                    mean += S1[k][h];
                    count += 1;
                }
            }
            if(count > 0){
                mean = mean / count;
                if(mean >= core){
                    i_core += i;
                    j_core += j;
                    c_core += 1;
                }

            }
        }
    }

    // check core not exist
    if(c_core > 0) {
        i_core = round(i_core / c_core);
        j_core = round(j_core / c_core);    
    }
    
    // Tim diem delta ben phai (neu co)
    double delta = 0.5;
    double i_delta = 0;
    double j_delta = 0;

    for(int i = i_core + (N2 +1)/2 ;i <m-(N2-1)/2; ++i){
        for(int j = j_core + (N2 +1)/2 ;j <n-(N2-1)/2; ++j){
            double mean = 0;
            int count = 0;

            for(int k = i-(N2-1)/2; k <i+(N2-1)/2; ++k){
                for(int h = j-(N2-1)/2; h< j+(N2-1)/2; ++h){
                    mean += S2[k][h];
                    count += 1;
                }
            }

            if(count > 0){
                mean = mean / count;
                if(mean > delta){
                    delta = mean;
                    i_delta = i;
                    j_delta = j;
                }

            }
        }
    }

    if( (i_delta !=0) and (j_delta != 0) ){
        i_delta_right = i_delta - i_core;
        j_delta_right = j_delta - j_core;

    }

    // Tim diem delta ben trai (neu co)
    delta = 0.5;
    i_delta = 0;
    j_delta = 0;

    for(int i = i_core + (N2 +1)/2 ;i < m-(N2-1)/2; ++i){
        for(int j = (N2 +1)/2 ;j < j_core-(N2-1)/2; ++j){
            double mean = 0;
            int count = 0;

            for(int k = i-(N2-1)/2; k < i+(N2-1)/2; ++k){
                for(int h = j-(N2-1)/2; h< j+(N2-1)/2; ++h){
                    mean += S2[k][h];
                    count += 1;
                }
            }

            if(count > 0){
                mean = mean / count;

                if(mean > delta){
                    delta = mean;
                    i_delta = i;
                    j_delta = j;
                }    
            }
        }
    }

    if( (i_delta !=0) and (j_delta != 0) ){
        i_delta_left = i_delta - i_core;
        j_delta_left = j_delta - j_core;

    }
}

void changeCoordinateSystem(vector<Minutiae> & minutiaes){
    // doi he truc toa do theo diem core
    for(int i = 0; i < minutiaes.size(); ++i){
        Minutiae m = (Minutiae)minutiaes[i];
        int nX = m.getLocX();
        int nY = m.getLocY();
        nX -= i_core;
        nY -= j_core;

        m.setLocX(nX);
        m.setLocY(nY);
    }
}

void output(std::string img_path, std::string path){
    Mat img = imread(img_path);
      
    LoadImageData(img);
    orientation(img, 8); 

    normalization(50, 300); 
    singularity(9, 5);

    assert( i_core == 0 and j_core == 0  ); // core not exits, stop program
    // future work
    // change core point to X-point (pending)

    vector<Minutiae> minutiaes;
    int result = getMinutiae(minutiaes, img_path);
    
    if(result == -1) {
        return ;
    }

    changeCoordinateSystem(minutiaes);

    // dem so diem minutiate tre moi goc phan tu
    int c_edge_1, c_edge_2, c_edge_3, c_edge_4;
    c_edge_1 = c_edge_2 = c_edge_3 = c_edge_4 = 0;

    int x1, x2, x3, x4, y1, y2, y3, y4;
    x1 = x2 = x3 = x4 = y1 = y2 = y3 = y4 = 0;

    for(int i = 0; i < minutiaes.size(); ++i){
        Minutiae m = (Minutiae)minutiaes[i];
        int nX = m.getLocX();
        int nY = m.getLocY();

        if (nY > j_core){
            if(nX <= i_core){
                c_edge_1 += 1;
                x1 += nX;
                y1 += nY;
            }else{
                // nX > i_core
                c_edge_2 += 1;
                x2 += nX;
                y2 += nY;
            }

        }else{
            // nY <= j_core
            if(nX > i_core){
                c_edge_3 += 1;
                x3 += nX;
                y3 += nY;                
            }else{
                // nX > i_core
                c_edge_4 += 1;
                x4 += nX;
                y4 += nY;                
            }
        }
     }

     if(c_edge_1 != 0){ x1 /= c_edge_1; y1 /= c_edge_1; }
     if(c_edge_2 != 0){ x2 /= c_edge_2; y2 /= c_edge_2; }
     if(c_edge_3 != 0){ x3 /= c_edge_3; y3 /= c_edge_3; }
     if(c_edge_4 != 0){ x4 /= c_edge_4; y4 /= c_edge_4; }
     

     // std::cout<< "e1 = " << c_edge_1 << std::endl;
     // std::cout<< "e2 = " << c_edge_2 << std::endl;
     // std::cout<< "e3 = " << c_edge_3 << std::endl;
     // std::cout<< "e4 = " << c_edge_4 << std::endl;


     // std::cout<< "(x1, y1) = " << x1 << "," << y1 << std::endl;
     // std::cout<< "(x2, y2) = " << x2 << "," << y2 << std::endl;
     // std::cout<< "(x3, y3) = " << x3 << "," << y3 << std::endl;
     // std::cout<< "(x4, y4) = " << x4 << "," << y4 << std::endl;

// topology: 2 4 1
// in: 1.0 0.0 
// out: 0.0

    
    std::ofstream file(path, std::ios::app); //open in constructor
    //std::string data("data to write to file");

    file << "\n";
    file << "in: " ;

    file << i_delta_right << " " ;
    file << j_delta_right << " " ;

    file << i_delta_left << " " ;
    file << j_delta_left << " " ;
    
    file << x1 << " " ;
    file << y1 << " " ;
    file << x2 << " " ;
    file << y2 << " " ;
    file << x3 << " " ;
    file << y3 << " " ;
    file << x4 << " " ;
    file << y4 << " " ;

    file << c_edge_1 << " " ;
    file << c_edge_2 << " " ;
    file << c_edge_3 << " " ;
    file << c_edge_4 << " " ;

    file << "\n";
    file << "out: 0.0";
    file.close();
}

int main()
{
    std::string img_path = "/home/huynhld/Desktop/FIS_ES/WIP/Users/HuynhLD/FIS/img/101_";
    std::string path = "//home/huynhld/Desktop/FIS_ES/WIP/Users/HuynhLD/FIS/img/tranning/t1";
    
    std::ofstream file(path, std::ios::app); //open in constructor
    file << "topology: 16 4 1";
    file.close();
    
    std::string ext = ".tif";
    for(int i = 1; i <= 8; ++i){
        std::string img_path_full = "";
        img_path_full.append(img_path) ;
        img_path_full.append(std::to_string(i)) ;
        img_path_full.append(".tif") ;
        output(img_path_full , path);
    }
    
}