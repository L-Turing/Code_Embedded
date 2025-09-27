#include "DJIMotor.h"

#include "PID.h"
#include "Peripheral.h"
#include "Power.h"
#include "arm_math.h"
#include "bsp_can.h"

Motor M3508_motor[4];      //M3508电机数组
Motor GM6020_motor = {0};  //GM6020电机
Motor motor_2006 = {0};    //2006电机

/**

 * @brief 控制底盘电机
 * @param which_can CAN总线编号
 * @param target 目标速度数组
 *
 * 该函数用于根据PID控制器计算出的目标速度，向指定的CAN总线上发送电机控制指令。
 */
void Chassis_Motor(uint8_t which_can, float * target)
{
  float motor_current[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  for (uint8_t i = 0; i < 4; i++)
    motor_current[i] = PID_Calc(M3508_motor[i].pid_speed, M3508_motor[i].speed_rpm_out, target[i]);

  Chassis_Power_Control(motor_current);
  CanSend_DJIMotor(
    which_can, 0x200, (int16_t)(motor_current[0]), (int16_t)(motor_current[1]),
    (int16_t)(motor_current[2]), (int16_t)(motor_current[3]));
}

/**
 * @brief 控制GM6020电机
 * @param mode 控制模式（速度模式或位置模式）
 * @param target 目标值（速度或位置）
 *
 * 该函数根据指定的控制模式和目标值，计算GM6020电机的PID输出，并发送控制指令。
 */
void Drive_GM6020_Motor(type_motor_mode mode, float target)
{
  if (mode == Speed_Mode) {
    GM6020_motor.pid_speed->output_val =
      PID_Calc(GM6020_motor.pid_speed, GM6020_motor.speed_rpm_out, target);
  }
  else if (mode == Position_Mode) {
    if (fabsf(target) > 180.0) {
      target = fmodf(target, 180.0f);
    }

    if (target - GM6020_motor.ecd_angle_out < -180.0f) {
      target += 360.0f;
    }
    else if (target - GM6020_motor.ecd_angle_out > +180.0f) {
      target -= 360.0f;
    }

    PID_Calc(GM6020_motor.pid_position, GM6020_motor.ecd_angle_out, target);
    PID_Calc(
      GM6020_motor.pid_speed, GM6020_motor.speed_rpm_out, GM6020_motor.pid_position->output_val);
  }
  CanSend_DJIMotor(2, 0x2FE, (int16_t)GM6020_motor.pid_speed->output_val, 0, 0, 0);
}

float m2006_out = 0.0f;
void Set2006(float m2006_set, type_motor_mode mode)
{
  if (mode == Position_Mode) {
    PID_Calc(motor_2006.pid_position, motor_2006.accumlate_rad_out * 57.29583f, m2006_set);
    PID_Calc(motor_2006.pid_speed, motor_2006.speed_rad_rotor, motor_2006.pid_position->output_val);
  }
  if (mode == Speed_Mode) {
    PID_Calc(motor_2006.pid_speed, motor_2006.speed_rad_rotor, m2006_set);
  }
  m2006_out = (int16_t)(motor_2006.pid_speed->output_val);
  CanSend_DJIMotor(1, 0x200, m2006_out, 0, 0, 0);
}

/**
 * @brief 更新电机信息 
 * @param motor 电机数组
 * @param array_length 数组长度
 *
 * 该函数用于更新电机的累计角度和转子圈数等信息。
 */
void Update_Info_DJIMotor(Motor * motor, uint8_t array_length)
{
  for (uint8_t len = 0; len < array_length; len++) {
    Motor * motor_temp = &motor[len];  //数组结构体

    //转子过圈判断
    if (motor_temp->ecd - motor_temp->last_ecd < -4096) {
      motor_temp->circle_number_rotor++;
    }
    else if (motor_temp->ecd - motor_temp->last_ecd > 4096) {
      motor_temp->circle_number_rotor--;
    }

    //转子累计角度 rad
    motor_temp->accumlate_rad_rotor =
      (float)((motor_temp->circle_number_rotor) * 8192 + motor_temp->last_ecd) / 8192.0f * 360.0f /
      57.296f;
    //转子绝对角度 °
    motor_temp->ecd_angle_rotor = (float)(motor_temp->ecd) / 8192.0f * 360.0f - 180.0f;
    //转子绝对弧度 rad
    motor_temp->ecd_rad_rotor = motor_temp->ecd_angle_rotor / 180.0f * PI;

    //减速比
    switch (motor_temp->motor_types) {
      case M3508:
        motor_temp->reduction_ratio = 19.0f;
        break;
      case M2006:
        motor_temp->reduction_ratio = 36.0f;
        break;
      case GM6020:
        motor_temp->reduction_ratio = 1.0f;
        break;

      default:
        motor_temp->reduction_ratio = 1.0f;
        break;
    }

    //输出轴转速 rpm
    motor_temp->speed_rpm_out = (float)(motor_temp->speed_rpm_rotor) / motor_temp->reduction_ratio;
    //输出轴转速 rad/s
    motor_temp->speed_rad_out = motor_temp->speed_rad_rotor / motor_temp->reduction_ratio;

    //输出轴累计角度 rad
    motor_temp->accumlate_rad_out = motor_temp->accumlate_rad_rotor / motor_temp->reduction_ratio;
    //输出轴绝对角度 °
    motor_temp->ecd_angle_out = motor_temp->ecd_angle_rotor / motor_temp->reduction_ratio;
    //输出轴绝对弧度 rad
    motor_temp->ecd_rad_out = motor_temp->ecd_rad_rotor / motor_temp->reduction_ratio;
  }
}

/**
 * @brief 获取电机信息
 * @param motor 电机结构体指针
 * @param canbuf_receive 接收的CAN数据缓冲区
 *
 * 该函数用于从接收到的CAN数据中提取电机的当前编码器值、速度、反馈电流和温度等信息。
 */
void Get_Info_DJIMotor(Motor * motor, uint8_t * canbuf_receive)
{
  motor->last_ecd = motor->ecd;
  motor->ecd = (int16_t)((canbuf_receive[0] << 8) | canbuf_receive[1]);
  motor->speed_rpm_rotor = (int16_t)((canbuf_receive[2] << 8) | canbuf_receive[3]);  //转子转速 rpm
  motor->speed_rad_rotor = (float)(motor->speed_rpm_rotor) * 2.0f * PI / 60.0f;  //转子转速 rad/s
  motor->given_current = (int16_t)((canbuf_receive[4] << 8) | canbuf_receive[5]);
  motor->temperate = (int8_t)(canbuf_receive[6]);
}
