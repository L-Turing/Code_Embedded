#ifndef _MAIN_TASK_H
#define _MAIN_TASK_H

#include "Motor.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
    void StartIMU(void const* argument);
    void StartGimbal(void const* argument);
    void StartRemote(void const* argument);
    extern uint8_t array_receive[6][8];

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern DaMiaoMotor_Class motor_yaw;
extern DJIMotor_Class motor_pitch;
extern DJIMotor_Class motor_ammunition;
extern DJIMotor_Class motor_friwheel_left;
extern DJIMotor_Class motor_friwheel_right;



#endif

#endif
