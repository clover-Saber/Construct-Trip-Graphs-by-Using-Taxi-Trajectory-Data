#ifndef FILE_H
#define FILE_H

#include <fstream>
#include <map>
#include <vector>
#include <string>

#include "Position.h"
#include "Point.h"

//文件处理类
class File{
    private:
        //读文件流
        std::ifstream infile;
        //写文件流
        std::ofstream outfile;
    public:
        //写文件构造函数
        File();
        //读文件构造函数，path：文件路径
        File(std::string path);
        ~File();
        //读取文件中有多少不同的日期数据，并记录每个日期数据的数量，返回数量最多的日期
        std::string readDateMap(std::map<std::string,int> &mapD);
        //读取某一日营运或待运Position并按taxiId划分的map，rightDate为日期
        void readPositionMap(std::map<std::string,std::vector<Position> > &mapP,std::string rightDate);
        //读取某一日Position并按taxiId划分的map，rightDate为日期
        void readPositionMapAll(std::map<std::string,std::vector<Position> > &mapP,std::string rightDate);
        /**
         * @brief 读取Point.txt订单任务,并按时间划分时间片
         * @param 读取的订单结果数组vecP
         * @param 下标数组num
         */
        void readPointVector(std::vector<Point> &vecP,std::vector<int> &num);
        //写出Point数组到文件
        void writePointVector(std::vector<Point> &vecP,char *path);
};

#endif