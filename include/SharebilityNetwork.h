#ifndef _SHAREBILITYNETWORK_H
#define _SHAREBILITYNERWORK_H

#include <vector>
#include <queue>
#include <ctime>

#include "Point.h"
#include "RTree.h"
#include "Util.h"
#include "SpeedGrid.h"

//速度单元索引
struct SpeedCellIndex;

//速度单元合并范围
struct SpeedCellRange;

//连接边类，单向
class Edge{
    private:
        int start; //起点id
        int end; //终点id
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
        //第i个时间片的结束时间
        std::vector<int> slotStartTime;
        //R树,vector莫名报错
        RTree<int,double,2, double> vecTree[4000];
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
        //友元类
        friend class HungaryAlgorithm;
        friend class HopcroftKarpAlgorithm;
        friend class MinimumCost;

        void readPoint(std::string filePath);
        //二分查找某时刻对应的R树下标
        int findRtreeByTime(int t);
        //可达网格搜索
        void fetchReachableSpeedCells(int startT,int startX,int startY,int timeThreshold,int slotAmount);
        void fetchReachableSpeedCells_byPriorityQueueMerge(int startT, int startX,int startY,int timeThreshold,int slotAmount);
        //网格车辆查询
        void cellQuery(int index,int currentSliceAmount);
        //网格车辆查询-优先队列网格合并
        void cellQuery_byPriorityQueueMerge(int index,int slotAmount);
    public:
        SharebilityNetwork(SpeedGrid *sg);
        ~SharebilityNetwork();
        //订单按开始时间段分割，每块数据按开始位置建立R树
        //初始化搭建网络，注:文件流传参需要使用引用
        void buildNetworkFromFile(std::string filePath);
        void buildNetworkFromFile_bySpeedGrid(std::string filePath);
        //添加一条单向边
        void addEdge(int x,int y);
        //x->y存在单向边，true存在，false不存在
        bool existEdge(int x,int y);
        //输出边
        void outputEdges(std::string filePath);
        void printNetwork();
};

#endif