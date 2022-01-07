#include <iostream>
#include <thread>

#include "include/SharebilityNetwork.h"
using namespace std;

std::vector<int> vecAns;

Edge::Edge(){

}

Edge::~Edge(){

}

void Edge::init(int x,int y,int z){
    start = x;
    end = y;
    next = z;
}

int Edge::getStart(){
    return start;
}

int Edge::getEnd(){
    return end;
}

int Edge::getNext(){
    return next;
}

void Edge::printEdge(){
    printf("[%d,%d,%d]",start,end,next);
}

SharebilityNetwork::~SharebilityNetwork(){
    vecPoint.clear();
    vecEdge.clear();
    head.clear();
}

void SharebilityNetwork::addEdge(int x,int y){
    Edge e;
    int t = head[x];
    head[x] = vecEdge.size();
    e.init(x,y,t);
    vecEdge.push_back(e);
}

bool SharebilityNetwork::existEdge(int x,int y){
    //两个订单时间间隔=y开始时间-x结束时间
    int t=vecPoint[y].getStartTime()-vecPoint[x].getEndTime();
    //x时间要比y早，且时间间隔不能大于规定时限
    if(t<=0 || t>maxLimitTime) return false;
    //时间内车能从x点到达y点 
    /*
    //设定汽车平均行驶速度36.8km/h，即11m/s
    if(distance(vecPoint[x].getEndLongitude(),vecPoint[x].getEndLatitude(),vecPoint[y].getStartLongitude(),vecPoint[y].getEndLatitude())>11.0*t){
        return false;
    }
    */
    return true;
}

void SharebilityNetwork::init(File &f){
    timer.init();
    //读取订单点集
    f.readPointVector(vecPoint,orderNumber);
    //分块建立R树
    //只有orderNumber.size()-1块
    for(int i=0;i<orderNumber.size()-1;i++){
        for(int j=orderNumber[i];j<orderNumber[i+1];j++){
            //节点
            Rect rt(vecPoint[j].getStartLatitude(),vecPoint[j].getStartLongitude());
            vecTree[i].Insert(rt.min,rt.max,j);
        }
    }
    //初始化赋值-1
    for(int i=0;i<vecPoint.size();i++) head.push_back(-1);
    //连接单向边
    for(int i=0;i<vecPoint.size();i++){
        //线程数
        const int threadNumber = 8;
        //R树搜索结果清空
        vecAns.clear();
        int endTime=vecPoint[i].getEndTime();
        int index=endTime/60;
        
        //多线程
        for(int k=0;k<20/threadNumber;k++){
            thread vecTh[threadNumber];
            for(int z=0;z<threadNumber;z++){
                //时间片块数
                int j = k*threadNumber+z;
                int indey=index+j;
                if(indey>=orderNumber.size()-1){
                    vecTh[z] = thread(nothing);
                    continue;
                }
                vecTh[z] = thread(searchRtree,ref(vecTree[indey]),j,endTime,vecPoint[i].getEndLongitude(),vecPoint[i].getEndLatitude());
            }
            //线程join
            for(int z=0;z<threadNumber;z++) vecTh[z].join();
        }
        //剩余时间片
        int nowThreadNumber = 20%threadNumber;
        thread vecTh[nowThreadNumber];
        for(int z=0;z<nowThreadNumber;z++){
            //时间片块数
            int j = (20/threadNumber)*threadNumber+z;
            int indey=index+j;
            if(indey>=orderNumber.size()-1){
                vecTh[z] = thread(nothing);
                continue;
            }
            vecTh[z] = thread(searchRtree,ref(vecTree[indey]),j,endTime,vecPoint[i].getEndLongitude(),vecPoint[i].getEndLatitude());
        }
        //线程join
        for(int z=0;z<nowThreadNumber;z++) vecTh[z].join();
        
        //建边
        for(int j=0;j<vecAns.size();j++) addEdge(i,vecAns[j]);
            //if(existEdge(i,vecAns[j])) addEdge(i,vecAns[j]);
    }
}

void SharebilityNetwork::printVecPoint(){
    cout<<"#VecPoint#:"<<vecPoint.size()<<endl;
}

void SharebilityNetwork::printVecEdge(){
    cout<<"#VecEdge#:"<<vecEdge.size()<<endl;
}