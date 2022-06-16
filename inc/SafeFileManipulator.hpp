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

        bool saveContentToFile(const std::string& content, const std::string& filename) const override;
        bool appendContentToFile(const std::string& content, const std::string& filename) const override;
        std::pair<bool, std::string> loadContentFromFile(const std::string& filename) const override;
        uint32_t countFilesInDirectory(const std::string& directory) const override;
        std::vector<std::string> dataFilesList (const std::string& directory) const override;
        bool doesFileExist(const std::string& filename) const override;
        bool deleteFile(const std::string& filename) const override;
        bool deleteAllFiles(const std::string& directory) const;
    private:
        eSafeFileSaverMode _mode;
        bool saveInNormalMode(const std::string& content, const std::string& filename, const bool append) const;
        bool saveInMd5Mode(const std::string& content, const std::string& filename, const bool append) const;
        bool saveInMd5BackupMode(const std::string& content, const std::string& filename, const bool append) const;
        std::pair<bool, std::string> loadInNormalMode(const std::string& filename) const;
        std::pair<bool, std::string> loadInMd5Mode(const std::string& filename) const;
        std::pair<bool, std::string> loadInMd5BackupMode(const std::string& filename) const;
        std::string getFileNameWithExtension(const std::string& filename, const eSafeSaverFileType type) const;
};