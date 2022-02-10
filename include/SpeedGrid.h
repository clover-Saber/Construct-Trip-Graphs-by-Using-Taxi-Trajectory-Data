#ifndef SPEEDGRID_H
#define SPEEDGRID_H

#include <fstream>
#include <vector>
#include <string>

class SpeedCell{
    private:
        double totalSpeed,averageSpeed;
        int speedDataAmount;
    public:
        SpeedCell();
        ~SpeedCell();
        void addSpeedData(double speedDataOne); 
        void calculateAverageSpeed();
        double getAverageSpeed();
        double getTotalSpeed();
        int getSpeedDataAmount();
};


//车辆速度网格
class SpeedGrid{
    private:
        std::vector<std::vector<SpeedCell> > grid;
    public:
        SpeedGrid();
        ~SpeedGrid();
        void updateGridByFile(std::string filePath);
        void printSpeedGrid();
};


#endif
