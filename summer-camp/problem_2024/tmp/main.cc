// Created by hjk on 23-9-22.
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include "MersenneTwister.h"

std::ifstream file;
std::vector<std::vector<int>> graph, floyd, outEdges;
std::vector<int> inEdgeCount, pointsArriveCount, passenger;
int N, M, Q, P;

inline void BuildGraph() {
    int X, Y, W, maxInEdge = 0, maxInEdgePoint = 0;
    for (int i = 0; i < M; i++) {
        file >> X >> Y >> W;
        inEdgeCount[Y]++;
        outEdges[X].push_back(Y);
        graph[X][Y] = W;
    }
    for(int i = 0; i < N; i++)
        if(inEdgeCount[i] > maxInEdge){
            maxInEdge = inEdgeCount[i];
            maxInEdgePoint = i;
        }
    std::cout << maxInEdgePoint << " " << maxInEdge << std::endl;
    for(int i = 0; i < N; i++)
        sort(outEdges[i].begin(), outEdges[i].end());
}

inline void ComputePath(){
    floyd = graph;
    long dis;
    int maxMinLength = 0, maxMinLengthI, maxMinLengthJ;
    for(int k = 0; k < N; k++)
        for(int i = 0; i < N; i++)
            for(int j = 0; j < N; j++)
                if((dis = (long)floyd[i][k] + (long)floyd[k][j]) < (long)floyd[i][j])
                    floyd[i][j] = (int)dis;
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            if(floyd[i][j] > maxMinLength){
                maxMinLengthI = i;
                maxMinLengthJ = j;
                maxMinLength = floyd[i][j];
            }
    std::cout << maxMinLengthI << " " << maxMinLengthJ << " " << maxMinLength << std::endl;
}

inline void RandomWalk(){
    unsigned int currentPoint = 0, currentInCar = 0, nextNum, currentNum, totalDis = 0, nextPassengerIndex = 0, gen;
    MersenneTwister randomGen;
    std::vector<int> Points(N,0);
    std::ofstream fileOut("./path.txt");
    for(int i = 0; i < P; i++)
        file >> passenger[i];
    while(nextPassengerIndex < P || currentPoint){
        fileOut << currentPoint << std::endl;
        pointsArriveCount[currentPoint]++;
        if(nextPassengerIndex < P && currentPoint == passenger[nextPassengerIndex] && currentInCar < Q){
            nextPassengerIndex++;
            currentInCar++;
        }
        if(!currentPoint)
            currentInCar = 0;
        gen = randomGen.generate();
        nextNum = gen%(outEdges[currentPoint].size());
        totalDis += graph[currentPoint][outEdges[currentPoint][nextNum]];
        currentPoint = outEdges[currentPoint][nextNum];
    }
    fileOut << currentPoint << std::endl;
    pointsArriveCount[currentPoint]++;
    fileOut.close();
    std::cout << totalDis << std::endl;
}

inline void PathAnalyze(){
    int _1st = 0, _2nd = 0, _3rd = 0, _1st_pt= 0, _2nd_pt = 0, _3rd_pt = 0;
    for(int i = 0; i < N; i++){
        if(pointsArriveCount[i] > _1st){
            _3rd = _2nd;
            _3rd_pt = _2nd_pt;
            _2nd = _1st;
            _2nd_pt = _1st_pt;
            _1st = pointsArriveCount[i];
            _1st_pt = i;
        }
        else if(pointsArriveCount[i] > _2nd){
            _3rd = _2nd;
            _3rd_pt = _2nd_pt;
            _2nd = pointsArriveCount[i];
            _2nd_pt = i;
        }else if(pointsArriveCount[i] > _3rd){
            _3rd = pointsArriveCount[i];
            _3rd_pt = i;
        }
    }
    std::cout << _1st_pt << " " << _1st << "\n" << _2nd_pt << " " << _2nd << "\n" << _3rd_pt << " " << _3rd << std::endl;
}

inline void BestPath() {
    long f[P][Q+1];
    for (int i = 0; i < P; ++i) 
        for (int j = 0; j <= Q; ++j) 
            f[i][j] = INT32_MAX;
    f[0][1] = floyd[0][passenger[0]];
    for (int i = 1; i < P; ++i)
        for (int j = 1; j <= Q; ++j) {
            f[i][j] = f[i-1][j-1] + floyd[passenger[i-1]][passenger[i]];
            for (int k = 0; k <= Q; ++k) {
                long newdist = f[i-1][k] + floyd[passenger[i-1]][0] + floyd[0][passenger[i]];
                if (newdist < f[i][j])
                    f[i][j] = newdist;
            }
        }
    int best = f[P-1][1];
    for (int j = 2; j <= Q; ++j) 
        if (f[P-1][j] < best)
            best = f[P-1][j];
    std::cout << (best + floyd[passenger[P-1]][0]) << std::endl;
}

int main() {
    std::string filePath;
    // std::cin >> filePath; 
    filePath = "/home/hjk/workspace/handout/handout-v0.1alpha/data/large.road-pass.txt";
    file.open(filePath, std::ios::in);
    file >> N >> M >> Q >> P;
    std::vector<int> a(N, INT32_MAX);
    graph.resize(N, a);
    inEdgeCount.resize(N, 0);
    outEdges.resize(N);
    pointsArriveCount.resize(N, 0);
    passenger.resize(P, 0);
    BuildGraph();
    ComputePath();
    RandomWalk();
    PathAnalyze();
    BestPath();
    file.close();
    return 0;
}