#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

map<char, int> freqTable;
vector<pair<char, int>> freqVec;
map<char, string> codingTable;
string code = "";

static bool cmp(pair<char, int>& p1, pair<char, int>& p2) {
    if (p1.second == p2.second) return p1.first < p2.first;
    return p1.second > p2.second;
}

static bool cmp1(pair<char, string>& p1, pair<char, string>& p2) {
    return p1.first < p2.first;
}

struct huffNode {
    char c;
    int w;
    string ascii;
    huffNode* left;
    huffNode* right;
    huffNode(char c1, int w1, string a1) {
        c = c1;
        w = w1;
        ascii = a1;
        left = NULL;
        right = NULL;
    }
    bool operator>(const huffNode* h) const {
        return w > h->w; 
    }
};

struct compare {
    bool operator() (huffNode* a, huffNode* b) 
    {
        return a->w > b->w;
    }
};

huffNode* huffmanTree = NULL;

vector<huffNode*> traverse;

// step 1: statistics
void do_statistics(const char* txt) {
    freqTable.clear();
    ifstream f;
    f.open(txt, ios::in);
    if (!f.is_open()) {
        cout << "fail to open txt path!" << endl;
        return ;
    }
    string line = "";
    while (getline(f, line)) {
        // freqTable['\n']++;
        // cout << line << endl;
        for (int i = 0; i < line.size(); ++i) {
            freqTable[line[i]]++;
        }
    }
    // sort the map
    std::vector<std::pair<char, int>> vec(freqTable.begin(), freqTable.end());
    sort(vec.begin(), vec.end(), cmp);
    freqVec = vec;
    int N = freqTable.size();
    cout << N << endl;
    for (int i = 0; i < 3; ++i) {
        cout << vec[i].first << " " << vec[i].second << endl;
    }
}

// step 2: build_tree
void build_tree() {
    priority_queue<huffNode*, vector<huffNode*>, compare> pq;
    for (int i = 0; i < freqVec.size(); ++i) {
        huffNode* h = new huffNode(freqVec[i].first, freqVec[i].second, "");
        pq.push(h);
    }
    while (!pq.empty()) {
        if (pq.size() == 1) break;
        huffNode* h1 = pq.top();
        pq.pop();
        huffNode* h2 = pq.top();
        pq.pop();
        // cout << h1->w << " " << h2->w << endl;
        huffNode* new_h = new huffNode('@', h1->w + h2->w, "");
        if (h1->w == h2->w) {
            new_h->left = (h1->c < h2->c) ? h1: h2;
            new_h->right = (h1->c < h2->c) ? h2 : h1;
        } else {
            new_h->left = (h1->w < h2->w) ? h1: h2;
            new_h->right = (h1->w < h2->w) ? h2 : h1;
        }
        new_h->c = (new_h->left->c < new_h->right->c) ? new_h->left->c : new_h->right->c;
        pq.push(new_h);
    }
    huffmanTree = pq.top();
    pq.pop();
}

// step 2: compute tree's height
int computeHeight(huffNode* h) {
    if (h == NULL) return 0;
    int left = computeHeight(h->left);
    int right = computeHeight(h->right);
    int tmp = (left < right) ? right : left;
    return 1 + tmp;
}

// step 3: encode huffmanTree's nodes
void encode(huffNode* h) {
    if (h == NULL) return ;
    if (h->left == NULL && h->right == NULL) {
        h->ascii = code;
        return ;
    }
    if (h->left) {
        code += "0";
        encode(h->left);
        code = code.substr(0, code.size() - 1);
    }
    if (h->right) {
        code += "1";
        encode(h->right);
        code = code.substr(0, code.size() - 1);
    }
    return ;
}

// step 3: get all leaf nodes
void Traverse(huffNode* h) {
    if (h == NULL) return ;
    traverse.push_back(h);
    Traverse(h->left);
    Traverse(h->right);
}

// step 4: compress the given stringstream to generate a byte stream
void compress(const char* txt, const char* huffzip) {
    ifstream f;
    f.open(txt, ios::in);
    string line = "";
    string stream = "";
    while (getline(f, line)) {
        for (int i = 0; i < line.size(); ++i) {
            stream += codingTable[line[i]];
        }
    }
    f.close();
    // first cout the available bit stream length.
    cout << stream.size() << endl;
    // then generate the real bute stream and write it into a target file.
    int len = stream.size();
    // add appending zeros
    if (len % 8 != 0) {
        int num = len / 8;
        for (int i = len; i < 8 * (num + 1); ++i) stream += "0";
    }
    ofstream out;
    out.open(huffzip, ios::binary);
    int N = stream.size();
    char* array = new char[stream.size() / 8];
    for (int i = 0; i < stream.size() / 8; ++i) {
        string p = stream.substr(0, 8);
        stream = stream.substr(8);
        int n = 0;
        for (int j = 7; j >= 0; --j) {
            n += (p[j] - '0') * (1 << (7 - j));
        }
        array[i] = n;
    }
    out.write(array, N / 8);
    out.close();
}

int main() {
    string txt = "";
    string huffidx = "";
    string huffzip = "";
    cin >> txt >> huffidx >> huffzip;
    // step 1
    do_statistics(txt.c_str());
    // step 2
    build_tree();
    huffNode* tmpH = huffmanTree;
    int height = computeHeight(tmpH) - 1; // root's height = 0
    cout << height << endl;
    // step 3
    tmpH = huffmanTree;
    encode(tmpH);
    // then traverse all leaf nodes
    tmpH = huffmanTree;
    Traverse(tmpH);
    for (int i = 0; i < traverse.size(); ++i) {
        huffNode* node = traverse[i];
        if (node->left == NULL && node->right == NULL) {
            codingTable[node->c] = node->ascii;
            // cout << node->c << " " << node->ascii << endl;
        }
    }
    cout << codingTable['e'] << endl;
    vector<pair<char, string>> vec1(codingTable.begin(), codingTable.end());
    sort(vec1.begin(), vec1.end(), cmp1);
    // then write the result codings into file
    ofstream out;
    out.open(huffidx.c_str(), ios::trunc | ios::out);
    for (int i = 0; i < vec1.size(); ++i) {
        stringstream ss;
        std::string first = "";
        first = vec1[i].first;
        string line = first + " " + vec1[i].second;
        ss << line;
        out << ss.str() << endl;
    }
    out.close();
    // step 4: compress
    compress(txt.c_str(), huffzip.c_str());
    return 0;
}