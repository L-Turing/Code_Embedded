#include "main_task.h"

#include <iostream>

#include "Motor.h"
#include "DaMiao.h"
#include "FreeRTOS.h"
#include "Referee.h"
#include "bsp_can.h"
#include "bsp_crc.h"
#include "bsp_dwt.h"
#include "bsp_usart.h"
#include "cmsis_os.h"
#include "ins_task.h"
#include "iwdg.h"
#include "string.h"
#include "math.h"
#include "task.h"

DaMiaoMotor_Class motor_yaw(which_cans::can1, motor_types::DaMiao4310, 0X01, 0X10);
DJIMotor_Class motor_pitch(which_cans::can2, motor_types::GM6020, 0X1FE, 0X205);
DJIMotor_Class motor_ammunition(which_cans::can1, motor_types::M2006, 0X200, 0X201);
DJIMotor_Class motor_friwheel_left(which_cans::can2, motor_types::M3508, 0X200, 0X201);
DJIMotor_Class motor_friwheel_right(which_cans::can2, motor_types::M3508, 0X200, 0X202);

static void ShootHeat(
  uint16_t shoot_heat_data1, uint16_t shoot_heat_data2, uint16_t shoot_cooling,
  uint16_t shoot_heat_limit, float& shoot_freq);
static void ShootPart();
static void PTZPart();
static void PTZPart_Auto();
static void PTZPart_Disable();

/*
  L0:0x123 遥控器
*/
uint8_t array_send[6][8] = { 0 };
uint8_t array_receive[6][8] = { 0 };
void StartRemote(void const* argument)
{
  (void)argument;
  for (;;) {
    Remote_CallbackHandle();  //DT7:约70Hz  图传约30Hz
    Receive_Vision();  //来自上位机
    osDelay(1);

    memcpy(&array_send[0][0], &RC_Ctl.ch3, 2);
    memcpy(&array_send[0][2], &RC_Ctl.ch2, 2);
    memcpy(&array_send[0][4], &RC_Ctl.ch4, 2);
    memcpy(&array_send[0][6], &RC_Ctl.s1, 1);
    memcpy(&array_send[0][7], &RC_Ctl.s2, 1);

    CanSendBoard(1, 0x123, array_send[0]);
    osDelay(1);

    HAL_IWDG_Refresh(&hiwdg);
  }
}

static uint8_t last_s1 = 0;
static uint8_t last_s2 = 0;
static float shoot_freq_t_dr = 8500.0f;
static float target_pitch = 0.0f, target_yaw = 0.0f, target_angle = 0.0f;
float referee_shoot_speed = 22.0f;
uint16_t referee_shoot_heat1, referee_shoot_heat2, referee_shoot_cooling, referee_shoot_heat_limit, referee_color;
void StartGimbal(void const* argument)
{
  (void)argument;
  for (;;) {
    if (RC_Ctl.s1 == 1 || RC_Ctl.s1 == 3) {

      //ShootPart();
      PTZPart();
    }
    else {
      PTZPart_Disable();
    }

    //电机中断反馈处理
    motor_yaw.DaMiaoFeedback(data_buffer[0]);
    motor_ammunition.DJIFeedback(data_buffer[1]);
    motor_pitch.DJIFeedback(data_buffer[8]);
    motor_friwheel_left.DJIFeedback(data_buffer[9]);
    motor_friwheel_right.DJIFeedback(data_buffer[10]);

    //电机数据信息更新
    motor_yaw.Update_Info();
    motor_pitch.Update_Info();
    motor_ammunition.Update_Info();

    //掉线提示
    if (motor_yaw.flag_connect > 300) motor_yaw.flag_connect = 300;
    if (motor_ammunition.flag_connect > 300) motor_ammunition.flag_connect = 300;
    if (motor_pitch.flag_connect > 300) motor_pitch.flag_connect = 300;
    if (motor_friwheel_left.flag_connect > 300) motor_friwheel_left.flag_connect = 300;
    if (motor_friwheel_right.flag_connect > 300) motor_friwheel_right.flag_connect = 300;
    if (motor_yaw.flag_connect == 300 || motor_ammunition.flag_connect == 300 || motor_pitch.flag_connect == 300 ||
      motor_friwheel_left.flag_connect == 300 || motor_friwheel_right.flag_connect == 300) {

    }

    osDelay(1);
    HAL_IWDG_Refresh(&hiwdg);
  }
}

