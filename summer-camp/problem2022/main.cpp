#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <algorithm>
using namespace std;

#define CORE 1
#define BORDER 2
#define OUTLIER 3

int N = 0;
int E = 0;
int P = 0;
int num_cores = 0;
int num_borders = 0;
int num_outliers = 0;
int num_clusters = 0;
vector<string> lines;
vector<int> types;
vector<vector<int>> sets;
vector<vector<int>> all_dis;

// 2.1
void LoadData(const char* filename) {
    ifstream f;
    f.open(filename, ios::in);
    if (!f.is_open()) {
        cout << "fail to open " << filename << endl;
        return ;
    }
    string line = "";
    getline(f, line); // read the first line
    // then get N E P from the first line
    int idx = line.find_first_of(' ');
    string ss = line.substr(0, idx);
    N = atoi(ss.c_str());
    line = line.substr(idx + 1);
    idx = line.find_first_of(' ');
    ss = line.substr(0, idx);
    E = atoi(ss.c_str());
    ss = line.substr(idx + 1);
    P = atoi(ss.c_str());
    int min = INT32_MAX;
    int max = 0;
    while (!f.eof()) {
        string line = "";
        f >> line;
        if (line == "") continue;
        lines.push_back(line);
        if (min > line.size()) min = line.size();
        if (max < line.size()) max = line.size();
        // cout << line << endl;
    }
    cout << min << " " << max << endl;
    f.close(); // remeber to close file after all operations are finished
}

int three_min(int a, int b, int c) {
    if (a <= b && a <= c) return a;
    if (b <= a && b <= c) return b;
    return c;
}

// 2.2
int LevDist(const string &a, const string &b) {
    // a dp problem
    int l1 = a.size();
    int l2 = b.size();
    vector<vector<int>> dp(l1 + 1, vector<int>(l2 + 1, 0));
    for (int i = 0; i <= l1; ++i) {
        dp[i][0] = i;
    }
    for (int i = 0; i <= l2; ++i) {
        dp[0][i] = i;
    }
    for (int i = 1; i <= l1; ++i) {
        for (int j = 1; j <= l2; ++j) {
            int tmp = (a[i - 1] == b[j - 1]) ? dp[i - 1][j - 1] : (dp[i - 1][j - 1] + 1);
            int tmp1 = dp[i][j - 1] + 1;
            int tmp2 = dp[i - 1][j] + 1;
            dp[i][j] = three_min(tmp, tmp1, tmp2);
        }
    }
    return dp[l1][l2];
}

// 2.3
vector<string> RangeQuery(vector<string>& lines, string& target, int eps) {
    vector<string> res;
    for (int i = 0; i < lines.size(); ++i) {
        int dis = LevDist(target, lines[i]);
        if (dis <= eps) res.push_back(lines[i]);
    }
    return res;
}

// // 2.4
vector<vector<string>> DBSCAN(vector<string>& lines, int eps, int p, int& num_cores, int& num_borders, int& num_outliers, int& num_clusters) {
    num_cores = 0;
    num_borders = 0;
    num_outliers = 0;
    num_clusters = 0;
    vector<vector<string>> res; // clusters set
    // vector<int> types(lines.size(), 0);
    // vector<vector<int>> sets;
    types = vector<int>(lines.size(), 0);
    sets.clear();
    // compute dis
    for (int i = 0; i < lines.size(); ++i) {
        vector<int> one_set;
        for (int j = 0; j < lines.size(); ++j) {
            int dis = LevDist(lines[i], lines[j]);
            if (dis <= eps) one_set.push_back(j);
        }
        sets.push_back(one_set);
    }
    // cout << "get here!" << endl;
    // decide types
    for (int i = 0; i < sets.size(); ++i) {
        vector<int> one_set = sets[i];
        if (one_set.size() >= p) {
            types[i] = CORE;
        }
    }
    for (int i = 0; i < sets.size(); ++i) {
        vector<int> one_set = sets[i];
        int size = one_set.size();
        if (size < p) {
            bool have_core = false;
            for (int j = 0; j < size; ++j) {
                if (types[one_set[j]] == CORE) {
                    have_core = true;
                    break;
                }
            }
            if (have_core) types[i] = BORDER;
            else types[i] = OUTLIER;
        }
    }
    // count number for every kind of node
    for (int i = 0; i < lines.size(); ++i) {
        if (types[i] == CORE) num_cores++;
        if (types[i] == BORDER) num_borders++;
        if (types[i] == OUTLIER) num_outliers++;
    }
    // compute cluster number
    // similar to do BFS inside a forest.
    vector<int> checked(lines.size(), 0);
    for (int i  = 0; i < lines.size(); ++i) {
        if (checked[i] == 1) continue;
        if (types[i] == OUTLIER) continue;
        checked[i] = 1;
        vector<string> one_cluster;
        queue<int> que;
        que.push(i);
        while (!que.empty()) {
            int top = que.front();
            que.pop();
            one_cluster.push_back(lines[top]);
            for (int j = 0; j < sets[top].size(); ++j) {
                if (checked[sets[top][j]] == 1) continue;
                if (types[sets[top][j]] == OUTLIER) continue;
                que.push(sets[top][j]);
                checked[sets[top][j]] = 1;
            }
        }
        res.push_back(one_cluster);
    }
    num_clusters = res.size();
    return res;
}

// 2.5
int MinEPS(vector<string>& lines, int p) {
    // // passes, but may timeout in large dataset)
    // while (true) {
    //     // sets.clear();
    //     // for (int i = 0; i < lines.size(); ++i) types[i] = 0;
    //     DBSCAN(lines, E, P, num_cores, num_borders, num_outliers, num_clusters);
    //     if (num_outliers == 0) return E;
    //     E++;
    // }
    // return E;

    // compute outlier points first
    DBSCAN(lines, E, P, num_cores, num_borders, num_outliers, num_clusters);
    vector<int> min_dis;
    for (int i = 0; i < lines.size(); ++i) {
        int min = INT32_MAX;
        if (types[i] != OUTLIER) continue;
        for (int j = 0; j < lines.size(); ++j) {
            if (types[j] != CORE) continue;
            int dis = LevDist(lines[i], lines[j]);
            if (min > dis) {
                min = dis;
            }
        }
        min_dis.push_back(min);
    }
    sort(min_dis.begin(), min_dis.end());
    return min_dis[min_dis.size() - 1]; // get the biggest smallest dis from a outlier to core point.
}

int main() {
    string file_name;
    cin >> file_name;
    LoadData(file_name.c_str());
    int res = LevDist(lines[0], lines[1]);
    cout << res << endl;
    vector<string> got_set = RangeQuery(lines, lines[0], E);
    cout << got_set.size() << endl;
    DBSCAN(lines, E, P, num_cores, num_borders, num_outliers, num_clusters);
    cout << num_cores << " " << num_borders << " " << num_outliers << " " << num_clusters << endl;
    cout << MinEPS(lines, P) << endl;
    return 0;
}
