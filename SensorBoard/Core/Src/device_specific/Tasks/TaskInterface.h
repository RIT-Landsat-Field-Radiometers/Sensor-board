/*
 * TaskInterface.h
 *
 *  Created on: Nov 5, 2021
 *      Author: reedt
 */

#ifndef SRC_DEVICE_SPECIFIC_TASKS_TASKINTERFACE_H_
#define SRC_DEVICE_SPECIFIC_TASKS_TASKINTERFACE_H_


#include "cmsis_os.h"
#include "Logging/Logger.h"

class TaskInterface
{
protected:
	osMutexId_t taskMutex;
	osEventFlagsId_t taskEvents;

	Logger * tlog;

public:

	virtual bool start() = 0;
	virtual bool kill() = 0;
	uint32_t setFlags(uint32_t flags)
	{
		return osEventFlagsSet(taskEvents, flags);
	}

	uint32_t getFlags()
	{
		return osEventFlagsGet(taskEvents);
	}

	uint32_t waitFlags(uint32_t flags, uint32_t timeout)
	{
		return osEventFlagsWait(taskEvents, flags, osFlagsWaitAny, timeout);
	}

};


#endif /* SRC_DEVICE_SPECIFIC_TASKS_TASKINTERFACE_H_ */
