#pragma once

#include "AbstractFileSystemDriver.hpp"
#include "esp_spiffs.h"

#define SPIFFS_IDF_DEFAULT_PATH             "/spiffs"
#define SPIFFS_IDF_DEFAULT_MAX_FILES        1024

struct SPIFFS_IDFDriver : AbstractFileSystemDriver {
    explicit SPIFFS_IDFDriver(const char* const path = SPIFFS_IDF_DEFAULT_PATH,
        const size_t maxFiles = SPIFFS_IDF_DEFAULT_MAX_FILES);
    ~SPIFFS_IDFDriver();
    std::vector<std::string> filesList(const std::string& path)const override;
    bool deleteFile(const std::string& fullFileName) const override;
    uint32_t countFiles (const std::string& path)const override;
    bool writeContentToFile (const std::string& content, const std::string& filename) const override;
    bool appendContentToFile (const std::string& content, const std::string& filename) const override;
    bool readEntireFileToString (const std::string& filename, std::string& output) const override;
    bool doesFileExist(const std::string& filename) const override;
    float usagePercent() const override;
    bool format() const override;
    void initialize() override;
private:
    bool doWriteContentToFile(const std::string& content, FILE* file) const;
    esp_vfs_spiffs_conf_t _conf{};
};