/*
 * CANOpenTask.h
 *
 *  Created on: Nov 5, 2021
 *      Author: reedt
 */

#ifndef SRC_DEVICE_SPECIFIC_TASKS_CANOPENTASK_H_
#define SRC_DEVICE_SPECIFIC_TASKS_CANOPENTASK_H_

#include "TaskInterface.h"
#include <set>
#include <map>
#include <vector>
#include "can.h"
#include "tim.h"
#include "crc.h"
#include "CANopenNode/CANopen.h"


class CANOpenTask: public TaskInterface
{
private:
	uint64_t fullSerial;
	CO_LSS_address_t lssAddress;
	bool syncActive = false;
	bool produceHB = false;
	CO_t *canOpenHandle;

	uint8_t pendingID = CO_LSS_NODE_ID_ASSIGNMENT;
	uint16_t pendingRate = 1000;

	static CANOpenTask * singleton;

	void processTick(void);

	void CANOpenInit(void);

public:
	CANOpenTask(uint64_t fullSerial);
	~CANOpenTask();

	bool start() override;
	bool kill() override;

	CO_LSS_address_t getOwnAddress();
};

#endif /* SRC_DEVICE_SPECIFIC_TASKS_CANOPENTASK_H_ */
