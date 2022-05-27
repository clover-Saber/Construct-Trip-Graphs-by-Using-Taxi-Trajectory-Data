#ifndef _SHAREBILITYNETWORK_H
#define _SHAREBILITYNERWORK_H

#include <vector>
#include <queue>
#include <ctime>

#include "File.h"
#include "Point.h"
#include "RTree.h"
#include "Util.h"
#include "SpeedGrid.h"

//速度单元索引
struct SpeedCellIndex{
    int x,y,time;
    SpeedCellIndex(int x,int y,int time) :x(x), y(y), time(time){}
    bool operator < (const SpeedCellIndex& t) const{
        return time > t.time;
    }
};

//速度单元合并范围
struct SpeedCellRange{
    //左上角cell下标和右下角cell下标
    int lx,ly,rx,ry;
    int reachableAmount; //可达网格数
    SpeedCellRange(int lx,int ly,int rx,int ry) :lx(lx), ly(ly), rx(rx), ry(ry){}
    SpeedCellRange(int lx,int ly,int rx,int ry,int ra) :lx(lx), ly(ly), rx(rx), ry(ry), reachableAmount(ra){} 
};

//连接边类，单向
class Edge{
    private:
        //起点id
        int start;
        //终点id
        int end;
        //同节点下一条边的下标，-1表示没有了
        int next;
    public:
        Edge();
        ~Edge();
        void init(int x,int y,int z);
        int getStart();
        int getEnd();
        int getNext();
        void printEdge();
};

//交通网络图类
class SharebilityNetwork{
    private:
        SpeedGrid *speedGrid;
        //第i个订单块首订单的序号数
        std::vector<int> orderNumber;
        //R树,vector莫名报错
        RTree<int,double,2, double> vecTree[1440];
        //第i分钟可达的速度网格队列
        std::vector<std::vector<SpeedCellIndex>> speedCellIndexList;
        //第i个时间片(或前i分钟)可达的速度网络范围队列（合并后）
        std::vector<std::vector<SpeedCellRange>> speedCellRangeList;
        //第i分钟搜出来的可达范围边界
        std::vector<SpeedCellRange> speedCellLimit;
        //除法计算时间
        double divisionCalculationTime;
        //查询总块数
        int blockTotalAmount = 0;
        //队列元素数
        int queueTotalAmount = 0;
        //订单点集
        std::vector<Point> vecPoint;
        //边集数组，存放边
        std::vector<Edge> vecEdge;
        //head[i]表示点i连接的第一条边的下标（实际为最后一条，顺序无碍）
        std::vector<int> head;
        //最长允许连接的时间 15min
        static const int maxLimitTime=15*60;
        //友元类
        friend class HungaryAlgorithm;
        friend class HopcroftKarpAlgorithm;

        void readPoint(std::string filePath);
        //可达网格搜索
        void fetchReachableSpeedCells(int startX,int startY,int currentSliceAmount);
        void fetchReachableSpeedCells_byMerge(int startX,int startY,int currentSliceAmount);
        void fetchReachableSpeedCells_byPriorityQueueMerge(int startX,int startY,int currentSliceAmount);
        //网格车辆查询
        void cellQuery(int index,int currentSliceAmount);
        //网格车辆查询-网格合并分治法
        void cellQuery_byDivideAndConquer(int index,int currentSliceAmount);
        void cellQuery_byPriorityQueueMerge(int index,int currentSliceAmount);
    public:
        SharebilityNetwork(SpeedGrid *sg);
        ~SharebilityNetwork();
        //订单按开始时间段分割，每块数据按开始位置建立R树
        //初始化搭建网络，注:文件流传参需要使用引用
        void buildNetworkFromFile(std::string filePath);
        void buildNetworkFromFile_bySpeedGrid(std::string filePath);
        void buildNetworkFromFile_multithread(std::string filePath);
        //添加一条单向边
        void addEdge(int x,int y);
        //x->y存在单向边，true存在，false不存在
        bool existEdge(int x,int y);
        void printNetwork();
};

#endif