#include "../header/Filter.h"

#include <math.h>
#include <iostream>

namespace Filter{

    void filterMinutiae(std::vector<Minutiae>& minutiae, int &locX, int &locY){
        double minDistanceForMinutiae = 10;
        bool same = true;
        //calculate for every minutiae the distance to all other minutiae
        for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){
            if(minutiae[i].getType() == Minutiae::RIDGEENDING){
                for(std::vector<Minutiae>::size_type j = 0; j<minutiae.size(); j++){
                    if(j==i){
                        same = true;
                    }
                    if((minutiae[j].getType() == Minutiae::RIDGEENDING) && !same){ //(minutiae[j].getType() == Minutiae::Type::RIDGEENDING) &&
                        double distance = euclideanDistance(minutiae[i].getLocX(), minutiae[i].getLocY(), minutiae[j].getLocX(), minutiae[j].getLocY());
                        //if the distance is to low...
                        if(distance < minDistanceForMinutiae){
                            //... then mark minutiae for erasing
                            minutiae[i].setMarkTrue();
                            minutiae[j].setMarkTrue();
                        }
                    }
                    same = false;
                }
            }
        }
        int cnt = 0;
        std::vector<Minutiae> minutiaeNew;
        locX = 0;
        locY = 0;
        int ridgeending = 0;
        int bifurcation = 0;
        double locXRid = 0;
        double locYRid = 0;
        double locXBif = 0;
        double locYBif = 0;
        //erase the marked minutiae
        for(std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){
            if(!minutiae[i].getMark()){
                if(minutiae[i].getType() == Minutiae::RIDGEENDING)
                {
                    locXRid += minutiae[i].getLocX();
                    locYRid += minutiae[i].getLocY();
                    ridgeending++;
                }else {
                    locXBif += minutiae[i].getLocX();
                    locYBif += minutiae[i].getLocY();
                    bifurcation++;
                }
                minutiaeNew.push_back(minutiae[i]);
            }else{
                cnt++;
            }

        }
        minutiae = minutiaeNew;
        if(bifurcation < ridgeending && bifurcation != 0) {
            locX = locXBif / bifurcation;
            locY = locYBif / bifurcation;
        }else {
            locX = locXRid / ridgeending;
            locY = locYRid / ridgeending;
        }
        std::cout << "New Loc: " << "X = " << locX << " Y = " << locY << std::endl;
        std::cout << "bifurcation: " << bifurcation << std::endl;
        //std::cout<<"Anzahl gelöschter Minutien: " << cnt << std::endl;
    }

    double euclideanDistance(int x1, int y1, int x2, int y2){
        return sqrt(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)));
    }

}
