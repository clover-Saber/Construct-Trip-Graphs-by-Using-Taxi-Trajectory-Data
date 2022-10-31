#include <iostream>
#include <fstream>
#include "include/MinimumCost.h"
#include "include/Util.h"
using namespace std;

MinimumCost::~MinimumCost(){
    crossingVector.clear();
    costVector.clear();
}

void MinimumCost::initCost(string filePath){
    Timer initClock;
    ifstream infile;
    infile.open(filePath.c_str(),ios::in);
    int n, m; // n行m列
    infile>>n>>m;
    cout<<n<<' '<<m<<endl;
    for(int i=0;i<n;i++){
        if(i%1000 == 0) cout<<i<<endl;
        vector<double> tmp;
        for(int j=0;j<m;j++){
            double t;
            infile>>t;
            tmp.push_back(t);
        }
        costVector.push_back(tmp);
    }
    cout<<"Size of path-cost is: "<<n<<"x"<<m<<endl;
    cout<<"Time of path-cost is: "<<initClock.getTimeCost()<<" s"<<endl;
}

void MinimumCost::initCrossing(string filePath){
    ifstream infile;
    infile.open(filePath.c_str(),ios::in);
    int id;
    double x, y;
    while(infile>>id>>x>>y){
        Crossing c;
        c.init(x,y);
        crossingVector.push_back(c);
    }
    cout<<"Size of crossing is: "<<crossingVector.size()<<endl;
}

int MinimumCost::findNearestCrossing(double x, double y){
    int res = -1;
    double min = 100000007.0;
    for(int i=0;i<crossingVector.size();i++){
        double tx = crossingVector[i].getLongitude();
        double ty = crossingVector[i].getLatitude();
        double dis = (x-tx)*(x-tx) + (y-ty)*(y-ty);
        if(min-dis > 1e-5){
            min = dis;
            res = i;
        }
    }
    return res;
}

double MinimumCost::calculateMinimumCostWhenThisFlow(string costFilePath, string crossingFilePath, SharebilityNetwork &shareNet, int flow){
    Timer calculateClock;

    initCost(costFilePath);
    initCrossing(crossingFilePath);

    int num_nodes = 2*shareNet.vecPoint.size()+2;
	int num_arcs = 2*shareNet.vecPoint.size()+shareNet.vecEdge.size();

 	MCMF_CS2 my_mcmf_problem( num_nodes, num_arcs);
    for(int i=1;i<=shareNet.vecPoint.size();i++){
        my_mcmf_problem.set_arc( num_nodes-1, i, 0, 1, 0);
        my_mcmf_problem.set_arc( i+shareNet.vecPoint.size(), num_nodes, 0, 1, 0);
    }

    for(int i=0;i<shareNet.vecEdge.size();i++){
        int x = shareNet.vecEdge[i].getStart()+1;
        int y = shareNet.vecEdge[i].getEnd()+shareNet.vecPoint.size()+1;
        int crossingX = findNearestCrossing(shareNet.vecPoint[x].getEndLongitude(), shareNet.vecPoint[x].getEndLatitude());
        int crossingY = findNearestCrossing(shareNet.vecPoint[y].getStartLongitude(), shareNet.vecPoint[y].getStartLatitude());
        my_mcmf_problem.set_arc( x, y, 0, 1, costVector[crossingX][crossingY]);
    }

    //源节点和汇聚节点
	my_mcmf_problem.set_supply_demand_of_node( num_nodes-1, flow);
	my_mcmf_problem.set_supply_demand_of_node( num_nodes, -flow);
    
    double minCost = my_mcmf_problem.run_cs2();

    cout<<"Time of minimumCost is: "<<calculateClock.getTimeCost()<<" s"<<endl;

    return minCost;
}