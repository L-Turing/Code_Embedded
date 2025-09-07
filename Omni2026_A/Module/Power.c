#include "Power.h"

#include "DJIMotor.h"
#include "PID.h"
#include "Referee.h"
#include "arm_math.h"
#include "bsp_can.h"
#include "main.h"
#include "main_task.h"
#include "math.h"

CAP cap = {0, 0, 0, 0};

//M3508 输出轴转速为反馈时
static float Power_K0_3508_out = 0.01562123f;
static float Power_K1_3508_out = 0.00001324f;
static float Power_K2_3508_out = 0.08254390f;
static float Power_Constant_3508 = 1.3544f;
static float Current_To_Out_3508 = 16384.0f / 20.0f;

//GM6020 输出轴转速为反馈时
// static float Power_K0_6020 = 0.8130f;
// static float Power_K1_6020 = -0.0005f;
// static float Power_K2_6020 = 6.0021f;
// static float Power_Constant_6020 = 1.3715f;
// static float Current_To_Out_6020 = 16384.0f / 3.0f;

static void Get_Chassis_Referee(uint16_t * chassis_power_limit, uint16_t * buffer_energy);
static void Predicted_Power(
  float k0, float k1, float k2, float constant, float * current_t, Motor * motor);
static void Sum_Power(
  float * pred_total_power, float * ept_total_power, float * wheel_factor_t, Motor * motor);
static void Scale_Power(
  float k0, float k1, float k2, float constant, float wheel_factor_t, float current_to_out,
  float * current_t, Motor * motor);

/**
 * @brief 控制底盘功率
 * @param current 当前电流数组
 *
 * 该函数根据底盘功率限制和缓冲能量，计算每个电机的电流，以满足功率控制要求。
 */
void Chassis_Power_Control(float * current)
{
  uint16_t buffer_energy = 0;
  uint16_t chassis_power_limit = 0;
  Get_Chassis_Referee(&chassis_power_limit, &buffer_energy);
  chassis_power_limit = 35;  //测试用

  float predicted_total_power = 0.0f;
  float expected_total_power = chassis_power_limit - 3;
  float wheel_factor = 1.0f;

  //与超级电容通信
  if (!robot_status.power_management_chassis_output) { 
    CanSend_Message(2, 0x112, expected_total_power, 1, 0);
  }  //阵亡
  else if (robot_status.power_management_chassis_output) { 
    CanSend_Message(2, 0x112, expected_total_power, 0, 0);
  }  //存活

  Predicted_Power(
    Power_K0_3508_out, Power_K1_3508_out, Power_K2_3508_out, Power_Constant_3508, current,
    M3508_motor);

  Sum_Power(&predicted_total_power, &expected_total_power, &wheel_factor, M3508_motor);

  Scale_Power(
    Power_K0_3508_out, Power_K1_3508_out, Power_K2_3508_out, Power_Constant_3508, wheel_factor,
    Current_To_Out_3508, current, M3508_motor);
}

/**
 * @brief 获取底盘功率限制和缓冲能量
 * @param chassis_power_limit 底盘功率限制
 * @param buffer_energy 缓冲能量
 *
 * 该函数从机器人状态和功率热数据中获取底盘功率限制和缓冲能量。
 */
static void Get_Chassis_Referee(uint16_t * chassis_power_limit, uint16_t * buffer_energy)
{
  *chassis_power_limit = robot_status.chassis_power_limit;
  *buffer_energy = power_heat_data.buffer_energy;
}

/**
 * @brief 预测功率
 * @param k0, k1, k2, constant 预测功率的系数
 * @param current_t 当前电流数组
 * @param motor 电机数组
 *
 * 该函数根据当前电流和电机转速计算每个电机的预测功率。
 */
static void Predicted_Power(
  float k0, float k1, float k2, float constant, float * current_t, Motor * motor)
{
  for (uint8_t i = 0; i < 4; i++) {
    motor[i].predicted_power =
      k0 * (current_t[i] / 16384 * 20.0f) * motor[i].speed_rad_rotor +
      k1 * motor[i].speed_rad_rotor * motor[i].speed_rad_rotor +
      k2 * (current_t[i] / 16384 * 20.0f) * (current_t[i] / 16384 * 20.0f) + constant;
  }
}

/**
 * @brief 计算总功率和轮子因子
 * @param pred_total_power 预测总功率
 * @param ept_total_power 期望总功率
 * @param wheel_factor_t 轮子因子
 * @param motor 电机数组
 *
 * 该函数计算所有电机的预测功率和期望功率，并根据它们的关系计算轮子因子。
 */
static void Sum_Power(
  float * pred_total_power, float * ept_total_power, float * wheel_factor_t, Motor * motor)
{
  for (uint8_t i = 0; i < 4; i++) {
    if (motor[i].predicted_power > 0.0f) {
      *pred_total_power += motor[i].predicted_power;
    }
    else {
      *ept_total_power += -motor[i].predicted_power;
    }
  }

  if (*pred_total_power < *ept_total_power) {
    *wheel_factor_t = 1.0f;
  }
  else {
    *wheel_factor_t = *ept_total_power / *pred_total_power;
  }
}

/**
 * @brief 缩放功率
 * @param k0, k1, k2, constant 功率缩放的系数
 * @param wheel_factor_t 轮子因子
 * @param current_to_out 电流到输出的转换系数
 * @param current_t 当前电流数组
 * @param motor 电机数组
 *
 * 该函数根据轮子因子和预测功率调整每个电机的电流，以满足功率限制。
 */
static void Scale_Power(
  float k0, float k1, float k2, float constant, float wheel_factor_t, float current_to_out,
  float * current_t, Motor * motor)
{
  for (uint8_t i = 0; i < 4; i++) {
    if (motor[i].predicted_power > 0) {
      motor[i].scale_power = wheel_factor_t;
    }
    else {
      motor[i].scale_power = 1.0f;
    }

    if (motor[i].predicted_power > 0) {
      if (motor[i].scale_power > 0.99999f) {
        //无需功率控制
      }
      else {
        float a = k2;
        float b = k0 * motor[i].speed_rad_rotor;
        float c = k1 * motor[i].speed_rad_rotor * motor[i].speed_rad_rotor -
                  motor[i].scale_power * motor[i].predicted_power + constant;
        float delta = 0.0f, h = 0.0f;
        delta = b * b - 4 * a * c;
        if (delta < 0.0f) {
          // 无解
          current_t[i] = 0.0f;
        }
        else {
          arm_sqrt_f32(delta, &h);
          float result_1 = 0.0f, result_2 = 0.0f;
          result_1 = (-b + h) / (2.0f * a);
          result_2 = (-b - h) / (2.0f * a);

          if ((result_1 > 0.0f && result_2 < 0.0f) || (result_1 < 0.0f && result_2 > 0.0f)) {
            if (
              (current_t[i] > 0.0f && result_1 > 0.0f) ||
              (current_t[i] < 0.0f && result_1 < 0.0f)) {
              current_t[i] = result_1 * current_to_out;
            }
            else {
              current_t[i] = result_2 * current_to_out;
            }
          }
          else {
            if (fabs(result_1) < fabs(result_2)) {
              current_t[i] = result_1 * current_to_out;
            }
            else {
              current_t[i] = result_2 * current_to_out;
            }
          }
        }
      }
    }
  }
}
