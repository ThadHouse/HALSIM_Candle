#include "HALSIM_Candle.h"
#include "hal/HALBase.h"
#include "hal/CANAPI.h"

#include <stdio.h>
#include <thread>
#include <iostream>

int main() {
    HAL_Initialize(500, 0);

    int32_t count  = 0;
    auto devices = HALSIM_Candle_GetDevices(&count);

    for (int i = 0; i < count; i++) {
        printf("%s\n", devices[i].deviceId);
    }

	auto candleHandle = HALSIM_Candle_Enable(devices[0].deviceId, 250000);

	if (!candleHandle) {
		printf("Candle Init Failure\n");
	}

	int32_t status = 0;
	auto can = HAL_InitializeCAN(HAL_CANManufacturer::HAL_CAN_Man_kREV, 1, HAL_CANDeviceType::HAL_CAN_Dev_kMotorController, &status);

    HALSIM_Candle_FreeDevices(devices, count);

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		uint8_t data[8];
		int32_t len = 0;
		uint64_t ts = 0;
		status = 0;
		HAL_ReadCANPacketLatest(can, 0x060, data, &len, &ts, &status);
		std::cout << status << std::endl;
	}

	HAL_CleanCAN(can);
	HALSIM_Candle_Clean(candleHandle);
}