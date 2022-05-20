#pragma once

#include <string>
using namespace std; 

namespace HttpUtils{
    bool getHostFromUrl(const string& url, string& out);
    bool trimHostFromUrl(const string& url, string& out);
    bool isUrlCorrect(const string& url);
    bool getProtocolFromUrl(const string& url, string& out);
};