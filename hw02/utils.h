//
// Created by thong on 5/11/20.
//

#include <string>
#include <utility>
#include <vector>

using namespace std;

bool allow_pathname(const char *pathname) {
    return true;
}

string get_full_path(const string& path) {
    char full_path[1024];
    realpath(path.c_str(), full_path);
    return string(full_path);
}

vector<string> split_path(string path) {
    if (path.back() != '/') path += '/';
    auto fpos = path.find('/', 0);

    vector<string> res;
    while (true) {
        auto spos = path.find('/', fpos + 1);
        if (spos == string::npos) break;
        res.push_back(path.substr(fpos, spos - fpos + 1));
        fpos = spos;
    }
    return res;
}

bool check_valid(char* input, string gt) {

    auto check_string = get_full_path(string(input));
    vector<string> split1, split2;
    auto sp1 = split_path(std::move(gt));
    auto sp2 = split_path(check_string);

    int i = 0, j = 0;
    while (true) {
        if (i >= sp1.size()) return true;
        if (j >= sp2.size()) return false;
        if (sp1[i] != sp2[j]) return false;
        i++;  j++;
    }
}