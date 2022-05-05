#include <iostream>
#include <fstream>
#include <queue>
#include <thread>

#include "include/SharebilityNetwork.h"
#include "include/Parameter.h"
#include "include/Util.h"
using namespace std;

vector<int> vecAns;

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

void SharebilityNetwork::readPoint(string filePath){
    ifstream infile;
    infile.open(filePath.c_str(),ios::in);
    vector<vector<Point>> tp;
    vector<Point> t;
    for(int i=0; i<24*60*60/SLICE_TIME; i++) tp.push_back(t);
    Point p;
    double sx,sy,ex,ey; //经纬度
    int st,et; //时间
    int i = 0;
    //读取point.txt文件
    while((i<POINT_AMOUNT || POINT_AMOUNT==-1) && infile>>st>>sx>>sy>>et>>ex>>ey){
        p.init(st,sx,sy,et,ex,ey);
        int order = st/SLICE_TIME;
        tp[order].push_back(p);
        i++;
    }
    //将二维数组合并为一维数组，并记录下标
    orderNumber.push_back(0);
    for(int j=0;j<tp.size();j++){
        vecPoint.insert(vecPoint.end(),tp[j].begin(),tp[j].end());
        orderNumber.push_back(vecPoint.size());
    }
}

void SharebilityNetwork::fetchReachableSpeedCells(int startX,int startY,int currentSliceAmount){
    SpeedCellRange speedCellRangeTmpOne(INDEX_AMOUNT,INDEY_AMOUNT,0,0);
    speedCellLimit.clear();
    for(int i=0; i<SLICE_AMOUNT; i++){
        speedCellIndexList[i].clear();
        speedCellRangeList[i].clear();
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
        int sliceIndex = currentCell.time/SLICE_TIME;
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
            if(nextTime<SLICE_TIME*currentSliceAmount && nextTime<minTime[nextX][nextY]){
                minTime[nextX][nextY] = nextTime;
                q.push(SpeedCellIndex(nextX,nextY,nextTime));
            }
        }
    }
}

