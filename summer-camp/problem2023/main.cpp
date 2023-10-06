#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <memory>

using namespace std;

vector<string> words;
vector<string> encrypt_words;
vector<int> all_index;
vector<string> all_max;
vector<int> max_i;
vector<int> max_j;
vector<string> all_path;
vector<int> costs;

vector<vector<int>> dir{{0, -1}, {0, 1}, {1, 0}}; // left, right, down

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

int min_cost = 1000000000;
string min_path = "";

struct TreeNode {
    bool isWord;
    map<char, TreeNode*> children;
    TreeNode() = default;
    void Insert(string s) {
        if (s.empty()) {
            isWord = true;
            return ;
        }
        string ss = s.substr(1);
        if (children[s[0]] == NULL) {
            children[s[0]] = new TreeNode();
        }
        children[s[0]]->Insert(ss);
        // TreeNode* tmp = this;
        // for (int i = 0; i < s.size(); ++i) {
        //     if (children[s[i]] == NULL) {
        //         children[s[i]] = new TreeNode();
        //     }
        //     tmp = children[s[i]];
        // }
        // tmp->isWord = true;
    }
    bool find(string s) {
        if (s.empty()) return isWord;
        string ss = s.substr(1);
        return children.count(s[0]) != 0 && children[s[0]] != NULL && children[s[0]]->find(ss);
    }
};

// class TreeNode;

// class TreeNode {
// public:
//     std::map<char, std::shared_ptr<TreeNode>> children;
//     TreeNode() = default;


//     bool isWord = false;
//     void Insert(const std::string &w) {
//         if (w.empty()) {
//             isWord = true;
//             return;
//         }
//         auto n = std::string{w.begin() + 1, w.end()};
//         if (children[w[0]] == nullptr) {
//             children[w[0]] = std::make_shared<TreeNode>();
//         }
//         children[w[0]]->Insert(n);
//     }

//     [[nodiscard]] bool find(const std::string &w) const {
//         if (w.empty()) {
//             return isWord;
//         }
//         auto n = std::string{w.begin() + 1, w.end()};
//         return children.count(w[0]) && children.at(w[0]) != nullptr && children.at(w[0])->find(n);
//     }
// };

void LoadData(const char* filename) {
    ifstream in;
    in.open(filename, ios::in);
    if (!in.is_open()) return ;
    string map_name = "";
    getline(in, map_name);
    string encrypt_name = "";
    getline(in, encrypt_name);
    in.close();
    ifstream in1;
    in1.open(map_name.c_str(), ios::in);
    ifstream in2;
    in2.open(encrypt_name.c_str(), ios::in);
    string line = "";
    int c_num = 0;
    int encrypt_num = 0;
    while (!in1.eof()) {
        in1 >> line;
        if (line.empty()) continue;
        words.push_back(line);
        if (line.size() > 0 && line[0] == 'c') c_num++;
    }
    in1.close();
    line = "";
    while (!in2.eof()) {
        line = "";
        in2 >> line;
        if (line.empty()) continue;
        encrypt_words.push_back(line);
        // cout << line << endl;
        encrypt_num++;
    }
    in2.close();
    cout << c_num << " " << encrypt_num << endl;
    return ;
}

void InsertWord(TreeNode* root, string word) {
    for (int i = 0; i < word.size(); ++i) {
        if (root->children[word[i]] == NULL) {
            root->children[word[i]] = new TreeNode();
        }
        root = root->children[word[i]];
    }
}

TreeNode* BuildTree() {
    auto res = new TreeNode();
    // TreeNode* tmp = res;
    // cout << "init num = " << res->children.size() << endl;
    // cout << "word size = " << words.size() << endl;
    for (int i = 0; i < words.size(); ++i) {
        // TreeNode* tmp = res;
        // InsertWord(tmp, words[i]);
        res->Insert(words[i]);
    }
    return res;
}

