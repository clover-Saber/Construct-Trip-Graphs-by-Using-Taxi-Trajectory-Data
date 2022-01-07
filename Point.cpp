#include "include/Point.h"
#include <iostream>
using namespace std;

void Point::init(int st,double sx,double sy,int et,double ex,double ey){
    startTime = st;
    startLongitude = sx;
    startLatitude = sy;
    endTime = et;
    endLongitude = ex;
    endLatitude = ey;
}

void Point::initStart(int st,double sx,double sy){
    startTime = st;
    startLongitude = sx;
    startLatitude = sy;
}

void Point::initEnd(int et,double ex,double ey){
    endTime = et;
    endLongitude = ex;
    endLatitude = ey;
}

void Point::printPoint(){
    printf("[%d,%lf,%lf]-->[%d,%lf,%lf]\n",startTime,startLongitude,startLatitude,endTime,endLongitude,endLatitude);
}