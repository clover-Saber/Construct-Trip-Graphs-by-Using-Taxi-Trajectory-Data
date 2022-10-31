#include <iostream>
#include <cstring>
#include <string>

#include "include/Parameter.h"
#include "include/PositionClean.h"
#include "include/PointCollect.h"
#include "include/RoadNetwork.h"     
#include "include/SpeedGrid.h"
#include "include/Util.h"
#include "include/MinimumCost.h"
using namespace std;

int main(){
    //string file = BASE_PATH + "tripTaskGraph.txt";
    //tripTaskGraph(file);
    /*  
    //建立路网
    char roadPath[100]="C://Users//stud//Desktop//1.txt";
    RoadNetwork roadNet;
    roadNet.init(roadPath); 
    roadNet.printNetwork();
    */
    //构建车辆速度网格
    SpeedGrid speedGrid;
    string speedGridFilePath = BASE_PATH + "speedGrid500m.txt";
    //1>直接读取现有网格
    speedGrid.readGridByFile(speedGridFilePath);
    /*
    //2>构建网格
    for(int i=0;i<SPEED_GRID_FILE_LIST.size();i++){
        string speedFilePath = BASE_PATH + SPEED_GRID_FILE_LIST[i] + "//part-00000//part-00000.txt";
        speedGrid.updateGridByFile(speedFilePath);
    }
    speedGrid.writeSpeedGirdToFile(speedGridFilePath);
    speedGrid.printSpeedGrid();
    */
    for(int i=0;i<POSITION_FILE_LIST.size();i++){
        cout<<POSITION_FILE_LIST[i];
        cout<<"--------------------------"<<endl;
        /*
        //1-对初始位置文件进行清洗
        string positionFilePath = BASE_PATH + POSITION_FILE_LIST[i] + "//part-00000//part-00000.txt";
        PositionClean positionClean;
        positionClean.cleanPositionFromFile(p  ositionFilePath);
        string simplePositionFilePath = BASE_PATH + POSITION_FILE_LIST[i] + "//simplePosition.txt";
        positionClean.writeSimplePositionToFile(simplePositionFilePath);
        positionClean.printPointClean();
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
        
        //3-构建出行任务图
        string pointFilePath = BASE_PATH + POSITION_FILE_LIST[i] + "//point.txt";
        SharebilityNetwork sharebilityNetwork(&speedGrid);
        sharebilityNetwork.buildNetworkFromFile(pointFilePath);
        //sharebilityNetwork.buildNetworkFromFile_bySpeedGrid(pointFilePath);
        sharebilityNetwork.printNetwork();
        
        //输出边
        string edgeFilePath = BASE_PATH + POSITION_FILE_LIST[i] + "//edges.txt";
        sharebilityNetwork.outputEdges(edgeFilePath);
        
        /*
        //匈牙利算法
        HungaryAlgorithm algorithm1;
        cout<<"Hungary-MaxMatch: "<<algorithm1.hungary(shareNet)<<endl;
        cout<<"Hungary-Time: "<<algorithm1.time()<<endl;
        */
        
        
        //Hopcroft-Karp算法
        HopcroftKarpAlgorithm algorithm2;
        int maxMatch = algorithm2.hopcroftKarp(sharebilityNetwork);
        cout<<"HopcroftKarp-MaxMatch: "<<maxMatch<<endl;
        cout<<"HopcroftKarp-Time: "<<algorithm2.time()<<endl;
        //输出匹配
        string matchFilePath = BASE_PATH + POSITION_FILE_LIST[i] + "//match.txt";
        algorithm2.outputMatch(matchFilePath);
        /*
        // 最短里程
        MinimumCost minimumCost;
        string lengthCostFilePath = BASE_PATH + "path_cost_length_nm.txt";
        string timeCostFilePath = BASE_PATH + "path_cost_time_nm.txt";
        string crossingFilePath = BASE_PATH + "crossing_location.txt";
        double minCost = minimumCost.calculateMinimumCostWhenThisFlow(lengthCostFilePath, crossingFilePath, sharebilityNetwork, maxMatch);
        cout<<"minCost: "<<minCost<<endl;
        */

        /*
        // 计算实际里程
        PointCollect pointCollect;
        string simplePositionFilePath = BASE_PATH + POSITION_FILE_LIST[i] + "//simplePosition.txt";
        pointCollect.collectPointFromFile(simplePositionFilePath);
        pointCollect.calculateOriginalMileage();
        cout<<"OriginalAcMileage: "<<pointCollect.getOriginalMileage()<<endl;

        // 调度优化后的里程
        cout<<"OptimizeMileage: "<<pointCollect.getTotalTripMileage()+minCost<<endl;
        */
    }
    return 0;
}