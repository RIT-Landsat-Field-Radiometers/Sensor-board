/*
 * Calibration.h
 *
 *  Created on: Nov 22, 2021
 *      Author: reedt
 */

#ifndef SRC_DEVICE_SPECIFIC_TASKS_CALIBRATION_H_
#define SRC_DEVICE_SPECIFIC_TASKS_CALIBRATION_H_

#include <Tasks/TaskInterface.h>
#include "fatfs.h"
#include "sdmmc.h"
#include <cmath>


typedef struct
{
	uint8_t sensorID; // Which socket on the board
	uint32_t * radiance; // [in] array of raw readings 2 for dual, 4 for quad
	uint8_t numChannles; // size of above array
	uint32_t thermal; // raw internal sensor temperature
} calibReq;  // input struct for a calibration request

typedef struct
{
	uint8_t sensorID; // Which socket on the board
	float * radiance; // [out] Converted values after processing
	uint8_t numChannles; // size of above array
	float thermal; // raw internal sensor temperature
} calibRes;  // output struct for a calibration request



class Calibration: public TaskInterface
{
private:
	osMessageQueueId_t preConvert; // put a calibrequest in to perform a calibrated conversion
	osMessageQueueId_t postConvert; // read a calibresponse out to get requested conversion

	osThreadId_t CalibTaskHandle;
	static void CalibTask(void * arg);
	FATFS fs;  // file system

	uint8_t numSensors;
	uint8_t numChannels;




public:
	Calibration();
	~Calibration();

	bool start() override;
	bool kill() override;

	void startCalibration(); // needs to be in cal unit, will take a LONG time

	bool convertReadings(calibReq *data, calibRes &conv);

	static float toVoltage(int32_t raw)
	{
		raw = raw & 0xFF'FF'FF; // clear upper bits just in case

		float converted = 1;
		if(raw & (1 << 23))
		{
			// value is negative

			raw = raw ^ 0xFF'FF'FF; // Do twos compliment
			raw += 1;

			converted = -1; // multiply result by -1
		}

		converted = converted * (raw / 8388608.0) * 3.3; // divide by 2^23 and multiply by vref
		return converted;
	}

	static float toInternalCelcius(int32_t raw)
	{
		float volt = toVoltage(raw);
		float res = volt / 0.000010;

		float ln = log(res);
		return 1.0 / (0.000949801222 +
				0.000216475249 * ln +
				0.000000483296004 * pow(ln, 2) +
				0.000000110644773 * pow(ln, 3)) - 273.15;
	}
};

#endif /* SRC_DEVICE_SPECIFIC_TASKS_CALIBRATION_H_ */
