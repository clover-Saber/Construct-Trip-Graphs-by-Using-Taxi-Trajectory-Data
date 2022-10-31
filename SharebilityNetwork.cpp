#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>

#include "include/SharebilityNetwork.h"
#include "include/mcmf.h"
#include "include/Parameter.h"
using namespace std;

vector<int> vecAns;

struct SpeedCellIndex{
    int x,y,time;
    SpeedCellIndex(int x,int y,int time) :x(x), y(y), time(time){}
    bool operator < (const SpeedCellIndex& t) const{
        return time > t.time;
    }
};

struct SpeedCellRange{
    //左上角cell下标和右下角cell下标
    int lx,ly,rx,ry;
    int reachableAmount; //可达网格数
    SpeedCellRange(int lx,int ly,int rx,int ry) :lx(lx), ly(ly), rx(rx), ry(ry){}
    SpeedCellRange(int lx,int ly,int rx,int ry,int ra) :lx(lx), ly(ly), rx(rx), ry(ry), reachableAmount(ra){} 
};

Edge::Edge(){

}

Edge::~Edge(){

}

void Edge::init(int x,int y,int z){
    start = x;
    end = y;
    next = z;
}

int Edge::getStart(){
    return start;
}

int Edge::getEnd(){
    return end;
}

int Edge::getNext(){
    return next;
}

void Edge::printEdge(){
    printf("[%d,%d,%d]",start,end,next);
}

SharebilityNetwork::SharebilityNetwork(SpeedGrid *sg){
    speedGrid = sg;
    const int INDEX_AMOUNT = (END_LATITUDE-START_LATITUDE)/CELL_LATITUDE_LENGTH;
    const int INDEY_AMOUNT = (END_LONGITUDE-START_LONGITUDE)/CELL_LONGITUDE_LENGTH;
}

SharebilityNetwork::~SharebilityNetwork(){
    vecPoint.clear();
    vecEdge.clear();
    head.clear();
}

void SharebilityNetwork::readPoint(string filePath){
    ifstream infile;
    infile.open(filePath.c_str(),ios::in);
    Point p;
    double sx,sy,ex,ey; //经纬度
    int st,et; //时间
    int i = 0;
    int j = 0; //时间片下标
    //读取point.txt文件
    while((i<POINT_AMOUNT || POINT_AMOUNT==-1) && infile>>st>>sx>>sy>>et>>ex>>ey){
        p.init(st,sx,sy,et,ex,ey);
        vecPoint.push_back(p);
        i++;
    }
    sort(vecPoint.begin(),vecPoint.end(),cmpStart);
}

int SharebilityNetwork::findRtreeByTime(int t){
    int l = 0, r = slotStartTime.size()-1;
    while(l < r){
        int mid = (l+r)/2;
        if(t >= slotStartTime[mid] && t < slotStartTime[mid+1]) return mid;
        else if(t < slotStartTime[mid]) r = mid-1;
        else l = mid + 1;
    }
    return l;
}

