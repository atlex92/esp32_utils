#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string>

class ITask {
	public:
		static const uint32_t defaultPriority {1};
		explicit ITask(const std::string& taskName = "Task", const uint16_t stackSize = configMINIMAL_STACK_SIZE, const uint8_t priority = defaultPriority, const BaseType_t coreID = 0);
		virtual ~ITask();
		uint32_t getPeriod() const;
		bool isRunning()const;
		virtual void suspend();
		virtual void resume();

		void setStackSize(const uint16_t stackSize);
		void setPriority(const uint8_t priority);
		void setName(const std::string& name);
		void setCore(BaseType_t coreID);
		void start(void* taskData = nullptr);
		void stop();
		void setPeriod(const uint32_t period);

		virtual void run(void* data) = 0; // Make run pure virtual
		static void delay(const int ms);
		virtual void onStop();

	private:
		static void runTask(void* data);

	protected:
		uint32_t _periodMs;
		xTaskHandle _taskDescr;
	private:
		volatile bool _isRunning;
		void* _taskParam;
		std::string _taskName;
		uint16_t _stackSize;
		uint8_t _priority;
		BaseType_t _coreID;
};