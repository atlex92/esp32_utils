#pragma once
#include "IFileSystemDriver.hpp"

struct AbstractFileSystemDriver : IFileSystemDriver {
    std::string getFileMd5(const std::string& filename) const override;
    bool isReady() const { return _isReady; }
protected:
    bool _isReady{false};
};