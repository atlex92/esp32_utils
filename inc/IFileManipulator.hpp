#pragma once

#include "IFileSystemDriver.hpp"
#include <assert.h>
#include <utility>

class IFileManipulator {
    protected:
        IFileSystemDriver* _driver;
    public:
        explicit IFileManipulator(IFileSystemDriver* driver)
                        : _driver(driver) {
            assert(NULL != driver);
        }

        virtual ~IFileManipulator() {
            delete _driver;
        }

        virtual bool saveContentToFile(const string& content, const string& filename) = 0;
        virtual std::pair<bool, std::string> loadContentFromFile(const string& filename) = 0;
        virtual bool appendContentToFile(const string& content, const string& filename) = 0; 
        virtual uint32_t countFilesInDirectory(const std::string& directory) = 0;
        virtual std::vector<std::string> dataFilesList (const std::string& directory) const = 0;        
        virtual bool doesFileExist(const string& filename) = 0;
        virtual bool deleteFile(const std::string& filename) = 0;
};