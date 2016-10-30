#include "../header/MaskGabor.h"


MaskGabor::MaskGabor()
{

}

MaskGabor::MaskGabor(int widthSquare,double filterDirect,double f,int fi)
{
	// double[,] rtMask = new double[2*widthSquare+1,2*widthSquare+1];
	// for(int x=0;x<2*widthSquare+1;x++)
	// 	for(int y=0;y<2*widthSquare+1;y++)
	// 	{
	// 		double x1 = Math.Sin(filterDirect)*(x-widthSquare) + Math.Cos(filterDirect)*(y-widthSquare);
	// 		double y1 = Math.Sin(filterDirect)*(y-widthSquare) - Math.Cos(filterDirect)*(x-widthSquare);
	// 		rtMask[x,y] = Math.Exp(-0.5*(Math.Pow(x1,2)/Math.Pow(fi,2) + Math.Pow(y1,2)/Math.Pow(fi,2)))*Math.Cos(2*Math.PI*f*x1);				
	// 	}
	// mask = rtMask;
	// width = widthSquare*2 + 1;
	double** rtMask = new double*[2*widthSquare+1];
	for(int x=0;x<2*widthSquare+1;x++) {
		rtMask[x] = new double[2*widthSquare+1];
		for(int y=0;y<2*widthSquare+1;y++)
		{
			double x1 = sin(filterDirect)*(x-widthSquare) + cos(filterDirect)*(y-widthSquare);
			double y1 = sin(filterDirect)*(y-widthSquare) - cos(filterDirect)*(x-widthSquare);
			rtMask[x][y] = exp(-0.5*(pow(x1,2)/pow(fi,2) + pow(y1,2)/pow(fi,2)))*cos(2*PI*f*x1);				
		}
	}
	mask = rtMask;
	width = widthSquare*2 + 1;
}

int MaskGabor::get_width()
{
	return width;
}
double** MaskGabor::get_mask()
{
	return mask;
}

MaskGabor::~MaskGabor() {
	
}