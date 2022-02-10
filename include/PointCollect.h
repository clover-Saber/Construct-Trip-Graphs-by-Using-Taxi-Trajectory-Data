#ifndef POINTCOLLECT_H
#define POINTCOLLECT_H

#include <string>
#include <vector>
#include <map>

#include "PositionClean.h"
#include "File.h"

//从SimplePosition中获取Point
class PointCollect{
    private:
        //位置信息map
        std::map<std::string,std::vector<SimplePosition> > simplePositionMap;
        //订单点集
        std::vector<Point> vecPoint;
        //订单里程,单位m
        std::vector<double> mileage;
        double originalMileage=0;
        friend class File;
        void readSimplePosition(std::string filePath);
        void collectPoint();
    public:
        PointCollect();
        ~PointCollect();
        void clear();
        void collectPointFromFile(std::string filePath);
        void writePointToFile(std::string filePath);
        //计算原始车辆总计里程
        void calculateOriginalMileage(File &f,std::string rightDate);
        double getOriginalMileage(){return originalMileage;};
        //返回全部行程总计里程
        double getTotalTripMileage();
        void printVecPoint();
};

#endif