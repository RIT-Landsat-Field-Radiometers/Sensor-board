/*
 * CANOpenTask.cpp
 *
 *  Created on: Nov 5, 2021
 *      Author: reedt
 */

#include <Tasks/CANOpenTask.h>
#include "CANopenNode/301/CO_ODinterface.h"
#include "CANOpenNode/OD.h"
#include <functional>


#define NMT_CONTROL \
            CO_NMT_ERR_ON_ERR_REG \
          | CO_ERR_REG_GENERIC_ERR \
          | CO_ERR_REG_COMMUNICATION
#define FIRST_HB_TIME 500
#define SDO_SRV_TIMEOUT_TIME 1000
#define SDO_CLI_TIMEOUT_TIME 500
#define SDO_CLI_BLOCK false
#define OD_STATUS_BITS NULL

#define COMMSPC 1
#define SENSORSPC 2
#define BMEPC 3

CANOpenTask *CANOpenTask::singleton = nullptr;

CANOpenTask::CANOpenTask(uint64_t fullSerial) :
		fullSerial(fullSerial)
{
	tlog = new Logger("CAN");
}

CANOpenTask::~CANOpenTask()
{
	// TODO Auto-generated destructor stub
}

bool CANOpenTask::start()
{
	singleton = this;
	taskEvents = osEventFlagsNew(nullptr);
	CANOpenInit();
	CO_CANsetNormalMode(canOpenHandle->CANmodule);

	HAL_TIM_RegisterCallback(&htim15, HAL_TIM_PERIOD_ELAPSED_CB_ID,
			[](TIM_HandleTypeDef *htim)
			{
				if (htim->Instance == TIM15)
				{
					if(singleton != nullptr)
					{
						singleton->processTick();
					}
				}
			});

	HAL_TIM_Base_Start_IT(&htim15);
	HAL_TIM_Base_Start_IT(&htim16);
	__HAL_TIM_SET_COUNTER(&htim16, 0);

	tlog->info("CANOpen started");
	return true;
}

bool CANOpenTask::kill()
{
}

CO_LSS_address_t CANOpenTask::getOwnAddress()
{
	return lssAddress;
}

void CANOpenTask::processTick(void)
{
	uint32_t us = __HAL_TIM_GET_COUNTER(&htim16);
	auto reset = CO_RESET_NOT;

	uint32_t next = 0;
	reset = CO_process(canOpenHandle, false, us, &next);

	switch (reset)
	{
	/** 0, Normal return, no action */
	case CO_RESET_NOT:
		break;
		/** 1, Application must provide communication reset. */
	case CO_RESET_COMM:
		HAL_TIM_Base_Stop_IT(&htim15);
		CO_CANsetConfigurationMode((void*) &hcan1);
		CO_delete(canOpenHandle);
		CANOpenInit();
		CO_CANsetNormalMode(canOpenHandle->CANmodule);
		HAL_TIM_Base_Start_IT(&htim15);
		__HAL_TIM_SET_COUNTER(&htim16, 0);
		return;
		/** 2, Application must provide complete device reset */
	case CO_RESET_APP:
		HAL_NVIC_SystemReset(); // DOES NOT RETURN
		break;
		/** 3, Application must quit, no reset of microcontroller (command is not
		 * requested by the stack.) */
	case CO_RESET_QUIT:
		CO_CANsetConfigurationMode((void*) &hcan1);
		CO_delete(canOpenHandle);
		HAL_TIM_Base_Stop_IT(&htim15);
		return;
	default:
		Error_Handler();
	}

	bool syncWas = false;
#if (CO_CONFIG_SYNC) & CO_CONFIG_SYNC_ENABLE
            syncWas = CO_process_SYNC(canOpenHandle, us, NULL);
#endif
#if (CO_CONFIG_PDO) & CO_CONFIG_RPDO_ENABLE
            CO_process_RPDO(canOpenHandle, syncWas, us, NULL);
#endif
#if (CO_CONFIG_PDO) & CO_CONFIG_TPDO_ENABLE
            CO_process_TPDO(canOpenHandle, syncWas, us, NULL);
#endif

	__HAL_TIM_SET_COUNTER(&htim16, 0);
}

void CANOpenTask::CANOpenInit(void)
{
	CO_ReturnError_t err;
	canOpenHandle = CO_new( /*config*/nullptr, /*heap used*/nullptr);
	err = CO_CANinit(canOpenHandle, (void*) &hcan1, 1000);
	uint32_t *in = (uint32_t*) &fullSerial;
	uint32_t hashed = HAL_CRC_Calculate(&hcrc, in, 4);

	auto entry = OD_ENTRY_H1018_identity;
	OD_set_u32(entry, 4, hashed, true);		// Set device serial number

	//	@formatter:off
	lssAddress.identity.vendorID = OD_PERSIST_COMM.x1018_identity.vendor_ID;
	lssAddress.identity.productCode = OD_PERSIST_COMM.x1018_identity.productCode;
	lssAddress.identity.revisionNumber = OD_PERSIST_COMM.x1018_identity.revisionNumber;
	lssAddress.identity.serialNumber = OD_PERSIST_COMM.x1018_identity.serialNumber;
	//	@formatter:on


	err = CO_LSSinit(canOpenHandle, &lssAddress, &pendingID, &pendingRate);

	uint32_t errInf = 0;
	//	@formatter:off
	err = CO_CANopenInit(canOpenHandle,
			nullptr, /* alternate NMT */
			nullptr, /* alternate em */
			OD, /* Object dictionary */
			OD_STATUS_BITS, /* Optional OD_statusBits */
			(CO_NMT_control_t)(NMT_CONTROL),
			FIRST_HB_TIME, /* ??? firstHBTime_ms */
			SDO_SRV_TIMEOUT_TIME, /*??? SDOserverTimeoutTime_ms */
			SDO_CLI_TIMEOUT_TIME, /*??? SDOclientTimeoutTime_ms */
			SDO_CLI_BLOCK, /*??? SDOclientBlockTransfer */
			pendingID, /*??? CO_activeNodeId */
			&errInf /*errInfo*/);
	//	@formatter:on

	err = CO_CANopenInitPDO(canOpenHandle, canOpenHandle->em, OD, pendingID, &errInf);
}