void SharebilityNetwork::fetchReachableSpeedCells_byPriorityQueueMerge(int startX,int startY,int currentSliceAmount){
    for(int i=0; i<SLICE_AMOUNT; i++){
        speedCellRangeList[i].clear();
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
    int sliceFlag = 0;
    while(!q.empty()){ 
        queueTotalAmount++;
        SpeedCellIndex currentCell = q.top();
        q.pop();
        //因为更新时间更短的cell时，原来的cell未删除，所以需要判断当前是否最新的cell
        if(currentCell.time != minTime[currentCell.x][currentCell.y]) continue;
        //将可达的cell存到队列中
        int sliceIndex = currentCell.time/SLICE_TIME;
        //记录前sliceFlag个时间片的可达block
        if(sliceIndex > sliceFlag){
            for(int i=0;i<currentSpeedCellRange.size();i++){
                speedCellRangeList[sliceFlag].push_back(currentSpeedCellRange[i]);
            }
            sliceFlag++;
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
            if(nextTime<SLICE_TIME*currentSliceAmount && nextTime<minTime[nextX][nextY]){
                minTime[nextX][nextY] = nextTime;
                q.push(SpeedCellIndex(nextX,nextY,nextTime));
            }
        }
        //将当前cell进行合并，只有上下左右4个方向
        //该cell已合并
        if(blockIndex[currentCell.x][currentCell.y] != -1) continue;
        double maxReachableRate = STANDARD_RATE; //必须高于标准分值
        int maxRateDirection = -1; //最高得分合并方向
        int maxRateBlockIndex = -1; //最高得分block下标
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

            int reachableAmount = 1; //可达网格数
            int totalAmount;
            double rate = double(reachableAmount)/double(totalAmount);
            if(rate > maxReachableRate){
                maxReachableRate = rate;
                maxRateDirection = i;
                maxRateBlockIndex = currentBlockIndex;
            }
            
            if(DIRECTION_X[i] == 0){
                totalAmount = currentBlock->rx-currentBlock->lx;
                for(int j=currentBlock->lx;j<=currentCell.x-1;j++){
                    //此处无速度或估计可达(从相邻点走)
                    int estimatedTime = minTime[j][neighborY] + speedGrid->getCellSpendTime(j,neighborY,i) + speedGrid->getCellSpendTime(j,currentCell.y,i);
                    if(speedGrid->getCellSpeedByMS(j,currentCell.y)<0.01 || estimatedTime<SLICE_TIME*currentSliceAmount) reachableAmount++;
                }
                for(int j=currentCell.x+1;j<=currentBlock->rx;j++){
                    int estimatedTime = minTime[j][neighborY] + speedGrid->getCellSpendTime(j,neighborY,i) + speedGrid->getCellSpendTime(j,currentCell.y,i);
                    if(speedGrid->getCellSpeedByMS(j,currentCell.y)<0.01 || estimatedTime<SLICE_TIME*currentSliceAmount) reachableAmount++;
                }
            }else if(DIRECTION_Y[i] == 0){
                totalAmount = currentBlock->ry-currentBlock->ly;
                for(int j=currentBlock->ly;j<=currentCell.y-1;j++){
                    int estimatedTime = minTime[neighborX][j] + speedGrid->getCellSpendTime(neighborX,j,i) + speedGrid->getCellSpendTime(currentCell.x,j,i);
                    if(speedGrid->getCellSpeedByMS(currentCell.x,j)<0.01 || estimatedTime<SLICE_TIME*currentSliceAmount) reachableAmount++;
                }
                for(int j=currentCell.y+1;j<=currentBlock->ry;j++){
                    int estimatedTime = minTime[neighborX][j] + speedGrid->getCellSpendTime(neighborX,j,i) + speedGrid->getCellSpendTime(currentCell.x,j,i);
                    if(speedGrid->getCellSpeedByMS(currentCell.x,j)<0.01 || estimatedTime<SLICE_TIME*currentSliceAmount) reachableAmount++;
                }
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
            //可达网格数量加一
            currentSpeedCellRange[maxRateBlockIndex].reachableAmount++;
        }
    }
    for(int i=0;i<currentSpeedCellRange.size();i++){
        speedCellRangeList[sliceFlag].push_back(currentSpeedCellRange[i]);
    }
}

SharebilityNetwork::SharebilityNetwork(SpeedGrid *sg){
    speedGrid = sg;
    vector<SpeedCellIndex> speedCellIndexTmp;
    vector<SpeedCellRange> speedCellRangeTmp;
    const int INDEX_AMOUNT = (END_LATITUDE-START_LATITUDE)/CELL_LATITUDE_LENGTH;
    const int INDEY_AMOUNT = (END_LONGITUDE-START_LONGITUDE)/CELL_LONGITUDE_LENGTH;
    SpeedCellRange speedCellRangeTmpOne(INDEX_AMOUNT,INDEY_AMOUNT,0,0);
    for(int i=0; i<SLICE_AMOUNT; i++) {
        speedCellIndexList.push_back(speedCellIndexTmp);
        speedCellRangeList.push_back(speedCellRangeTmp);
        speedCellLimit.push_back(speedCellRangeTmpOne);
    }
}

SharebilityNetwork::~SharebilityNetwork(){
    vecPoint.clear();
    vecEdge.clear();
    head.clear();
}

void SharebilityNetwork::buildNetworkFromFile(string filePath){
    Timer buildClock;
    readPoint(filePath);
     //分块建立R树,只有orderNumber.size()-1块
    for(int i=0;i<orderNumber.size()-1;i++){
        for(int j=orderNumber[i];j<orderNumber[i+1];j++){
            //节点
            Rect rt(vecPoint[j].getStartLatitude(),vecPoint[j].getStartLongitude());
            vecTree[i].Insert(rt.min,rt.max,j);
        }
    }
    //初始化赋值-1
    for(int i=0;i<vecPoint.size();i++) head.push_back(-1);
    //连接单向边
    for(int i=0;i<vecPoint.size();i++){
        //R树搜索结果清空
        vecAns.clear();
        int endTime=vecPoint[i].getEndTime();
        int index=endTime/60;
        for(int j=0;j<SLICE_AMOUNT;j++){
            int indey = index+j;
            if(indey>=orderNumber.size()-1) continue;
            searchRtree(vecTree[indey],j,endTime,vecPoint[i].getEndLongitude(),vecPoint[i].getEndLatitude());
        }
        //建边
        for(int j=0;j<vecAns.size();j++) addEdge(i,vecAns[j]);
            //if(existEdge(i,vecAns[j])) addEdge(i,vecAns[j]);
    }
    cout<<"[SharebilityNetWork] - Cost of time: "<<buildClock.getTimeCost()<<"s"<<endl;
}

void SharebilityNetwork::cellQuery(int index, int currentSliceAmount){
    for(int j=0; j<currentSliceAmount; j++){
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

void SharebilityNetwork::cellQuery_byDivideAndConquer(int index, int currentSliceAmount){
    for(int j=1; j<currentSliceAmount; j++){
        //范围边界更新，speedCellLimit[j]是第j分钟的边界，不是前j分钟的，更新为前j分钟的边界
        if(speedCellLimit[j-1].lx < speedCellLimit[j].lx) speedCellLimit[j].lx = speedCellLimit[j-1].lx;
        if(speedCellLimit[j-1].ly < speedCellLimit[j].ly) speedCellLimit[j].ly = speedCellLimit[j-1].ly;
        if(speedCellLimit[j-1].rx > speedCellLimit[j].rx) speedCellLimit[j].rx = speedCellLimit[j-1].rx;
        if(speedCellLimit[j-1].ry > speedCellLimit[j].ry) speedCellLimit[j].ry = speedCellLimit[j-1].ry;
    }
    //不需要使用整个速度网格，空间时间浪费过大
    int mergeLengthX = (speedCellLimit[currentSliceAmount-1].rx-speedCellLimit[currentSliceAmount-1].lx)/MAX_CELL_RANGE*MAX_CELL_RANGE+MAX_CELL_RANGE;
    int mergeLengthY = (speedCellLimit[currentSliceAmount-1].ry-speedCellLimit[currentSliceAmount-1].ly)/MAX_CELL_RANGE*MAX_CELL_RANGE+MAX_CELL_RANGE;
    //是否会越界
    if(speedCellLimit[currentSliceAmount-1].lx+mergeLengthX >= INDEX_AMOUNT) speedCellLimit[currentSliceAmount-1].lx = INDEX_AMOUNT-mergeLengthX;
    if(speedCellLimit[currentSliceAmount-1].ly+mergeLengthY >= INDEY_AMOUNT) speedCellLimit[currentSliceAmount-1].ly = INDEY_AMOUNT-mergeLengthY;
    vector<vector<int>> mergeRange;
    vector<int> mergeTmp;
    for(int k=0;k<mergeLengthY;k++) mergeTmp.push_back(-2);
    for(int k=0;k<mergeLengthX;k++) mergeRange.push_back(mergeTmp);
    //从第index分钟到第indey分钟的全部网格
    for(int k=0; k<currentSliceAmount; k++){
        for(int z=0; z<speedCellIndexList[k].size(); z++){
            int x = speedCellIndexList[k][z].x-speedCellLimit[currentSliceAmount-1].lx;
            int y = speedCellIndexList[k][z].y-speedCellLimit[currentSliceAmount-1].ly;
            //表示可达
            mergeRange[x][y] = k;
        }
    }
    //如果该网格平均速度为0，可以一同合并，但标记为-1
    for(int p=0;p<mergeLengthX;p++)
        for(int q=0;q<mergeLengthY;q++)
            if(speedGrid->getCellSpeedByMS(p+speedCellLimit[currentSliceAmount-1].lx,q+speedCellLimit[currentSliceAmount-1].ly) < 0.01){
                mergeRange[p][q] = -1;
            }
    for(int k=2;k<=MAX_CELL_RANGE;k*=2){
        for(int p=0;p<mergeLengthX/k;p++)
            for(int q=0;q<mergeLengthY/k;q++){
                int p2 = p*2, q2 = q*2;
                int x1 = mergeRange[p2][q2];
                int x2 = mergeRange[p2+1][q2];
                int x3 = mergeRange[p2][q2+1];
                int x4 = mergeRange[p2+1][q2+1];
                int lx1 = p*k, ly1 = q*k, rx1 = lx1+k/2-1, ry1 = ly1+k/2-1;
                int lx2 = rx1+1, ly2 = ly1, rx2 = lx2+k/2-1, ry2 = ly2+k/2-1;
                int lx3 = lx1, ly3 = ry1+1, rx3 = lx3+k/2-1, ry3 = ly3+k/2-1;
                int lx4 = rx1+1, ly4 = ry1+1, rx4 = lx4+k/2-1, ry4 = ly4+k/2-1;
                int timeI = -2;
                if(x1 >= 0) timeI = x1;
                else if(x2 >= 0) timeI = x2;
                else if(x3 >= 0) timeI = x3;
                else if(x4 >= 0) timeI = x4;
                if(timeI == -2) continue; //没有可查询网格
                //4格均为-1，合并为-1
                if(x1==-1 && x2==-1 && x3==-1 && x4==-1){
                    mergeRange[p][q] = -1;
                }//4格均为i或-1，合并为i
                else if((x1==-1||x1==timeI) && (x2==-1||x2==timeI) && (x3==-1||x3==timeI) && (x4==-1||x4==timeI)){
                    mergeRange[p][q] = timeI;
                    if(k == MAX_CELL_RANGE){
                        SpeedCellRange rangeTmp(lx1,ly1,rx4,ry4);
                        speedCellRangeList[timeI].push_back(rangeTmp);
                    }
                }else{
                    mergeRange[p][q] = -2;
                    //相邻2格相同，合并并记录
                    if(x1>=0 && x2>=0 && x1==x2){
                        SpeedCellRange rangeTmp(lx1,ly1,rx2,ry2);
                        speedCellRangeList[x1].push_back(rangeTmp);
                        x1 = -2;
                        x2 = -2;
                    }
                    if(x1>=0 && x3>=0 && x1==x3){
                        SpeedCellRange rangeTmp(lx1,ly1,rx3,ry3);
                        speedCellRangeList[x1].push_back(rangeTmp);
                        x1 = -2;
                        x3 = -2;
                    }
                    if(x2>=0 && x4>=0 && x2==x4){
                        SpeedCellRange rangeTmp(lx2,ly2,rx4,ry4);
                        speedCellRangeList[x2].push_back(rangeTmp);
                        x2 = -2;
                        x4 = -2;
                    }
                    if(x3>=0 && x4>=0 && x3==x4){
                        SpeedCellRange rangeTmp(lx3,ly3,rx4,ry4);
                        speedCellRangeList[x3].push_back(rangeTmp);
                        x3 = -2;
                        x4 = -2;
                    }
                    //只有1格，直接记录
                    if(x1 >= 0){
                        SpeedCellRange rangeTmp(lx1,ly1,rx1,ry1);
                        speedCellRangeList[x1].push_back(rangeTmp);
                    }
                    if(x2 >= 0){
                        SpeedCellRange rangeTmp(lx2,ly2,rx2,ry2);
                        speedCellRangeList[x2].push_back(rangeTmp);
                    }
                    if(x3 >= 0){
                        SpeedCellRange rangeTmp(lx3,ly3,rx3,ry3);
                        speedCellRangeList[x3].push_back(rangeTmp);
                    }
                    if(x4 >= 0){
                        SpeedCellRange rangeTmp(lx4,ly4,rx4,ry4);
                        speedCellRangeList[x4].push_back(rangeTmp);
                    }
                }
            }
    }
    for(int j=0; j<currentSliceAmount; j++){
        int indey = index+j;
        //从第index分钟到第indey分钟的全部网格
        for(int k=0; k<=j; k++){
            blockTotalAmount += speedCellRangeList[k].size();
            for(int z=0;z<speedCellRangeList[k].size();z++){
                int lx = speedCellRangeList[k][z].lx+speedCellLimit[currentSliceAmount-1].lx;
                int ly = speedCellRangeList[k][z].ly+speedCellLimit[currentSliceAmount-1].ly;
                int rx = speedCellRangeList[k][z].rx+speedCellLimit[currentSliceAmount-1].lx;
                int ry = speedCellRangeList[k][z].ry+speedCellLimit[currentSliceAmount-1].ly;
                double minLat = START_LATITUDE+CELL_LATITUDE_LENGTH*lx;
                double minLon = START_LONGITUDE+CELL_LONGITUDE_LENGTH*ly;
                double maxLat = minLat+CELL_LATITUDE_LENGTH*(rx-lx+1);
                double maxLon = minLon+CELL_LONGITUDE_LENGTH*(ry-ly+1);  
                Rect searchRect(minLat,minLon,maxLat,maxLon);
                vecTree[indey].Search(searchRect.min, searchRect.max, MySearchCallback, NULL);
            }
        }
    }
}

void SharebilityNetwork::cellQuery_byPriorityQueueMerge(int index,int currentSliceAmount){
    for(int j=0; j<currentSliceAmount; j++){
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
     //分块建立R树,只有orderNumber.size()-1块
    for(int i=0;i<orderNumber.size()-1;i++){
        for(int j=orderNumber[i];j<orderNumber[i+1];j++){
            //节点
            Rect rt(vecPoint[j].getStartLatitude(),vecPoint[j].getStartLongitude());
            vecTree[i].Insert(rt.min,rt.max,j);
        }
    }
    //初始化赋值-1
    for(int i=0;i<vecPoint.size();i++) head.push_back(-1);
    //连接单向边
    for(int i=0;i<vecPoint.size();i++){
        //R树搜索结果清空
        vecAns.clear();
        int endTime=vecPoint[i].getEndTime();
        int index=endTime/60;
        int currentSliceAmount = SLICE_AMOUNT;
        if(index+SLICE_AMOUNT-1 >= orderNumber.size()-1){
            currentSliceAmount = orderNumber.size()-index-1;
        }
        //位置所在速度cell下标
        int startX = (vecPoint[i].getEndLatitude()-START_LATITUDE)/CELL_LATITUDE_LENGTH;
        int startY = (vecPoint[i].getEndLongitude()-START_LONGITUDE)/CELL_LONGITUDE_LENGTH;
        //第一步：搜索全部可达网格
        Timer findClock1;
        //fetchReachableSpeedCells(startX,startY,currentSliceAmount);
        fetchReachableSpeedCells_byPriorityQueueMerge(startX,startY,currentSliceAmount);
        findTime1 +=  findClock1.getTimeCost();
        //第二步：查询每一个可达网格
        Timer findClock2;
        //方法1：一个一个查询
        //cellQuery(index,currentSliceAmount);
        //方法2：分治法合并后查询
        //cellQuery_byDivideAndConquer(index,currentSliceAmount);
        //方法3：优先队列合并搜索
        cellQuery_byPriorityQueueMerge(index,currentSliceAmount);
        findTime2 +=  findClock2.getTimeCost();
        //第三步：连边
        Timer findClock3;
        for(int j=0;j<vecAns.size();j++) addEdge(i,vecAns[j]);
        findTime3 +=  findClock3.getTimeCost();
    }
    cout<<"findTime1: "<<findTime1<<endl;
    cout<<"divisionCalculationTime: "<<divisionCalculationTime<<endl;
    cout<<"findTime2: "<<findTime2<<endl;
    cout<<"rtreeTime: "<<rtreeTime<<endl;
    cout<<"findTime3: "<<findTime3<<endl;
    cout<<"blockTotalAmount: "<<blockTotalAmount<<endl; 
    cout<<"Amount of all priority queues: "<<queueTotalAmount<<endl;
    cout<<"[SharebilityNetWork] - Cost of time: "<<buildClock.getTimeCost()<<"s"<<endl;
}


void SharebilityNetwork::buildNetworkFromFile_multithread(string filePath){
    Timer buildClock;
    readPoint(filePath);
    //分块建立R树,只有orderNumber.size()-1块
    for(int i=0;i<orderNumber.size()-1;i++){
        for(int j=orderNumber[i];j<orderNumber[i+1];j++){
            //节点
            Rect rt(vecPoint[j].getStartLatitude(),vecPoint[j].getStartLongitude());
            vecTree[i].Insert(rt.min,rt.max,j);
        }
    }
    //初始化赋值-1
    for(int i=0;i<vecPoint.size();i++) head.push_back(-1);
    //连接单向边
    for(int i=0;i<vecPoint.size();i++){
        //R树搜索结果清空
        vecAns.clear();
        int endTime=vecPoint[i].getEndTime();
        int index=endTime/60;
        //多线程
        for(int k=0;k<=SLICE_AMOUNT/THREAD_AMOUNT;k++){
            thread vecTh[THREAD_AMOUNT];
            for(int z=0;z<THREAD_AMOUNT;z++){
                //时间片块数
                int j = k*THREAD_AMOUNT+z;
                int indey=index+j;
                if(j>=SLICE_AMOUNT || indey>=orderNumber.size()-1){
                    vecTh[z] = thread(nothing);
                    continue;
                }
                vecTh[z] = thread(searchRtree,ref(vecTree[indey]),j,endTime,vecPoint[i].getEndLongitude(),vecPoint[i].getEndLatitude());
            }
            //线程join
            for(int z=0;z<THREAD_AMOUNT;z++) vecTh[z].join();
        }
        //建边
        for(int j=0;j<vecAns.size();j++) addEdge(i,vecAns[j]);
    }
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
    if(t<=0 || t>maxLimitTime) return false;
    //时间内车能从x点到达y点 
    //设定汽车平均行驶速度36.8km/h，即11m/s
    if(distance(vecPoint[x].getEndLongitude(),vecPoint[x].getEndLatitude(),vecPoint[y].getStartLongitude(),vecPoint[y].getEndLatitude())>11.0*t){
        return false;
    }
    return true;
}

void SharebilityNetwork::printNetwork(){
    cout<<"Amount of Points: "<<vecPoint.size()<<endl;
    cout<<"Amount of Edges: "<<vecEdge.size()<<endl;
}