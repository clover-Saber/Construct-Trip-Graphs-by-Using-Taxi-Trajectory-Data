#include <iostream>

#include "include/SpeedGrid.h"
#include "include/Parameter.h"
#include "include/Position.h"
#include "include/Util.h"
using namespace std;

SpeedCell::SpeedCell(){
    totalSpeed = 0;
    speedDataAmount = 0;
    averageSpeed = 0;
}

SpeedCell::~SpeedCell(){

}

void SpeedCell::addSpeedData(double speedDataOne){
    totalSpeed += speedDataOne;
    speedDataAmount++;
}

void SpeedCell::calculateAverageSpeed(){
    if(speedDataAmount == 0) averageSpeed = 0;
    else averageSpeed = totalSpeed/speedDataAmount;
}

double SpeedCell::getAverageSpeed(){
    if(averageSpeed == 0) return 0.001;
    return averageSpeed;
}

double SpeedCell::getTotalSpeed(){
    return totalSpeed;
}

int SpeedCell::getSpeedDataAmount(){
    return speedDataAmount;
}

SpeedGrid::SpeedGrid(){
    //向上取整
    int latitudeCellNumber = ceil((END_LATITUDE-START_LATITUDE)/CELL_LATITUDE_LENGTH);
    int longitudeCellNumber = ceil((END_LONGITUDE-START_LONGITUDE)/CELL_LONGITUDE_LENGTH);
    for(int i=0;i<latitudeCellNumber;i++){
        vector<SpeedCell> cellVector;
        SpeedCell cellOne;
        grid.push_back(cellVector);
        for(int j=0;j<longitudeCellNumber;j++) grid[i].push_back(cellOne);
    }
}

SpeedGrid::~SpeedGrid(){
    grid.clear();
}

void SpeedGrid::updateGridByFile(string filePath){
    Timer updateClock;
    ifstream infile;
    infile.open(filePath.c_str(),ios::in);
    int i = 0;
    string s;
    Position p;
    getline(infile,s);
    while(s != ""){
        if(i>10000000) break;
        if(i%1000000==0) cout<<"read:"<<i<<endl;
        p.init(s);
        //向下取整
        int latitudeIndex = floor((p.getLatitude()-START_LATITUDE)/CELL_LATITUDE_LENGTH);
        int longitudeIndex = floor((p.getLongitude()-START_LONGITUDE)/CELL_LONGITUDE_LENGTH);
        if(latitudeIndex>=0 && latitudeIndex<grid.size() && longitudeIndex>=0 && longitudeIndex<grid[latitudeIndex].size()){
            grid[latitudeIndex][longitudeIndex].addSpeedData(p.getSpeed());
        }//else cout<<latitudeIndex<<" "<<longitudeIndex<<" "<<endl;
        getline(infile,s);
        i++;
    }
    //计算网格平均速度
    for(int i=0;i<grid.size();i++)
        for(int j=0;j<grid[i].size();j++){
            grid[i][j].calculateAverageSpeed();
        }
    cout<<"[SpeedGrid] - Cost of time: "<<updateClock.getTimeCost()<<"s ["<<filePath<<"]"<<endl;
}

double SpeedGrid::getCellSpeedByMS(int x,int y){
    return grid[x][y].getAverageSpeed()/3.6;
}

void SpeedGrid::printSpeedGrid(){
    cout<<"print SpeedGrid"<<endl;
    for(int i=0;i<grid.size();i++)
        for(int j=0;j<grid[i].size();j++){
            cout<<"("<<i<<","<<j<<"): "<<grid[i][j].getAverageSpeed()<<" "<<grid[i][j].getTotalSpeed()<<" "<<grid[i][j].getSpeedDataAmount()<<endl;
        }
}