#ifndef _SHAREBILITYNETWORK_H
#define _SHAREBILITYNERWORK_H

#include <vector>
#include<ctime>

#include "File.h"
#include "Point.h"
#include "RTree.h"
#include "Util.h"

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
        //第i个订单块首订单的序号数
        std::vector<int> orderNumber;
        //R树,vector莫名报错
        RTree<int,double,2, double> vecTree[1440];

        //订单点集
        std::vector<Point> vecPoint;
        //边集数组，存放边
        std::vector<Edge> vecEdge;
        //head[i]表示点i连接的第一条边的下标（实际为最后一条，顺序无碍）
        std::vector<int> head;
        //最长允许连接的时间 15min
        static const int maxLimitTime=20*60;
        //友元类
        friend class File;
        friend class HungaryAlgorithm;
        friend class HopcroftKarpAlgorithm;
    public:
        //计时器
        Timer timer;
        SharebilityNetwork(){};
        ~SharebilityNetwork();
        //添加一条单向边
        void addEdge(int x,int y);
        //x->y存在单向边，true存在，false不存在
        bool existEdge(int x,int y);
        //订单按开始时间段分割，每块数据按开始位置建立R树
        //初始化搭建网络，注:文件流传参需要使用引用
        void init(File &f);
        //打印点集
        void printVecPoint();
        //打印边集
        void printVecEdge();

};

#endif