void StartIMU(void const* argument)
{
  (void)argument;
  INS_Init();

  for (;;) {
    INS_Task();
    osDelay(2);

    HAL_IWDG_Refresh(&hiwdg);
  }
}

/*------------------------------------------------*/

//热量控制
static void ShootHeat(
  uint16_t shoot_heat_data1, uint16_t shoot_heat_data2, uint16_t shoot_cooling,
  uint16_t shoot_heat_limit, float& shoot_freq)
{
  uint16_t heat_remain = shoot_heat_limit - shoot_heat_data1;
  if (heat_remain >= 100) {
    shoot_freq = 8500;
  }
  else if ((heat_remain > 40) && (heat_remain < 100)) {
    float shoot_frequency =
      (15 * (40 - heat_remain) + (float)(shoot_cooling) / 10.0f * (heat_remain - 100)) /
      (40 - 100);
    if (shoot_frequency > 18) shoot_frequency = 18.0f;
    shoot_freq = shoot_frequency / 12 * 60 * 108;
  }
  else if ((heat_remain > 20) && (heat_remain <= 40)) {
    float shoot_frequency = (float)(shoot_cooling) / 10.0f;
    if (shoot_frequency > 18) shoot_frequency = 18.0f;
    shoot_freq = shoot_frequency / 12 * 60 * 108;
  }
  else if (heat_remain <= 20) {
    shoot_freq = 0;
  }
}

static void ShootPart()
{
  static int8_t motor_ammunition_flag_init = 0; //初始化部分
  if (!motor_ammunition_flag_init) {
    while (!motor_ammunition.flag_connect) { osDelay(10); }
    motor_ammunition_flag_init = 1;
    target_angle = motor_ammunition.accumlate_rad_out * 57.2958f;
  }

  if (RC_Ctl.s2 != 2) {
    //摩擦轮
    motor_friwheel_left.SetFriWheel(-6400.0f);
    motor_friwheel_right.SetFriWheel(+6400.0f);

    //单发
    if (RC_Ctl.s1 == 3 && motor_ammunition.pid_speed->Locked_Judge == 0) {
      if (last_s1 == 1) target_angle = motor_ammunition.accumlate_rad_out * 57.2958f;
      if ((RC_Ctl.s2 == 1) && (last_s2 == 3)) target_angle += 36.0f * 1.5f;
      motor_ammunition.SetAmmunition(target_angle, motor_control_modes::Position_Mode);
    }

    //连发  && (recepakge.boolpackage.can_shoot == 1)
    if (RC_Ctl.s1 == 1 && motor_ammunition.pid_speed->Locked_Judge == 0) {
      if (RC_Ctl.s2 == 1) {
        ShootHeat(referee_shoot_heat1, referee_shoot_heat2, referee_shoot_cooling, referee_shoot_heat_limit, shoot_freq_t_dr);
        //motor_ammunition.SetAmmunition(shoot_freq_t_dr, motor_control_modes::Speed_Mode);
        motor_ammunition.SetAmmunition(6000, motor_control_modes::Speed_Mode);
      }
      if (RC_Ctl.s2 == 3) motor_ammunition.SetAmmunition(0, motor_control_modes::Speed_Mode);
    }

    //堵转保护，电机反转
    if (motor_ammunition.pid_speed->Locked_Judge == 1)
      motor_ammunition.SetAmmunition(-1000, motor_control_modes::Speed_Mode);

  }
  else {
    target_angle = motor_ammunition.accumlate_rad_out * 57.2958f;  //立刻停止在当前角度
    motor_ammunition.SetAmmunition(0.0f, motor_control_modes::Speed_Mode);
    motor_friwheel_left.SetFriWheel(0.0f);
    motor_friwheel_right.SetFriWheel(0.0f);
  }

  last_s1 = RC_Ctl.s1;
  last_s2 = RC_Ctl.s2;
  CanSend(1, DJI, motor_ammunition.motor_send_id, motor_ammunition.output, 0, 0, 0);
  CanSend(2, DJI, 0x200, motor_friwheel_left.output, motor_friwheel_right.output, 0, 0);
  osDelay(1);
}

