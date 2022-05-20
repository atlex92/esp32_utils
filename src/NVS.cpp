#include "NVS.hpp"

#include "ThreadSafeDbg.hpp"

const char* const TAG = "NVS";

bool NVS::read(const char* const storage, const char* const tag, int64_t& res) {

    bool ret = false;
    int64_t readRes = 0;
    if (true == openToRead(storage)) {
        esp_err_t err = nvs_get_i64(_handle, tag, &readRes);
        switch (err) {
            case ESP_OK:
                ret = true;
            break;
            case ESP_ERR_NVS_NOT_FOUND:
                DBG_PRINT_TAG(TAG, "The value %s is not initialized yet!", tag);
            break;

            default :
                DBG_PRINT_TAG(TAG,"Error (%s) reading!\n", esp_err_to_name(err));
        }
        nvs_close(_handle);
    }

    if (true == ret) {
        res = readRes;
    }
    return ret;
}

bool NVS::read(const char* const storage, const char* const tag, string& res) {

    bool ret = false;
    string readRes = "";
    size_t readLength = 0;
    
    if (true == openToRead(storage)) {
        esp_err_t err = nvs_get_str(_handle, tag, NULL, &readLength);
        char* charArr = new char[readLength+1];
        err = nvs_get_str(_handle, tag, charArr, &readLength);
        switch (err) {
            case ESP_OK:
                ret = true;
                charArr[readLength] = '\0';
                readRes = string(charArr);
            break;
            case ESP_ERR_NVS_NOT_FOUND:
                DBG_PRINT_TAG(TAG, "The value %s is not initialized yet!", tag);
            break;

            default :
                DBG_PRINT_TAG(TAG,"Error (%s) reading!\n", esp_err_to_name(err));
        }
        delete[] charArr;
        nvs_close(_handle);
    }

    if (true == ret) {
        res = readRes;
    }
    return ret;
}

bool NVS::write(const char* const storage, const char* const tag, const int64_t val) {
    bool ret = false;

    if (true == openToWrite(storage)) {

        esp_err_t err = nvs_set_i64(_handle, tag, val);
        if (err != ESP_OK){
            DBG_PRINT_TAG(TAG, "failed to set_u32");
        }

        err = nvs_commit(_handle);

        if (err == ESP_OK) {            
            ret = true;
        }

        else {
            DBG_PRINT_TAG(TAG, "failed to commit");
        }
        
        nvs_close(_handle);
    }

    return ret;
}

bool NVS::write(const char* const storage, const char* const tag, const char* val) {
    bool ret = false;

    if (true == openToWrite(storage)) {

        esp_err_t err = nvs_set_str(_handle, tag, val);
        if (err != ESP_OK){
            DBG_PRINT_TAG(TAG, "failed to set_char");
        }

        err = nvs_commit(_handle);

        if (err == ESP_OK) {
            ret = true;
        }
        else {
            DBG_PRINT_TAG(TAG, "failed to commit");
        }
        nvs_close(_handle);
    }
    return ret;
}

bool NVS::openToRead(const char* const storage) {

    bool ret = false;

    esp_err_t err = nvs_open(storage, NVS_READONLY, &_handle);

    if (err != ESP_OK) {
        DBG_PRINT_TAG(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else {
        ret = true;
    }
    return ret;
}

bool NVS::openToWrite(const char* const storage) {

    bool ret = false;

    esp_err_t err = nvs_open(storage, NVS_READWRITE, &_handle);

    if (err != ESP_OK) {
        DBG_PRINT_TAG(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else {
        ret = true;
    }
    return ret;
}

bool NVS::erase(const char* const storage, const char* const tag) {

    bool ret = false;

    if (true == openToWrite(storage)) {

        esp_err_t err = nvs_erase_key(_handle, tag);
        if (err != ESP_OK){
            DBG_PRINT_TAG(TAG, "failed to erase");
        }

        err = nvs_commit(_handle);
        if (err == ESP_OK) {
            ret = true;
        }
        else {
            DBG_PRINT_TAG(TAG, "failed to commit");
        }
        nvs_close(_handle);
    }
    return ret;
}