void SharebilityNetwork::fetchReachableSpeedCells(int startT,int startX,int startY,int timeThreshold,int slotAmount){
    int startIndex = findRtreeByTime(startT);
    vector<SpeedCellIndex> speedCellIndexTmp;
    vector<SpeedCellRange> speedCellRangeTmp;
    SpeedCellRange speedCellRangeTmpOne(INDEX_AMOUNT,INDEY_AMOUNT,0,0);
    speedCellLimit.clear();
    speedCellIndexList.clear();
    speedCellRangeList.clear();
    for(int i=0; i<slotAmount; i++){
        speedCellIndexList.push_back(speedCellIndexTmp);
        speedCellRangeList.push_back(speedCellRangeTmp);
        speedCellLimit.push_back(speedCellRangeTmpOne);
    }
    //到达每个cell的最短时间
    vector<vector<int> > minTime;
    vector<int> tmp;
    for(int i=0; i<INDEY_AMOUNT; i++) tmp.push_back(INF);
    for(int i=0; i<INDEX_AMOUNT; i++){
        minTime.push_back(tmp);
    }
    //搜索队列
    priority_queue<SpeedCellIndex> q;
    q.push(SpeedCellIndex(startX,startY,0));
    minTime[startX][startY] = 0;
    while(!q.empty()){
        queueTotalAmount++;
        SpeedCellIndex currentCell = q.top();
        q.pop();
        //因为更新时间更短的cell时，原来的cell未删除，所以需要判断当前是否最新的cell
        if(currentCell.time != minTime[currentCell.x][currentCell.y]) continue;
        //将可达的cell存到队列中
        int sliceIndex = findRtreeByTime(startT+currentCell.time)-startIndex;
        speedCellIndexList[sliceIndex].push_back(currentCell);
        //更新可达范围边界
        if(currentCell.x < speedCellLimit[sliceIndex].lx) speedCellLimit[sliceIndex].lx = currentCell.x;
        if(currentCell.y < speedCellLimit[sliceIndex].ly) speedCellLimit[sliceIndex].ly = currentCell.y;
        if(currentCell.x > speedCellLimit[sliceIndex].rx) speedCellLimit[sliceIndex].rx = currentCell.x;
        if(currentCell.y > speedCellLimit[sliceIndex].ry) speedCellLimit[sliceIndex].ry = currentCell.y;
        //每个方向判断是否可达
        for(int i=0; i<DIRECTION_AMOUNT; i++){
            int nextX = currentCell.x + DIRECTION_X[i];
            int nextY = currentCell.y + DIRECTION_Y[i];
            //边界判断
            if(nextX < 0 || nextX >= INDEX_AMOUNT) continue;
            if(nextY < 0 || nextY >= INDEY_AMOUNT) continue;
            Timer divisionClock;
            //耗时计算，一半在当前cell，一半在下一个cell
            int nextTime = currentCell.time + speedGrid->getCellSpendTime(currentCell.x,currentCell.y,i) + speedGrid->getCellSpendTime(nextX,nextY,i);
            //double halfDistance = DIRECTION_DISTANCE[i];
            //int nextTime = currentCell.time + round(halfDistance/speedGrid->getCellSpeedByMS(currentCell.x,currentCell.y) + halfDistance/speedGrid->getCellSpeedByMS(nextX,nextY));
            divisionCalculationTime += divisionClock.getTimeCost();
            //不超过时间阈值，并且不重复前往耗时更长的cell
            if(nextTime<timeThreshold && nextTime<minTime[nextX][nextY]){
                minTime[nextX][nextY] = nextTime;
                q.push(SpeedCellIndex(nextX,nextY,nextTime));
            }
        }
    }
}

