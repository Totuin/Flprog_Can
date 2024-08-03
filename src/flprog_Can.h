#pragma once
#include "flprogUtilites.h"

#ifdef _STM32_DEF_
#define FLPROG_STM32_CAN
#else
#define FLPROG_PLUG_CAN
#endif

#ifdef FLPROG_STM32_CAN
#include "flprogStm32Can.h"
#endif

#ifdef FLPROG_PLUG_CAN
//#include "software/flprogAvrSoftRTC.h"
#endif

