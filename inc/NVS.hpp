#pragma once

#include <string>
#include <stdint.h>

#include <nvs.h>
#include <esp_system.h>
#include <nvs_flash.h>

using namespace std;

class NVS{
    public:
        bool read(const char* const storage, const char* const tag, string& res);
        bool read(const char* const storage, const char* const tag, int64_t& res);
        
        bool write(const char* const storage, const char* const tag, const int64_t val);
        bool write(const char* const storage, const char* const tag, const char* val);

        bool erase(const char* const storage, const char* const tag);
    private:
        bool openToRead(const char* const storage);
        bool openToWrite(const char* const storage);
    private:
        nvs_handle _handle;
};