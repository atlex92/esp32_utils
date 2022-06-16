#include "SafeFileManipulator.hpp"
#include "md5.hpp"
#include "ThreadSafeDbg.hpp"
#include <algorithm>

static const char* const TAG {"SAFE_SAVER"};
static const char* const dataExtension {".d"};
static const char* const md5Extension {".m"};
static const char* const backupExtension {".b"};
static const char* const backupPostfix {"_b"};


SafeFileManipulator::SafeFileManipulator(const eSafeFileSaverMode mode, IFileSystemDriver* driver) 
                :   IFileManipulator(driver), 
                    _mode(mode) {
}

bool SafeFileManipulator::saveContentToFile(const string& content, const string& filename) const {

    switch (_mode) {
        case eSafeFileSaverMode::MODE_NORMAL:
            return saveInNormalMode(content, filename, false);
        
        case eSafeFileSaverMode::MODE_USE_MD5:
            return saveInMd5Mode(content, filename, false);

        case eSafeFileSaverMode::MODE_USE_MD5_AND_BACKUP:
            return saveInMd5BackupMode(content, filename, false);
    
        default:
            assert(false);
        break;
    }
    return false;
}

std::pair<bool, std::string> SafeFileManipulator::loadContentFromFile(const string& filename) const {
    switch (_mode) {
        case eSafeFileSaverMode::MODE_NORMAL:
            return loadInNormalMode(filename);
        
        case eSafeFileSaverMode::MODE_USE_MD5:
            return loadInMd5Mode(filename);

        case eSafeFileSaverMode::MODE_USE_MD5_AND_BACKUP:
            return loadInMd5BackupMode(filename);
    
        default:
            assert(false);
        break;
    }
    return std::make_pair(false, "");
}

bool SafeFileManipulator::appendContentToFile(const string& content, const string& filename) const {
    switch (_mode) {
        case eSafeFileSaverMode::MODE_NORMAL:
            return saveInNormalMode(content, filename, true);
        
        case eSafeFileSaverMode::MODE_USE_MD5:
            return saveInMd5Mode(content, filename, true);

        case eSafeFileSaverMode::MODE_USE_MD5_AND_BACKUP:
            return saveInMd5BackupMode(content, filename, true);
    
        default:
            assert(false);
        break;
    }
    return false;
}

bool SafeFileManipulator::saveInNormalMode(const string& content, const string& filename, const bool append) const {

    bool res = false;
    static const uint8_t maxSaveTriesInNormalMode = 3;
    uint8_t currentTry = 0;

    while (res != true and currentTry < maxSaveTriesInNormalMode) {
        ++currentTry;
        DBG_PRINT_TAG(TAG, "Saving in NORMAl mode, append: %s, current try: %u", append? "true":"false", currentTry);
        res = append? _driver->appendContentToFile(content, filename) : _driver->writeContentToFile(content, filename);
    }
    return res;
}

bool SafeFileManipulator::saveInMd5Mode(const string& content, const string& filename, const bool append) const {

    bool res = false;
    static const uint8_t maxSaveTriesInMd5Mode = 3;
    uint8_t currentTry = 0;
    const string mainFileName = getFileNameWithExtension(filename, eSafeSaverFileType::FILE_MAIN);
    const string mainMd5FileName = getFileNameWithExtension(filename, eSafeSaverFileType::FILE_MD5_MAIN);

    while (res != true and currentTry < maxSaveTriesInMd5Mode) {
        ++currentTry;
        // DBG_PRINT_TAG(TAG, "Saving in MD5 mode, append: %s, current try: %u", append? "true":"false", currentTry);
        const bool mainFileUpdated = append? _driver->appendContentToFile(content, mainFileName) : _driver->writeContentToFile(content, mainFileName);
        if (false == mainFileUpdated) {
            DBG_PRINT_TAG(TAG, "Failed to save mainFile");
            continue;
        }

        const string mainFileMd5 = _driver->getFileMd5(mainFileName);

        if (false == _driver->writeContentToFile(mainFileMd5, mainMd5FileName)) {
            DBG_PRINT_TAG(TAG, "failed to write MD5");
            continue;
        }

        string readMd5 = "";

        if (false == _driver->readEntireFileToString(mainMd5FileName, readMd5)) {
            DBG_PRINT_TAG(TAG, "Failed to read MD5 from .md5 file");
            continue;
        }
        const bool isMd5FileUpdated = readMd5.compare(mainFileMd5) == 0;

        if (false == isMd5FileUpdated) {
            DBG_PRINT_TAG(TAG, ".md5 is not properly updated!");
            continue;
        }
        res = true;
    }

    return res;
}

std::string SafeFileManipulator::getFileNameWithExtension(const string& filename, const eSafeSaverFileType type) const {

    switch (type) {
        case eSafeSaverFileType::FILE_MAIN:
            return string(filename).append(dataExtension);

        case eSafeSaverFileType::FILE_BACKUP:
            return string(filename).append(backupExtension);

        case eSafeSaverFileType::FILE_MD5_MAIN:
            return string(filename).append(md5Extension);

        case eSafeSaverFileType::FILE_MD5_BACKUP:
            return string(filename).append(backupPostfix).append(md5Extension);

        default:
            assert(false);
        break;
    }
    return "";
}

