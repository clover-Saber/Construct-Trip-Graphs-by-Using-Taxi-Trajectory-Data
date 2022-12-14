#include <iostream>
#include <iomanip>

#include "include/SpeedGrid.h"
#include "include/Parameter.h"
#include "include/Position.h"
#include "include/Util.h"
using namespace std;

SpeedCell::SpeedCell(){
    averageSpeed = 0;
    totalSpeed = 0;
    speedDataAmount = 0;
    zeroAmount = 0;
    for(int i=0;i<DIRECTION_AMOUNT;i++) spendTime.push_back(0);
}

SpeedCell::~SpeedCell(){
}

void SpeedCell::init(double avesp,double totsp,int amount){
    averageSpeed = avesp;
    totalSpeed = totsp;
    speedDataAmount = amount;
    for(int i=0;i<DIRECTION_AMOUNT;i++) spendTime[i] = round(DIRECTION_DISTANCE[i]/averageSpeed*3.6);
}

void SpeedCell::addSpeedData(double speedDataOne){
    totalSpeed += speedDataOne;
    speedDataAmount++;
    if(speedDataOne == 0) zeroAmount++;
}

void SpeedCell::calculateAverageSpeed(){
    if(speedDataAmount == 0) averageSpeed = 0.001;
    else averageSpeed = totalSpeed/speedDataAmount;
    for(int i=0;i<DIRECTION_AMOUNT;i++) spendTime[i] = round(DIRECTION_DISTANCE[i]/averageSpeed*3.6);
}

int SpeedCell::getSpendTime(int i){
    return spendTime[i];
}

double SpeedCell::getAverageSpeed(){
    return averageSpeed;
}

double SpeedCell::getTotalSpeed(){
    return totalSpeed;
}

int SpeedCell::getSpeedDataAmount(){
    return speedDataAmount;
}

int SpeedCell::getZeroAmount(){
    return zeroAmount;
}

SpeedGrid::SpeedGrid(){
    SpeedCell cellOne;
    vector<SpeedCell> cellVector;
    for(int i=0;i<INDEY_AMOUNT;i++) cellVector.push_back(cellOne);
    for(int i=0;i<INDEX_AMOUNT;i++) grid.push_back(cellVector);
    for(int i=0;i<20;i++) speedAmount[i] = 0;
    zeroSpeedAmount = 0;
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
        if(i%10000000==0) cout<<"read:"<<i<<endl;
        p.init(s);
        //εδΈεζ΄
        int latitudeIndex = floor((p.getLatitude()-START_LATITUDE)/CELL_LATITUDE_LENGTH);
        int longitudeIndex = floor((p.getLongitude()-START_LONGITUDE)/CELL_LONGITUDE_LENGTH);
        //δΈζ ζͺεΊη
        if(latitudeIndex>=0 && latitudeIndex<grid.size() && longitudeIndex>=0 && longitudeIndex<grid[latitudeIndex].size()){
            if(p.getLightStatus() == '0' || p.getLightStatus() == '1'){
                //ιεΊ¦δΈθ½δΈΊ0
                if(p.getSpeed() > 1e-5){
                    grid[latitudeIndex][longitudeIndex].addSpeedData(p.getSpeed());
                }
            }
        }
        getline(infile,s);
        i++;
    }
    //θ?‘η?η½ζ ΌεΉ³ειεΊ¦
    for(int i=0;i<grid.size();i++)
        for(int j=0;j<grid[i].size();j++){
            grid[i][j].calculateAverageSpeed();
        }
    cout<<"[SpeedGrid] - Cost of time: "<<updateClock.getTimeCost()<<"s ["<<filePath<<"]"<<endl;
}

void SpeedGrid::readGridByFile(string filePath){
    ifstream infile;
    infile.open(filePath.c_str(),ios::in);
    int n,m;
    infile>>n>>m;
    for(int i=0;i<n;i++)
        for(int j=0;j<m;j++){
            double avesp,totsp;
            int amount;
            infile>>avesp>>totsp>>amount;
            grid[i][j].init(avesp,totsp,amount);
        }
}

int SpeedGrid::getCellSpendTime(int x,int y,int i){
    return grid[x][y].getSpendTime(i);
}

double SpeedGrid::getCellSpeedByMS(int x,int y){
    return grid[x][y].getAverageSpeed()/3.6;
}

void SpeedGrid::writeSpeedGirdToFile(string filePath){
    std::ofstream outfile;
    outfile.open(filePath);
    outfile<<setprecision(9); //9δ½ζζζ°ε­
    outfile<<grid.size()<<' '<<grid[0].size()<<endl;
    for(int i=0;i<grid.size();i++){
        for(int j=0;j<grid[i].size();j++){
            outfile<<grid[i][j].getAverageSpeed()<<' '<<grid[i][j].getTotalSpeed()<<' '<<grid[i][j].getSpeedDataAmount()<<endl;
        }
    }
    outfile.close();
}

void SpeedGrid::printSpeedGrid(){
    cout<<"print SpeedGrid"<<endl;
    /*
    for(int i=0;i<grid.size();i++)
        for(int j=0;j<grid[i].size();j++){
            cout<<"("<<i<<","<<j<<"): "<<grid[i][j].getAverageSpeed()<<" "<<grid[i][j].getTotalSpeed()<<" ";
            cout<<grid[i][j].getSpeedDataAmount()<<" "<<grid[i][j].getZeroAmount()<<endl;
        }
    */
   for(int i=0;i<grid.size();i++)
        for(int j=0;j<grid[i].size();j++){
            //η»θ?‘δΈειεΊ¦ζ?΅ηη½ζ Όζ°
            if(grid[i][j].getAverageSpeed()<0 || grid[i][j].getAverageSpeed()>200) continue;
            if(grid[i][j].getAverageSpeed()-0.001 < 1e-6){
                zeroSpeedAmount++;
                continue;
            }
            int t = floor(grid[i][j].getAverageSpeed()/10);
            speedAmount[t]++;
        }
   cout<<"0 "<<zeroSpeedAmount<<endl;
   for(int i=0;i<20;i++) cout<<i*10<<"~"<<i*10+10<<" "<<speedAmount[i]<<endl;
}