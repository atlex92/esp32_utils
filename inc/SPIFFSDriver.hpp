#pragma once
#if CONFIG_ENABLE_ARDUINO_SPIFFS_DRIVER
#include "Arduino.h"
#include "IFileSystemDriver.hpp"

class SPIFFSDriver : public IFileSystemDriver {

    public:
        SPIFFSDriver();
        ~SPIFFSDriver();
        vector<std::string> filesList(const std::string& path)const override;
        bool deleteFile(const std::string& fullFileName) override;
        uint32_t countFiles (const string& path)const override;
        bool writeContentToFile (const string& content, const string& filename) override;
        bool appendContentToFile (const string& content, const string& filename) override;
        bool readEntireFileToString (const string& filename, string& output) override;
        bool doesFileExist(const string& filename) const override;
        string getFileMd5(const string& filename) override;
        float usagePercent() const override;
    private:
        void initialize();
        bool printContentToStream(const string& content, Stream& stream);
        bool doWriteContentToFile(const string& content, const string& filename, const bool append = false);
};
#endif