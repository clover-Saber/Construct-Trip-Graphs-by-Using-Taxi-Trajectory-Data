#include <iostream>
#include <fstream>
#include <queue>
#include <thread>

#include "include/SharebilityNetwork.h"
#include "include/Parameter.h"
#include "include/Util.h"
using namespace std;

vector<int> vecAns;

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

void SharebilityNetwork::readPoint(string filePath){
    ifstream infile;
    infile.open(filePath.c_str(),ios::in);
    vector<vector<Point>> tp;
    vector<Point> t;
    for(int i=0; i<24*60*60/SLICE_TIME; i++) tp.push_back(t);
    Point p;
    double sx,sy,ex,ey; //经纬度
    int st,et; //时间
    int i = 0;
    //读取point.txt文件
    while((i<POINT_AMOUNT || POINT_AMOUNT==-1) && infile>>st>>sx>>sy>>et>>ex>>ey){
        p.init(st,sx,sy,et,ex,ey);
        int order = st/SLICE_TIME;
        tp[order].push_back(p);
        i++;
    }
    //将二维数组合并为一维数组，并记录下标
    orderNumber.push_back(0);
    for(int j=0;j<tp.size();j++){
        vecPoint.insert(vecPoint.end(),tp[j].begin(),tp[j].end());
        orderNumber.push_back(vecPoint.size());
    }
}

void SharebilityNetwork::fetchSpeedCellIndexList(int startX,int startY,int currentSliceAmount){
    for(int i=0; i<SLICE_AMOUNT; i++) speedCellIndexList[i].clear();
    //到达每个cell的最短时间
    vector<vector<int> > minTime;
    for(int i=0; i<(END_LATITUDE-START_LATITUDE)/CELL_LATITUDE_LENGTH; i++){
        vector<int> tmp;
        for(int j=0; j<(END_LONGITUDE-START_LONGITUDE)/CELL_LONGITUDE_LENGTH; j++) tmp.push_back(INF);
        minTime.push_back(tmp);
    }
    //搜索队列
    priority_queue<SpeedCellIndex> q;
    q.push(SpeedCellIndex(startX,startY,0));
    minTime[startX][startY] = 0;
    while(!q.empty()){
        SpeedCellIndex currentCell = q.top();
        speedCellIndexList[currentCell.time/SLICE_TIME].push_back(currentCell);
        q.pop();
        //因为更新时间更短的cell时，原来的cell未删除，所以需要判断当前是否最新的cell
        if(currentCell.time != minTime[currentCell.x][currentCell.y]) continue;
        for(int i=0; i<DIRECTION_AMOUNT; i++){
            int nextX = currentCell.x + DIRECTION_X[i];
            int nextY = currentCell.y + DIRECTION_Y[i];
            //边界判断
            if(nextX<0 || nextX>=(END_LATITUDE-START_LATITUDE)/CELL_LATITUDE_LENGTH) continue;
            if(nextY<0 || nextY>=(END_LONGITUDE-START_LONGITUDE)/CELL_LONGITUDE_LENGTH) continue;
            //距离-耗时计算
            double halfDistance = sqrt(pow(0.5*CELL_LATITUDE_LENGTH*EVERY_LATITUDE_DISTANCE*DIRECTION_X[i],2)+pow(0.5*CELL_LONGITUDE_LENGTH*EVERY_LONGITUDE_DISTANCE*DIRECTION_Y[i],2));
            int nextTime = currentCell.time + round(halfDistance/speedGrid->getCellSpeedByMS(currentCell.x,currentCell.y) + halfDistance/speedGrid->getCellSpeedByMS(nextX,nextY));
            //不超过时间阈值，并且不重复前往耗时更长的cell
            if(nextTime<SLICE_TIME*currentSliceAmount && nextTime<minTime[nextX][nextY]){
                minTime[nextX][nextY] = nextTime;
                q.push(SpeedCellIndex(nextX,nextY,nextTime));
            }
        }
    }

}

SharebilityNetwork::SharebilityNetwork(SpeedGrid *sg){
    speedGrid = sg;
    vector<SpeedCellIndex> speedCellIndexTmp;
    for(int i=0; i<SLICE_AMOUNT; i++) speedCellIndexList.push_back(speedCellIndexTmp);
}

SharebilityNetwork::~SharebilityNetwork(){
    vecPoint.clear();
    vecEdge.clear();
    head.clear();
}

