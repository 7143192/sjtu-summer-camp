#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

int N = 0;
int M = 0;
int Q = 0;
int P = 0;

struct Edge {
    int w;
    int e;
    Edge() = default;
    Edge(int w1, int e1) {
        w = w1;
        e = e1;
    }
};

struct Shortest {
    int x;
    int y;
    int w;
    Shortest() = default;
    Shortest(int x1, int y1, int w1) {
        x = x1;
        y = y1;
        w = w1;
    }
};

struct cmpEdge {
    bool operator() (Edge* e1, Edge* e2) {
        return e1->w > e2->w;
    }
};

// a struct used to generate new random number
class MersenneTwister {
public:
    MersenneTwister() {
        index = 0;
        mt[0] = 2023; // seed
        for (int i = 1; i < 624; ++i) {
            mt[i] = (0x6C078965 * (mt[i-1] ^ (mt[i-1] >> 30)) + i)
                & 0xFFFFFFFF;
        }
    }

    unsigned int generate() {
        if (index == 0) {
            for (int i = 0; i < 624; ++i) {
                unsigned int y =
                    ((mt[i] & 0x80000000) + (mt[(i + 1) % 624] & 0x7FFFFFFF))
                    & 0xFFFFFFFF;
                mt[i] = mt[(i + 397) % 624] ^ (y >> 1);
                if (y % 2 != 0) {
                    mt[i] ^= 0x9908B0DF;
                }
            }
        }

        unsigned int y = mt[index];
        y ^= (y >> 11);
        y ^= ((y << 7) & 0x9D2C5680);
        y ^= ((y << 15) & 0xEFC60000);
        y ^= (y >> 18);

        index = (index + 1) % 624;
        return y;
    }

    unsigned int mt[624];
    int index;
};

vector<vector<Edge*>> edges; // used to store all edges.
vector<vector<int>> dist; // used to store all nodes' shortest path to other nodes.
map<int, int> in_map; // used to count all in edge num for every node
vector<int> positions; // used to store all P node num.
vector<int> random_path; // used to record all nodes passed in problem 2.3
MersenneTwister* mm = new MersenneTwister();

static bool cmp1(pair<int, int> p1, pair<int, int> p2) {
    if (p1.second == p2.second) return p1.first < p2.first;
    return p1.second > p2.second;
}

static bool cmp2(Shortest* s1, Shortest* s2) {
    if (s1->w == s2->w) {
        if (s1->x == s2->x) return s1->y < s2->y;
        return s1->x < s2->x;
    }
    return s1->w > s2->w;
}

static bool cmp3(Edge* e1, Edge* e2) {
    return e1->e < e2->e;
}

// 2.1
void BuildGraph(const char* filename) {
    ifstream in;
    in.open(filename, ios::in);
    in >> N >> M >> Q >> P;
    edges = vector<vector<Edge*>>(N);
    for (int i = 0; i < M; ++i) {
        int x = 0;
        int y = 0;
        int w = 0;
        in >> x >> y >> w;
        auto edge = new Edge(w, y);
        edges[x].push_back(edge);
        in_map[y]++;
    }
    for (int i = 0; i < N; ++i) {
        sort(edges[i].begin(), edges[i].end(), cmp3);
    }
    // then sort the in edge number and output as 2.1 required.
    vector<pair<int, int>> pairs(in_map.begin(), in_map.end());
    sort(pairs.begin(), pairs.end(), cmp1);
    cout << pairs[0].first << " " << pairs[0].second << endl;
    // then input p lines.
    for (int i = 0; i < P; ++i) {
        int num = 0;
        in >> num;
        positions.push_back(num);
    }
    in.close();
}

void Dijkstra(int start) {
    dist[start][start] = 0;
    priority_queue<Edge*, vector<Edge*>, cmpEdge> pq;
    pq.push(new Edge(0, start));
    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        int end = top->e;
        int weight = top->w;
        for (int i = 0; i < edges[end].size(); ++i) {
            auto one = edges[end][i];
            int tmpE = one->e;
            int tmpW = one->w;
            if (dist[start][tmpE] > dist[start][end] + tmpW) {
                dist[start][tmpE] = dist[start][end] + tmpW;
                pq.push(edges[end][i]);
            }
        }
    }
}

