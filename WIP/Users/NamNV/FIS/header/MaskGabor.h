#ifndef MASKGABOR_H
#define MASKGABOR_H
#include <cmath>
#include <iostream>
#include "Minutiae.h"
class MaskGabor
{
	private:
		double** mask;
		int width;
	public:
		MaskGabor(int widthSquare,double filterDirect,double f,int fi);
		MaskGabor();
		~MaskGabor();
		int get_width();
		double **get_mask();
};

#endif