#include "DJIMotor.h"

#include "PID.h"
#include "Peripheral.h"
#include "Power.h"
#include "arm_math.h"
#include "bsp_can.h"

Motor M3508_motor[4];  //M3508电机数组

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
    motor_current[i] = PID_Calc(&M3508_pid[i], M3508_motor[i].speed_rpm_out, target[i]);

  Chassis_Power_Control(motor_current);
  CanSend_DJIMotor(
    which_can, 0x200, (int16_t)(motor_current[0]), (int16_t)(motor_current[1]),
    (int16_t)(motor_current[2]), (int16_t)(motor_current[3]));
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

    if (motor_temp->ecd - motor_temp->last_ecd < -4096) {  //转子过圈判断
      motor_temp->circle_number_rotor++;
    }
    else if (motor_temp->ecd - motor_temp->last_ecd > 4096) {
      motor_temp->circle_number_rotor--;
    }

    motor_temp->accumlate_angle_rotor =
      (float)((motor_temp->circle_number_rotor) * 8192 + motor_temp->last_ecd) / 8192.0f * 360.0f /
      57.296f;  //转子累计角度 rad

    switch (motor_temp->motor_types) {  //减速比
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

    motor_temp->speed_rad_out =
      motor_temp->speed_rad_rotor / motor_temp->reduction_ratio;  //输出轴转速 rad/s
    motor_temp->speed_rpm_out =
      (float)(motor_temp->speed_rpm_rotor) / motor_temp->reduction_ratio;  //输出轴转速 rpm
    motor_temp->accumlate_angle_out =
      motor_temp->accumlate_angle_rotor / motor_temp->reduction_ratio;  //输出轴累计角度 rad
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
  motor->speed_rpm_rotor = (int16_t)((canbuf_receive[2] << 8) | canbuf_receive[3]);
  motor->speed_rad_rotor = (float)(motor->speed_rpm_rotor) * 2.0f * PI / 60.0f;  //转子转速 rad/s
  motor->given_current = (int16_t)((canbuf_receive[4] << 8) | canbuf_receive[5]);
  motor->temperate = (int8_t)(canbuf_receive[6]);
}
