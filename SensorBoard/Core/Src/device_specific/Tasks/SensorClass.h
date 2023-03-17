/*
 * SensorClass.h
 *
 *  Created on: Nov 22, 2021
 *      Author: reedt
 */

#ifndef SRC_DEVICE_SPECIFIC_TASKS_SENSORCLASS_H_
#define SRC_DEVICE_SPECIFIC_TASKS_SENSORCLASS_H_

#include "Tasks/TaskInterface.h"
#include "ADC/ads123S0_enums.h"
#include "ADC/ads124S0.h"
#include "Tasks/Calibration.h"

class SensorClass: public TaskInterface
{
private:
	osThreadId_t DataTaskHandle;

	static void DataTask(void * arg);

	Calibration * calib;

public:
	SensorClass(Calibration * calib);
	~SensorClass();
	bool start() override;
	bool kill() override;
};

#endif /* SRC_DEVICE_SPECIFIC_TASKS_SENSORCLASS_H_ */
