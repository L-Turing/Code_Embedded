#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stdint.h"

#pragma pack(push, 1)
#ifdef __cplusplus
extern "C" {
#endif
typedef struct
{
  uint8_t rx_dt7_dr16[18];

  int16_t ch0;
  int16_t ch1;
  int16_t ch2;
  int16_t ch3;
  int16_t ch4;
  uint8_t s1;
  uint8_t s2;
} DT7_DR16_t;

typedef struct
{
  uint8_t rx_ps2_uint[34];

  uint8_t frame_header[2];  //0 1
  uint8_t operation_code;   //2
  uint8_t length;           //3
  uint8_t identifier;       //4

  //5：位域 或 16~23：一一对应
  uint8_t left[2];     //20 22
  uint8_t right[2];    //21 23
  uint8_t right_x[2];  //16 18
  uint8_t right_y[2];  //19 17

  //6：位域
  uint8_t select : 1;            //1     0000 0001
  uint8_t mode : 1;              //16    0001 0000
  uint8_t start : 1;             //2     0000 0010
  uint8_t left_joystick_z : 1;   //4     0000 0100
  uint8_t right_joystick_z : 1;  //8     0000 1000

  //7：位域 或 12~15：一一对应
  uint8_t left_x[2];  //14 15
  uint8_t left_y[2];  //13 12

  uint8_t left_joystick_x;   //9
  uint8_t left_joystick_y;   //8
  uint8_t right_joystick_x;  //11
  uint8_t right_joystick_y;  //10

  //24~31:无意义

  uint16_t serial_number;  //32
  uint16_t verify;         //33

  uint16_t ps2_online;  //离线计数
} PS2_t;

typedef struct
{
  uint8_t ps2_start;
  uint8_t ps2_start_now;
  uint8_t ps2_start_last;

  uint8_t ps2_select;
  uint8_t ps2_select_now;
  uint8_t ps2_select_last;

  uint8_t ps2_mode;
  uint8_t ps2_mode_now;
  uint8_t ps2_mode_last;
} Flag_t;

typedef struct
{
  uint8_t header;
  uint8_t state;
  float yaw;    // 目标yaw角度
  float pitch;  // 目标pitch角度
  float roll;   // 目标roll角度
  float acc_x;  // 加速度x轴
  float acc_y;  // 加速度y轴
  float acc_z;  // 加速度z轴
  uint8_t datatx_all_u8[26];
} S_Packet;

typedef struct
{
  uint8_t header;
  uint8_t state;
  float v;        // 目标速度
  float yaw_tar;      // 目标yaw角度
  float yaw_ros;  // yaw角度（ROS）

  float yaw_ros_last;      // 上次yaw角度（ROS）
  float yaw_ros_total;     // yaw累计角度（ROS）
  uint32_t yaw_ros_count;  // yaw角度（ROS）计数
} R_Packet;
extern S_Packet s_packet;
extern R_Packet r_packet;

extern void Send_Vsp();
extern void Receive_Vsp();

extern DT7_DR16_t dt7_dr16;
extern PS2_t PS2;
extern Flag_t flag;
extern void (*USART_Callback[9])();

void DT7_DR16_Handle();
void PS2_Handle();
void CDC_Receive_Handle(uint8_t * Buf, uint32_t * Len);

#ifdef __cplusplus
}
#endif
#pragma pack(pop)

#endif
