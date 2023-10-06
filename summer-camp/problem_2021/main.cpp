#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>

using namespace std;

struct rainInfo {
    int T; // time to rain
    int L; // node id to rain
    int W; // size of this rain
    rainInfo() = default;
    rainInfo(int t1, int l1, int w1) {
        T = t1;
        L = l1;
        W = w1;
    }
};

struct Edge {
    int w;
    int e;
    Edge() = default;
    Edge(int w1, int e1) {
        w = w1;
        e = e1;
    }
};

struct cmpEdge {
    bool operator() (Edge* e1, Edge* e2) {
        return e1->w > e2->w;
    }
};

static bool cmpInfo (struct rainInfo* r1, struct rainInfo* r2) {
    return  r1->T < r2->T;
}

int N = 0; // number of nodes.
int M = 0; // number of edges.
int R = 0; // number of rains.
vector<pair<int, int>> heights; // heights of every node in the graph
vector<vector<int>> edges; // use a adjcent list to present this graph edges.
vector<rainInfo*> rainInfos; // used to store all rainInfo
vector<vector<int>> water; // used to store every node's water size at every time t (until no water in all nodes after a t')
vector<vector<int>> no_water_paths; // used in func FindNoWaterPath to record all 0 -- N-1 paths
vector<vector<int>> water_paths; // used to record all 0 -- N-1 paths when considering water

// step 1 ???
void output_symbol() {
    string lines = 
" SSSSS     JJJJJJJJJ       TTTTTTTTT       UUU     UU\n\
SSSS SSS       JJJ             TTT          UU     UUU\n\
SS    SS        JJ             TT           UU     UU\n\
 SSS            JJ             TT          UU      UU\n\
  SSSSS         JJ             TT          UU     UU\n\
    SSS         JJ            TTT          UU     UU\n\
      SS       JJJ             TT          UU    UU\n\
SSS SSS      JJ JJ             TT           UU UU\n\
SSSSS      JJJJJ              TTT         UUUUUU";
    printf("%s\n", lines.c_str());
    return ;
}

// step 2
void LoadFile(const char* filename) {
    // step 1
    output_symbol();
    ifstream in;
    in.open(filename, ios::in);
    string line = "";
    // read N and M
    in >> N >> M;
    // cout << "N = " << N << " " << "M = " << M << endl;
    for (int i = 0; i < N; ++i) {
        vector<int> one;
        edges.push_back(one);
    }
    // read all heights
    for (int i = 0; i < N; ++i) {
        int h = 0;
        in >> h;
        heights.push_back(make_pair(i, h));
    }
    // cout << "get here 1" << endl;
    // read all edges information
    for (int i = 0; i < M; ++i) {
        int start = 0;
        int end = 0;
        in >> start >> end;
        edges[start].push_back(end);
        edges[end].push_back(start); // undirected edge
    }
    // cout << "get here 2" << endl;
    // read R
    in >> R;
    // cout << "R = " << R << endl;
    // read all rain information
    for (int i = 0; i < R; ++i) {
        int t, l, w;
        in >> t >> l >> w;
        struct rainInfo* ri = new rainInfo(t, l, w);
        rainInfos.push_back(ri);
    }
    // then sort all rainInfo according to rain time
    sort(rainInfos.begin(), rainInfos.end(), cmpInfo);
    // then output the node with most num of edges.
    int max = 0;
    int max_i = 0;
    for (int i = 0; i < N; ++i) {
        if (edges[i].size() > max) {
            max = edges[i].size();
            max_i = i;
        }
    }
    cout << max_i << endl;
}

bool CheckEmpty(vector<int> line) {
    for (int i = 0; i < line.size(); ++i) {
        if (line[i] != 0) return false;
    }
    return true;
}

int findIdx(vector<pair<int, int>> heights, int idx) {
    int res = 0;
    for (int i = 0; i < heights.size(); ++i) {
        if (heights[i].first == idx) return i;
    }
    return -1;
}

static bool cmp(pair<int, int> p1, pair<int, int> p2) {
    if (p1.second == p2.second) return p1.first < p2.first;
    return p1.second > p2.second;
}

// TEST ONLY
void out_vec(vector<int> line) {
    for (int i = 0; i < line.size(); ++i) {
        cout << line[i] << " ";
    }
    cout << endl;
}

