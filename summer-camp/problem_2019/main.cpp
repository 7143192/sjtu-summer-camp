#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <ctime>
#include <time.h>
#include <sys/time.h>

using namespace std;

int linear_size = 8; // used to record current linearHash size, original val = 8
int cuckoo_size = 8;
map<unsigned int, unsigned int> linear_map; // used to store all linearHash pairs
map<unsigned int, unsigned int> cuckoo_map;
vector<string> linear_vec = vector<string>(linear_size, ""); // linear hash array
vector<string> cuckoo_vec_1 = vector<string>(cuckoo_size, "");
vector<string> cuckoo_vec_2 = vector<string>(cuckoo_size, "");

void LinearInsert(unsigned int key, unsigned int val);

void CuckooInsert(unsigned int key, unsigned int val);

string CuckooGet(unsigned int key);

void CuckooDel(unsigned int key);

unsigned int convertStringToUint(string numStr) {
    unsigned int res = 0;
    for (int i = 0; i < numStr.size(); ++i) {
        res = res * 10 + (numStr[i] - '0');
        // cout << res << endl;
    }
    return res;
}

void LinearResize() {
    // first double-size the array
    linear_size *= 2;
    linear_vec.clear();
    linear_vec = vector<string>(linear_size, "");
    // then insert all kv pairs into the new map
    for (auto it = linear_map.begin(); it != linear_map.end(); ++it) {
        LinearInsert(it->first, it->second);
    }
}

void LinearInsert(unsigned int key, unsigned int val) {
    if (linear_map.count(key) != 0) {
        // the key has already existed in this LinearHash
        // reset directly
        linear_map[key] = val;
        return ;
    }
    // set map first.
    linear_map[key] = val;
    unsigned int idx = key % linear_size;
    while (linear_vec[idx] != "") {
        idx = (idx + 1) % linear_size;
    }
    linear_vec[idx] = std::to_string(key); // only put keys at the hash vec.
    // then check whther resizing is required.
    if (linear_map.size() > linear_size / 2) {
        LinearResize();
    }
    return ;
}

// return a string beacause "null" case 
string LinearGet(unsigned int key) {
    // unsigned int idx = key % linear_size;
    // while (linear_vec[idx] != key) {
    //     idx = (idx + 1) % linear_size;
    // }
    if (linear_map.count(key) == 0) return "null";
    return std::to_string(linear_map[key]);
}

void LinearDel(unsigned int key) {
    // first erase the pair from the linearMap
    auto it = linear_map.find(key);
    if (it == linear_map.end()) return ;
    linear_map.erase(it);
    // then remove this key from the linearVec
    string keyStr = std::to_string(key);
    int idx = 0;
    for (int i = 0; i < linear_size; ++i) {
        if (keyStr == linear_vec[i]) {
            idx = i;
            linear_vec[i] = ""; // reset to ""
            break;
        }
    }
    // then reorder the left pairs.
    for (int i = idx + 1; i < linear_size; ++i) {
        if (linear_vec[i] == "") continue;
        auto key = convertStringToUint(linear_vec[i]);
        linear_vec[i] = "";
        unsigned int idx1 = key % linear_size;
        while (linear_vec[idx1] != "") {
            idx1 = (idx1 + 1) % linear_size;
        }
        linear_vec[idx1] = std::to_string(key); // only put keys at the hash vec.
    }
    for (int i = 0; i < idx; ++i) {
        if (linear_vec[i] == "") continue;
        auto key = convertStringToUint(linear_vec[i]);
        linear_vec[i] = "";
        unsigned int idx1 = key % linear_size;
        while (linear_vec[idx1] != "") {
            idx1 = (idx1 + 1) % linear_size;
        }
        linear_vec[idx1] = std::to_string(key); // only put keys at the hash vec.
    }
    return ;
}

void LinearHash(const char* filename, const char* output) {
    ifstream in;
    in.open(filename, ios::in);
    string line = "";
    vector<string> res;
    while (getline(in, line)) {
        int idx = line.find_first_of(' ');
        string type = line.substr(0, idx);
        if (type == "Set") {
            string left = line.substr(idx + 1);
            idx = left.find_first_of(' ');
            string keyStr = left.substr(0, idx);
            string valStr = left.substr(idx + 1);
            valStr.pop_back();
            // cout << "val str = " << valStr << endl; 
            auto key = convertStringToUint(keyStr);
            auto val = convertStringToUint(valStr);
            // cout << "set key = " << key << ", val = " << val << endl;
            LinearInsert(key, val);
        }
        if (type == "Get") {
            string keyStr = line.substr(idx + 1);
            keyStr.pop_back();
            auto key = convertStringToUint(keyStr);
            // cout << "get key = " << key << endl;
            string got = LinearGet(key);
            res.push_back(got);
        }
        if (type == "Del") {
            string keyStr = line.substr(idx + 1);
            keyStr.pop_back();
            auto key = convertStringToUint(keyStr);
            // cout << "del key = " << key << endl;
            LinearDel(key);
        }
    }
    // then write the result into result output file
    ofstream out;
    out.open(output, ios::out | ios::trunc);
    for (int i = 0; i < res.size(); ++i) {
        stringstream sst;
        sst << res[i];
        out << sst.str() << endl;
    }
    // remeber to close opened files
    in.close();
    out.close();
}

void CuckooResize() {
    cuckoo_vec_1.clear();
    cuckoo_vec_2.clear();
    cuckoo_size *= 2;
    cuckoo_vec_1 = vector<string>(cuckoo_size, "");
    cuckoo_vec_2 = vector<string>(cuckoo_size, "");
    for (auto it = cuckoo_map.begin(); it != cuckoo_map.end(); ++it) {
        CuckooInsert(it->first, it->second);
    }
    return ;
}