// a level-traverse
void CountLeaf(TreeNode* root) {
    int num = 0;
    // cout << "root children num = " << root->children.size() << endl;
    queue<TreeNode*> que;
    que.push(root);
    while (!que.empty()) {
        int size = que.size();
        for (int i = 0; i < size; ++i) {
            auto node = que.front();
            // cout << "node children num = " << node->children.size() << endl;
            que.pop();
            if (node->children.size() == 0) num++;
            if (node->children.size() > 0) {
                for (auto it1 = node->children.begin(); it1 != node->children.end(); ++it1) {
                    // string a_char = it1->first;
                    // cout << it1->first;
                    que.push(it1->second);
                }
                // cout << endl;
            }
        }
        // cout << endl;
    }
    cout << num << endl;
}

string Decrypt(string word, int idx) {
    string res = word;
    for (int i = 0; i < word.size(); ++i) {
        res[i] = (res[i] - 'a' + 260 + idx % 26) % 26 + 'a';
    }
    return res;
}

void CrackCodeInc(TreeNode* root) {
    int idx = 0;
    for (int i = 0; i < encrypt_words.size(); ++i) {
        string word = encrypt_words[i];
        while (true) {
            // cout << idx << endl;
            string w1 = word;
            for (int j = 0; j < word.size(); ++j) {
                // int t = (w1[j] - 'a' - idx % 26);
                // if (t < 0) t = -t;
                // t = t % 26;
                // w1[j] = t + 'a';
                w1[j] = (w1[j] - 'a' + 25 - idx % 26) % 26 + 'a';
            }
            // cout << w1 << endl;
            if (root->find(w1) == true) {
                all_index.push_back(idx + 1);
                idx++;
                break;
            } else {
                idx++;
            }
        }
        // while (root->find(Decrypt(word, -idx)) == false) idx++;
        // all_index.push_back(idx);
        // idx++;
    }
    cout << all_index[0] << " " << all_index[all_index.size() - 1] << endl;
}

void AlwaysRight(TreeNode* root, string& res, int& start_i, int& start_j, int i, int j) {
    string tmp = "";
    for (int k = j; k < encrypt_words[i].size(); ++k) {
        tmp.push_back(encrypt_words[i][k]);
        if (!root->find(tmp)) continue ;
        // found
        if (res.size() < tmp.size() || (res.size() == tmp.size() && res > tmp)) {
            res = tmp;
            start_i = i;
            start_j = j;
        }
    }
    return ;
}

void AlwaysDown(TreeNode* root, string& res, int& start_i, int& start_j, int i, int j) {
    string tmp = "";
    for (int k = i; k < encrypt_words.size() && j < encrypt_words[k].size(); ++k) {
        tmp.push_back(encrypt_words[k][j]);
        if (!root->find(tmp)) continue ;
        // found
        if (res.size() < tmp.size() || (res.size() == tmp.size() && res > tmp)) {
            res = tmp;
            start_i = i;
            start_j = j;
        }
    }
    return ;
}

void AlwaysRightDown(TreeNode* root, string& res, int& start_i, int& start_j, int i, int j) {
    string tmp = "";
    for (int k = 0; i + k < encrypt_words.size() && j + k < encrypt_words[i + k].size(); ++k) {
        tmp.push_back(encrypt_words[i + k][j + k]);
        if (!root->find(tmp)) continue ;
        // found
        if (res.size() < tmp.size() || (res.size() == tmp.size() && res > tmp)) {
            res = tmp;
            start_i = i;
            start_j = j;
        }
    }
    return ;
}

void CodeInCode(TreeNode* root) {
    string res = "";
    int start_i = 0;
    int start_j = 0;
    TreeNode* tmp = root;
    for (int i = 0; i < encrypt_words.size(); ++i) {
        for (int j = 0; j < encrypt_words[i].size(); ++j) {
            // no need to change direction!!!!
            tmp = root;
            AlwaysRight(tmp, res, start_i, start_j, i, j);
            tmp = root;
            AlwaysDown(tmp, res, start_i, start_j, i, j);
            tmp = root;
            AlwaysRightDown(tmp, res, start_i, start_j, i, j);
        }
    }
    cout << start_i << " " << start_j << " " << res << endl;
}

int compute(char start, char end) {
    if (start == end) return 26;
    if (start < end) return end - start;
    return 26 + end - start;
}

