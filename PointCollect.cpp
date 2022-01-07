#include "include/PointCollect.h"
#include "include/Util.h"
#include <iostream>
#include <algorithm>
using namespace std;

PointCollect::~PointCollect(){
    mapPosition.clear();
    vecPoint.clear();
    mileage.clear();
}

void PointCollect::clear(){
    mapPosition.clear();
    vecPoint.clear();
    mileage.clear();
}

void PointCollect::collect(File &f,string rightDate){
    int number=0;
    Point p;
    //是否正在载客
    bool flag=false;
    clear();
    f.readPositionMap(mapPosition,rightDate);
    map<string,vector<Position> >::iterator it;
    for(it=mapPosition.begin();it!=mapPosition.end();it++){
        //cout<<it->first<<endl;
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
                    vecPoint[vecPoint.size()-1].initStart(it->second[i].getTime(),it->second[i].getLongitude(),it->second[i].getLatitude());
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
                    vecPoint[vecPoint.size()-1].initEnd(it->second[i-1].getTime(),it->second[i-1].getLongitude(),it->second[i-1].getLatitude());
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
            vecPoint[vecPoint.size()-1].initEnd(it->second[i].getTime(),it->second[i].getLongitude(),it->second[i].getLatitude());
            //订单里程筛选
            if(mileage[mileage.size()-1]<200){
                vecPoint.pop_back();
                mileage.pop_back();
            }
        }
    }
}

void PointCollect::collectOriginalMileage(File &f,string rightDate){
    int totn=0;
    originalMileage = 0;
    clear();
    f.readPositionMapAll(mapPosition,rightDate);
    map<string,vector<Position> >::iterator it;
    for(it=mapPosition.begin();it!=mapPosition.end();it++){
        //按时间排序，从早到晚
        sort(it->second.begin(),it->second.end());
        for(int i=1;i<it->second.size();i++){
            //添加两点距离
            originalMileage += distance(it->second[i-1].getLongitude(),it->second[i-1].getLatitude(),it->second[i].getLongitude(),it->second[i].getLatitude()); 
            totn++;
        }
    }
    cout<<"totn:"<<totn<<endl;
}

double PointCollect::getTotalTripMileage(){
    double total=0;
    for(int i=0;i<mileage.size();i++) total += mileage[i];
    return total;
}

void PointCollect::write(File &f,char *path,int h){
    if(h==1){
        //将订单按开始时间进行排序
        sort(vecPoint.begin(),vecPoint.end(),cmpStart);
    }else if(h==2){
        //将订单按结束时间进行排序
        sort(vecPoint.begin(),vecPoint.end(),cmpEnd);
    }
    f.writePointVector(vecPoint,path);
}

void PointCollect::printMapPosition(){
    cout<<"MapPositionSize:"<<mapPosition.size()<<endl;
    /*
    map<string,vector<Position> >::iterator it;
    for(it=mapPosition.begin();it!=mapPosition.end();it++){
        cout<<"["<<it->first<<","<<it->second.size()<<"]"<<endl;
        for(int i=0;i<it->second.size();i++) it->second[i].printPosition();
    }
    */
}

void PointCollect::printVecPoint(){
    cout<<"VecPointSize:"<<vecPoint.size()<<endl;
    cout<<"Mileage:"<<mileage.size()<<endl;
    /*
    for(int i=0;i<mileage.size();i++)
    if(mileage[i]>1000000){
        cout<<mileage[i]<<endl;
        cout<<vecPoint[i].getStartTime()<<' '<<vecPoint[i].getEndTime()<<endl;
        cout<<vecPoint[i].getStartLongitude()<<' '<<vecPoint[i].getStartLatitude()<<' '<<vecPoint[i].getEndLongitude()<<' '<<vecPoint[i].getEndLatitude()<<endl;
        cout<<distance(vecPoint[i].getStartLongitude(),vecPoint[i].getStartLatitude(),vecPoint[i].getEndLongitude(),vecPoint[i].getEndLatitude())<<endl;
        cout<<"========="<<endl;
    }
    */
    //for(int i=0;i<vecPoint.size();i++) vecPoint[i].printPoint();
}