#ifndef _POSITION_H
#define _POSITION_H
#include <string>

//出租车信息类
class Position{
    private:
        //车机号
        std::string taxiId;
        //业务状态 0:正常，1：报警
        char serviceStatus;
        //载客状态 0：重车，1：空车
        char passengerStatus;
        //顶灯状态 0：营运，1：待运，2：电调，3：暂停，4：求助，5：停运
        char lightStatus;
        //道路状态 0：地面道路，1：快速道路
        char roadStatus;
        //刹车状态0：无刹车，1：刹车
        char brakeStatus;
        //接收日期 yyyy-mm-dd
        std::string receiveDate;
        //接收时间hh:mm:ss
        std::string receiveTime;
        //GPS日期 yyyy-mm-dd
        std::string gpsDate;
        //GPS时间 hh:mm:ss
        std::string gpsTime;
        //经度 float精度不行
        double longitude;
        //纬度
        double latitude;
        //速度
        float speed;
        //方向
        float direction;
        //卫星数
        int satelliteNumber;
        //时间，转换为秒，xxx秒
        int time;
    public:
        Position(){};
        ~Position(){};
        //重载<
        bool operator < (const Position &t){return time<t.time;}
        //初始化，参数s形如：18834,0,0,0,0,0,2015-04-01 14:25:27,2015-04-01 14:25:21,121.441895,31.206928,0.0,228.0,7
        //或形如：27112,0,1,1,0,0,2015-04-01 00:00:05,2015-04-01,121.487605,31.119665,15.4,75.0,8
        //21027,0,1,V,0,0,2015-04-01 02:09:51,2010-01-01,121.368602,31.255895,30.5,0.0,0
        void init(std::string s);
        //返回taxiId
        std::string getTaxiId(){return taxiId;}
        //返回业务状态
        char getServiceStatus(){return serviceStatus;}
        //返回顶灯状态
        char getLightStatus(){return lightStatus;}
        //返回GPS日期
        std::string getReceiveDate(){return receiveDate;}
        //返回经度
        double getLongitude(){return longitude;}
        //返回纬度
        double getLatitude(){return latitude;}
        //返回时间
        int getTime(){return time;}
        //打印类内容
        void printPosition();
};

#endif