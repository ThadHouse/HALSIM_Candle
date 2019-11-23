#pragma once

#include "CANController.h"

#include "wpi/mutex.h"
#include <thread>
#include <atomic>
#include <vector>
#include "candle.h"

class WindowsCANController : public CANController
{
public:
  #if UNICODE
    typedef std::wstring string_type;
    typedef std::wstring_view string_view_type;
#else
    typedef std::string string_type;
    typedef std::string_view string_view_type;
#endif  

    int start(string_view_type port, int baud);
    void stop();
    std::optional<CANData> getData(uint32_t idFilter, uint32_t idMask) override;
    void putData(const CANData &data) override;


    static std::vector<string_type> getDevices();

private:
    void readThreadMain();

    std::atomic_bool m_running;
    std::thread m_incomingThread;

    candle_handle m_candleHandle = nullptr;

    wpi::mutex m_dataMutex;
    std::vector<CANData> m_dataMap;
};