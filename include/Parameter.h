#ifndef PARAMETER_H
#define PARAMETER_H

#include<string>
#include<vector>

//数据文件基础路径
const std::string BASE_PATH = "D://BaiduNetdiskDownload//";
//原始位置文件相对路径
const std::vector<std::string> POSITION_FILE_LIST = {"01"};
//const vector<std::string> FILELIST = {"01","04","07","08","09","10","11","12","14","15","18","24"};

//数据清洗规模，-1为全部
const int POSITION_CLEAN_AMOUNT = 1000000;
const int SIMPLE_POSITION_READ_AMOUNT = -1;

//订单排序方式，0不排序，1按开始时间排序，2按结束时间排序
const int POINT_SORT_METHOD = 0;

//时间片划分时间/秒
const int SLICE_TIME = 60;
const int SLICE_AMOUNT = 20;
//订单任务选取数量，-1为全部
const int POINT_AMOUNT = 10000;
//线程数
const int THREAD_AMOUNT = 4;
//搜索方向
const int DIRECTION_AMOUNT = 4;
const int DIRECTION_X[] = {0,0,-1,1};
const int DIRECTION_Y[] = {-1,1,0,0};
const int INF = 100000007;

//速度网格相关参数
//经度相等的情况下,纬度每隔1度，距离相差约111320米
const int EVERY_LATITUDE_DISTANCE = 111320;
const int EVERY_LONGITUDE_DISTANCE = 100000;
const double CELL_LATITUDE_LENGTH = 0.1;
const double CELL_LONGITUDE_LENGTH = 0.1;
const double START_LATITUDE = 30.666;
const double END_LATITUDE = 31.884;
const double START_LONGITUDE = 120.866;
const double END_LONGITUDE = 122.200;
//构建网格的文件相对路径
const std::vector<std::string> SPEED_GRID_FILE_LIST = {"01"};

#endif