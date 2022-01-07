#include <iostream>
#include <fstream>
#include <queue>
#include <cmath>

#include "include/RoadNetwork.h"
using namespace std;


void Crossing::init(double lon,double lat){
    longitude = lon;
    latitude = lat;
}

void Road::init(int s,int e,int nt,double len){
    start = s;
    end = e;
    next = nt;
    length = len;    
}

RoadNetwork::~RoadNetwork(){
    vecCrossing.clear();
    vecRoad.clear();
    head.clear();
    dis.clear();
}

int RoadNetwork::findCrossing(double lon,double lat){
    for(int i=0;i<vecCrossing.size();i++)
        //同一个路口，绝对值！！！
        if(abs(lon-vecCrossing[i].getLongitude())<1e-9&&abs(lat-vecCrossing[i].getLatitude())<1e-9){
            return i;
        }
    //添加路口
    Crossing c;
    c.init(lon,lat);
    vecCrossing.push_back(c);
    //初值-1
    head.push_back(-1);
    return vecCrossing.size()-1;
}

void RoadNetwork::addRoad(int s,int e,double len){
    Road r;
    int t = head[s];
    head[s] = vecRoad.size();
    r.init(s,e,t,len);
    vecRoad.push_back(r);
}

void RoadNetwork::init(char *path){
    ifstream file;
    file.open(path);
    double sx,sy,ex,ey,len;
    while(file>>sx>>sy>>ex>>ey>>len){
        //cout<<sx<<' '<<sy<<' '<<ex<<' '<<ey<<' '<<len<<endl;
        int u = findCrossing(sx,sy);
        int v = findCrossing(ex,ey);
        //双向道路
        addRoad(u,v,len);
        addRoad(v,u,len);
    }
    file.close();
}

void RoadNetwork::ping(int x){
    //是否能到达
    vector<bool> vis;
    for(int i=0;i<vecCrossing.size();i++) vis.push_back(false);
    //可到达路口
    queue<int> q;
    //从路口0出发，能否到达所有路口
    q.push(x);
    while(!q.empty()){
        int u = q.front();
        q.pop();
        vis[u] = true;
        int i = head[u];
        while(i!=-1){
            int v =  vecRoad[i].getEnd();
            if(!vis[v]) q.push(v);
            
            i = vecRoad[i].getNext();
        }
    }
    int num=0;
    for(int i=0;i<vis.size();i++) 
        if(vis[i]) num++;
    cout<<"ping: "<<x<<' '<<vis.size()<<' '<<num<<endl; 
}

//TODO
double RoadNetwork::distance(double sx,double sy,double ex,double ey){
    //初值
    for(int i=0;i<vecCrossing.size();i++) dis.push_back(INFDIS);
    //起点终点
    int u,v;

    dis.clear();
    return 0;
}

void RoadNetwork::printNetwork(){
    cout<<"Crossing Number: "<<vecCrossing.size()<<endl;
    cout<<"Road Number: "<<vecRoad.size()<<endl;
}