void SharebilityNetwork::fetchReachableSpeedCells_byPriorityQueueMerge(int startT, int startX,int startY,int timeThreshold,int slotAmount){
    int startIndex = findRtreeByTime(startT);
    speedCellRangeList.clear();
    vector<SpeedCellRange> speedCellRangeTmp;
    for(int i=0; i<slotAmount; i++){
        speedCellRangeList.push_back(speedCellRangeTmp);
    }
    //到达每个cell的最短时间
    vector<vector<int> > minTime;
    //每个cell对应的block下标
    vector<vector<int>> blockIndex; 
    vector<int> timeTmp; 
    vector<int> blockTmp;
    for(int i=0; i<INDEY_AMOUNT; i++){
        timeTmp.push_back(INF);
        blockTmp.push_back(-1);
    }
    for(int i=0; i<INDEX_AMOUNT; i++){
        minTime.push_back(timeTmp);
        blockIndex.push_back(blockTmp);
    }
    //当前分钟的全部block
    vector<SpeedCellRange> currentSpeedCellRange;
    //搜索队列
    priority_queue<SpeedCellIndex> q;
    q.push(SpeedCellIndex(startX,startY,0));
    minTime[startX][startY] = 0;
    //时间片标记
    int slotFlag = 0;
    while(!q.empty()){ 
        queueTotalAmount++;
        SpeedCellIndex currentCell = q.top();
        q.pop();
        //因为更新时间更短的cell时，原来的cell未删除，所以需要判断当前是否最新的cell
        if(currentCell.time != minTime[currentCell.x][currentCell.y]) continue;
        //将可达的cell存到队列中
        int slotIndex = findRtreeByTime(startT+currentCell.time)-startIndex;
        //记录前slotFlag个时间片的可达block
        if(slotIndex > slotFlag){
            for(int i=0;i<currentSpeedCellRange.size();i++){
                speedCellRangeList[slotFlag].push_back(currentSpeedCellRange[i]);
            }
            slotFlag++;
        }
        //每个方向判断是否可达
        for(int i=0; i<DIRECTION_AMOUNT; i++){
            int nextX = currentCell.x + DIRECTION_X[i];
            int nextY = currentCell.y + DIRECTION_Y[i];
            //边界判断
            if(nextX < 0 || nextX >= INDEX_AMOUNT) continue;
            if(nextY < 0 || nextY >= INDEY_AMOUNT) continue;
            Timer divisionClock;
            //耗时计算，一半在当前cell，一半在下一个cell
            int nextTime = currentCell.time + speedGrid->getCellSpendTime(currentCell.x,currentCell.y,i) + speedGrid->getCellSpendTime(nextX,nextY,i);
            divisionCalculationTime += divisionClock.getTimeCost();
            //不超过时间阈值，并且不重复前往耗时更长的cell
            if(nextTime<timeThreshold && nextTime<minTime[nextX][nextY]){
                //该cell已合并并且未到达
                if(blockIndex[nextX][nextY] != -1 && minTime[nextX][nextY] == INF){
                    currentSpeedCellRange[blockIndex[nextX][nextY]].reachableAmount++;
                }
                minTime[nextX][nextY] = nextTime;
                q.push(SpeedCellIndex(nextX,nextY,nextTime));
            }
        }
        //将当前cell进行合并，只有上下左右4个方向
        //该cell已合并
        if(blockIndex[currentCell.x][currentCell.y] != -1){
            continue; 
        }
        double maxReachableRate = STANDARD_RATE; //必须高于标准分值
        int maxRateDirection = -1; //最高得分合并方向
        int maxRateBlockIndex = -1; //最高得分block下标
        int maxreachableAmount = -1; //最高得分的可达网格数
        for(int i=0;i<4;i++){
            int neighborX = currentCell.x + DIRECTION_X[i];
            int neighborY = currentCell.y + DIRECTION_Y[i];
            //边界判断
            if(neighborX < 0 || neighborX >= INDEX_AMOUNT) continue;
            if(neighborY < 0 || neighborY >= INDEY_AMOUNT) continue;
            if(blockIndex[neighborX][neighborY] == -1) continue;
            //当前block下标
            int currentBlockIndex = blockIndex[neighborX][neighborY];
            SpeedCellRange *currentBlock = &currentSpeedCellRange[currentBlockIndex];
            int reachableAmount = currentBlock->reachableAmount+1; //block可达网格数
            int totalAmount = (currentBlock->rx-currentBlock->lx+1)*(currentBlock->ry-currentBlock->ly+1); //block网格总数
            bool noDuplicateFlag = true;
            if(DIRECTION_X[i] == 0){
                totalAmount += (currentBlock->rx-currentBlock->lx+1);
                for(int j=currentBlock->lx;j<=currentBlock->rx;j++){
                    //不允许重复网格
                    if(blockIndex[j][currentCell.y] != -1){
                        noDuplicateFlag = false;
                        break;
                    }
                    if(speedGrid->getCellSpeedByMS(j,currentCell.y)<0.01) reachableAmount++;
                }
            }else if(DIRECTION_Y[i] == 0){
                totalAmount += (currentBlock->ry-currentBlock->ly+1);
                for(int j=currentBlock->ly;j<=currentBlock->ry;j++){
                    if(blockIndex[currentCell.x][j] != -1){
                        noDuplicateFlag = false;
                        break;
                    } 
                    if(speedGrid->getCellSpeedByMS(currentCell.x,j)<0.01) reachableAmount++;
                }
            }
            double rate = double(reachableAmount)/double(totalAmount);
            if(noDuplicateFlag && rate > maxReachableRate){
                maxReachableRate = rate;
                maxRateDirection = i;
                maxRateBlockIndex = currentBlockIndex;
                maxreachableAmount = reachableAmount;
            } 
        }
        if(maxRateDirection == -1){
            currentSpeedCellRange.push_back(SpeedCellRange(currentCell.x,currentCell.y,currentCell.x,currentCell.y,1));
            blockIndex[currentCell.x][currentCell.y] = currentSpeedCellRange.size()-1;
        }else{
            if(DIRECTION_X[maxRateDirection] == 0){
                //扩大block边界
                if(DIRECTION_Y[maxRateDirection] == -1){
                    currentSpeedCellRange[maxRateBlockIndex].ry++;
                }else{
                    currentSpeedCellRange[maxRateBlockIndex].ly--;
                }
                //为扩入的cell记录block下标
                for(int j=currentSpeedCellRange[maxRateBlockIndex].lx;j<=currentSpeedCellRange[maxRateBlockIndex].lx;j++){
                    blockIndex[j][currentCell.y] = maxRateBlockIndex;
                }
            }else if(DIRECTION_Y[maxRateDirection] == 0){
                if(DIRECTION_X[maxRateDirection] == -1){
                    currentSpeedCellRange[maxRateBlockIndex].rx++;
                }else{
                    currentSpeedCellRange[maxRateBlockIndex].lx--;
                }
                for(int j=currentSpeedCellRange[maxRateBlockIndex].ly;j<=currentSpeedCellRange[maxRateBlockIndex].ry;j++){
                    blockIndex[currentCell.x][j] = maxRateBlockIndex;
                }
            }
            //可达网格数量更新
            currentSpeedCellRange[maxRateBlockIndex].reachableAmount = maxreachableAmount;
        }
    }
    for(int i=0;i<currentSpeedCellRange.size();i++){
        speedCellRangeList[slotFlag].push_back(currentSpeedCellRange[i]);
    }
}

