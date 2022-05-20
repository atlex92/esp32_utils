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
        // addError(eErrors::SPIFFS_ERR_MOUNT_FAILED, __LINE__);
        assert (true == SPIFFS.format());
        assert (true == SPIFFS.begin());
    }
}

bool SPIFFSDriver::writeContentToFile (const string& content, const string& filename) {
    if (filename.length() > maxFileNameLength) {
        ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
        return false;
    }
    return doWriteContentToFile(content, filename, false);
}

bool SPIFFSDriver::appendContentToFile (const string& content, const string& filename) {
    if (filename.length() > maxFileNameLength) {
        ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
        return false;
    }
    return doWriteContentToFile(content, filename, true);
}

uint32_t SPIFFSDriver::countFiles (const string& path)const {
    uint32_t ret {0};

    ESP_LOGE(TAG, "searching files in directory: %s", path.c_str());
    
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

vector<std::string> SPIFFSDriver::filesList(const std::string& path)const {
    
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

bool SPIFFSDriver::readEntireFileToString (const string& filename, string& output) {
    
    if (filename.length() > maxFileNameLength) {
        ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
        return false;
    }

    if (false == doesFileExist(filename)) {
        return false;
    }
    
    File file = SPIFFS.open(filename.c_str(), FILE_READ);

    if (false == file) {
        // addError(eErrors::SPIFFS_ERR_OPEN_FILE_FAILED, __LINE__);
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
        // addError(eErrors::SPIFFS_ERR_READ_FAILED, __LINE__);
        return false;
    }
}

bool SPIFFSDriver::deleteFile(const std::string& filename) {

    if (filename.length() > maxFileNameLength) {
        ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
        // addError(eErrors::SPIFFS_ERR_MAX_FILENAME_EXCEED, __LINE__);
        return false;
    }

    if (false == doesFileExist(filename)) {
        return true;
    }
    if (true == SPIFFS.remove(filename.c_str())) {
        return true;
    }
    else {
        // addError(eErrors::SPIFFS_ERR_FILE_DELETE_FAILED, __LINE__);
        return false;
    }
    return false;
}

bool SPIFFSDriver::doesFileExist(const string& filename) const{

    if (filename.length() > maxFileNameLength) {
        ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
        return false;
    }
    return SPIFFS.exists(filename.c_str());
}

bool SPIFFSDriver::doWriteContentToFile(const string& content, const string& filename, const bool append) {

    bool result = false;
    const char* modeKey = append? FILE_APPEND : FILE_WRITE;
    File file = SPIFFS.open(filename.c_str(), modeKey);

    if (true == file){
        result = printContentToStream(content, file);
    }
    else {
        ESP_LOGE(TAG, "failed to open file: %s", filename.c_str());
        // addError(eErrors::SPIFFS_ERR_OPEN_FILE_FAILED, __LINE__);
    }

    file.close();
    return result;
}


bool SPIFFSDriver::printContentToStream(const string& content, Stream& stream) {
    const size_t printedBytes {stream.write(content.data(), content.length())};
    if(printedBytes != content.length()) {
        ESP_LOGE(TAG, "error while printing content to Stream!");
        // addError(eErrors::SPIFFS_ERR_WRITE_FAILED, __LINE__);
        return false;
    }
    return true; 
}

string SPIFFSDriver::getFileMd5(const string& filename) {
    
    string ret = "";
    
    if (filename.length() > maxFileNameLength) {
        ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
        // addError(eErrors::SPIFFS_ERR_MAX_FILENAME_EXCEED, __LINE__);        
        return ret;
    }

    if(false == doesFileExist(filename)) {
        return ret;
    }

    string fileContent = "";

    if (false == readEntireFileToString(filename, fileContent)) {
        return ret;
    }

    ret = getMD5String(fileContent);

    return ret;
}
float SPIFFSDriver::usagePercent() const  {

    const size_t totalBytes {SPIFFS.totalBytes()};
    const size_t usedBytes {SPIFFS.usedBytes()};

    const float usagePercent {100.0F * static_cast<float>(usedBytes) / totalBytes};

    return usagePercent;
}
#endif