// 2.2
void ComputePath() {
    int limit = INT32_MAX / 2;
    dist = vector<vector<int>>(N, vector<int>(N, limit));
    // compute all node-pairs' shortest dist using Dijkstra algo
    for (int i = 0; i < N; ++i) {
        Dijkstra(i);
    }
    vector<Shortest*> all_paths;
    // then sort and output as required.
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i == j) continue;
            auto shortest = new Shortest(i, j, dist[i][j]);
            all_paths.push_back(shortest);
        }
    }
    sort(all_paths.begin(), all_paths.end(), cmp2);
    cout << all_paths[0]->x << " " << all_paths[0]->y << " " << all_paths[0]->w << endl;
    return ;
}

unsigned int Choose(int v) {
    unsigned int n = edges[v].size();
    auto idx = (mm->generate()) % n;
    return idx;
}

// 2.3
void RandomWalk() {
    int idx = 0;
    int total = 0;
    int start = 0;
    int res = 0;
    while (start != 0 || idx < P) {
        random_path.push_back(start);
        int end = positions[idx];
        if (idx < P && start == end && total < Q) {
            idx++;
            total++;
        }
        if (start == 0) total = 0;
        auto chosen = Choose(start);
        res += edges[start][chosen]->w;
        start = edges[start][chosen]->e;
    }
    ofstream out;
    out.open("/home/os/Desktop/problem_2024/path.txt", ios::out | ios::trunc);
    for (int i = 0; i < random_path.size(); ++i) {
        stringstream sst;
        sst << std::to_string(random_path[i]);
        out << sst.str() << endl;
    }
    out.close();
    cout << res << endl;
}

// 2.4
void PathAnalyze() {
    map<int, int> map1;
    for (int i = 0; i < random_path.size(); ++i) {
        map1[random_path[i]]++;
    }
    vector<pair<int, int>> pairs(map1.begin(), map1.end());
    // sort
    sort(pairs.begin(), pairs.end(), cmp1);
    // output the 3-most visited nodes and visited times.
    for (int i = 0; i < 3; ++i) {
        cout << pairs[i].first << " " << pairs[i].second << endl;
    }
    return ;
}

// 2.5
void BestPath() {
    // int res = 0;
    // int total = 0;
    // int start = 0;
    // int idx = 0;
    // int end = 0;
    // while (idx != P) {
    //     while (total < Q - 1 && idx < P) {
    //         end = positions[idx++];
    //         res += dist[start][end];
    //         start = end;
    //         total++;
    //     }
    //     if (idx == P) break;
    //     int end1 = positions[idx++];
    //     int t1 = dist[start][0] + dist[0][end];
    //     int t2 = dist[start][end] + dist[end][0];
    //     if (t1 < t2) {
    //         res += t1;
    //         start = end;
    //         total = 1;
    //     } else {
    //         res += t2;
    //         start = 0;
    //         total = 0;
    //     }
    // }
    // res += dist[positions.back()][0];
    // cout << res << endl;

    vector<vector<unsigned int>> dp(P, vector<unsigned int>(Q + 1, INT32_MAX));
    // dp[i][j] <--> the shortest total length when traversing all positions starting from node 0 where P = i && Q = j 
    dp[0][1] = dist[0][positions[0]];
    for (int i = 1; i < P; ++i) {
        for (int j = 1; j <= Q; ++j) {
            dp[i][j] = dp[i - 1][j - 1] + dist[positions[i - 1]][positions[i]];
            for (int k = 1; k <= Q; ++k) {
                unsigned int tmp = dp[i - 1][k] + dist[positions[i - 1]][0] + dist[0][positions[i]];
                if (tmp < dp[i][j]) dp[i][j] = tmp;
            }
        }
    }
    int res = dp[P - 1][1];
    for (int i = 2; i <= Q; ++i) {
        if (res > dp[P - 1][i]) res = dp[P - 1][i];
    }
    res += dist[positions.back()][0];
    cout << res << endl;
}

int main() {
    string filename;
    cin >> filename;
    // 2.1
    BuildGraph(filename.c_str());
    // 2.2
    ComputePath();
    // 2.3
    RandomWalk();
    // 2.4
    PathAnalyze();
    // 2.5
    BestPath();
    return 0;
}
