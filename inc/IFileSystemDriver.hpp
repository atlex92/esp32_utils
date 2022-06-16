#pragma once

#include <string>
#include <vector>
class IFileSystemDriver {

    public:
        virtual ~IFileSystemDriver() = default;
        virtual bool writeContentToFile (const std::string& content, const std::string& filename) const = 0;
        virtual float usagePercent() const = 0;
        virtual bool isReady() const = 0;
        virtual bool format() const = 0;
        virtual void initialize() = 0;

        virtual bool deleteFile(const std::string& fullFileName) const = 0;
        virtual std::vector<std::string> filesList(const std::string& path) const = 0;
        virtual uint32_t countFiles (const std::string& path) const = 0;
        virtual bool readEntireFileToString (const std::string& filename, std::string& output) const = 0;
        virtual bool appendContentToFile (const std::string& content, const std::string& filename) const = 0;
        virtual bool doesFileExist(const std::string& filename) const = 0;
        virtual std::string getFileMd5(const std::string& filename) const = 0;
};