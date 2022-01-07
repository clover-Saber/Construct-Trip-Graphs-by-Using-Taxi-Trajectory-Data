#include <iostream>
#include <cstring>
#include <string>

#include "include/Parameter.h"
#include "include/File.h"
#include "include/HopcroftKarpAlgorithm.h"
#include "include/RoadNetwork.h"
using namespace std;

int main(){
    /*
    //建立路网
    char roadPath[100]="C://Users//stud//Desktop//1.txt";
    RoadNetwork roadNet;
    roadNet.init(roadPath); 
    roadNet.printNetwork();
    */

    for(int i=0;i<FILELIST.size();i++){
        cout<<"--------------------------"<<endl;
        cout<<FILELIST[i]<<endl;
        /*
        //位置信息文件
        char positionPath[100] = "D://yh//VehicleSize//data//";
        strcat(positionPath,f[i].c_str());
        strcat(positionPath,"//part-00000//part-00000.txt");
        File positionFile(-1,positionPath);
        //文件中每个日期的数据量
        //map<string,int> mapDate;
        //string rightDate=positionFile.readDateMap(mapDate);
    
        //数据文件对应的日期
        string rightDate="2015-04-"+f[i];
        //提取订单信息
        PointCollect pc;
        */
        /*
        pc.collectOriginalMileage(positionFile,rightDate);
        cout<<"originalMileage:"<<pc.getOriginalMileage()<<endl;
        */
       /*
        pc.collect(positionFile,rightDate);
        pc.printMapPosition();
        pc.printVecPoint(); 
        
        
        //写出到文件
        File writeFile;
        char writePath[100] = "D://yh//VehicleSize//data//";
        strcat(writePath,f[i].c_str());
        strcat(writePath,"//Point.txt");
        pc.write(writeFile,writePath,0);
        */
        
        //提取后的订单信息文件
        string pointPath = BASEPATH+FILELIST[i]+"//Point.txt";
        File pointFile(pointPath);
    
        SharebilityNetwork shareNet;
        shareNet.init(pointFile);
        shareNet.printVecPoint();
        shareNet.printVecEdge();
        cout<<"ShareNetInit-Time: "<<shareNet.timer.time()<<"s"<<endl;
        
        /*
        //匈牙利算法
        HungaryAlgorithm algorithm1;
        cout<<"Hungary-MaxMatch: "<<algorithm1.hungary(shareNet)<<endl;
        cout<<"Hungary-Time: "<<algorithm1.time()<<endl;
        */
        
        //Hopcroft-Karp算法
        HopcroftKarpAlgorithm algorithm2;
        cout<<"HopcroftKarp-MaxMatch: "<<algorithm2.hopcroftKarp(shareNet)<<endl;
        cout<<"HopcroftKarp-Time: "<<algorithm2.time()<<endl;
        
        //cout<<"OptimizeMileage:"<<pc.getTotalTripMileage()+algorithm2.getOptimizeMileage(shareNet)<<endl;
        
    }
    return 0;
}