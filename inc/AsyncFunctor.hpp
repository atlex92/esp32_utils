#pragma once
#include <functional>
#include "ITask.hpp"
#include "freertos/task.h"

struct AsyncFunctor : public ITask{
    using function_t = std::function<void()>;

    explicit AsyncFunctor(function_t function, const std::string& taskName = "Task", const uint16_t stackSize = configMINIMAL_STACK_SIZE, const uint8_t priority = defaultPriority)
        :   ITask(taskName, stackSize, priority),
            _func{function} {

    }

    void run(void* args) override {
        if (_func) {
            _func();
        }
    }
    private:
        function_t _func = nullptr;
};