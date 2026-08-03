#ifndef __DJIMOTOR_H
#define __DJIMOTOR_H

#include "PID.h"
#include "Peripheral.h"
#include "stdint.h"

#pragma pack(push, 1)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
enum class motor_types
{
  M3508 = 0,
  M2006 = 1,
  GM6020 = 2,
  DaMiao4310 = 3,
  DaMiao3507 = 4,
};

enum class motor_control_modes
{
  Speed_Mode = 0,
  Position_Mode = 1,
};

class Motor_Class
{
private:
public:
  bool flag_state;                           //电机在线标志
  int16_t flag_connect{};                    //电机连接计数
  motor_types motor_type{};                  // 电机类型
  motor_control_modes motor_control_mode{};  // 电机控制模式
  int8_t temperature{};
  int16_t given_current{};
  int16_t last_ecd{};
  int16_t ecd{};
  int16_t speed_rpm_rotor{};  // 转子转速 rpm
  float speed_rpm_out{};      // 输出轴转速 rpm
  float speed_rad_rotor{};    // 转子转速 rad/s
  float speed_rad_out{};      // 输出轴转速 rad/s

  float accumlate_rad_rotor{};  // 转子累积角度 rad
  float accumlate_rad_out{};    // 输出轴累积角度 rad
  float ecd_rad_rotor{};        // 转子编码器值 rad
  float ecd_rad_out{};          // 输出轴编码器值 rad

  int32_t circle_number_rotor{};  // 转子过圈数
  float reduction_ratio{};        // 减速比

  uint32_t motor_send_id{};      // 电机发送ID
  uint32_t motor_feedback_id{};  // 电机反馈ID
  type_signal sign_mark{};       //电机掉线信号

  float predicted_power{};  // 预测功率
  float expected_power{};   // 期望功率
  float scale_power{};      // 功率比例
  float output{};           //电机输出
  PID * pid_speed{};        // 速度PID指针
  PID * pid_position{};     // 位置PID指针

  explicit Motor_Class(
    motor_types m, uint32_t send_id, uint32_t feedback_id, type_signal sign)
  : motor_type(m),
    motor_send_id(send_id),
    motor_feedback_id(feedback_id),
    sign_mark(sign)
  {
  }  // 电机类型 电机can 电机ID

  bool Lost_Judge();
  void Update_Info();
  void SetWheel(float wheel_tar_rpm);
};

class DJIMotor_Class : public Motor_Class
{
private:
public:
  explicit DJIMotor_Class(
    motor_types m, uint32_t send_id, uint32_t feedback_id, type_signal sign)
  : Motor_Class(m, send_id, feedback_id, sign)
  {
  }  // 电机类型 电机can 电机ID
  void Update_Status(uint8_t * canbuf_receive);
};

class DaMiaoMotor_Class : public Motor_Class
{
private:
  inline static float P_MIN = -3.141593f;
  inline static float P_MAX = 3.141593f;
  inline static float V_MIN = -30.0f;
  inline static float V_MAX = 30.0f;
  inline static float KP_MIN = 0.0f;
  inline static float KP_MAX = 500.0f;
  inline static float KD_MIN = 0.0f;
  inline static float KD_MAX = 5.0f;
  inline static float T_MIN = -10.0f;
  inline static float T_MAX = 10.0f;

public:
  uint16_t state{};
  int p_int{};
  int v_int{};
  int t_int{};
  float pos{};
  float vel{};
  float tor{};
  float Tmos{};
  float Tcoil{};
  float pos_last{};
  int32_t cirnum{};
  float accumulate_angle{};

  explicit DaMiaoMotor_Class(
    motor_types m, uint32_t send_id, uint32_t feedback_id, type_signal sign)
  : Motor_Class(m, send_id, feedback_id, sign)
  {
  }  // 电机类型 电机can 电机ID

  void Enable_Damiao_Motor();
  void Disable_Damiao_Motor();
  void Drive_Damiao_Motor_MIT(float _pos, float _vel, float _KP, float _KD, float _torq);
  void Update_Status(uint8_t * canbuf_receive);
};
#endif

#pragma pack(pop)

#endif
