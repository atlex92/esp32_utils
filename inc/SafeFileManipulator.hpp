#pragma once

#include "IFileManipulator.hpp"

enum class eSafeFileSaverMode{
    MODE_NORMAL,
    MODE_USE_MD5,
    MODE_USE_MD5_AND_BACKUP,
    MODE_INVALID,
};

enum class eSafeSaverFileType{
    FILE_MAIN,
    FILE_BACKUP,
    FILE_MD5_MAIN,
    FILE_MD5_BACKUP,
};

class SafeFileManipulator : public IFileManipulator {

    public:
        explicit SafeFileManipulator(const eSafeFileSaverMode mode, IFileSystemDriver* driver);

        bool saveContentToFile(const string& content, const string& filename) override;
        bool appendContentToFile(const string& content, const string& filename) override;
        std::pair<bool, std::string> loadContentFromFile(const string& filename) override;
        uint32_t countFilesInDirectory(const std::string& directory) override;
        std::vector<std::string> dataFilesList (const std::string& directory) const override;
        bool doesFileExist(const string& filename) override;
        bool deleteFile(const std::string& filename) override;
        bool deleteAllFiles(const std::string& directory);
    private:
        eSafeFileSaverMode _mode;
        bool saveInNormalMode(const string& content, const string& filename, const bool append);
        bool saveInMd5Mode(const string& content, const string& filename, const bool append);
        bool saveInMd5BackupMode(const string& content, const string& filename, const bool append);
        std::pair<bool, std::string> loadInNormalMode(const string& filename);
        std::pair<bool, std::string> loadInMd5Mode(const string& filename);
        std::pair<bool, std::string> loadInMd5BackupMode(const string& filename);
        string getFileNameWithExtension(const string& filename, const eSafeSaverFileType type)const;
};