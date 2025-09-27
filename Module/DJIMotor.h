#ifndef __DJIMOTOR_H
#define __DJIMOTOR_H

#include "stdint.h"
#include "PID.h"

// 定义结构体对齐方式为1字节对齐
// 这可以确保结构体在内存中的布局与C语言标准一致，避免编译器对齐填充的影响
// 节省内存空间，并确保数据的正确读取和写入
#pragma pack(push, 1)

// C++兼容性声明
// 如果在C++环境中编译此头文件，则使用extern "C"来避免C++名称修饰
// 这使得C++编译器能够正确链接C语言编写的函数
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  M3508 = 0,
  M2006 = 1,
  GM6020 = 2,
} type_motor;

typedef struct
{
  int8_t temperate;
  int16_t last_ecd;
  int16_t ecd;
  float ecd_rad_out;      //输出轴编码器值 rad
  float ecd_rad_rotor;    //转子编码器值 rad
  float ecd_angle_out;    //输出轴角度 °  -180°~+180°
  float ecd_angle_rotor;  //转子角度 °

  int16_t speed_rpm_rotor;
  float speed_rpm_out;
  float speed_rad_rotor;
  float speed_rad_out;

  int16_t given_current;
  type_motor motor_types;
  int32_t circle_number_rotor;
  uint32_t motor_stdid;
  float reduction_ratio;
  float accumlate_rad_rotor;  //rad
  float accumlate_rad_out;    //rad
  float predicted_power;
  float expected_power;
  float scale_power;

  PID * pid_speed;  //速度PID指针
  PID * pid_position;  //位置PID指针
  
} Motor;

typedef enum{
  Speed_Mode = 0,
  Position_Mode = 1,
}type_motor_mode;

extern Motor M3508_motor[4];
extern Motor GM6020_motor;
extern Motor motor_2006;

void Chassis_Motor(uint8_t which_can, float * target);
void Drive_GM6020_Motor(type_motor_mode mode, float target);
void Set2006(float m2006_set,type_motor_mode mode);
void Update_Info_DJIMotor(Motor * motor, uint8_t array_length);
void Get_Info_DJIMotor(Motor * motor, uint8_t * canbuf_receive);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif
