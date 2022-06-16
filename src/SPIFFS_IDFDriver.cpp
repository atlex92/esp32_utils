#include "sdkconfig.h"
#include "SPIFFS_IDFDriver.hpp"
#include "md5.hpp"
#include "FilePathUtils.hpp"
#include "esp_log.h"
#include "unistd.h"
#include <memory>
#include <dirent.h>
#include <cstring>
#include <string.h>
#include <sys/stat.h>

static const char* const TAG {"SPIFFS_IDFDriver"};

SPIFFS_IDFDriver::SPIFFS_IDFDriver(const char* const path, const size_t maxFiles)
    :   _conf{path, NULL, maxFiles, true} {
    initialize();
}

SPIFFS_IDFDriver::~SPIFFS_IDFDriver() {
    esp_vfs_spiffs_unregister(_conf.partition_label);
}


void SPIFFS_IDFDriver::initialize() {
    _isReady = false;
    esp_err_t err = esp_vfs_spiffs_register(&_conf);

    if (err != ESP_OK) {
        if (err == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (err == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(err));
        }
        return;
    }
    err = esp_spiffs_check(_conf.partition_label);

    if(ESP_OK != err) {
        ESP_LOGE(TAG, "spiffs check procedure failed!");
        return;
    }
    _isReady = true;
}

bool SPIFFS_IDFDriver::format() const {
    return ESP_OK == esp_spiffs_format(_conf.partition_label);
}

bool SPIFFS_IDFDriver::writeContentToFile (const std::string& content, const std::string& filename) const {

    FILE* file = fopen(filename.c_str(), "w");
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return false;
    }

    return doWriteContentToFile(content, file);
}

bool SPIFFS_IDFDriver::appendContentToFile (const std::string& content, const std::string& filename) const {

    FILE* file = fopen(filename.c_str(), "a");
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return false;
    }

    return doWriteContentToFile(content, file);
}

bool SPIFFS_IDFDriver::doWriteContentToFile(const std::string& content, FILE* file) const {
    const auto blocksWritten = fprintf(file, content.c_str());
    if (!blocksWritten) {
        fclose(file);
        ESP_LOGE(TAG, "fwrite operation failed");
        return false;
    }

    fclose(file);

    return true;
}


uint32_t SPIFFS_IDFDriver::countFiles (const std::string& path) const {

    DIR *dir_ptr = NULL;
    struct dirent *direntp;
    char *npath;
    if (!path.c_str()) return 0;
    if( (dir_ptr = opendir(path.c_str())) == NULL ) return 0;

    int count=0;
    while( (direntp = readdir(dir_ptr)))
    {
        if (strcmp(direntp->d_name,".")==0 ||
            strcmp(direntp->d_name,"..")==0) continue;
        switch (direntp->d_type) {
            case DT_REG:
                ++count;
                break;
        }
    }
    closedir(dir_ptr);
    return count;

    // uint32_t ret {0};

    // struct dirent* entry;

    // DIR* dirp = opendir(path.c_str());

    // if(!dirp) {
    //     return 0;
    // }

    // while ((entry = readdir(dirp)) != NULL) {
    //     if (entry->d_type == DT_REG) { /* If the entry is a regular file */
    //         ret++;
    //     }
    // }
    // closedir(dirp);

    // return ret;
    // return n;
}

std::vector<std::string> SPIFFS_IDFDriver::filesList(const std::string& path) const {
    
    std::vector<std::string> ret;

    DIR * dirp;
    struct dirent* entry;

    dirp = opendir(path.c_str());

    if(!dirp) {
        return ret;
    }

    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) {
            ret.emplace_back(entry->d_name);
        }
    }
    closedir(dirp);

    return ret;
}

bool SPIFFS_IDFDriver::readEntireFileToString (const std::string& filename, std::string& output) const {
    
    if (false == doesFileExist(filename)) {
        return false;
    }

    FILE* file = fopen(filename.c_str(), "r");
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return false;
    }

    fseek(file, 0, SEEK_END);
    const auto fileSize = ftell(file);
    rewind(file);

    auto pBuffer {std::make_shared<char[]>(fileSize + 1)};
    const auto blocksRead {fread(pBuffer.get(), fileSize, 1, file)};
    if (!blocksRead) {
        fclose(file);
        ESP_LOGE(TAG, "fread operation failed");
        return false;
    }

    pBuffer.get()[fileSize] = '\0';

    fclose(file);

    std::string{pBuffer.get()}.swap(output);

    return true;
}

bool SPIFFS_IDFDriver::deleteFile(const std::string& filename) const {
    if (false == doesFileExist(filename)) {
        return true;
    }
    return 0 == unlink(filename.c_str());
}

bool SPIFFS_IDFDriver::doesFileExist(const std::string& filename) const {
    struct stat st;
    if(0 == stat(filename.c_str(), &st)) {
        if (S_ISREG(st.st_mode)) {
            return true;
        }
    }
    return false;
}

float SPIFFS_IDFDriver::usagePercent() const  {

    size_t totalBytes {};
    size_t usedBytes {};

    if (ESP_OK != esp_spiffs_info(_conf.partition_label, &totalBytes, &usedBytes))
        return 0.0;

    const float usagePercent {100.0F * static_cast<float>(usedBytes) / totalBytes};

    return usagePercent;
}