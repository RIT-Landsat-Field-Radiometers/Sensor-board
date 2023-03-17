/*
 * Calibration.cpp
 *
 *  Created on: Nov 22, 2021
 *      Author: reedt
 */

#include <Tasks/Calibration.h>


Calibration::Calibration()
{
	// TODO Auto-generated constructor stub
	tlog = new Logger("Calibration");
}

void Calibration::CalibTask(void *arg)
{
	if (arg == nullptr)
	{
		osThreadExit();
	}

	Calibration *_this = (Calibration*) arg;

//	FRESULT fresult;  // result
//	MX_FATFS_Init();
//	const char *path = "/";
//	fresult = f_mount(&_this->fs, path, 1);	// mount partition
//
//	bool useCalib = true;
//
//	if (fresult != FR_OK)
//	{
//		_this->log->warn("Could not mount SD card, code: %d", fresult);
//		_this->log->info("No calibration data loaded, reporting voltages");
//		useCalib = false;
//	}
//
//	if(useCalib)
//	{
//
//	}



	calibReq request;
	calibRes response;

	float storage[8];

	response.radiance = storage;

	for (;;)
	{
		auto res = osMessageQueueGet(_this->preConvert, &request, nullptr,
				osWaitForever); // wait for a conversion request;
		if (res == osOK)
		{
			// TODO THIS IS NOT FINAL, JUST CONVERTS TO VOLTAGES
			for(int idx = 0; idx < request.numChannles; idx++)
			{
				response.radiance[idx] = Calibration::toVoltage(request.radiance[idx]);
			}
			response.thermal = Calibration::toInternalCelcius(request.thermal);
			osMessageQueuePut(_this->postConvert, &response, 0, osWaitForever);
		}
	}
}

Calibration::~Calibration()
{
	// TODO Auto-generated destructor stub
}

bool Calibration::start()
{
	preConvert = osMessageQueueNew(1, sizeof(calibReq), nullptr);
	postConvert = osMessageQueueNew(1, sizeof(calibRes), nullptr);

	osThreadAttr_t Task_attributes
	{ 0 };
	Task_attributes.stack_size = 2048 * 4;
	Task_attributes.name = "Calibration";
	Task_attributes.priority = (osPriority_t) osPriorityNormal;
	CalibTaskHandle = osThreadNew(CalibTask, this, &Task_attributes);
}

bool Calibration::kill()
{
}

void Calibration::startCalibration()
{
}

bool Calibration::convertReadings(calibReq *data, calibRes &conv)
{
	auto res = osMessageQueuePut(preConvert, data, 0, 100); // add a conversion request, block if busy
	if (res == osOK)
	{
		res = osMessageQueueGet(postConvert, &conv, nullptr, 100); // wait for conversion to complete
	}
	return res == osOK;
}
