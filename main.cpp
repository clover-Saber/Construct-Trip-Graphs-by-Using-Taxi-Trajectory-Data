#include <iostream>
#include <cstring>
#include <string>

#include "include/Parameter.h"
#include "include/PositionClean.h"
#include "include/File.h"
#include "include/PointCollect.h"
#include "include/HopcroftKarpAlgorithm.h"
#include "include/RoadNetwork.h"
#include "include/SpeedGrid.h"
using namespace std;

int main(){
    /*
    //建立路网
    char roadPath[100]="C://Users//stud//Desktop//1.txt";
    RoadNetwork roadNet;
    roadNet.init(roadPath); 
    roadNet.printNetwork();
    */

    /*
    //构建车辆速度网格
    SpeedGrid speedGrid;
    for(int i=0;i<SPEED_GRID_FILE_LIST.size();i++){
        string speedFilePath = BASE_PATH + SPEED_GRID_FILE_LIST[i] + "//part-00000//part-00000.txt";
        speedGrid.updateGridByFile(speedFilePath);
    }
    speedGrid.printSpeedGrid();
    */

    for(int i=0;i<POSITION_FILE_LIST.size();i++){
        cout<<POSITION_FILE_LIST[i];
        cout<<"--------------------------"<<endl;
        /*
        //1-对初始位置文件进行清洗
        string positionFilePath = BASE_PATH + POSITION_FILE_LIST[i] + "//part-00000//part-00000.txt";
        PositionClean positionClean;
        positionClean.cleanPositionFromFile(positionFilePath);
        string simplePositionFilePath = BASE_PATH + POSITION_FILE_LIST[i] + "//simplePosition.txt";
        positionClean.writeSimplePositionToFile(simplePositionFilePath);
        */
        /*
        //2-提取订单信息
        string simplePositionFilePath = BASE_PATH + POSITION_FILE_LIST[i] + "//simplePosition.txt";
        PointCollect pointCollect;
        pointCollect.collectPointFromFile(simplePositionFilePath);
        string pointFilePath = BASE_PATH + POSITION_FILE_LIST[i] + "//point.txt";
        pointCollect.writePointToFile(pointFilePath);
        pointCollect.printVecPoint();
        */

        //File positionFile(positionFilePath);
        //文件中每个日期的数据量
        //map<string,int> mapDate;
        //string rightDate=positionFile.readDateMap(mapDate); 
        /*
        //数据文件对应的日期
        string rightDate="2015-04-"+f[i];
        //提取订单信息
        PointCollect pc;
        */

        
        //3-构建出行任务图
        string pointFilePath = BASE_PATH + POSITION_FILE_LIST[i] + "//point.txt";
        File pointFile(pointFilePath);
    
        SharebilityNetwork shareNet;
        shareNet.init(pointFile);
        shareNet.printVecPoint();
        shareNet.printVecEdge();
        
        /*
        //匈牙利算法
        HungaryAlgorithm algorithm1;
        cout<<"Hungary-MaxMatch: "<<algorithm1.hungary(shareNet)<<endl;
        cout<<"Hungary-Time: "<<algorithm1.time()<<endl;
        */
        /*
        //Hopcroft-Karp算法
        HopcroftKarpAlgorithm algorithm2;
        cout<<"HopcroftKarp-MaxMatch: "<<algorithm2.hopcroftKarp(shareNet)<<endl;
        cout<<"HopcroftKarp-Time: "<<algorithm2.time()<<endl;
        */
        //cout<<"OptimizeMileage:"<<pc.getTotalTripMileage()+algorithm2.getOptimizeMileage(shareNet)<<endl;
        
    }
    return 0;
}