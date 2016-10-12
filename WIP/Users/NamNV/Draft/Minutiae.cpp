#include "Minutiae.h"
#include <cmath>

Minutiae::Minutiae(int x, int y, Type t)
         : locX(x), locY(y), type(t)
{
    Minutiae::markedForErasing = false;
}

Minutiae::Minutiae(int x, int y, Type t, double _direct)
         : locX(x), locY(y), type(t), direct(_direct)
{
    Minutiae::markedForErasing = false;
}

Minutiae::Minutiae(int x, int y, double _direct)
         : locX(x), locY(y), direct(_direct)
{

}

void Minutiae::setDirect(double _direct)
{
	direct = _direct;
}

Minutiae::Minutiae()
{

}

Minutiae::~Minutiae()
{
    //dtor
}

int Minutiae::getLocX()
{
    return locX;
}

int Minutiae::getLocY()
{
    return locY;
}

double Minutiae::getDirect()
{
return direct;
}

Minutiae::Type Minutiae::getType()
{
    return type;
}

void Minutiae::setMarkTrue()
{
    Minutiae::markedForErasing = true;
}

bool Minutiae::getMark()
{
    return Minutiae::markedForErasing;
}


 Minutiae Minutiae::GetMinutiaeAfterChange(int deltaX,int deltaY,double angleRotation, int xRoot,int yRoot)
 {
    int xCurrent = locX - xRoot;
    int yCurrent = yRoot - locY;
    int _x = round((xCurrent*cos(angleRotation) - yCurrent*sin(angleRotation)) + deltaX);
    int _y = round((xCurrent*sin(angleRotation) + yCurrent*cos(angleRotation)) + deltaY);
    double _direct = direct + angleRotation;
    if(_direct>PI)
        _direct = _direct - PI;
    return Minutiae(xRoot + _x,yRoot - _y,type,_direct);
 }


void Minutiae::setFingerPrintId(int id)
{
    fingerprint_id = id;
}
int Minutiae::getFingerPrintId()
{
    return fingerprint_id;
}