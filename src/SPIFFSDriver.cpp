#include "sdkconfig.h"
#if CONFIG_ENABLE_ARDUINO_SPIFFS_DRIVER
#include "SPIFFSDriver.hpp"
#include <SPIFFS.h>
#include <Stream.h>
#include "md5.hpp"
#include "FilePathUtils.hpp"
#include "ThreadSafeDbg.hpp"

static const char* const TAG {"SPIFFS_DRIVER"};

static const uint32_t maxFileNameLength {31};

SPIFFSDriver::SPIFFSDriver() {
    initialize();
}

SPIFFSDriver::~SPIFFSDriver() {

}

void SPIFFSDriver::initialize() {
    if (false == SPIFFS.begin()) {
        ESP_LOGE(TAG, "Failed to begin SPIFFS, formatting now...");
        SPIFFS.format();
        _isReady = SPIFFS.begin();
    }
    else {
        _isReady = true;
    }
}

bool SPIFFSDriver::writeContentToFile (const std::string& content, const std::string& filename) const {
    if (filename.length() > maxFileNameLength) {
        ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
        return false;
    }
    return doWriteContentToFile(content, filename, false);
}

bool SPIFFSDriver::appendContentToFile (const string& content, const string& filename) const {
    if (filename.length() > maxFileNameLength) {
        ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
        return false;
    }
    return doWriteContentToFile(content, filename, true);
}

uint32_t SPIFFSDriver::countFiles (const string& path) const {
    uint32_t ret {0};

    File root = SPIFFS.open(path.c_str());

    if (false == root) {
        ESP_LOGE(TAG, "Failed to open path");
        root.close();
        return 0;
    }

    File file = root.openNextFile();

    while (true == file) {
        ++ret;
        file.close();
        file = root.openNextFile();
    }

    root.close();
    file.close();
    return ret;
}

vector<std::string> SPIFFSDriver::filesList(const std::string& path) const {
    
    std::vector<std::string> ret;

    File root = SPIFFS.open(path.c_str());
    if (false == root) {
        ESP_LOGE(TAG, "Failed to open path: %s", path.c_str());
        root.close();
        return ret;
    }

    File file = root.openNextFile();

    while (true == file) {
        ret.push_back(file.name());
        file.close();
        file = root.openNextFile();
    }

    root.close();
    file.close();

    return ret;
}

bool SPIFFSDriver::readEntireFileToString (const std::string& filename, std::string& output) const {
    
    if (filename.length() > maxFileNameLength) {
        ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
        return false;
    }

    if (false == doesFileExist(filename)) {
        return false;
    }
    
    File file = SPIFFS.open(filename.c_str(), FILE_READ);

    if (false == file) {
        return false;
    }

    const size_t fileSize = file.size();
    
    vector<char> tempBuffer;
    tempBuffer.resize(fileSize, '\0');

    const size_t bytesRead = file.readBytes((char*)tempBuffer.data(), fileSize);

    file.close();

    if (fileSize == bytesRead) {
        output = string(tempBuffer.begin(), tempBuffer.end());
        return true;
    }
    else {
        return false;
    }
}

bool SPIFFSDriver::deleteFile(const std::string& filename) const {

    if (filename.length() > maxFileNameLength) {
        ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
        return false;
    }

    if (false == doesFileExist(filename)) {
        return true;
    }
    if (true == SPIFFS.remove(filename.c_str())) {
        return true;
    }
    else {
        return false;
    }
    return false;
}

bool SPIFFSDriver::doesFileExist(const std::string& filename) const {

    if (filename.length() > maxFileNameLength) {
        ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
        return false;
    }
    return SPIFFS.exists(filename.c_str());
}

bool SPIFFSDriver::doWriteContentToFile(const std::string& content, const std::string& filename, const bool append) const {

    bool result = false;
    const char* modeKey = append? FILE_APPEND : FILE_WRITE;
    File file = SPIFFS.open(filename.c_str(), modeKey);

    if (true == file){
        result = printContentToStream(content, file);
    }
    else {
        ESP_LOGE(TAG, "failed to open file: %s", filename.c_str());
    }

    file.close();
    return result;
}


bool SPIFFSDriver::printContentToStream(const std::string& content, Stream& stream) const {
    const size_t printedBytes {stream.write(content.data(), content.length())};
    if(printedBytes != content.length()) {
        ESP_LOGE(TAG, "error while printing content to Stream!");
        return false;
    }
    return true; 
}

bool SPIFFSDriver::format() const {
    return SPIFFS.format();
}

float SPIFFSDriver::usagePercent() const {

    const size_t totalBytes {SPIFFS.totalBytes()};
    const size_t usedBytes {SPIFFS.usedBytes()};

    const float usagePercent {100.0F * static_cast<float>(usedBytes) / totalBytes};

    return usagePercent;
}
#endif