bool CheckNeighbor(int start, int end) {
    for (int i = 0; i < edges[start].size(); ++i) {
        if (edges[start][i] == end) return true;
    }
    return false;
}

// step 3
int SimFlow() {
    // sort the heights first.
    sort(heights.begin(), heights.end(), cmp);
    int cur_t = 0;
    for (int i = 0; i < rainInfos[0]->T; ++i) {
        vector<int> line = vector<int>(N, 0);
        water.push_back(line);
    }
    // init states at the t of the first rain.
    vector<int> start = vector<int>(N, 0);
    int idx = 0;
    for (int i = 0; i < R; ++i) {
        if (rainInfos[i]->T == rainInfos[0]->T) {
            start[rainInfos[i]->L] += rainInfos[i]->W;
            idx = i;
        }
    }
    // out_vec(start);
    cur_t = rainInfos[0]->T;
    idx++;
    water.push_back(start);
    while (true) {
        vector<int> last = water.back();
        vector<int> new_t = last;
        // assume current time is t
        // stop condition: no water in last and no water at time t.
        if (CheckEmpty(last) == true && cur_t > rainInfos.back()->T) {
            break;
        }
        // first handle left water at t - 1
        for (int i = 0; i < new_t.size(); ++i) {
            if (new_t[i] == 0) continue;
            new_t[i]--; // the water system can handle 1 size.
            if (new_t[i] == 0) continue;
            int si = findIdx(heights, i);
            int cur_h = heights[si].second;
            si++;
            while (si < heights.size() && new_t[i] > 0 && CheckNeighbor(i, heights[si].first)) {
                if (heights[si].second < cur_h) {
                    new_t[heights[si].first]++;
                    new_t[i]--;
                }
                si++;
            }
        }
        // if a new vector is all-empty, return directly
        if (CheckEmpty(new_t) && cur_t > rainInfos.back()->T) break;
        // then handle new input water at time t
        if (idx >= R) {
            // out_vec(new_t);
            water.push_back(new_t);
            cur_t++;
            continue;
        }
        int one_t = rainInfos[idx]->T;
        if (cur_t < one_t) {
            // out_vec(new_t);
            water.push_back(new_t);
            cur_t++;
            continue;
        }
        for (; idx < R; ++idx) {
            if (rainInfos[idx]->T == one_t) {
                new_t[rainInfos[idx]->L] += rainInfos[idx]->W;
            } else {
                break;
            }
        }
        cur_t++;
        // ut_vec(new_t);
        water.push_back(new_t);
    }
    for (int i = 0; i < water.size(); ++i) {
      out_vec(water[i]);
    }
    return cur_t; // t starts from 0.
}

// a recursive function to find all paths to reach N-1 node
// not sure here....
void FindNoWaterPath(vector<int> path, vector<int> checked, int i, int cur_t) {
    if (checked[i] == 1) return ;
    // path.push_back(i);
    // cout << "i = " << i << endl;
    if (i == N - 1) {
        // cout << "get here" << endl;
        water_paths.push_back(path);
        return ;
    }
    // if not the last line of all water simulations...
    if (cur_t < water.size() - 1) {
        int t = cur_t + 1;
        auto info = water[t];
        bool found = false;
        for (int k = 0; k < edges[i].size(); ++k) {
            if (checked[edges[i][k]] == 1) continue;
            if (info[edges[i][k]] == 0) {
                found = true;
                break;
            }
        }
        // if all adjcent nodes have water at the NEXT time point, just WAIT in the current node at time point t -- t + 1.
        if (found == false) {
            // wait in the current node.
            path.push_back(i);
            FindNoWaterPath(path, checked, i, cur_t + 1);
            // path.pop_back();
            // cur_t--;
        }
    }
    // if cur_t == water.size() - 1, the next line must be all-zero, so go ahead directly.
    checked[i] = 1;
    for (int j = 0; j < edges[i].size(); ++j) {
        if (checked[edges[i][j]] == 1) continue;
        cur_t++;
        if (water[cur_t][edges[i][j]] != 0) {
            cur_t--;
            continue;
        }
        // checked[edges[i][j]] = 1;
        path.push_back(edges[i][j]);
        FindNoWaterPath(path, checked, edges[i][j], cur_t);
        path.pop_back();
        cur_t--;
        // checked[edges[i][j]] = 0;
    }
    // checked[i] = 0;
}

