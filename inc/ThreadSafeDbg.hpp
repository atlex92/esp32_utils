#pragma once

#include "StringUtils.hpp"

#ifndef USE_DBG
    #define DBG_PRINTF(format, ...) 
    #define DBG_PRINTLN(text)
    #define DBG_PRINT_TAG(TAG, format, ...)
#else
    #define DBG_PRINTF(format, ...) (DBG.printX(format, ##__VA_ARGS__))
    #define DBG_PRINTLN(text) DBG.printLine(text)
    #define DBG_PRINT_TAG(TAG, format, ...) (DBG.printWithTag(TAG, format, ##__VA_ARGS__))
#endif

#ifndef NATIVE_BUILD
    #include "freertos/FreeRTOS.h"
    #include "freertos/queue.h"
    #include "string.h"
    #include "ITask.hpp"
    #include "Singleton.hpp"
    #include "inttypes.h"

    typedef struct {
    const char *pointer;
}debugMessage_t;

using namespace std;
class ThreadSafeDbg : public ITask, public Singleton<ThreadSafeDbg>{
    friend class Singleton;
    public:
        void setAllowPrint(const bool val);

        void init();
        virtual void run(void* data) override;

        void printLine();
        void printLine(const string& message);
        void printLine(const char * const message);
        template<typename ... Args>
        void printX(const char * const frmt, Args ... args) {
            printf(frmt, args ...);
        }
        void print(const char* const message);

        void printHex(const uint8_t byte);
        template<typename ... Args>
        inline void printWithTag(const char* const TAG, const char * const frmt, Args ... args) {
            if (false == allowPrint) {
                return;
            }
            std::string appendedFormatString {StringUtils::format("%s: ", TAG).append(frmt).append("\r\n")};
            printX(appendedFormatString.c_str(), args ... );
        }
    private:
        void doPrint(const char * const message);
    private:
        bool allowPrint;
        ThreadSafeDbg();
        xQueueHandle _dbgQueue;
};
#define DBG ThreadSafeDbg::instance()
#endif





