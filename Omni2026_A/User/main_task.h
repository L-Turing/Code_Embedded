#ifndef __MAIN_TASK_H
#define __MAIN_TASK_H

#define D_YAW_BASE_FIRST 0.0f  //角度 °

#include "stdint.h"

#pragma pack(push, 1)
#ifdef __cplusplus
extern "C" {
#endif

double msp(double x, double in_min, double in_max, double out_min, double out_max);

#ifdef __cplusplus
}
#endif
#pragma pack(pop)

#endif
