#include "ThreadSafeDbg.hpp"
#include "StringUtils.hpp"
#include <cstdarg>

static const char* const TAG = "DBG";
#define MAX_QUEUE_LENGTH 100
#define MAX_MSG_LENGTH  3000UL

ThreadSafeDbg::ThreadSafeDbg()
    :   Task{"ThreadSafeDbg", configMINIMAL_STACK_SIZE*3, 3},
        allowPrint(true) {
    init();
}

void ThreadSafeDbg::init() {
    _dbgQueue = xQueueCreate(MAX_QUEUE_LENGTH, sizeof(debugMessage_t));
}

void ThreadSafeDbg::run(void* data) {

    DBG_PRINT_TAG(TAG, "started!");
    while(1) {

        debugMessage_t message;
        xQueueReceive(_dbgQueue, &message, portMAX_DELAY);
        printf(message.pointer);
        delete[] message.pointer; 
    }   
}

void ThreadSafeDbg::printHex(const uint8_t byte) {
    if (false == allowPrint) {
        return;
    }
    printX("%02X", byte);
}

void ThreadSafeDbg::printLine() {
    if (false == allowPrint) {
        return;
    }
    const char* const newLn = "\r\n";
    doPrint(newLn);
}

void ThreadSafeDbg::printLine(const string& message) {
    if (false == allowPrint) {
        return;
    }
    printX("%s\r\n", message.c_str());
}

void ThreadSafeDbg::printLine(const char * const message) {
    if (false == allowPrint) {
        return;
    }
    printX("%s\r\n", message);
}

void ThreadSafeDbg::doPrint(const char * const message) {
    
        if (false == allowPrint) {
            return;
        }
        size_t length {strlen(message)};
        size_t offset {0};

        while (length > MAX_MSG_LENGTH) {
            char* buff = new char[MAX_MSG_LENGTH+1];
            memcpy(buff, message + offset, MAX_MSG_LENGTH);
            buff[MAX_MSG_LENGTH] = '\0';
            debugMessage_t messageRx = {.pointer = buff};
            xQueueSendToBack(_dbgQueue, &messageRx, portMAX_DELAY);

            offset += MAX_MSG_LENGTH;
            length -= MAX_MSG_LENGTH;
        }

        char* buff = new char[length+1];
        memcpy(buff, message + offset, length);
        buff[length] = '\0';
        debugMessage_t messageRx = {.pointer = buff};
        xQueueSendToBack(_dbgQueue, &messageRx, portMAX_DELAY);
}

void ThreadSafeDbg::setAllowPrint(const bool val) {

    allowPrint = val;
}

void ThreadSafeDbg::print(const char* const message) {
    doPrint(message);
}