std::pair<bool, std::string> SafeFileManipulator::loadInNormalMode(const string& filename) const {

    if (false == doesFileExist(filename)) {
        return std::make_pair(false, "");
    }
    std::string loadedString{""};
    const bool loadResult {_driver->readEntireFileToString(filename, loadedString)};
    return std::make_pair(loadResult, loadedString);
}

std::pair<bool, std::string> SafeFileManipulator::loadInMd5Mode(const string& filename) const {
    
    const string mainFileName = getFileNameWithExtension(filename, eSafeSaverFileType::FILE_MAIN);
    const string mainMd5FileName = getFileNameWithExtension(filename, eSafeSaverFileType::FILE_MD5_MAIN);

    if (false == doesFileExist(filename)) {
        return std::make_pair(false, "");
    }

    std::string md5FileContent {""};
    if (false == _driver->readEntireFileToString(mainMd5FileName, md5FileContent)) {
        return std::make_pair(false, "");
    }

    std::string fileContent {""};
    if (false == _driver->readEntireFileToString(mainFileName, fileContent)) {
        return std::make_pair(false, "");
    }

    const string contentMd5 = getMD5String(fileContent);

    if(contentMd5.compare(md5FileContent) != 0) {
        return std::make_pair(false, "");
    }
    
    return std::make_pair(true, fileContent);
}

std::pair<bool, std::string> SafeFileManipulator::loadInMd5BackupMode(const string& filename) const {
    // TODO: Will be implemented later
    return std::make_pair(false, "");
}

bool SafeFileManipulator::saveInMd5BackupMode(const string& content, const string& filename, const bool append) const {
    // TODO: Will be implemented later
    return false;
}

uint32_t SafeFileManipulator::countFilesInDirectory(const std::string& directory) const {

    if (eSafeFileSaverMode::MODE_NORMAL == _mode) {
        return _driver->countFiles(directory);
    }

    else if (eSafeFileSaverMode::MODE_USE_MD5 == _mode) {
        return dataFilesList(directory).size();
    }

    return 0;
}

std::vector<std::string> SafeFileManipulator::dataFilesList (const std::string& directory) const {

    if (eSafeFileSaverMode::MODE_NORMAL == _mode) {
        return _driver->filesList(directory);
    }
    
    else if (eSafeFileSaverMode::MODE_USE_MD5 == _mode) {
        std::vector<std::string> allFilesList {_driver->filesList(directory)};
        std::vector<std::string> dataFilesList;

        for (auto it = allFilesList.begin(); it != allFilesList.end();) {
            const size_t dotIndex {it->find_first_of('.')};
            if (std::string::npos == dotIndex) {
                allFilesList.erase(it);
                continue;
            }
            it ++;
        }

        for (auto it = allFilesList.begin(); it != allFilesList.end();) {
            const size_t dotIndex {it->find_first_of('.')};
            const std::string extension {it->substr(dotIndex)};
            if (extension == dataExtension) {
                const std::string filename {it->substr(0, dotIndex)};
                const std::string md5FileName {filename + md5Extension};
                const auto md5File = std::find(allFilesList.begin(), allFilesList.end(), md5FileName);
                if (md5File != allFilesList.end()) {
                    dataFilesList.push_back(filename);
                }

                allFilesList.erase(it);
            continue;
            }
            it ++;
        }
        return dataFilesList;
    }
    return {};
}

bool SafeFileManipulator::doesFileExist(const string& filename) const {

    switch (_mode) {
        case eSafeFileSaverMode::MODE_NORMAL:
            return _driver->doesFileExist(filename);
        case eSafeFileSaverMode::MODE_USE_MD5: {
            const std::string dataFileName {getFileNameWithExtension(filename, eSafeSaverFileType::FILE_MAIN)};
            const std::string md5FileName {getFileNameWithExtension(filename, eSafeSaverFileType::FILE_MD5_MAIN)};
            return ( _driver->doesFileExist(dataFileName) and _driver->doesFileExist(md5FileName) );
        }
        case eSafeFileSaverMode::MODE_USE_MD5_AND_BACKUP:
            // TODO : will be implemented later
            return false;
    
        default:
            assert(false); // shouldn't be here
        break;
    }
}

bool SafeFileManipulator::deleteFile(const std::string& filename) const {

    switch (_mode) {
        case eSafeFileSaverMode::MODE_NORMAL:
            return _driver->deleteFile(filename);
        case eSafeFileSaverMode::MODE_USE_MD5: {
            const std::string dataFileName {getFileNameWithExtension(filename, eSafeSaverFileType::FILE_MAIN)};
            const std::string md5FileName {getFileNameWithExtension(filename, eSafeSaverFileType::FILE_MD5_MAIN)};
            return ( _driver->deleteFile(dataFileName) and _driver->deleteFile(md5FileName) );
        }
        case eSafeFileSaverMode::MODE_USE_MD5_AND_BACKUP:
            // TODO : will be implemented later
            return true;
    
        default:
            assert(false); // shouldn't be here
        break;
    }
}

bool SafeFileManipulator::deleteAllFiles(const std::string& directory) const {

    bool ret {true};
    std::vector<std::string> filesInDirectory {dataFilesList(directory)};

    for (auto &file : filesInDirectory) {
        if(false == deleteFile(file)) {
            ret = false;
            break;
        }
    }
    return ret;
}