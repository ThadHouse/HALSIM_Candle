#pragma once

#include <stdint.h>

typedef void* HALSIM_Candle_Handle;

extern "C" {
HALSIM_Candle_Handle HALSIM_Candle_Enable(const char* name, int32_t baud);
void HALSIM_Candle_Clean(HALSIM_Candle_Handle handle);
}
