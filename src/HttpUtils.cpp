#include "HttpUtils.hpp"
#include "StringUtils.hpp"
#include <stdio.h>
#include <string.h>

#define PROTOCOL_MARKER "://"
#define PROTOCOL_MARKER_LENGTH  3

static size_t getProtocolIndex (const string& str) {
    return str.find(PROTOCOL_MARKER);
}

bool HttpUtils::trimHostFromUrl(const string& url, string& out){

    if (false == isUrlCorrect(url)) {
        return false;
    }

    int protocolIndex = getProtocolIndex(url);
    int protocolSubStrLength = strlen(PROTOCOL_MARKER);
    int slashIndex = url.find('/',protocolIndex + protocolSubStrLength);

    if (-1 == slashIndex) {
        out = "/";
    }
    else {
        out = url.substr(slashIndex);
    }

    return true;
}

bool HttpUtils::isUrlCorrect(const string& url) {

    size_t length = url.length();
    if (length == 0) {
        return false;
    }
    if (true == StringUtils::containsSpace(url)) {
        return false;
    }
    int protocolIndex = getProtocolIndex(url);
    int protocolSubStrLength = strlen(PROTOCOL_MARKER);
    if (protocolIndex < 0){
        return false;
    }
    string protocol = url.substr(0, protocolIndex);
    if (false == StringUtils::caseInsEquals(protocol,"http") and false == StringUtils::caseInsEquals(protocol,"https")){
        return  false;
    }
    int slashIndex = url.find("/", protocolIndex + protocolSubStrLength);
    int hostLength = 0;
    if (slashIndex > 0 ) {
        hostLength = slashIndex - (protocolIndex + protocolSubStrLength);
    }
    else {
        hostLength = length - (protocolIndex + protocolSubStrLength);
    }
    if (hostLength <= 0){
        return false;
    }
    return true;
}

bool HttpUtils::getHostFromUrl(const string& url, string& out){

    if (false == isUrlCorrect(url)) {
        return false;
    }
    int protocolIndex = getProtocolIndex(url);
    int protocolSubStrLength = strlen(PROTOCOL_MARKER);
    int slashIndex = url.find("/", protocolIndex + protocolSubStrLength);

    if(slashIndex >= 0){
        int hostLength = slashIndex-(protocolIndex + protocolSubStrLength);
        out = url.substr(protocolIndex + protocolSubStrLength, hostLength);
    }
    else{
        out = url.substr(protocolIndex + protocolSubStrLength);
    }
    return true;
}

bool HttpUtils::getProtocolFromUrl(const string& url, string& out) {
    const auto protocolIndex{getProtocolIndex(url)};
    if(std::string::npos == protocolIndex) {
        return false;
    }
    out = url.substr(0, protocolIndex);
    return true;
}