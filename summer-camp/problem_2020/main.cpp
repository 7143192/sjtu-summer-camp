#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <bitset>
#include <algorithm>
using namespace std;

// used to store original file contents.
vector<string> lines;
// used to store a map for <char, occur times>
map<char, int> freqTable;
// used to store map of <char, coding>
map<char, string> codingTable;

struct huffmanNode {
    int w; // weight
    char c; // character that this node presents
    string coding; // ascii code, but this ascii code is DIFFERENT from the original acsii code
    huffmanNode* left;
    huffmanNode* right;
    huffmanNode() = default;
    huffmanNode(int w1, char c1, string a1) {
        w = w1;
        c = c1;
        coding = a1;
        left = NULL;
        right = NULL;
    }
};

// used to store the result of build_tree
struct huffmanNode* huffmanTree;

vector<huffmanNode*> code_vec;
vector<huffmanNode*> code_vec_sort;

static bool cmp1(pair<char, int> p1, pair<char, int> p2) {
    if (p1.second == p2.second) return p1.first < p2.first;
    return p1.second > p2.second;
}

struct cmp_node {
    bool operator() (huffmanNode* h1, huffmanNode* h2) {
        if (h1->w == h2->w) return h1->c > h2->c;
        return h1->w > h2->w;
    }
};

static bool cmp2(huffmanNode* h1, huffmanNode* h2) {
    return h1->c < h2->c;
}

static bool cmp3(huffmanNode* h1, huffmanNode* h2) {
    if (h1->w == h2->w) return h1->c < h2->c;
    return h1->w < h2->w;
}

// step 1
void do_statics(const char* filename) {
    ifstream in;
    in.open(filename, ios::in);
    // read file content
    string s = "";
    // NOTE: '\n' needs to be counted
    while (getline(in, s)) {
        // count frequence
        lines.push_back(s);
        for (int i = 0; i < s.size(); ++i) {
            freqTable[s[i]]++;
        }
    }
    in.close();
    // sort
    vector<pair<char, int>> v1(freqTable.begin(), freqTable.end());
    sort(v1.begin(), v1.end(), cmp1);
    // output
    cout << freqTable.size() << endl;
    for (int i = 0; i < 3; ++i) {
        cout << v1[i].first << " " << v1[i].second << endl;
    }
}

// step 2
void build_tree() {
    // use a max heap (?) to build this tree.
    priority_queue<huffmanNode*, vector<huffmanNode*>, cmp_node> pq;
    // build the min heap first.
    for (auto it = freqTable.begin(); it != freqTable.end(); ++it) {
        huffmanNode* h = new huffmanNode(it->second, it->first, "");
        pq.push(h);
    }
    // then build the huffmanTree
    while (pq.size() > 1) {
        // pop two nodes from the min heap
        huffmanNode* h1 = pq.top();
        pq.pop();
        huffmanNode* h2 = pq.top();
        pq.pop();
        huffmanNode* new_h = new huffmanNode();
        new_h->c = (h1->c < h2->c) ? h1->c : h2->c;
        new_h->w = h1->w + h2->w;
        new_h->coding = "";
        // new_h->left = (h1->c < h2->c) ? h1 : h2;
        // new_h->right = (h1->c < h2->c) ? h2 : h1;
        if (h1->w == h2->w) {
            new_h->left = (h1->c < h2->c) ? h1 : h2;
            new_h->right = (h1->c < h2->c) ? h2 : h1;
        } else {
            new_h->left = (h1->w < h2->w) ? h1 : h2;
            new_h->right = (h1->w < h2->w) ? h2 : h1;
        }
        pq.push(new_h);
    }
    huffmanTree = pq.top();
    pq.pop();
    return ;

    // vector<huffmanNode*> nodes;
    // for (auto it = freqTable.begin(); it != freqTable.end(); ++it) {
    //     huffmanNode* h = new huffmanNode(it->second, it->first, "");
    //     nodes.push_back(h);
    // }
    // sort(nodes.begin(), nodes.end(), cmp3);
    // while (nodes.size() > 1) {
    //     huffmanNode* h1 = nodes[0];
    //     huffmanNode* h2 = nodes[1];
    //     nodes = vector<huffmanNode*>(nodes.begin() + 2, nodes.end());
    //     huffmanNode* new_h = new huffmanNode();
    //     new_h->c = (h1->c < h2->c) ? h1->c : h2->c;
    //     new_h->w = h1->w + h2->w;
    //     new_h->coding = "";
    //     if (h1->w == h2->w) {
    //         new_h->left = (h1->c < h2->c) ? h1 : h2;
    //         new_h->right = (h1->c < h2->c) ? h2 : h1;
    //     } else {
    //         new_h->left = (h1->w < h2->w) ? h1 : h2;
    //         new_h->right = (h1->w < h2->w) ? h2 : h1;
    //     }
    //     nodes.push_back(new_h);
    //     sort(nodes.begin(), nodes.end(), cmp3);
    // }
    // huffmanTree = nodes[0];
    // return ;
}