void CuckooInsert(unsigned int key, unsigned int val) {
    // cout << "cuckoo insert" << endl;
    cuckoo_map[key] = val;
    if (CuckooGet(key) != "null") {
        // inserted before
        return ;
    }
    // check vec1
    auto idx1 = key % cuckoo_size;
    if (cuckoo_vec_1[idx1] == "") {
        // find a pos in vec1
        cuckoo_vec_1[idx1] = std::to_string(key);
        return ;
    }
    int times = 0;
    int turn = 0;
    unsigned int tmp = key;
    while (times != cuckoo_size * 2) {
        if (turn == 0) {
            int idx1 = tmp % cuckoo_size;
            auto got = cuckoo_vec_1[idx1];
            if (got == "") {
                cuckoo_vec_1[idx1] = std::to_string(tmp);
                break;
            } else {
                cuckoo_vec_1[idx1] = std::to_string(tmp);
                tmp = convertStringToUint(got);
                turn = 1 - turn;
            }
        } else {
            // turn == 1
            int idx1 = (tmp / cuckoo_size) % cuckoo_size;
            auto got = cuckoo_vec_2[idx1];
            if (got == "") {
                cuckoo_vec_2[idx1] = std::to_string(tmp);
                break;
            } else {
                cuckoo_vec_2[idx1] = std::to_string(tmp);
                tmp = convertStringToUint(got);
                turn = 1 - turn;
            }
        }
        times++;
    }
    if (times == cuckoo_size * 2) {
        // form a cycle, resize is required.
        CuckooResize();
    }
    return ;
}

string CuckooGet(unsigned int key) {
    // cout << "cuckoo get" << endl;
    auto idx1 = key % cuckoo_size;
    auto idx2 = (key / cuckoo_size) % cuckoo_size;
    if (cuckoo_vec_1[idx1] == "" && cuckoo_vec_2[idx2] == "") return "null";
    if (cuckoo_vec_1[idx1] != "" && cuckoo_vec_1[idx1] == std::to_string(key)) {
        return std::to_string(cuckoo_map[key]);
    }
    if (cuckoo_vec_2[idx2] != "" && cuckoo_vec_2[idx2] == std::to_string(key)) {
        return std::to_string(cuckoo_map[key]);
    }
    return "null";
}

void CuckooDel(unsigned int key) {
    // cout << "cuckoo del" << endl;
    auto got = CuckooGet(key);
    // not exist
    if (got == "null") return ;
    auto idx1 = key % cuckoo_size;
    auto idx2 = (key / cuckoo_size) % cuckoo_size;
    if (cuckoo_vec_1[idx1] != "" && cuckoo_vec_1[idx1] == std::to_string(key)) {
        cuckoo_vec_1[idx1] = "";
    }
    if (cuckoo_vec_2[idx2] != "" && cuckoo_vec_2[idx2] == std::to_string(key)) {
        cuckoo_vec_2[idx2] = "";
    }
    // then remove this kv from cuckooMap
    auto it = cuckoo_map.find(key);
    if (it == cuckoo_map.end()) return ;
    cuckoo_map.erase(it);
    return ;
}

void CuckooHash(const char* filename, const char* output) {
    ifstream in;
    in.open(filename, ios::in);
    string line = "";
    vector<string> res;
    while (getline(in, line)) {
        int idx = line.find_first_of(' ');
        string type = line.substr(0, idx);
        if (type == "Set") {
            string left = line.substr(idx + 1);
            idx = left.find_first_of(' ');
            string keyStr = left.substr(0, idx);
            string valStr = left.substr(idx + 1);
            valStr.pop_back();
            // cout << "val str = " << valStr << endl;
            auto key = convertStringToUint(keyStr);
            auto val = convertStringToUint(valStr);
            // cout << "set key = " << key << ", val = " << val << endl;
            CuckooInsert(key, val);
        }
        if (type == "Get") {
            string keyStr = line.substr(idx + 1);
            keyStr.pop_back();
            auto key = convertStringToUint(keyStr);
            // cout << "get key = " << key << endl;
            string got = CuckooGet(key);
            res.push_back(got);
        }
        if (type == "Del") {
            string keyStr = line.substr(idx + 1);
            keyStr.pop_back();
            auto key = convertStringToUint(keyStr);
            // cout << "del key = " << key << endl;
            CuckooDel(key);
        }
    }
    // then write the result into result output file
    ofstream out;
    out.open(output, ios::out | ios::trunc);
    for (int i = 0; i < res.size(); ++i) {
        // stringstream sst;
        // sst << res[i];
        // out << sst.str() << endl;
        res[i] += "\n";
        out.write(res[i].c_str(), res[i].size());
    }
    // remeber to close opened files
    in.close();
    out.close();
}

int main() {
    int type;
    cin >> type;
    string filename;
    cin >> filename;
    string output;
    cin >> output;
    struct timeval time0;
    gettimeofday(&time0, NULL);
    long start = time0.tv_sec * 1000 + time0.tv_usec / 1000;
    if (type == 0) {
        LinearHash(filename.c_str(), output.c_str());
    } else {
        CuckooHash(filename.c_str(), output.c_str());
    }
    gettimeofday(&time0, NULL);
    long end = time0.tv_sec * 1000 + time0.tv_usec / 1000;
    cout << "used time = " << (end - start) << endl;
    // srand(time(0));
    // for (int i = 0; i < 100; ++i) {
    //     unsigned int key = (unsigned int) (rand() % 10000 + 1);
    //     cout << "a rand key = " << key << endl;
    // }
    // LinearHash(filename.c_str(), output.c_str());
    // CuckooHash(filename.c_str(), output.c_str());
    return 0;
}