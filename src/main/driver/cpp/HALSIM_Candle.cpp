#include "HALSIM_Candle.h"

#include "hal/CAN.h"
#include "mockdata/CanData.h"
#include "cstring"

#ifdef _WIN32
#include "WindowsCANController.h"
#else
#include "CANController.h"
#endif

namespace
{
struct HALSIM_Candle
{
#ifdef _WIN32
    WindowsCANController controller;
#else
#endif
    int32_t sendMessageHandle;
    int32_t receiveMessageHandle;
    int32_t canStatusCallback;
};
} // namespace

static void SendMessageCallback(const char *name, void *param,
                                uint32_t messageID,
                                const uint8_t *data,
                                uint8_t dataSize, int32_t periodMs,
                                int32_t *status)
{
    HALSIM_Candle &controller = *static_cast<HALSIM_Candle *>(param);
    CANData canData;
    std::memcpy(canData.data, data, dataSize);
    canData.length = dataSize;
    canData.id = messageID;
#ifdef _WIN32
    // TODO: Handle Repeating
    controller.controller.putData(canData);
#else
    (void)controller;
#endif
    *status = 0;
}

static void ReceiveMessageCallback(const char *name, void *param, uint32_t *messageID, uint32_t messageIDMask,
                                   uint8_t *data, uint8_t *dataSize, uint32_t *timeStamp, int32_t *status)
{
    HALSIM_Candle &controller = *static_cast<HALSIM_Candle *>(param);
#ifdef _WIN32
    auto readData = controller.controller.getData(*messageID, messageIDMask);
    if (!readData.has_value())
    {
        *status = HAL_ERR_CANSessionMux_MessageNotFound;
        return;
    }

    std::memcpy(data, readData->data, readData->length);
    *dataSize = readData->length;
    *timeStamp = readData->timestamp;
    *messageID = readData->id;
    *status = 0;

#else
    (void)controller;
    *status = HAL_ERR_CANSessionMux_MessageNotFound;
#endif
}

static void StatusCallback(const char *name, void *param, float *percentBusUtilization,
                           uint32_t *busOffCount, uint32_t *txFullCount, uint32_t *receiveErrorCount,
                           uint32_t *transmitErrorCount, int32_t *status)
{
    *status = 0;
}

extern "C"
{
    HALSIM_Candle_Handle HALSIM_Candle_Enable(const char *name, int32_t baud)
    {
        auto controller = new HALSIM_Candle{};
#ifdef _WIN32
        if (controller->controller.start(name, baud) < 0)
        {
            delete controller;
            return nullptr;
        }
#endif

        controller->sendMessageHandle = HALSIM_RegisterCanSendMessageCallback(SendMessageCallback, controller);
        controller->receiveMessageHandle = HALSIM_RegisterCanReceiveMessageCallback(ReceiveMessageCallback, controller);
        controller->canStatusCallback = HALSIM_RegisterCanGetCANStatusCallback(StatusCallback, controller);

        return controller;
    }
    void HALSIM_Candle_Clean(HALSIM_Candle_Handle handle)
    {

        HALSIM_Candle *controller = static_cast<HALSIM_Candle *>(handle);

#ifdef _WIN32
        controller->controller.stop();
#endif

        HALSIM_CancelCanGetCANStatusCallback(controller->canStatusCallback);
        HALSIM_CancelCanReceiveMessageCallback(controller->receiveMessageHandle);
        HALSIM_CancelCanSendMessageCallback(controller->sendMessageHandle);

        delete controller;
    }
}
