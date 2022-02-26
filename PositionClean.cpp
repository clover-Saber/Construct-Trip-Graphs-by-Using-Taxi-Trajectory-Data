#include <fstream>
#include <iostream>
#include <iomanip>

#include "include/PositionClean.h"
#include "include/Position.h"
#include "include/Parameter.h"
#include "include/Util.h"
using namespace std;

SimplePosition::SimplePosition(){
}

SimplePosition::~SimplePosition(){
}

void SimplePosition::init(string tid,char light,double lat,double lon,double spd,int date,int time){
    taxiId = tid;
    lightStatus = light;
    latitude = lat;
    longitude = lon;
    speed = spd;
    dateIntFormat = date;
    timeIntFormat = time;
}


string SimplePosition::getTaxiId(){
    return taxiId;
}

char SimplePosition::getLightStatus(){
    return lightStatus;
}
        
double SimplePosition::getLatitude(){
    return latitude;
}
        
double SimplePosition::getLongitude(){
    return longitude;
}

double SimplePosition::getSpeed(){
    return speed;
}

int SimplePosition::getDateIntFormat(){
    return dateIntFormat;
}
        
int SimplePosition::getTimeIntFormat(){
    return timeIntFormat;
}

PositionClean::PositionClean(){
}

PositionClean::~PositionClean(){
    simplePositionFile.clear();
}

void PositionClean::cleanPositionFromFile(string filePath){
    Timer cleanClock;

    simplePositionFile.clear();
    errorServiceStatusAmount = 0;
    errorLightStatusAmount = 0;
    beyondBoundaryAmount = 0;

    ifstream infile;
    infile.open(filePath.c_str(),ios::in);
    string s;
    Position p;
    SimplePosition sp;
    int i = 0;
    getline(infile,s);
    while(s!="" && (i<POSITION_CLEAN_AMOUNT || POSITION_CLEAN_AMOUNT==-1)){
        if(i%10000000 == 0) cout<<"read:"<<i<<endl;
        bool positionIsNormal = true;
        p.init(s);
        if(p.getServiceStatus()!='0'){ //业务状态
            errorServiceStatusAmount++;
            positionIsNormal = false;
        }
        if(p.getLightStatus()!='0' && p.getLightStatus()!='1'){ //顶灯状态
            errorLightStatusAmount++;
            positionIsNormal = false;
        }
        if(p.getLatitude()<START_LATITUDE || p.getLatitude()>END_LATITUDE || p.getLongitude()<START_LONGITUDE || p.getLongitude()>END_LONGITUDE){
            beyondBoundaryAmount++;
            positionIsNormal = false;
        }
        if(positionIsNormal){
            int date = convertDateToInt(p.getReceiveDate());
            int time = convertTimeToInt(p.getReceiveTime());
            sp.init(p.getTaxiId(),p.getLightStatus(),p.getLatitude(),p.getLongitude(),p.getSpeed(),date,time);
            simplePositionFile.push_back(sp); 
        }
        getline(infile,s);
        i++;
    }
    infile.close();
    cout<<"[PositionClean] - Cost of time: "<<cleanClock.getTimeCost()<<"s"<<endl;
}

void PositionClean::writeSimplePositionToFile(string filePath){
    std::ofstream outfile;
    outfile.open(filePath);
    outfile<<setprecision(9); //9位有效数字
    for(int i=0;i<simplePositionFile.size();i++){
        outfile<<simplePositionFile[i].getTaxiId()<<' '<<simplePositionFile[i].getLightStatus()<<' ';
        outfile<<simplePositionFile[i].getLatitude()<<' '<<simplePositionFile[i].getLongitude()<<' '<<simplePositionFile[i].getSpeed()<<' ';
        outfile<<simplePositionFile[i].getDateIntFormat()<<' '<<simplePositionFile[i].getTimeIntFormat()<<endl;
    }
    outfile.close();
}

void PositionClean::printPointClean(){
    cout<<"Size of simplePositionFile: "<<simplePositionFile.size()<<endl;
    cout<<"Amount of abnormal serviceStatus: "<<errorServiceStatusAmount<<endl;
    cout<<"Amount of abnormal lightStatus: "<<errorLightStatusAmount<<endl;
    cout<<"Amount of beyond the boundary: "<<beyondBoundaryAmount<<endl;
}