// can pass sample.txt, but is too slow!!!
void CheckPath(int i, int j, string path, int cost, vector<vector<int>>& checked) {
    // if (i < 0 || i > encrypt_words.size() - 1 || j < 0 || j > encrypt_words[i].size() - 1 || checked[i][j] == 1) {
    //     return ;
    // }
    // int old = 0;
    // if (dir == 'd') {
    //     if (i > 0 && j < encrypt_words[i - 1].size()) {
    //         old = compute(encrypt_words[i - 1][j], encrypt_words[i][j]);
    //     }
    // }
    // if (dir == 'l') {
    //     if (j < encrypt_words[i].size() - 1) {
    //         old = compute(encrypt_words[i][j + 1], encrypt_words[i][j]);
    //     }
    // }
    // if (dir == 'r') {
    //     if (j > 0) {
    //         old = compute(encrypt_words[i][j - 1], encrypt_words[i][j]);
    //     }
    // }
    // cost += old;
    // if (i == encrypt_words.size() - 1 && j == encrypt_words[i].size() - 1) {
    //     // all_path.push_back(path);
    //     if (min_cost > cost) {
    //         min_cost = cost;
    //         min_path = path;
    //     } else {
    //         if (min_cost == cost) {
    //             if (path < min_path) min_path = path;
    //         }
    //     }
    //     return ;
    // }
    // checked[i][j] = 1;
    // path += "d";
    // CheckPath(i + 1, j, path, cost, checked, 'd');
    // // if (b1) return true;
    // path.pop_back();
    // cost -= old;
    // cost += old;
    // path += "l";
    // CheckPath(i, j - 1, path, cost, checked, 'l');
    // // if (b2) return true;
    // path.pop_back();
    // cost -= old;
    // cost += old;
    // path += "r";
    // CheckPath(i, j + 1, path, cost, checked, 'r');
    // // if (b3) return true;
    // path.pop_back();
    // cost -= old;
    // checked[i][j] = 0;
    // // return b1 || b2 || b3;

    if (i < 0 || i >= encrypt_words.size() || j < 0 || j >= encrypt_words[i].size()) return ;
    if (i == encrypt_words.size() - 1 && j == encrypt_words[i].size() - 1) {
        // cout << "path = " << path << endl;
        if (min_cost > cost) {
            min_cost = cost;
            min_path = path;
        } else {
            if (min_cost == cost) {
                if (path < min_path) min_path = path;
            }
        }
        return ;
    }
    if (checked[i][j] == 1) return ;
    checked[i][j] = 1;
    for (int k = 0; k < 3; ++k) {
        int x = i + dir[k][0];
        int y = j + dir[k][1];
        if (x < 0 || x >= encrypt_words.size() || y < 0 || y >= encrypt_words[x].size()) continue;
        if (k == 0) {
            path.push_back('l');
        }
        if (k == 1) {
            path.push_back('r');
        }
        if (k == 2) {
            path.push_back('d');
        }
        int c = compute(encrypt_words[i][j], encrypt_words[x][y]);
        cost += c;
        if (cost > min_cost) {
            cost -= c;
            path.pop_back();
            continue;
        }
        if (cost == min_cost && path >= min_path) {
            cost -= c;
            path.pop_back();
            continue;
        }
        if (cost == min_cost && (x != encrypt_words.size() || y != encrypt_words[x].size())) {
            cost -= c;
            path.pop_back();
            continue;
        }
        CheckPath(x, y, path, cost, checked);
        cost -= c;
        path.pop_back();
    }
    checked[i][j] = 0;
}

string getPath(vector<int> prev, int start_pos, int end_pos, map<int, pair<int, int>> reverse_pairs) {
    vector<int> shortest;
    while (start_pos != end_pos) {
        shortest.push_back(end_pos);
        end_pos = prev[end_pos];
    }
    shortest.push_back(start_pos);
    int min_len = shortest.size();
    string min_path = "";
    for (int i = shortest.size() - 1; i > 0; --i) {
        auto p1 = reverse_pairs[shortest[i]];
        auto p2 = reverse_pairs[shortest[i - 1]];
        int x1 = p1.first;
        int y1 = p1.second;
        int x2 = p2.first;
        int y2 = p2.second;
        if (x1 == x2 && y2 == y1 - 1) min_path += "l";
        if (x1 == x2 && y2 == y1 + 1) min_path += "r";
        if (y1 == y2 && x2 == x1 + 1) min_path += "d";
    }
    return min_path;
}

