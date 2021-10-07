/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "can.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <cstdint>
#include <stdio.h>
#include <cmath>

#include "device_specific/ADC/ads123S0_enums.h"
#include "device_specific/ADC/ads124S0.h"
#include "bsp/DS28CM00ID/DS28CM00ID.h"
#include "bsp/LEDs/LEDManager.h"
#include "bsp/UART/UARTManager.h"
#include "Logging/Logger.h"
#include "CANOpenNode/OD.h"
#include "301/CO_ODinterface.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern DS28CM00_ID id1;
extern LEDManager leds;
extern UARTManager uartMan;
extern Logger Log;

/* Definitions for SysMonitor */
osThreadId_t SysMonitorHandle;
osThreadAttr_t SysMonitor_attributes =
{ .name = "SysMonitor", };
/* Definitions for SerialID_lock */
osMutexId_t SerialID_lockHandle;
const osMutexAttr_t SerialID_lock_attributes =
{ .name = "SerialID_lock" };

/* USER CODE END Variables */
/* Definitions for defaultTask */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void startMonitoring(void *argument);
extern void canopen_start(void);
void measurementTask(void*);
/* USER CODE END FunctionPrototypes */

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
	/* Run time stack overflow checking is performed if
	 configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
	 called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	 configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
	 function that will get called if a call to pvPortMalloc() fails.
	 pvPortMalloc() is called internally by the kernel whenever a task, queue,
	 timer or semaphore is created. It is also called by various parts of the
	 demo application. If heap_1.c or heap_2.c are used, then the size of the
	 heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	 FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	 to query the size of free heap space that remains (although it does not
	 provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
	/* USER CODE BEGIN Init */

	SysMonitor_attributes.priority = (osPriority_t) osPriorityLow;
	SysMonitor_attributes.stack_size = 2048 * 4;

	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	SerialID_lockHandle = osMutexNew(&SerialID_lock_attributes);
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of defaultTask */

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* creation of SysMonitor */
//	SysMonitorHandle = osThreadNew(startMonitoring, (void*) nullptr,
//			&SysMonitor_attributes);
	osThreadNew(measurementTask, nullptr, nullptr);

	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	uartMan.start();
	leds.start(false);
	Log.info("Application Started");
	canopen_start();
	/* USER CODE END RTOS_EVENTS */

}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
double toVoltage(int32_t val)
{
	return (val / 8388608.0) * 3.3; // divide by 2^23 and multiply by vref
}

double toTemp(double resistance)
{
	double ln = log(resistance);
	double result = 0.000949801222;
	double inter = 0.000216475249 * ln;
	result = result + inter;
	inter = pow(ln, 2);
	inter = inter * 0.000000483296004;
	result = result + inter;
	inter = pow(ln, 3);
	inter = inter * 0.000000110644773;
	result = result + inter;
	result = pow(result, -1.0);
	result = result - 273.15;
	return result;
}

void measurementTask(void*)
{
	Logger ilog("MeasureTask");

	ilog.info("Starting measurements");

	ADS124S0 adc(&hspi1, ADC_RST_Pin, ADC_RST_GPIO_Port, ADC_CS_Pin,
	ADC_CS_GPIO_Port,
	ADC_DRDY_Pin, ADC_DRDY_GPIO_Port, ADC_Start_Pin,
	ADC_Start_GPIO_Port);

	if (!adc.init())
	{
		ilog.error("Could not initialize ADC");
		for (;;)
			osDelay(1000);
	}
	adc.setExcitationMagnitude(ADS124S0::EXCITATION_CURRENT::_50uA);
	adc.routeExcitation1(10); // route the excitation current to the thermistor
	adc.routeExcitation2(11);
	double internals[2];
	double externals[8];

	auto thermEntry1 = OD_ENTRY_H6000_thermopile1;
	auto thermEntry2 = OD_ENTRY_H6001_thermopile2;

	for (;;)
	{
		internals[0] = toTemp((toVoltage(adc.readChannel(10)) - 1.25) / 0.00005); // get voltage then divide by excitation current to get resistance
		internals[1] = toTemp((toVoltage(adc.readChannel(11)) - 1.25) / 0.00005); // get voltage then divide by excitation current to get resistance

		for (int idx = 0; idx < 8; idx++)
		{
			externals[idx] = toVoltage(adc.readChannel(idx));
			if(idx < 4)
			{
				OD_set_f32(thermEntry1, idx+1, externals[idx], true);
			}
			else
			{
				OD_set_f32(thermEntry2, idx-3, externals[idx], true);
			}
			osDelay(1);
		}

		ilog.info(
				"Thermopile1- cA=%8.4fV, cB=%8.4fV, cC=%8.4fV, cD=%8.4fV, therm=%8.4fohm",
				externals[0], externals[1], externals[2], externals[3],
				internals[0]);
		ilog.info(
				"Thermopile2- cA=%8.4fV, cB=%8.4fV, cC=%8.4fV, cD=%8.4fV, therm=%8.4fohm",
				externals[4], externals[5], externals[6], externals[7],
				internals[1]);
		ilog.info(
				"==========================================================================================================================================");
		osDelay(500);
	}
}

/* USER CODE BEGIN Header_startMonitoring */
/**
 * @brief Function implementing the SysMonitor thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_startMonitoring */
void startMonitoring(void *argument)
{
	/* USER CODE BEGIN startMonitoring */
	Logger privLog("SystemMonitor");

	auto retval = osMutexAcquire(SerialID_lockHandle, 1000);
	if (retval == osOK)
	{
		auto family = id1.getFamily();
		auto id = id1.getID();
		auto dev_mode = id1.getMode();
		privLog.info("Family=%hhx, id=%llx, mode=%hhx", family, id, dev_mode);
		osMutexRelease(SerialID_lockHandle);
	}
	else
	{
		auto holder = osMutexGetOwner(SerialID_lockHandle);
		privLog.error("Could not acquire mutex on DS28CM00_ID, held by: %s",
				osThreadGetName(holder));
	}

//	leds.slowFlash(WHITE);

	for (;;)
	{
//		osDelay(1000);

//		leds.fastFlash(RED);

//		privLog.debug("RED fast");

		osDelay(1000);
//		leds.slowFlash(GREEN);
//		privLog.debug("GREEN slow");

//		osDelay(1000);
//		leds.turnOn(BLUE);
//		privLog.debug("BLUE on");

//		osDelay(1000);
//		leds.turnOff(WHITE);
//		privLog.debug("LEDs all off");

		size_t freeHeap = xPortGetFreeHeapSize();
		if (freeHeap < 2000)
		{
			privLog.error("LOW HEAP, %u bytes remaining", freeHeap);
		}
	}
	/* USER CODE END startMonitoring */
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
