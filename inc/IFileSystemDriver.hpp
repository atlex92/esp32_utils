#pragma once

#include <string>
#include <vector>

using namespace std;

class IFileSystemDriver {

    public:
        virtual ~IFileSystemDriver() = default;
        virtual bool writeContentToFile (const string& content, const string& filename) = 0;
        virtual float usagePercent() const = 0;

        virtual bool deleteFile(const std::string& fullFileName) = 0;
        virtual vector<std::string> filesList(const std::string& path)const = 0;
        virtual uint32_t countFiles (const string& path)const = 0;
        virtual bool readEntireFileToString (const string& filename, string& output) = 0;
        virtual bool appendContentToFile (const string& content, const string& filename) = 0;
        virtual bool doesFileExist(const string& filename)const = 0;
        virtual string getFileMd5(const string& filename) = 0;
};