#include <iostream>
#include <iomanip>

#include "include/File.h"
#include "include/Parameter.h"
using namespace std;

File::File(){

}

File::File(string path){
    //打开文件用于读取
    infile.open(path.c_str(),ios::in);
}

File::~File(){
    //关闭文件
    infile.close();
}

string File::readDateMap(map<string,int> &mapD){
    string s;
    Position p;
    //已读取个数
    int i = 0;
    //读取一行string
    getline(infile,s);
    //n=-1表示全部读取
    while(s!="" && (i<POINTNUMBER || POINTNUMBER==-1)){
        //查看进度
        if(i%10000000==0) cout<<"read:"<<i<<endl;
        p.init(s);
        //map中没有，则将其加入map
        if(mapD.find(p.getReceiveDate())==mapD.end()) {
            mapD.insert(pair<string,int>(p.getReceiveDate(),1));
        }
        else mapD[p.getReceiveDate()]++;
        getline(infile,s);
        i++;
    }
    //找到数量最大的
    int res=0;
    string ans="";
    map<string,int>::iterator it;
    for(it=mapD.begin();it!=mapD.end();it++){
        if(it->second>res){
            res = it->second;
            ans = it->first;
        }
    }
    return ans;
}


void File::readPositionMap(map<string,vector<Position> > &mapP,string rigthDate){
    map<string,int> mapT;
    string s;
    Position p;
    //已读取个数
    int i = 0;
    //读取一行string
    getline(infile,s);
    //n=-1表示全部读取
    while(s!="" && (i<POINTNUMBER || POINTNUMBER==-1)){
        //查看进度
        if(i%10000000==0) cout<<"read:"<<i<<endl;
        p.init(s);
        //业务状态正常，是所要求日期，顶灯状态为运营或待运
        if(p.getServiceStatus()=='0'&&p.getReceiveDate()==rigthDate&&(p.getLightStatus()=='0'||p.getLightStatus()=='1')){
            //map中没有此车号，将其加入map
            if(mapP.find(p.getTaxiId())==mapP.end()){
                //只取3000辆车的数据
                if(mapP.size()<50000){
                    vector<Position> vecP;
                    vecP.push_back(p);
                    mapP.insert(pair<string,vector<Position> >(p.getTaxiId(),vecP));
                }
            }
            else mapP[p.getTaxiId()].push_back(p);
        }
        getline(infile,s);
        i++;
    }
}

void File::readPositionMapAll(map<string,vector<Position> > &mapP,string rigthDate){
    map<string,int> mapT;
    string s;
    Position p;
    //已读取个数
    int i = 0;
    //读取一行string
    getline(infile,s);
    //n=-1表示全部读取
    while(s!="" && (i<POINTNUMBER || POINTNUMBER==-1)){
        //查看进度
        if(i%10000000==0) cout<<"read:"<<i<<endl;
        p.init(s);
        //业务状态正常，是所要求日期
        if(p.getServiceStatus()=='0'&&p.getReceiveDate()==rigthDate){
            //map中没有此车号，将其加入map
            if(mapP.find(p.getTaxiId())==mapP.end()){
                //只取3000辆车的数据
                if(mapP.size()<50000){
                    vector<Position> vecP;
                    vecP.push_back(p);
                    mapP.insert(pair<string,vector<Position> >(p.getTaxiId(),vecP));
                }
            }
            else mapP[p.getTaxiId()].push_back(p);
        }
        getline(infile,s);
        i++;
    }
}

void File::readPointVector(vector<Point> &vecP,vector<int> &num){
    vector<vector<Point>> tp;
    vector<Point> t;
    for(int i=0; i<24*60*60/SLICETIME; i++) tp.push_back(t);
    Point p;
    double sx,sy,ex,ey; //经纬度
    int st,et; //时间
    int i = 0;
    //读取point.txt文件
    while((i<POINTNUMBER || POINTNUMBER==-1) && infile>>st>>sx>>sy>>et>>ex>>ey){
        p.init(st,sx,sy,et,ex,ey);
        int order = st/SLICETIME;
        tp[order].push_back(p);
        i++;
    }
    //将二维数组合并为一维数组，并记录下标
    num.push_back(0);
    for(int j=0;j<tp.size();j++){
        vecP.insert(vecP.end(),tp[j].begin(),tp[j].end());
        num.push_back(vecP.size());
    }
}

void File::writePointVector(vector<Point> &vecP,char *path){
    outfile.open(path);
    //9位有效数字
    outfile<<setprecision(9);
    for(int i=0;i<vecP.size();i++){
        outfile<<vecP[i].getStartTime()<<' '<<vecP[i].getStartLongitude()<<' '<<vecP[i].getEndLatitude()<<' ';
        outfile<<vecP[i].getEndTime()<<' '<<vecP[i].getEndLongitude()<<' '<<vecP[i].getEndLatitude()<<endl;
    }
    outfile.close();
}