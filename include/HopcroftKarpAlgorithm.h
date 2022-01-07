#ifndef _HOPCROFTKARPALGORITHM_H
#define _HOPCROFTKARPALGORITHM_H
#include <vector>
#include <queue>
#include "SharebilityNetwork.h"

class HopcroftKarpAlgorithm{
    private:
        //匹配：i的匹配为match[i]，x、y分别为二分图两个点集
        std::vector<int> xMatch;
        std::vector<int> yMatch;
        //重复标记数组
        std::vector<bool> vis;
        //增广路长度
        std::vector<int> xDistance;
        std::vector<int> yDistance;
        //BFS容器
        std::queue<int> q;
        static const int INF=10000007;
        //计算器
        clock_t start,end;
    public:
        HopcroftKarpAlgorithm(){};
        ~HopcroftKarpAlgorithm();
        //Hopcroft-Karp算法中查找增广路函数
        bool hopcroftKarpSearch(SharebilityNetwork &shareNet);
        //Hopcroft-Karp算法中匹配函数
        bool hopcroftKarpMatch(int u,SharebilityNetwork &shareNet);
        //Hopcroft-Karp算法
        int hopcroftKarp(SharebilityNetwork &shareNet);
        //计算各个行程的连接里程
        double getOptimizeMileage(SharebilityNetwork &shareNet);
        //花费时间，单位为秒
        double time(){return double(end-start)/1000;};
};

#endif