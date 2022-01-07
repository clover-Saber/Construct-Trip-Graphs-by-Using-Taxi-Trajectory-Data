#include "include/Position.h"
#include <iostream>
using namespace std;

void Position::init(string s){
    int i=0,j,k;
    //逗号分割
    //车机号
    j = s.find(',',i);
    taxiId = s.substr(i,j-i);
    i = j+1;
    //状态字
    serviceStatus = s[i];
    i = i+2;
    passengerStatus = s[i];
    i = i+2;
    lightStatus = s[i];
    i = i+2;
    roadStatus = s[i];
    i = i+2;
    brakeStatus = s[i];
    i = i+2;
    //接收日期和时间
    //有的只有日期没有时间
    j = s.find(',',i);
    string t1 = s.substr(i,j-i);
    k = t1.find(' ',0);
    receiveDate = t1.substr(0,k);
    receiveTime = t1.substr(k+1,j-i-k-1);
    i = j+1;
    //GPS日期和时间
    j = s.find(',',i);
    string t2 = s.substr(i,j-i);
    k = t2.find(' ',0);
    gpsDate = t2.substr(0,k);
    gpsTime = t2.substr(k+1,j-i-k-1);
    i = j+1;
    //经度纬度
    j = s.find(',',i);
    //string转浮点
    longitude = atof(s.substr(i,j-i).c_str());
    i = j+1;
    j = s.find(',',i);
    latitude = atof(s.substr(i,j-i).c_str());
    i = j+1;
    //速度
    j = s.find(',',i);
    speed = atof(s.substr(i,j-i).c_str());
    i = j+1;
    //方向
    j = s.find(',',i);
    direction = atof(s.substr(i,j-i).c_str());
    i = j+1;
    //卫星数
    j = s.find(',',i);
    //string转int
    satelliteNumber = atoi(s.substr(i,j-i).c_str());
    i = j+1;
    //计算时间，通过接收时间计算
    //冒号分割
    i = receiveTime.find(":",0);
    j = receiveTime.find(":",i+1);
    time = atoi(receiveTime.substr(0,i).c_str())*3600+atoi(receiveTime.substr(i+1,j-i-1).c_str())*60+atoi(receiveTime.substr(j+1,receiveTime.size()-j-1).c_str());
}

void Position::printPosition(){
    printf("{%s,%c,%c,%c,%c,%c,",taxiId.c_str(),serviceStatus,passengerStatus,lightStatus,roadStatus,brakeStatus);
    printf("%s,%s,%s,%s,",receiveDate.c_str(),receiveTime.c_str(),gpsDate.c_str(),gpsTime.c_str());
    printf("%lf,%lf,%0.1f,%0.1f,%d,%d}\n",longitude,latitude,speed,direction,satelliteNumber,time);
}