void SharebilityNetwork::buildNetworkFromFile(string filePath){
    Timer buildClock;
    readPoint(filePath);
    // 构建R树,共有slotStartTime.size棵
    int i=0; // R树下标
    slotStartTime.push_back(0);
    for(int j=0;j<vecPoint.size();j++){
        if(j != 0 && j % SLOT_TRIPS == 0){
            i++;
            slotStartTime.push_back(vecPoint[j].getStartTime());
        }
        //节点
        Rect rt(vecPoint[j].getStartLatitude(),vecPoint[j].getStartLongitude());
        vecTree[i].Insert(rt.min,rt.max,j);
    }
    //初始化赋值-1
    for(int i=0;i<vecPoint.size();i++) head.push_back(-1);
    //连接单向边
    for(int i=0;i<vecPoint.size();i++){
        // R树搜索结果清空
        vecAns.clear();
        int endTime=vecPoint[i].getEndTime();
        int index = findRtreeByTime(endTime);
        // 不能超过当天24时
        int timeThreshold = min(24*60*60-endTime, MAX_TIME_THRESHOLD);
        // 查询的时间片数量
        int slotAmount = findRtreeByTime(endTime+timeThreshold)-index+1;
        for(int j=0;j<slotAmount;j++){
            int indey = index+j;
            int slotTime = 24*60*60;
            if(indey+1 < slotStartTime.size()) slotTime = slotStartTime[indey+1];
            searchRtree(vecTree[indey], slotTime, endTime, vecPoint[i].getEndLongitude(), vecPoint[i].getEndLatitude());
        }
        //建边
        for(int j=0;j<vecAns.size();j++)
            if(existEdge(i,vecAns[j])) addEdge(i,vecAns[j]);
    }
    cout<<"[SharebilityNetWork] - Cost of time: "<<buildClock.getTimeCost()<<"s"<<endl;
}

void SharebilityNetwork::cellQuery(int index, int slotAmount){
    for(int j=0; j<slotAmount; j++){
        int indey = index+j;
        //从第index分钟到第indey分钟的全部网格
        for(int k=0; k<=j; k++){
            blockTotalAmount += speedCellIndexList[k].size();
            for(int z=0; z<speedCellIndexList[k].size(); z++){
                int x = speedCellIndexList[k][z].x;
                int y = speedCellIndexList[k][z].y;
                double minLat = START_LATITUDE+CELL_LATITUDE_LENGTH*x;
                double minLon = START_LONGITUDE+CELL_LONGITUDE_LENGTH*y;
                double maxLat = minLat+CELL_LATITUDE_LENGTH;
                double maxLon = minLon+CELL_LONGITUDE_LENGTH;  
                Rect searchRect(minLat,minLon,maxLat,maxLon);
                vecTree[indey].Search(searchRect.min, searchRect.max, MySearchCallback, NULL);
            }
        }
    }
}

void SharebilityNetwork::cellQuery_byPriorityQueueMerge(int index,int slotAmount){
    for(int j=0; j<slotAmount; j++){
        int indey = index+j;
        blockTotalAmount += speedCellRangeList[j].size();
        for(int k=0;k<speedCellRangeList[j].size();k++){
            int lx = speedCellRangeList[j][k].lx;
            int ly = speedCellRangeList[j][k].ly;
            int rx = speedCellRangeList[j][k].rx;
            int ry = speedCellRangeList[j][k].ry;
            double minLat = START_LATITUDE+CELL_LATITUDE_LENGTH*lx;
            double minLon = START_LONGITUDE+CELL_LONGITUDE_LENGTH*ly;
            double maxLat = minLat+CELL_LATITUDE_LENGTH*(rx-lx+1);
            double maxLon = minLon+CELL_LONGITUDE_LENGTH*(ry-ly+1);  
            Rect searchRect(minLat,minLon,maxLat,maxLon);
            vecTree[indey].Search(searchRect.min, searchRect.max, MySearchCallback, NULL);
        }
    }
}

