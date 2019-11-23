#pragma once

#include <stdint.h>
#include <optional>
#include <vector>
#include <string>
#include "wpi/Twine.h"

struct CANData {
  uint64_t timestamp;
  int32_t id;
  bool valid;
  uint8_t length;
  uint8_t data[8];
};

class CANController {
public:
    virtual std::optional<CANData> getData(uint32_t idFilter, uint32_t idMask) = 0;
    virtual void putData(const CANData& data) = 0;
};