#include "AbstractFileSystemDriver.hpp"
#include "md5.hpp"

std::string AbstractFileSystemDriver::getFileMd5(const string& filename) const {
    
    std::string ret{};
    
    if(false == doesFileExist(filename)) {
        return ret;
    }

    std::string fileContent {};

    if (false == readEntireFileToString(filename, fileContent)) {
        return ret;
    }

    ret = getMD5String(fileContent);

    return ret;
}