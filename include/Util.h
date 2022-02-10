#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <ctime>
#include <string>

#include "Point.h"
#include "RTree.h"

double distance1(double lon1,double lat1,double lon2,double lat2);
double distance(double lon1,double lat1,double lon2,double lat2);

//以起始时间排序
bool cmpStart(Point &x,Point &y);
//以结束时间排序
bool cmpEnd(Point &x,Point &y);

//R树节点
class Rect{
  public:
    double min[2];
    double max[2];
    Rect();
    Rect(double x, double y);
    //区块
    Rect(double a_minX, double a_minY, double a_maxX, double a_maxY);
    ~Rect();
};

//R树范围搜索结果
extern std::vector<int> vecAns;
//R树回调函数
bool MySearchCallback(int id, void* arg);

void nothing();

//R树搜索，多线程
void searchRtree(RTree<int,double,2, double> &rtree,int j,int endTime,double lon,double lat);

/**
 * @brief 计时器
 */
class Timer{
  private:
    clock_t start;
  public:
    Timer();
    ~Timer();
    /**
     * @return 时间间隔,单位/秒
     */
    double getTimeCost();
};

int convertDateToInt(std::string date);
int convertTimeToInt(std::string time);

#endif