//相关代码参数

#ifndef PARAMETER_H
#define PARAMETER_H

#include<string>
#include<vector>

//文件基础路径
const std::string BASEPATH = "F:data//";
//待处理文件相对路径
const std::vector<std::string> FILELIST = {"01"};
//const vector<std::string> FILELIST = {"01","04","07","08","09","10","11","12","14","15","18","24"};

//订单任务选取数量，-1为全部
const int POINTNUMBER = 1000;

//时间片划分时间/秒
const int SLICETIME = 60;

#endif