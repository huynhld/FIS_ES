#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Minutiae.h"
#include <vector>
#include<cmath>
using namespace std;

class Functions
{
    public:
        Functions();
        ~Functions();
        static bool IsMinutiaeMatching(Minutiae m1,
            Minutiae m2,int distanceLimit,double angleLimit,Minutiae minuChanging);
        static Minutiae GetMinutiaeChanging_UseHoughTransform(vector<Minutiae> minuSet1,
                vector<Minutiae> minuSet2, int angleSet[21], int deltaXSet[],
                int deltaYSet[], int anglesCount, int deltaXCount, int deltaYCount,
                double angleLimit, int xRoot, int yRoot);
        static int CountMinuMatching(vector<Minutiae> minuSet1, vector<Minutiae> minuSet2,
            Minutiae minuChanging,int distanceLimit, double angleLimit);
    protected:
    private:
};

#endif // FUNCTIONS_H
