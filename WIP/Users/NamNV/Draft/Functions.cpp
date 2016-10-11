#include "Functions.h"
#include <cmath>
#include <algorithm> 
#include <iostream>
bool Functions::IsMinutiaeMatching(Minutiae m1,
	Minutiae m2, int distanceLimit, double angleLimit, Minutiae minuChanging)
{
	Minutiae m = m2.GetMinutiaeAfterChange(minuChanging.getLocX(), minuChanging.getLocY(), minuChanging.getDirect(), 128, 128);
	int distance = round(sqrt(pow(m.getLocX() - m1.getLocX(), 2) + pow(m.getLocY() - m1.getLocY(), 2)));
	double angleRotate = std::min(abs(m.getDirect() - m1.getDirect()), PI * 2 - abs(m.getDirect() - m1.getDirect()));
	if ((distance < distanceLimit) && (angleRotate < angleLimit))
		return true;
	return false;
}


Minutiae Functions::GetMinutiaeChanging_UseHoughTransform(vector<Minutiae> minuSet1,
	vector<Minutiae> minuSet2, int angleSet[21], int deltaXSet[], int deltaYSet[], int anglesCount, int deltaXCount, int deltaYCount,
	double angleLimit, int xRoot, int yRoot)
{
	//int Height = 375;
	//int Width = 389;
	//int Depth = 21;

	//float* ary = new float[Height*Width*Depth]();

	//for (int k = 0; k < Depth; k++)
	//	for (int r = 0; r < Height; r++)
	//		for (int c = 0; c < Width; c++)
	//			/*Init array*/
	//			ary[k*Height*Width + r*Width + c] = 1;
	int i, j, k;
	int deltaXSetLength = deltaXCount;
	int deltaYSetLength = deltaYCount;
	int angleSetLength = anglesCount;
	int length = deltaXSetLength*deltaYSetLength*angleSetLength;
	int* A = new int[deltaXSetLength*deltaYSetLength*angleSetLength]();
	std::cout << deltaXSetLength*deltaYSetLength*angleSetLength << std::endl;
	Minutiae m1;
	Minutiae m2;
	double _deltaX;
	double _deltaY;
	for (i = 0; i < angleSetLength; i++) {
		for (j = 0; j < deltaYSetLength; j++) {
			for (k = 0; k < deltaXSetLength; k++)
				A[i*deltaXSetLength*deltaYSetLength + j*deltaXSetLength + k] = 0;
		}
	}
	std::cout << i << "   " << j << "   " << k << std::endl;
	std::cout << (i-1)*deltaXSetLength*deltaYSetLength + (j-1)*deltaXSetLength + k << std::endl;
	for (i = 0; i < minuSet1.size(); i++)
	{
		m1 = (Minutiae)minuSet1[i];
		for (j = 0; j < minuSet2.size(); j++)
		{
			m2 = (Minutiae)minuSet2[j];
			for (int angleIndex = 0; angleIndex < angleSetLength; angleIndex++)
			{
				double tempAngle = abs((m2.getDirect() + angleSet[angleIndex] * 3.1415926535897931 / 180) - m1.getDirect());
				if ((tempAngle < angleLimit) || ((3.1415926535897931 * 2 - tempAngle) < angleLimit))
				{
					int c1X = m1.getLocX() - xRoot;
					int c1Y = yRoot - m1.getLocY();
					int c2X = m2.getLocX() - xRoot;
					int c2Y = yRoot - m2.getLocY();

					_deltaX = c1X - (cos(angleSet[angleIndex] * 3.1415926535897931 / 180)*c2X - sin(angleSet[angleIndex] * 3.1415926535897931 / 180)*c2Y);
					_deltaY = c1Y - (sin(angleSet[angleIndex] * 3.1415926535897931 / 180)*c2X + cos(angleSet[angleIndex] * 3.1415926535897931 / 180)*c2Y);

					int deltaXSelect = deltaXSet[0];
					int deltaXIndexSelect = 0;
					int deltaYSelect = deltaYSet[0];
					int deltaYIndexSelect = 0;
					for (int deltaXIndex = 0; deltaXIndex < deltaXSetLength; deltaXIndex++)
						if (abs(static_cast<double>(deltaXSet[deltaXIndex]) - _deltaX) < abs(static_cast<double>(deltaXSelect) - _deltaX))
						{
							deltaXSelect = deltaXSet[deltaXIndex];
							deltaXIndexSelect = deltaXIndex;
						}
					for (int deltaYIndex = 0; deltaYIndex < deltaYSetLength; deltaYIndex++)
						if (abs(static_cast<double>(deltaYSet[deltaYIndex]) - _deltaY) < abs(static_cast<double>(deltaYSelect) - _deltaY))
						{
							deltaYSelect = deltaYSet[deltaYIndex];
							deltaYIndexSelect = deltaYIndex;
						}
					A[angleIndex*deltaXSetLength*deltaYSetLength + deltaYIndexSelect*deltaXSetLength + deltaXIndexSelect]++;
				}
			}
		}
	}

	int _deltaXIndex = 0;
	int _deltaYIndex = 0;
	int _angleIndex = 1;
	for (i = 0; i < angleSetLength; i++)
	{
		for (j = 0; j < deltaYSetLength; j++)
		{
			for (k = 0; k < deltaXSetLength; k++)
			{
				int pre = A[i*deltaXSetLength*deltaYSetLength + j*deltaXSetLength + k];
				int preus = A[_angleIndex*deltaXSetLength*deltaYSetLength + _deltaYIndex*deltaXSetLength + _deltaXIndex];
				if (pre > preus)
				{
					_deltaXIndex = k;
					_deltaYIndex = j;
					_angleIndex = i;
				}
			}
		}
	}	
	if(A  != NULL) {
		delete A;
	}
	return Minutiae(deltaXSet[_deltaXIndex], deltaYSet[_deltaYIndex], angleSet[_angleIndex] * PI / 180);
}

int Functions::CountMinuMatching(vector<Minutiae> minuSet1, vector<Minutiae> minuSet2,
	Minutiae minuChanging, int distanceLimit, double angleLimit)
{
	unsigned int i, j;
	bool *mark = new bool[minuSet2.size()];
	for (i = 0; i < minuSet2.size(); i++)
		mark[i] = false;

	for (i = 0; i < minuSet1.size(); i++)
	{
		Minutiae m1 = (Minutiae)minuSet1[i];
		for (j = 0; j < minuSet2.size(); j++)
		{
			if (!mark[j])
			{
				Minutiae m2 = (Minutiae)minuSet2[j];
				if (Functions::IsMinutiaeMatching(m1, m2, distanceLimit, angleLimit, minuChanging))
				{
					mark[j] = true;
					break;
				}
			}
		}
	}
	int count = 0;
	for (i = 0; i < minuSet2.size(); i++)
		if (mark[i]) count++;
	return count;
}
