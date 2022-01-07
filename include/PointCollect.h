#ifndef _POINTCOLLECT_H
#define _POINTCOLLECT_H
#include <string>
#include <vector>
#include <map>
#include "File.h"

//从Position中获取Point的类
class PointCollect{
    private:
        //初始位置信息map
        std::map<std::string,std::vector<Position> > mapPosition;
        //订单点集
        std::vector<Point> vecPoint;
        //订单里程,单位m
        std::vector<double> mileage;
        double originalMileage=0;
        friend class File;
    public:
        PointCollect(){};
        ~PointCollect();
        //清理缓存
        void clear();
        //获取positionMap，并将其整理为Point
        //File引用，rightDate为要求日期，返回总车辆数
        void collect(File &f,std::string rightDate);
        //计算原始车辆总计里程
        void collectOriginalMileage(File &f,std::string rightDate);
        double getOriginalMileage(){return originalMileage;};
        //返回全部行程总计里程
        double getTotalTripMileage();
        //写出到文件,h=0不排序，h=1按开始时间排序，h=2按结束时间排序
        void write(File &f,char *path,int h);
        //打印mapPosition
        void printMapPosition();
        //打印vecPoint
        void printVecPoint();
};

#endif