void SharebilityNetwork::buildNetworkFromFile_bySpeedGrid(string filePath){
    divisionCalculationTime = 0;
    double findTime1 = 0;
    double findTime2 = 0;
    double findTime3 = 0;
    double rtreeTime = 0;
    blockTotalAmount = 0;
    queueTotalAmount = 0;
    Timer buildClock;
    readPoint(filePath);
    // 构建R树,共有slotStartTime.size棵
    Timer rtreeClock;
    int i=0; // R树下标
    slotStartTime.push_back(0);
    for(int j=0;j<vecPoint.size();j++){
        if(j != 0 && j % SLOT_TRIPS == 0){
            i++;
            slotStartTime.push_back(vecPoint[j].getStartTime());
        }
        //节点
        Rect rt(vecPoint[j].getStartLatitude(),vecPoint[j].getStartLongitude());
        vecTree[i].Insert(rt.min,rt.max,j);
    }
    rtreeTime = rtreeClock.getTimeCost();
    //初始化赋值-1
    for(int i=0;i<vecPoint.size();i++) head.push_back(-1);
    //连接单向边
    for(int i=0;i<vecPoint.size();i++){
        //R树搜索结果清空
        vecAns.clear();
        int endTime = vecPoint[i].getEndTime();
        int index = findRtreeByTime(endTime);
        // 不能超过当天24时
        int timeThreshold = min(24*60*60-endTime, MAX_TIME_THRESHOLD);
        // 查询的时间片数量
        int slotAmount = findRtreeByTime(endTime+timeThreshold)-index+1;
        //位置所在速度cell下标
        int startX = (vecPoint[i].getEndLatitude()-START_LATITUDE)/CELL_LATITUDE_LENGTH;
        int startY = (vecPoint[i].getEndLongitude()-START_LONGITUDE)/CELL_LONGITUDE_LENGTH;
        //第一步：搜索全部可达网格
        Timer findClock1;
        //fetchReachableSpeedCells(startX,startY,currentSliceAmount);
        fetchReachableSpeedCells_byPriorityQueueMerge(endTime, startX, startY, timeThreshold, slotAmount);
        findTime1 += findClock1.getTimeCost();
        //第二步：查询每一个可达网格
        Timer findClock2;
        //方法1：一个一个查询
        //cellQuery(index,slotAmount);
        //方法2：优先队列合并搜索
        cellQuery_byPriorityQueueMerge(index, slotAmount);
        findTime2 +=  findClock2.getTimeCost();
        //第三步：连边 
        Timer findClock3;
        for(int j=0;j<vecAns.size();j++) 
            if(existEdge(i,vecAns[j])) addEdge(i,vecAns[j]);
        findTime3 +=  findClock3.getTimeCost();
    }
    cout<<"rtreeTime: "<<rtreeTime<<endl;
    cout<<"findTime1: "<<findTime1<<endl;
    cout<<"divisionCalculationTime: "<<divisionCalculationTime<<endl;
    cout<<"findTime2: "<<findTime2<<endl;
    cout<<"findTime3: "<<findTime3<<endl;
    cout<<"blockTotalAmount: "<<blockTotalAmount<<endl; 
    cout<<"Amount of all priority queues: "<<queueTotalAmount<<endl;
    cout<<"[SharebilityNetWork] - Cost of time: "<<buildClock.getTimeCost()<<"s"<<endl;
}

void SharebilityNetwork::addEdge(int x,int y){
    Edge e;
    int t = head[x];
    head[x] = vecEdge.size();
    e.init(x,y,t);
    vecEdge.push_back(e);
}

bool SharebilityNetwork::existEdge(int x,int y){
    //两个订单时间间隔=y开始时间-x结束时间
    int t=vecPoint[y].getStartTime()-vecPoint[x].getEndTime();
    //x时间要比y早，且时间间隔不能大于规定时限
    if(t <= 0 || t > MAX_TIME_THRESHOLD) return false;
    /*
    //时间内车能从x点到达y点,设定汽车平均行驶速度36.8km/h,即11m/s
    if(distance(vecPoint[x].getEndLongitude(),vecPoint[x].getEndLatitude(),vecPoint[y].getStartLongitude(),vecPoint[y].getEndLatitude())>11.0*t){
        return false;
    }*/
    return true;
}

void SharebilityNetwork::outputEdges(std::string filePath){
    ofstream outfile;
    outfile.open(filePath.c_str(),ios::out);
    for(int i=0;i<vecEdge.size();i++){
        outfile<<vecEdge[i].getStart()<<' '<<vecEdge[i].getEnd()<<endl;
    }
    outfile.close();
}

void SharebilityNetwork::printNetwork(){
    cout<<"Amount of Points: "<<vecPoint.size()<<endl;
    cout<<"Amount of Edges: "<<vecEdge.size()<<endl;
}