void PathInCode() {
    // string path = "";
    // vector<vector<int>> checked;
    // for (int i = 0; i < encrypt_words.size(); ++i) {
    //     vector<int> line;
    //     for (int j = 0; j < encrypt_words[i].size(); ++j) line.push_back(0);
    //     checked.push_back(line);
    // }
    // CheckPath(0, 0, path, 0, checked);

    // cout << min_path.size() << " " << min_cost << " " << min_path << endl;

    map<pair<int, int>, int> pairs;
    map<int, pair<int, int>> reverse_pairs;
    vector<vector<Edge*>> edges;
    int sum = 0;
    for (int i = 0; i < encrypt_words.size(); ++i) {
        for (int j = 0; j < encrypt_words[i].size(); ++j) {
            auto p = make_pair(i, j);
            pairs[p] = sum;
            reverse_pairs[sum] = p;
            sum++;
        }
    }
    edges = vector<vector<Edge*>>(sum);
    int limit = INT32_MAX;
    vector<int> dist(sum, limit);
    vector<int> prev(sum, 0);
    dist[0] = 0;
    prev[0] = 0;
    int idx = 0;
    for (int i = 0; i < encrypt_words.size(); ++i) {
        for (int j = 0; j < encrypt_words[i].size(); ++j) {
            for (int k = 0; k < 3; ++k) {
                int x = i + dir[k][0];
                int y = j + dir[k][1];
                if (x < 0 || x >= encrypt_words.size() || y < 0 || y >= encrypt_words[x].size()) continue;
                int w = compute(encrypt_words[i][j], encrypt_words[x][y]);
                edges[idx].push_back(new Edge(w, pairs[make_pair(x, y)]));
            }
            idx++;
        }
    }
    priority_queue<Edge*, vector<Edge*>, cmpEdge> pq;
    pq.push(new Edge(0, 0));
    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        int end = top->e;
        int weight = top->w;
        for (int i = 0; i < edges[end].size(); ++i) {
            int tmpE = edges[end][i]->e;
            int tmpW = edges[end][i]->w;
            if (dist[tmpE] > dist[end] + tmpW) {
                dist[tmpE] = dist[end] + tmpW;
                prev[tmpE] = end;
                pq.push(edges[end][i]);
            } else {
                if (dist[tmpE] == dist[end] + tmpW) {
                    string path1 = getPath(prev, 0, tmpE, reverse_pairs);
                    vector<int> prev1 = prev;
                    prev1[tmpE] = end;
                    string path2 = getPath(prev1, 0, tmpE, reverse_pairs);
                    if (path1 > path2) {
                        prev[tmpE]= end;
                        pq.push(edges[end][i]);
                    }
                }
            }
        }
    }
    int min_dist = dist[sum - 1];
    vector<int> shortest;
    int end_pos = sum - 1;
    int start_pos = 0;
    while (start_pos != end_pos) {
        shortest.push_back(end_pos);
        end_pos = prev[end_pos];
    }
    shortest.push_back(start_pos);
    int min_len = shortest.size();
    string min_path = "";
    for (int i = shortest.size() - 1; i > 0; --i) {
        auto p1 = reverse_pairs[shortest[i]];
        auto p2 = reverse_pairs[shortest[i - 1]];
        int x1 = p1.first;
        int y1 = p1.second;
        int x2 = p2.first;
        int y2 = p2.second;
        if (x1 == x2 && y2 == y1 - 1) min_path += "l";
        if (x1 == x2 && y2 == y1 + 1) min_path += "r";
        if (y1 == y2 && x2 == x1 + 1) min_path += "d";
    }
    min_len = min_path.size();
    // vector<string> parts;
    // while (min_path.size() > 20) {
    //     string part = min_path.substr(0, 20);
    //     min_path = min_path.substr(20);
    //     parts.push_back(part);
    // }
    // parts.push_back(min_path);
    cout << min_len << " " << min_dist << " " << min_path << endl;
}

int main() {
    string filename;
    cin >> filename;
    LoadData(filename.c_str());
    // sort(words.begin(), words.end());
    auto res = BuildTree();
    // shared_ptr<TreeNode> tmp = res;
    CountLeaf(res);
    CrackCodeInc(res);
    CodeInCode(res);
    PathInCode();
    return 0;
}