// use a recursive function to compute height
int Height(huffmanNode* root) {
    if (root == NULL) return 0;
    int left = Height(root->left);
    int right = Height(root->right);
    int tmp = (left > right) ? left : right;
    tmp++;
    return tmp;
}

// step 3
// use a recursive function to build all codings
void encode(huffmanNode* root, string code) {
    if (root == NULL) return ;
    // terminate condition: meet a real leaf node, a.k.a, a real-char node
    if (root->left == NULL && root->right == NULL) {
        root->coding = code;
        return ;
    }
    code += "0";
    encode(root->left, code);
    code.pop_back();
    code += "1";
    encode(root->right, code);
    code.pop_back();
    return ;
}

// use a mid-traverse to collect all coding
void traverse(huffmanNode* root) {
    // if (root == NULL) return ;
    // code_vec.push_back(root);
    // traverse(root->left);
    // traverse(root->right);

    queue<huffmanNode*> que;
    que.push(root);
    while (!que.empty()) {
        int size = que.size();
        for (int i = 0; i < size; ++i) {
            auto top = que.front();
            que.pop();
            code_vec.push_back(top);
            // cout << "(" << top->c << ", " << top->coding << ", " << top->w << "), ";
            if (top->left) que.push(top->left);
            if (top->right) que.push(top->right);
        }
        // cout << endl;
    }
}

// step 4
void compress(const char* zip_name) {
    string final_code = "";
    for (int i = 0; i < lines.size(); ++i) {
        string line = lines[i];
        for (int j = 0; j < line.size(); ++j) {
            final_code += codingTable[line[j]];
        }
    }
    cout << final_code.size() << endl;
    int size = final_code.size();
    // convert the 'size' to little-endian byte code first.
    string code1 = "";
    while (size != 0) {
        int tmp = size % 2;
        size /= 2;
        code1 = to_string(tmp) + code1;
    }
    // use 8 bytes to store this size element.
    while (code1.size() < 64) {
        code1 = "0" + code1;
    }
    // cout << "code1 = " << code1 << endl;
    // then convert to little endian mode.
    string little_code1 = "";
    for (int i = 0; i < 8; ++i) {
        string part = code1.substr(0, 8);
        code1 = code1.substr(8);
        little_code1 = part + little_code1;
    }
    // cout << "little code = " << little_code1 << endl;
    size = final_code.size();
    string tmp_code = final_code;
    if (size % 8 != 0) {
        int num = size / 8;
        for (int i = size; i < (num + 1) * 8; ++i) tmp_code += "0";
    }
    // get bit-stream (in string format)
    string got_code = little_code1 + tmp_code;
    // then change the bit string-bit-stream to real-byte stream.
    int c_num = got_code.size() / 8;
    char* res = new char[c_num];
    // every 8-bit <--> 1 byte <--> a char
    for (int i = 0; i < c_num; ++i) {
        string one = got_code.substr(0, 8);
        got_code = got_code.substr(8);
        int one_num = 0;
        for (int j = 0; j < 8; ++j) {
            one_num += ((one[j] - '0') << (7 - j));
        }
        char one_c = (char) one_num;
        res[i] = one_c;
    }
    ofstream out;
    out.open(zip_name, ios::binary);
    out.write(res, c_num);
    out.close();
}

int main() {
    string filename;
    string idx_name;
    string zip_name;
    cin >> filename;
    cin >> idx_name;
    cin >> zip_name;
    // step 1
    do_statics(filename.c_str());
    // step 2
    build_tree();
    huffmanNode* tmp = huffmanTree;
    // compute height
    int height = Height(tmp) - 1;
    cout << height << endl;
    // step 3
    string code = "";
    tmp = huffmanTree;
    encode(tmp, code);
    // then traverse the tree.
    tmp = huffmanTree;
    traverse(tmp);
    for (int i = 0; i < code_vec.size(); ++i) {
        if (code_vec[i]->left || code_vec[i]->right) continue;
        code_vec_sort.push_back(code_vec[i]);
        codingTable[code_vec[i]->c] = code_vec[i]->coding;
    }
    sort(code_vec_sort.begin(), code_vec_sort.end(), cmp2);
    // then write the encoding result into one output file.
    ofstream out;
    out.open(idx_name.c_str(), ios::out | ios::trunc);
    for (int i = 0; i < code_vec_sort.size(); ++i) {
        string c = "";
        c.push_back(code_vec_sort[i]->c);
        c += " ";
        c += code_vec_sort[i]->coding;
        stringstream sst;
        sst << c;
        out << sst.str() << endl;
    }
    out.close();
    // output 'e''s coding
    cout << codingTable['e'] << endl;
    // step 4
    compress(zip_name.c_str());
    return 0;
}