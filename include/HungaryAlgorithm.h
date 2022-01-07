#ifndef _HUNGARYALGORITHM_H
#define _HUNGARYALGORITHM_H
#include <vector>
#include <ctime>
#include "SharebilityNetwork.h"

//匈牙利算法求最大匹配
class HungaryAlgorithm{
    private:
        //匹配：i的匹配为match[i]，x、y分别为二分图两个点集
        std::vector<int> xMatch;
        std::vector<int> yMatch;
        //重复标记数组
        std::vector<bool> vis;
        //计时器
        clock_t start,end;
    public:
        HungaryAlgorithm(){};
        ~HungaryAlgorithm();
        //匈牙利算法中的匹配函数
        bool hungaryMatch(int u,SharebilityNetwork &shareNet);
        //匈牙利算法求最大匹配
        int hungary(SharebilityNetwork &shareNet);
        //花费时间，单位为秒
        double time(){return double(end-start)/1000;};
};

#endif