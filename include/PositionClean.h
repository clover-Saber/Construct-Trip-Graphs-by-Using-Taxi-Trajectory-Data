#ifndef POSITIONCLEAN_H
#define POSITIONCLEAN_H

#include <vector>
#include <string>


//简化后的Position类
class SimplePosition{
    private:
        std::string taxiId;
        char lightStatus;
        double latitude;
        double longitude;
        double speed;
        int dateIntFormat;
        int timeIntFormat;
    public:
        SimplePosition();
        ~SimplePosition();
        bool operator < (const SimplePosition &sp){return timeIntFormat<sp.timeIntFormat;}
        void init(std::string tid,char light,double lat,double lon,double spd,int date,int time);
        std::string getTaxiId();
        char getLightStatus();
        double getLatitude();
        double getLongitude();   
        double getSpeed();
        int getDateIntFormat();
        int getTimeIntFormat();

};

//车辆GPS位置文件清洗
class PositionClean{
    private:
        std::vector<SimplePosition> simplePositionFile;
        int errorServiceStatusAmount;
        int errorLightStatusAmount;
        int beyondBoundaryAmount;
    public:
        PositionClean();
        ~PositionClean();
        void cleanPositionFromFile(std::string filePath);
        void writeSimplePositionToFile(std::string filePath);
        void printPointClean();
};

#endif