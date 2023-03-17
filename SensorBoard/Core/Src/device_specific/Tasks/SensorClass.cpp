///*
// * SensorClass.cpp
// *
// *  Created on: Nov 22, 2021
// *      Author: reedt
// */
//
//#include <Tasks/SensorClass.h>
//#include "CANOpenNode/OD.h"
//#include "CANopenNode/301/CO_ODinterface.h"
//
//SensorClass::SensorClass(Calibration *calib) :
//		calib(calib)
//{
//	// TODO Auto-generated constructor stub
//	tlog = new Logger("Sensors");
//	taskEvents = osEventFlagsNew(nullptr);
//	taskMutex = osMutexNew(nullptr);
//}
//
//void SensorClass::DataTask(void *arg)
//{
//	if (arg == nullptr)
//	{
//		osThreadExit();
//	}
//
//	SensorClass *_this = (SensorClass*) arg;
//
//	ADS124S0 adc(&hspi1, ADC_RST_Pin, ADC_RST_GPIO_Port, ADC_CS_Pin,
//	ADC_CS_GPIO_Port,
//	ADC_DRDY_Pin, ADC_DRDY_GPIO_Port, ADC_Start_Pin,
//	ADC_Start_GPIO_Port);
//
//	uint32_t internals[2];
//	uint32_t externals[8];
//
//	auto thermEntry1 = OD_ENTRY_H6000_thermopile1;
//	auto thermEntry2 = OD_ENTRY_H6001_thermopile2;
//
//	int channelIDX = 0;
//	bool excitationOn = false;
//
//	auto sampleTick = HAL_GetTick();
//	bool sampling = true;
//
//	for (;;)
//	{
//		if(!sampling)
//		{
//			if ((HAL_GetTick() - sampleTick) >= 1000)
//			{
//				sampling = true;
//				sampleTick = HAL_GetTick();
//			}
//			else
//			{
//				osDelay(10);
//				continue;
//			}
//		}
//
//		if (channelIDX < 8)
//		{
//			externals[channelIDX] = adc.readChannel(channelIDX);
//			channelIDX++;
//		}
//		else if (channelIDX < 10)
//		{
//			if (!excitationOn)
//			{
//				adc.setExcitationMagnitude(ADS124S0::EXCITATION_CURRENT::_10uA);
//				adc.routeExcitation1(10); // route the excitation current to the thermistors
//				adc.routeExcitation2(11);
//				excitationOn = true;
//				osDelay(10);
//			}
//			internals[channelIDX - 8] = adc.readChannel(channelIDX + 2);
//			channelIDX++;
//		}
//		else
//		{
//			if (excitationOn)
//			{
//				adc.stopExcitation();
//				excitationOn = false;
//			}
//			sampling = false;
//			channelIDX = 0;
//
//			calibReq request;
//
//			request.sensorID = 0;
//			request.numChannles = 4;
//			request.radiance = &externals[0];
//			request.thermal = internals[0];
//
//			calibRes response;
//			auto ok = _this->calib->convertReadings(&request, response);
//			if(ok)
//			{
//				for(int idx = 0; idx < 4; idx++)
//				{
//					OD_set_f32(thermEntry1, idx+1, response.radiance[idx], true);
//				}
//			}
//			else
//			{
//				_this->tlog->error("Calibration conversion failed");
//				for(int idx = 0; idx < 4; idx++)
//				{
//
//					OD_set_f32(thermEntry1, idx+1, NAN, true);
//				}
//			}
//
//			_this->tlog->trace(
//							"==========================================================================================================================================");
//
//			_this->tlog->trace(
//							"Thermopile1- cA=%8.4fV, cB=%8.4fV, cC=%8.4fV, cD=%8.4fV, therm=%8.4f*C",
//							response.radiance[0], response.radiance[1], response.radiance[2], response.radiance[3],
//							response.thermal);
//
//
//			request.sensorID = 1;
//			request.numChannles = 4;
//			request.radiance = &externals[4];
//			request.thermal = internals[1];
//
//			ok = _this->calib->convertReadings(&request, response);
//			if(ok)
//			{
//				for(int idx = 0; idx < 4; idx++)
//				{
//					OD_set_f32(thermEntry2, idx+1, response.radiance[idx], true);
//				}
//			}
//			else
//			{
//				_this->tlog->error("Calibration conversion failed");
//				for(int idx = 0; idx < 4; idx++)
//				{
//
//					OD_set_f32(thermEntry2, idx+1, NAN, true);
//				}
//			}
//
//			_this->tlog->trace(
//							"Thermopile2- cA=%8.4fV, cB=%8.4fV, cC=%8.4fV, cD=%8.4fV, therm=%8.4fohm",
//							response.radiance[0], response.radiance[1], response.radiance[2], response.radiance[3],
//							response.thermal);
//			_this->tlog->trace(
//							"==========================================================================================================================================");
//
//		}
//	}
//}
//
//SensorClass::~SensorClass()
//{
//	// TODO Auto-generated destructor stub
//}
//
//bool SensorClass::start()
//{
//	osThreadAttr_t Task_attributes
//	{ 0 };
//	Task_attributes.stack_size = 2048 * 4;
//	Task_attributes.name = "Sensor";
//	Task_attributes.priority = (osPriority_t) osPriorityNormal;
//	DataTaskHandle = osThreadNew(DataTask, this, &Task_attributes);
//}
//
//bool SensorClass::kill()
//{
//}
