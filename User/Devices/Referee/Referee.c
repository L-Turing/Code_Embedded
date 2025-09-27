#include "Referee.h"

#include "bsp_crc.h"
#include "bsp_usart.h"
#include "cmsis_os.h"
#include "main.h"
#include "main_task.h"
#include "string.h"
#include "usart.h"

game_status_t game_status;
game_result_t game_result;
game_robot_HP_t game_robot_HP;
event_data_t event_data;
ext_supply_projectile_action_t ext_supply_projectile_action;
referee_warning_t referee_warning;
dart_info_t dart_info;
robot_status_t robot_status;
power_heat_data_t power_heat_data;
robot_pos_t robot_pos;
buff_t buff;
air_support_data_t air_support_data;
hurt_data_t hurt_data;
shoot_data_t shoot_data;
projectile_allowance_t projectile_allowance;
rfid_status_t rfid_status;
dart_client_cmd_t dart_client_cmd;
ground_robot_position_t ground_robot_position;
radar_mark_data_t radar_mark_data;
sentry_info_t sentry_info;
radar_info_t radar_info;
robot_interaction_data_t robot_interaction_data;
map_command_t map_command;
map_robot_data_t map_robot_data;
map_data_t map_data;
custom_info_t custom_info;
custom_robot_data_t custom_robot_data;
remote_data_t remote_data;
custom_client_data_t custom_client_data;
Key key;

xFrameHeader FrameHeader;

void KeyBoard(uint16_t keyboard_value);

void decode(uint8_t * rawData_t)
{
  if (rawData_t == NULL) {
    return;
  }
  if ((rawData_t[0] == 0XA9) && (rawData_t[1] == 0X53)) {
    if (Verify_CRC16_Check_Sum(rawData_t, 21)) {
      memcpy(&remote_data, rawData_t, 21);
      KeyBoard(remote_data.keyboard_value);
    }

    if (*(rawData_t + 21) == 0xA9 && *(rawData_t + 22) == 0x53) {
      // 如果一个数据包出现了多帧数据,则再次调用解析函数,直到所有数据包解析完毕
      decode(rawData_t + 21);
    }
  }
}

uint16_t flag_q, flag_e, flag_r, flag_f, flag_g, flag_z, flag_x, flag_c, flag_b, flag_ctrl,
  flag_shift, flag_v;
int16_t vx_t, vy_t;
uint8_t s1_t, s2_t;
uint8_t last_key_q, last_key_e, last_key_r, last_key_f, last_key_g, last_key_z, last_key_x,
  last_key_c, last_key_v, last_key_b, last_key_ctrl, last_key_shift;

void KeyBoard(uint16_t keyboard_value)
{
  last_key_q = key.q;          //1
  last_key_e = key.e;          //2
  last_key_r = key.r;          //3
  last_key_f = key.f;          //4
  last_key_g = key.g;          //5
  last_key_z = key.z;          //6
  last_key_x = key.x;          //7
  last_key_c = key.c;          //8
  last_key_v = key.v;          //9
  last_key_b = key.b;          //10
  last_key_ctrl = key.ctrl;    //11
  last_key_shift = key.shift;  //12

  key.w = (keyboard_value & 0x0001) != 0;
  key.s = (keyboard_value & 0x0002) != 0;
  key.a = (keyboard_value & 0x0004) != 0;
  key.d = (keyboard_value & 0x0008) != 0;
  key.shift = (keyboard_value & 0x0010) != 0;  //12
  key.ctrl = (keyboard_value & 0x0020) != 0;   //11
  key.q = (keyboard_value & 0x0040) != 0;      //1
  key.e = (keyboard_value & 0x0080) != 0;      //2
  key.r = (keyboard_value & 0x0100) != 0;      //3
  key.f = (keyboard_value & 0x0200) != 0;      //4
  key.g = (keyboard_value & 0x0400) != 0;      //5
  key.z = (keyboard_value & 0x0800) != 0;      //6
  key.x = (keyboard_value & 0x1000) != 0;      //7
  key.c = (keyboard_value & 0x2000) != 0;      //8
  key.v = (keyboard_value & 0x4000) != 0;      //9
  key.b = (keyboard_value & 0x8000) != 0;      //10

  if (key.w) {
    vx_t = +660;
  }
  if (key.s) {
    vx_t = -660;
  }
  if (key.w == 0 && key.s == 0) {
    vx_t = 0;
  }
  if (key.a) {
    vy_t = -660;
  }
  if (key.d) {
    vy_t = +660;
  }
  if (key.a == 0 && key.d == 0) {
    vy_t = 0;
  }

  if (key.shift) {
    flag_shift = 1;
  }  //start SuperCap
  else {
    flag_shift = 0;
  }
  if (key.ctrl) {
    flag_ctrl = 1;
  }  //是否热量开环 默认0:闭环
  else {
    flag_ctrl = 0;
  }
  if (key.v) {
    flag_v = 1;
  }  //start UI
  else {
    flag_v = 0;
  }

  if ((key.z > last_key_z) && (flag_z == 0)) {
    flag_z = 1;
  }  //f_speed +
  else if ((key.x > last_key_x) && (flag_x == 0)) {
    flag_x = 1;
  }  //f_speed -

  if ((key.q > last_key_q) && (flag_q == 0)) {
    flag_q = 1;
  }  //手动退弹

  // if(flag_c==0){s1_t=3;gyro_able=0;roll_wz=0;}//start gyro
  // else {s1_t=1;gyro_able=1;roll_wz=500;}////////////////////////

  if (key.g) {
    flag_g = 1;
  }  //5    IWDG
  else {
    flag_g = 0;
  }

  //

  if ((key.e > last_key_e) && (flag_e == 0)) {
    flag_e = 1;
  }  //2
  else if ((key.e > last_key_e) && (flag_e == 1)) {
    flag_e = 0;
  }

  if ((key.r > last_key_r) && (flag_r == 0)) {
    flag_r = 1;
  }  //3         单发连发模式
  else if ((key.r > last_key_r) && (flag_r == 1)) {
    flag_r = 0;
  }

  if ((key.f > last_key_f) && (flag_f == 0)) {
    flag_f = 1;
  }  //4         开关摩擦轮
  else if ((key.f > last_key_f) && (flag_f == 1)) {
    flag_f = 0;
  }

  if ((key.c > last_key_c) && (flag_c == 0)) {
    flag_c = 1;
  }  //8         开关小陀螺
  else if ((key.c > last_key_c) && (flag_c == 1)) {
    flag_c = 0;
  }

  if ((key.b > last_key_b) && (flag_b == 0)) {
    flag_b = 1;
  }  //10        是否火控 默认0:开启
  else if ((key.b > last_key_b) && (flag_b == 1)) {
    flag_b = 0;
  }
}
