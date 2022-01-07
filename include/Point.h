#ifndef POINT_H
#define POINT_H

//节点订单类
class Point{
    private:
        //起点经纬度
        double startLongitude,startLatitude;
        //终点经纬度
        double endLongitude,endLatitude;
        //起点时间，xxx秒
        int startTime;
        //终点时间
        int endTime;
    public:
        Point(){};
        ~Point(){};
        //初始化
        void init(int st,double sx,double sy,int et,double ex,double ey);
        //初始化起点
        void initStart(int st,double sx,double sy);
        //初始化终点
        void initEnd(int et,double ex,double ey);
        //返回起点时间
        int getStartTime(){return startTime;};
        //返回起点经度
        double getStartLongitude(){return startLongitude;}
        //返回起点纬度
        double getStartLatitude(){return startLatitude;}
        //返回终点时间
        int getEndTime(){return endTime;};
        //返回终点经度
        double getEndLongitude(){return endLongitude;}
        //返回终点纬度
        double getEndLatitude(){return endLatitude;}
        //打印
        void printPoint();
};

#endif