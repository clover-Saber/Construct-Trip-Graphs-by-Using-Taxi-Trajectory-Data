#include "include/HopcroftKarpAlgorithm.h"
#include "include/Util.h"
using namespace std;

HopcroftKarpAlgorithm::~HopcroftKarpAlgorithm(){
    xMatch.clear();
    yMatch.clear();
    vis.clear();
    xDistance.clear();
    yDistance.clear();
}

bool HopcroftKarpAlgorithm::hopcroftKarpSearch(SharebilityNetwork &shareNet){
    //当前允许的最大增广路长度
    int maxDistance=INF;
    for(int i=0;i<shareNet.vecPoint.size();i++){
        if(xMatch[i]==-1){
            q.push(i);
            xDistance[i] = 0;
        }
    }
    while(!q.empty()){
        int u=q.front();
        q.pop();
        //广度优先搜索，长度超过即可停止
        if(xDistance[u]>=maxDistance) break;
        int i=shareNet.head[u];
        while(i!=-1){
            int v=shareNet.vecEdge[i].getEnd();
            if(yDistance[v]==-1){
                yDistance[v] = xDistance[u]+1;
                int w=yMatch[v];
                if(w==-1) maxDistance = yDistance[v];
                else{
                    xDistance[w] = yDistance[v]+1;
                    q.push(w);
                }
            }
            i = shareNet.vecEdge[i].getNext();
        }
    }
    q = queue<int>();
    if(maxDistance!=INF) return true;
    else return false;
}

bool HopcroftKarpAlgorithm::hopcroftKarpMatch(int u,SharebilityNetwork &shareNet){
    int i=shareNet.head[u];
    while(i!=-1){
        int v=shareNet.vecEdge[i].getEnd();
        if(vis[v]&&yDistance[v]==xDistance[u]+1){
            vis[v] = false;
            int w=yMatch[v];
            if(w==-1||hopcroftKarpMatch(w,shareNet)){
                xMatch[u] = v;
                yMatch[v] = u;
                return true;
            }
        }
        i = shareNet.vecEdge[i].getNext();    
    }    
    return false;
}

int HopcroftKarpAlgorithm::hopcroftKarp(SharebilityNetwork &shareNet){
    start = clock();
    //最大匹配数
    int matchNumber=0;
    xMatch.clear();
    yMatch.clear();
    xDistance.clear();
    yDistance.clear();
    vis.clear();
    //匹配初始化赋值-1，增广路长度初始化赋值-1
    for(int i=0;i<shareNet.vecPoint.size();i++){
        xMatch.push_back(-1);
        yMatch.push_back(-1);
        xDistance.push_back(-1);
        yDistance.push_back(-1);
        vis.push_back(true);
    }
    //能找到增广路
    while(hopcroftKarpSearch(shareNet)){
        for(int i=0;i<shareNet.vecPoint.size();i++)
            //未匹配的点
            if(xMatch[i]==-1){
                if(hopcroftKarpMatch(i,shareNet)) matchNumber++;
            }
        for(int i=0;i<shareNet.vecPoint.size();i++){
            xDistance[i] = -1;
            yDistance[i] = -1;
            vis[i] = true;
        }
    }
    end = clock();
    return matchNumber;
}

double HopcroftKarpAlgorithm::getOptimizeMileage(SharebilityNetwork &shareNet){
    double tot=0;
    for(int i=0;i<xMatch.size();i++){
        if(xMatch[i]!=-1){
            tot += distance(shareNet.vecPoint[i].getEndLongitude(),shareNet.vecPoint[i].getEndLatitude(),shareNet.vecPoint[xMatch[i]].getStartLongitude(),shareNet.vecPoint[xMatch[i]].getStartLatitude());
        }
    }
    return tot;
}