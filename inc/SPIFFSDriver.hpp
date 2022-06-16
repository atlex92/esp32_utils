#pragma once
#if CONFIG_ENABLE_ARDUINO_SPIFFS_DRIVER
#include "Arduino.h"
#include "IFileSystemDriver.hpp"
#include "AbstractFileSystemDriver.hpp"

class SPIFFSDriver : public AbstractFileSystemDriver {

    public:
        SPIFFSDriver();
        ~SPIFFSDriver();
        std::vector<std::string> filesList(const std::string& path)const override;
        bool deleteFile(const std::string& fullFileName) const override;
        uint32_t countFiles (const std::string& path) const override;
        bool writeContentToFile (const std::string& content, const std::string& filename) const override;
        bool appendContentToFile (const std::string& content, const std::string& filename) const override;
        bool readEntireFileToString (const std::string& filename, std::string& output) const override;
        bool doesFileExist(const std::string& filename) const override;
        float usagePercent() const override;
        void initialize() override;
        bool format() const override;
    private:
        bool printContentToStream(const std::string& content, Stream& stream) const;
        bool doWriteContentToFile(const std::string& content, const std::string& filename, const bool append = false) const;
};
#endif