void SharebilityNetwork::buildNetworkFromFile(string filePath){
    Timer buildClock;
    readPoint(filePath);
     //分块建立R树,只有orderNumber.size()-1块
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
        //R树搜索结果清空
        vecAns.clear();
        int endTime=vecPoint[i].getEndTime();
        int index=endTime/60;
        for(int j=0;j<SLICE_AMOUNT;j++){
            int indey = index+j;
            if(indey>=orderNumber.size()-1) continue;
            searchRtree(vecTree[indey],j,endTime,vecPoint[i].getEndLongitude(),vecPoint[i].getEndLatitude());
        }
        //建边
        for(int j=0;j<vecAns.size();j++) addEdge(i,vecAns[j]);
            //if(existEdge(i,vecAns[j])) addEdge(i,vecAns[j]);
    }
    cout<<"[SharebilityNetWork] - Cost of time: "<<buildClock.getTimeCost()<<"s"<<endl;
}

void SharebilityNetwork::buildNetworkFromFile_bySpeedGrid(string filePath){
    Timer buildClock;
    readPoint(filePath);
     //分块建立R树,只有orderNumber.size()-1块
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
        //R树搜索结果清空
        vecAns.clear();
        int endTime=vecPoint[i].getEndTime();
        int index=endTime/60;
        int currentSliceAmount = SLICE_AMOUNT;
        if(index+SLICE_AMOUNT-1 >= orderNumber.size()-1){
            currentSliceAmount = orderNumber.size()-index-1;
        }
        //位置所在速度cell下标
        int startX = (vecPoint[i].getEndLatitude()-START_LATITUDE)/CELL_LATITUDE_LENGTH;
        int startY = (vecPoint[i].getEndLongitude()-START_LONGITUDE)/CELL_LONGITUDE_LENGTH;
        fetchSpeedCellIndexList(startX,startY,currentSliceAmount);
        for(int j=0; j<currentSliceAmount; j++){
            int indey = index+j;
            //第j个时间片位置范围
            for(int k=0; k<=j; k++)
                for(int z=0; z<speedCellIndexList[k].size(); z++){
                    int x = speedCellIndexList[k][z].x;
                    int y = speedCellIndexList[k][z].y;
                    double minLat = START_LATITUDE+CELL_LATITUDE_LENGTH*x;
                    double minLon = START_LONGITUDE+CELL_LONGITUDE_LENGTH*y;
                    double maxLat = minLat+CELL_LATITUDE_LENGTH;
                    double maxLon = minLon+CELL_LONGITUDE_LENGTH;
                    Rect searchRect(minLat,minLon,maxLat,maxLon);
                    vecTree[indey].Search(searchRect.min, searchRect.max, MySearchCallback, NULL);
            }
        }
        //建边
        for(int j=0;j<vecAns.size();j++) addEdge(i,vecAns[j]);
    }
    cout<<"[SharebilityNetWork] - Cost of time: "<<buildClock.getTimeCost()<<"s"<<endl;
}


void SharebilityNetwork::buildNetworkFromFile_multithread(string filePath){
    Timer buildClock;
    readPoint(filePath);
    //分块建立R树,只有orderNumber.size()-1块
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
        //R树搜索结果清空
        vecAns.clear();
        int endTime=vecPoint[i].getEndTime();
        int index=endTime/60;
        //多线程
        for(int k=0;k<=SLICE_AMOUNT/THREAD_AMOUNT;k++){
            thread vecTh[THREAD_AMOUNT];
            for(int z=0;z<THREAD_AMOUNT;z++){
                //时间片块数
                int j = k*THREAD_AMOUNT+z;
                int indey=index+j;
                if(j>=SLICE_AMOUNT || indey>=orderNumber.size()-1){
                    vecTh[z] = thread(nothing);
                    continue;
                }
                vecTh[z] = thread(searchRtree,ref(vecTree[indey]),j,endTime,vecPoint[i].getEndLongitude(),vecPoint[i].getEndLatitude());
            }
            //线程join
            for(int z=0;z<THREAD_AMOUNT;z++) vecTh[z].join();
        }
        //建边
        for(int j=0;j<vecAns.size();j++) addEdge(i,vecAns[j]);
    }
    cout<<"[SharebilityNetWork] - Cost of time: "<<buildClock.getTimeCost()<<"s"<<endl;
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
    //设定汽车平均行驶速度36.8km/h，即11m/s
    if(distance(vecPoint[x].getEndLongitude(),vecPoint[x].getEndLatitude(),vecPoint[y].getStartLongitude(),vecPoint[y].getEndLatitude())>11.0*t){
        return false;
    }
    return true;
}

void SharebilityNetwork::printNetwork(){
    cout<<"Amount of Points: "<<vecPoint.size()<<endl;
    cout<<"Amount of Edges: "<<vecEdge.size()<<endl;
}