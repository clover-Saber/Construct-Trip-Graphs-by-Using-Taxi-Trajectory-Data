#ifndef _MINIMUMCOST_H
#define _MINIMUMCOST_H

#include <string>
#include <vector>
#include "HopcroftKarpAlgorithm.h"
#include "mcmf.h"
#include "RoadNetwork.h"

class MinimumCost{
    private:
        std::vector<Crossing> crossingVector; // 路口经纬度信息
        std::vector<std::vector<double>> costVector; // 两点之间的花费
        // 初始化cost数组
        void initCost(std::string filePath);
        // 初始化crossing数组
        void initCrossing(std::string filePath);
        // 找到某位置最近的路口id, 参数: 经度，纬度
        int findNearestCrossing(double x, double y);
    public:
        MinimumCost(){};
        ~MinimumCost();
        /**
         * @brief 计算最小花费
         * 
         * @param costFilePath 花费文件
         * @param crossingFilePath 路口文件
         * @param shareNet 出行任务图
         * @param flow 源节点流量
         * @return double 最小花费
         */
        double calculateMinimumCostWhenThisFlow(std::string costFilePath, std::string crossingFilePath, SharebilityNetwork &shareNet, int flow);
};

#endif