static void PTZPart()
{
  motor_yaw.Enable_Damiao_Motor();
  osDelay(1);
  float last_yaw = INS.Yaw;
  float t_angle = recepakge.yaw * 57.29578f - last_yaw;
  if (recepakge.state) {
    if (t_angle > 180) {
      target_yaw = t_angle - 360.0f + INS.YawTotalAngle;
    }
    else if (t_angle < -180) {
      target_yaw = t_angle + 360.0f + INS.YawTotalAngle;
    }
    else {
      target_yaw = t_angle + INS.YawTotalAngle;
    }
    target_pitch = -recepakge.pitch * 57.29578f;
  }
  else {
    if (fabs(RC_Ctl.ch1) > 10) target_pitch += RC_Ctl.ch1 / 1200.0f;
    if (fabs(RC_Ctl.ch0) > 10) target_yaw -= RC_Ctl.ch0 / 1200.0f;

    // if (RC_Ctl.ch1 > 200) {
    //   target_pitch = 20;
    // }
    // else if (RC_Ctl.ch1 < -200) {
    //   target_pitch = -20; 
    // }
    // else {
    //   target_pitch = 0;
    // }
  }
  recepakge.state = 0;

  if (target_pitch > 40) target_pitch = 40;
  if (target_pitch < -22) target_pitch = -22;
  motor_pitch.SetPitch(target_pitch);
  motor_yaw.SetYaw(target_yaw);

  motor_yaw.Drive_Damiao_Motor_MIT(0, 0, 0, 0, motor_yaw.output);
  CanSend(2, DJI, motor_pitch.motor_send_id, (int16_t)(motor_pitch.output), 0, 0, 0);
  osDelay(1);
}



static void PTZPart_Auto()
{
  static int8_t p_f = 1, y_f = 1;

  motor_yaw.Enable_Damiao_Motor();
  osDelay(1);
  static float last_yaw = INS.Yaw;
  float t_angle = recepakge.yaw * 57.29578f - last_yaw;
  //float t_angle = recepakge.yaw * 57.29578f;
  if (recepakge.state) {
    if (t_angle > 180) {
      target_yaw = t_angle - 360.0f + INS.YawTotalAngle;
    }
    else if (t_angle < -180) {
      target_yaw = t_angle + 360.0f + INS.YawTotalAngle;
    }
    else {
      target_yaw = t_angle + INS.YawTotalAngle;
    }
    target_pitch = -recepakge.pitch * 57.29578f;
  }
  else {
    if (INS.Roll > 10) {
      p_f = -1;
    }
    else if (INS.Roll < -15) {
      p_f = 1;
    }

    if (p_f == -1) {
      target_pitch -= 450.0f / 1200.0f;
    }
    else if (p_f == 1) {
      target_pitch += 450.0f / 1200.0f;
    }

    if (INS.YawTotalAngle > 720.0f) {
      y_f = -1;
    }
    else if (INS.YawTotalAngle < -720.0f) {
      y_f = 1;
    }

    if (y_f == -1) {
      target_yaw -= 450.0f / 1200.0f;
    }
    else if (y_f == 1) {
      target_yaw += 450.0f / 1200.0f;
    }
  }
  recepakge.state = 0;

  if (target_pitch > 40) target_pitch = 40;
  if (target_pitch < -22) target_pitch = -22;
  motor_pitch.SetPitch(target_pitch);
  motor_yaw.SetYaw(target_yaw);

  motor_yaw.Drive_Damiao_Motor_MIT(0, 0, 0, 0, motor_yaw.output);
  CanSend(2, DJI, motor_pitch.motor_send_id, (int16_t)(motor_pitch.output), 0, 0, 0);
  osDelay(1);
}

static void PTZPart_Disable()
{
  target_angle = motor_ammunition.accumlate_rad_out * 57.2958f;  //为下一次2006拨弹准备
  target_pitch = INS.Roll;
  target_yaw = INS.YawTotalAngle;
  motor_pitch.output = 0.0f;
  motor_yaw.output = 0.0f;
  motor_friwheel_left.SetFriWheel(0.0f);
  motor_friwheel_right.SetFriWheel(0.0f);
  motor_ammunition.SetAmmunition(0.0f, motor_control_modes::Speed_Mode);

  motor_yaw.Disable_Damiao_Motor();
  osDelay(1);
  CanSend(1, DJI, 0x1FE, 0, 0, 0, 0);
  osDelay(1);
  CanSend(1, DJI, motor_ammunition.motor_send_id, 0, 0, 0, 0);
  osDelay(1);
  CanSend(2, DJI, 0x200, (int16_t)(motor_friwheel_left.output), (int16_t)(motor_friwheel_right.output), 0, 0);
  osDelay(1);
  recepakge.state = 0;
  recepakge.can_shoot = 0;
}
