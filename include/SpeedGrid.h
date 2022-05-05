#ifndef SPEEDGRID_H
#define SPEEDGRID_H

#include <fstream>
#include <vector>
#include <string>

class SpeedCell{
    private:
        //单位：千米/小时
        double averageSpeed,totalSpeed;
        int speedDataAmount;
        //网格不同方向行驶一半的时间/秒
        std::vector<int> spendTime;
        //特殊数据统计
        int zeroAmount;
    public:
        SpeedCell();
        ~SpeedCell();
        void init(double avesp,double totsp,int amount);
        void addSpeedData(double speedDataOne); 
        void calculateAverageSpeed();
        int getSpendTime(int i);
        double getAverageSpeed();
        double getTotalSpeed();
        int getSpeedDataAmount();
        int getZeroAmount();
};


//车辆速度网格
class SpeedGrid{
    private:
        std::vector<std::vector<SpeedCell> > grid;
        int speedAmount[20];
        int zeroSpeedAmount;
    public:
        SpeedGrid();
        ~SpeedGrid();
        void updateGridByFile(std::string filePath);
        void readGridByFile(std::string filePath);
        //单位：秒
        int getCellSpendTime(int x,int y,int i);
        //单位：米/秒
        double getCellSpeedByMS(int x,int y);
        void writeSpeedGirdToFile(std::string filePath);
        void printSpeedGrid();
};


#endif
