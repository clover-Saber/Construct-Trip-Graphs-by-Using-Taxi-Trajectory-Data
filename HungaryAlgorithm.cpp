#include "include/HungaryAlgorithm.h"

HungaryAlgorithm::~HungaryAlgorithm(){
    xMatch.clear();
    yMatch.clear();
    vis.clear();
}

bool HungaryAlgorithm::hungaryMatch(int u,SharebilityNetwork &shareNet){
    vis[u] = false;
    int i=shareNet.head[u];
    while(i!=-1){
        int v=shareNet.vecEdge[i].getEnd();
        int w=yMatch[v];
        if((w<0)||(vis[w]&&hungaryMatch(w,shareNet))){
            xMatch[u]=v;
            yMatch[v]=u;
            return true;
        }
        i = shareNet.vecEdge[i].getNext();
    }
    return false;
}

int HungaryAlgorithm::hungary(SharebilityNetwork &shareNet){
    start = clock();
    int matchNumber=0;
    //匹配初始化赋值-1
    for(int i=0;i<shareNet.vecPoint.size();i++){
        xMatch.push_back(-1);
        yMatch.push_back(-1);
    }
    for(int i=0;i<shareNet.vecPoint.size();i++){
        vis.clear();
        for(int j=0;j<shareNet.vecPoint.size();j++) vis.push_back(true);
        if(hungaryMatch(i,shareNet)) matchNumber++;
    }
    end = clock();
    return matchNumber;
}