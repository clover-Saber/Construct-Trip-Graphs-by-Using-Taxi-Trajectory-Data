#ifndef _ROADNETWORK_H
#define _ROADNETWORK_H
#include <vector>

//路网路口
class Crossing{
    private:
        //经度 float精度不行
        double longitude;
        //纬度
        double latitude;
    public:
        Crossing(){};
        ~Crossing(){};
        //初始化
        void init(double lon,double lat);
        //返回经度
        double getLongitude(){return longitude;}
        //返回纬度
        double getLatitude(){return latitude;}
};

//道路  
class Road{
    private:
        //道路两侧路口下标
        int start,end;
        //下一条路下标，-1没有
        int next;
        //道路长度
        double length;
        //该路段平均行驶速度
        double speed;
    public:
        Road(){};
        ~Road(){};
        void init(int s,int e,int nt,double len);
        int getStart(){return start;}
        int getEnd(){return end;}
        int getNext(){return next;};
};

//路网
class RoadNetwork{
    private:
        //路口集
        std::vector<Crossing> vecCrossing;
        //道路集
        std::vector<Road> vecRoad;
        //head[i]表示点i连接的第一条边的下标
        std::vector<int> head; 
        //start路口到其他路口的最短距离  
        std::vector<double> dis;
        //最短距离初值
        const double INFDIS = 1e+8;
    public:
        RoadNetwork(){};
        ~RoadNetwork();
        //找位置的对应路口下标，没有则添加
        int findCrossing(double lon,double lat);
        //连边
        void addRoad(int s,int e,double len);
        //构建路网，path路网文件位置
        void init(char *path);
        //路网某个路口连通性测试
        void ping(int x);
        double distance(double sx,double sy,double ex,double ey);
        void printNetwork();
};

#endif