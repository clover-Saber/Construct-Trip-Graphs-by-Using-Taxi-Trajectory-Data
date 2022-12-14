#include <cmath>
#include <fstream>
#include <iostream>

#include"include/Util.h"
using namespace std;

double distance1(double lon1,double lat1,double lon2,double lat2){
    //地球半径6371.004km
    const double R=6371004;
    //圆周率
    const double PI=3.1415926535;
    double c = sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lon1-lon2);
    double dis = R*acos(c)*PI/180;
    return dis;
}

double distance(double lon1,double lat1,double lon2,double lat2){
    //纬度相等的情况下,经度每隔1度，距离相差约100000米
    double dlon=100000;
    //经度相等的情况下,纬度每隔1度，距离相差约111320米
    double dlat=111320;
    double dis = sqrt(((lon1-lon2)*dlon)*((lon1-lon2)*dlon)+((lat1-lat2)*dlat)*((lat1-lat2)*dlat));
    return dis;
}

//以起始时间排序
bool cmpStart(Point &x,Point &y){
    return x.getStartTime()<y.getStartTime();
}
//以结束时间排序
bool cmpEnd(Point &x,Point &y){
    return x.getEndTime()<y.getEndTime();
}

Rect::Rect(){
}
  
//起始只有一个点
Rect::Rect(double x, double y){
    min[0] = x;
    min[1] = y;

    max[0] = x;
    max[1] = y;
}

//区块
Rect::Rect(double a_minX, double a_minY, double a_maxX, double a_maxY){
    min[0] = a_minX;
    min[1] = a_minY;

    max[0] = a_maxX;
    max[1] = a_maxY;
}

Rect::~Rect(){
}

//R树回调函数
bool MySearchCallback(int id, void* arg) 
{
  vecAns.push_back(id);
  return true; // keep going
}

void nothing(){
}

//R树搜索，多线程
void searchRtree(RTree<int,double,2, double> &rtree,int slotTime,int endTime,double lon,double lat){
    //搜索经纬度边界
    //纬度相等的情况下,经度每隔1度，距离相差约100000米
    //经度相等的情况下,纬度每隔1度，距离相差约111320米
    //此处估算，出租车一分钟660米，经度0.0066，纬度0.0059
    double tmp=(slotTime-endTime)/60;
    double tlon=0.0066*tmp;
    double tlat=0.0059*tmp;
    Rect searchRect(lat-tlat,lon-tlon,lat+tlat,lon+tlon);
    int nhits = rtree.Search(searchRect.min, searchRect.max, MySearchCallback, NULL);
    return;
}

Timer::Timer(){
    start = clock(); 
}

Timer::~Timer(){
}

double Timer::getTimeCost(){
    return double(clock()-start)/1000;
}

int convertDateToInt(std::string date){
    //TODO 没考虑闰年大小月
    int i = date.find("-",0);
    int j = date.find("-",i+1);
    return atoi(date.substr(0,i).c_str())*12*31+atoi(date.substr(i+1,j-i-1).c_str())*31+atoi(date.substr(j+1,date.size()-j-1).c_str());
}

int convertTimeToInt(std::string time){
    int i = time.find(":",0);
    int j = time.find(":",i+1);
    return atoi(time.substr(0,i).c_str())*3600+atoi(time.substr(i+1,j-i-1).c_str())*60+atoi(time.substr(j+1,time.size()-j-1).c_str());
}

struct UtilNode{
    int id;
    double x1,y1,x2,y2;
    UtilNode(int a,double b,double c,double d,double e){
        id = a;
        x1 = b;
        y1 = c;
        x2 = d;
        y2 = e;
    }
};


//计算满足一定距离的点
void tripTaskGraph(string path){
    ifstream infile;
    infile.open(path.c_str(),ios::in);
    int id;
    double x1,y1,x2,y2;
    vector<UtilNode> u;
    while(infile>>id>>x1>>y1>>x2>>y2){
        UtilNode t(id,x1,y1,x2,y2);
        u.push_back(t);
    }
    //for(int i=0;i<u.size();i++) cout<<u[i].id<<' '<<u[i].x1<<' '<<u[i].y1<<' '<<u[i].x2<<' '<<u[i].y2<<endl;
    for(int i=0;i<u.size();i++)
        for(int j=i+1;j<u.size();j++){
            if(sqrt((u[i].x2-u[j].x1)*(u[i].x2-u[j].x1)+(u[i].y2-u[j].y1)*(u[i].y2-u[j].y1)) < 0.06)
                cout<<u[i].id<<' '<<u[i].x2<<' '<<u[i].y2<<' '<<u[j].id<<' '<<u[j].x1<<' '<<u[j].y1<<endl;
        }
}