// step 4.1
void ShortestPath_no_water() {
    // vector<int> checked = vector<int>(N, 0);
    // vector<int> path;
    // path.push_back(0);
    // FindNoWaterPath(path, checked, 0);
    // // cout << no_water_paths.size() << endl;
    // // choose the shortest path from all paths
    // int min = INT32_MAX;
    // for (int i = 0; i < no_water_paths.size(); ++i) {
    //     if (no_water_paths[i].size() < min) min = no_water_paths[i].size();
    // }
    // // NOTE: min--
    // min--;
    // cout << min << " ";
    // return ;

    // a Dijkstra algorithm to handle no-water shortest path.
    int limit = INT32_MAX / 2;
    vector<int> dist(N, limit);
    dist[0] = 0;
    // use a min-heap
    priority_queue<Edge*, vector<Edge*>, cmpEdge> pq;
    vector<vector<Edge*>> all = vector<vector<Edge*>>(edges.size(), vector<Edge*>());
    for (int i = 0; i < edges.size(); ++i) {
        for (int j = 0; j < edges[i].size(); ++j) {
            auto one = new Edge(1, edges[i][j]);
            all[i].push_back(one);
        }
    }
    auto start_e = new Edge(0, 0);
    pq.push(start_e);
    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        int tmpE = top->e;
        int tmpW = top->w;
        for (int i = 0; i < all[tmpE].size(); ++i) {
            int E = all[tmpE][i]->e;
            int W = all[tmpE][i]->w;
            if (dist[E] > dist[tmpE] + W) {
                dist[E] = dist[tmpE] + W;
                pq.push(all[tmpE][i]);
            }
        }
    }
    int dis = dist[N - 1];
    cout << dis << " ";
}

// step 4.2
void ShortestPath_water() {
    // // just check all no_water_path
    // for (int i = 0; i < no_water_paths.size(); ++i) {
    //     vector<int> path;
    //     vector<int> old = no_water_paths[i];
    //     path.push_back(0);
    //     int old_pos = 0;
    //     bool wrong = false;
    //     // add some wait nodes if it is necessary
    //     for (int j = 1; j < old.size(); ++j) {
    //         int pos = old[j];
    //         for (int k = 0; k < water.size(); ++k) {
    //             if (water[k][pos] > 0) {
    //                 if (water[k][old_pos] == 0) {
    //                     path.push_back(old_pos);
    //                 } else {
    //                     wrong = true;
    //                     break;
    //                 }
    //             } else {
    //                 old_pos = pos;
    //                 path.push_back(pos);
    //                 break;
    //             }
    //         }
    //     }
    //     if (wrong == true) continue;
    //     water_paths.push_back(path);
    // }
    // // cout << "size = " << water_paths.size() << endl;
    // int min = INT32_MAX;
    // vector<int> min_path;
    // // find the sortest water_path
    // // NOTE: min--
    // for (int i = 0; i < water_paths.size(); ++i) {
    //     if (water_paths[i].size() < min) {
    //         min = water_paths[i].size();
    //         min_path = water_paths[i];
    //     }
    // }
    // min--;
    // cout << min << endl;
    // for (int i = 0; i < min_path.size() - 1; ++i) {
    //     cout << min_path[i] << ",";
    // }
    // cout << min_path.back() << endl;
    // return ;

    vector<int> checked = vector<int>(N, 0);
    vector<int> path;
    path.push_back(0);
    int cur_t = 0;
    FindNoWaterPath(path, checked, 0, cur_t);
    int min = INT32_MAX;
    vector<int> res;
    for (int i = 0; i < water_paths.size(); ++i) {
        if (water_paths[i].size() < min) {
            min = water_paths[i].size();
            res = water_paths[i];
        }
    }
    min--;
    cout << min << endl;
    for (int i = 0; i < min; ++i) {
        cout << res[i] << ",";
    }
    cout << res.back() << endl;
}

// step 4
void CalcShortestPath() {
    // find shortest no water path
    ShortestPath_no_water();
    // find shortest water path
    ShortestPath_water();
    return ;
}

int main() {
    // step 2
    string filename;
    cin >> filename;
    LoadFile(filename.c_str());
    // step 3
    int t = SimFlow();
    cout << t << endl;
    // step 4
    CalcShortestPath();
    // step 5
    // (GUI part is escaped)
    return 0;
}
