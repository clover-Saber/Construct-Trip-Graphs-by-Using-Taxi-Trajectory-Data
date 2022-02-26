#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include "include/PointCollect.h"
#include "include/Util.h"
#include "include/Parameter.h"
using namespace std;

void PointCollect::readSimplePosition(string filePath){
    ifstream infile;
    infile.open(filePath.c_str(),ios::in);
    SimplePosition sp;
    string tid;
    char light;
    double lat,lon,spd;
    int date,time;
    int i = 1;
    while((i<SIMPLE_POSITION_READ_AMOUNT || SIMPLE_POSITION_READ_AMOUNT==-1) && infile>>tid>>light>>lat>>lon>>spd>>date>>time){
        if(i%10000000 == 0) cout<<"read:"<<i<<endl;
        sp.init(tid,light,lat,lon,spd,date,time);
        //map中没有此车号，将其加入map
        if(simplePositionMap.find(sp.getTaxiId())==simplePositionMap.end()){
            vector<SimplePosition> simplePositionList;
            simplePositionList.push_back(sp);
            simplePositionMap.insert(pair<string,vector<SimplePosition> >(sp.getTaxiId(),simplePositionList));
        }else{
            simplePositionMap[sp.getTaxiId()].push_back(sp);
        }
        i++;
    }
    infile.close();
}

void PointCollect::collectPoint(){
    int number=0;
    Point p;
    bool flag=false; //是否正在载客
    map<string,vector<SimplePosition> >::iterator it;
    for(it=simplePositionMap.begin();it!=simplePositionMap.end();it++){
        //只有一个状态就算了
        if(it->second.size()==1) continue;
        //按时间排序，从早到晚
        sort(it->second.begin(),it->second.end());
        //连续的一段运营状态即视为一个订单
        //持续时间近似为第一个运营状态时间到最后一个运营状态结束时间(即其后的第一个非运营状态时间)
        for(int i=0;i<it->second.size();i++){
            //顶灯状态为营运
            if(it->second[i].getLightStatus()=='0'){
                //正在载客则继续载客，无操作
                //没有载客则接单开始载客
                if(!flag){
                    flag = true;
                    //新增订单
                    vecPoint.push_back(p);
                    //初始化起点信息
                    vecPoint[vecPoint.size()-1].initStart(it->second[i].getTimeIntFormat(),it->second[i].getLongitude(),it->second[i].getLatitude());
                    //初始里程0
                    mileage.push_back(0);
                }else{
                    //添加两点距离
                    mileage[mileage.size()-1] += distance(it->second[i-1].getLongitude(),it->second[i-1].getLatitude(),it->second[i].getLongitude(),it->second[i].getLatitude()); 
                }
            }else{
                //没有载客则继续等待订单，无操作
                //正在载客则订单结束
                if(flag){
                    flag = false;
                    //初始化终点信息
                    vecPoint[vecPoint.size()-1].initEnd(it->second[i-1].getTimeIntFormat(),it->second[i-1].getLongitude(),it->second[i-1].getLatitude());
                    //订单里程筛选
                    if(mileage[mileage.size()-1]<200){
                        vecPoint.pop_back();
                        mileage.pop_back();
                    }
                }
            }    
        }
        //最后订单还未结束，直接结束订单
        //去掉停运数据时需要考虑
        if(flag){
            int i=it->second.size()-1;
            flag = false;
            vecPoint[vecPoint.size()-1].initEnd(it->second[i].getTimeIntFormat(),it->second[i].getLongitude(),it->second[i].getLatitude());
            //订单里程筛选
            if(mileage[mileage.size()-1]<200){
                vecPoint.pop_back();
                mileage.pop_back();
            }
        }
    }
}

PointCollect::PointCollect(){
}

PointCollect::~PointCollect(){
    simplePositionMap.clear();
    vecPoint.clear();
    mileage.clear();
}

void PointCollect::clear(){
    simplePositionMap.clear();
    vecPoint.clear();
    mileage.clear();
}

void PointCollect::collectPointFromFile(string filePath){
    Timer collecClock;
    clear();
    this->readSimplePosition(filePath);
    this->collectPoint();
    cout<<"[PointCollect] - Cost of time: "<<collecClock.getTimeCost()<<"s"<<endl;
}

void PointCollect::writePointToFile(string filePath){
    ofstream outfile;
    outfile.open(filePath);
    if(POINT_SORT_METHOD==1){
        sort(vecPoint.begin(),vecPoint.end(),cmpStart);
    }else if(POINT_SORT_METHOD==2){
        sort(vecPoint.begin(),vecPoint.end(),cmpEnd);
    }
    outfile<<setprecision(9);
    for(int i=0;i<vecPoint.size();i++){
        outfile<<vecPoint[i].getStartTime()<<' '<<vecPoint[i].getStartLongitude()<<' '<<vecPoint[i].getEndLatitude()<<' ';
        outfile<<vecPoint[i].getEndTime()<<' '<<vecPoint[i].getEndLongitude()<<' '<<vecPoint[i].getEndLatitude()<<endl;
    } 
    outfile.close();
}

void PointCollect::calculateOriginalMileage(File &f,string rightDate){
    originalMileage = 0;
    map<string,vector<SimplePosition> >::iterator it;
    for(it=simplePositionMap.begin();it!=simplePositionMap.end();it++){
        //按时间排序，从早到晚
        sort(it->second.begin(),it->second.end());
        for(int i=1;i<it->second.size();i++){
            //添加两点距离
            originalMileage += distance(it->second[i-1].getLongitude(),it->second[i-1].getLatitude(),it->second[i].getLongitude(),it->second[i].getLatitude()); 
        }
    }
}

double PointCollect::getTotalTripMileage(){
    double total=0;
    for(int i=0;i<mileage.size();i++) total += mileage[i];
    return total;
}

void PointCollect::printVecPoint(){
    cout<<"Amount of vehicles: "<<simplePositionMap.size()<<endl;
    cout<<"Size of pointFile: "<<vecPoint.size()<<endl;
}