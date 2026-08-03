#ifndef __DM_IMU_H
#define __DM_IMU_H

#include "stm32f4xx_hal.h"

#define ACCEL_CAN_MAX (235.2f)
#define ACCEL_CAN_MIN (-235.2f)
#define GYRO_CAN_MAX (34.88f)
#define GYRO_CAN_MIN (-34.88f)
#define PITCH_CAN_MAX (90.0f)
#define PITCH_CAN_MIN (-90.0f)
#define ROLL_CAN_MAX (180.0f)
#define ROLL_CAN_MIN (-180.0f)
#define YAW_CAN_MAX (180.0f)
#define YAW_CAN_MIN (-180.0f)
#define TEMP_MIN (0.0f)
#define TEMP_MAX (60.0f)
#define Quaternion_MIN (-1.0f)
#define Quaternion_MAX (1.0f)

#define CMD_READ 0
#define CMD_WRITE 1

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  COM_USB = 0,
  COM_RS485,
  COM_VOFA

} imu_com_port_e;

typedef enum
{
  REBOOT_IMU = 0,
  ACCEL_DATA,
  GYRO_DATA,
  EULER_DATA,
  QUAT_DATA,
  SET_ZERO,
  ACCEL_CALI,
  GYRO_CALI,
  MAG_CALI,
  CHANGE_COM,
  SET_DELAY,
  CHANGE_ACTIVE,
  SET_BAUD,
  SET_MST_ID,
  DATA_OUTPUT_SELECTION,
  SAVE_PARAM = 254,
  RESTORE_SETTING = 255
} reg_id_e;

typedef struct
{
  uint8_t mst_id;

  float pitch;
  float roll;
  float yaw;
  float yaw_total;  //累计的yaw角，单位为度
  float yaw_last;

  float gyro[3];
  float accel[3];

  float q[4];

  float cur_temp;

  int16_t yaw_cirnum;
  uint16_t imu_online;  //离线计数
} imu_t;

extern imu_t imu;

void imu_init(uint8_t mst_id);
// TODO: 以下函数依赖 CAN 通讯，CAN 已移除，需要替换通讯方式
// void imu_write_reg(uint8_t reg_id, uint32_t data);
// void imu_read_reg(uint8_t reg_id);
// void imu_reboot();
// void imu_accel_calibration();
// void imu_gyro_calibration();
// void imu_change_com_port(imu_com_port_e port);
// void imu_set_active_mode_delay(uint32_t delay);
// void imu_change_to_active();
// void imu_change_to_request();
// void imu_set_baud(imu_baudrate_e baud);
// void imu_set_mst_id(uint8_t mst_id);
// void imu_save_parameters();
// void imu_restore_settings();
// void imu_request_accel();
// void imu_request_gyro();
// void imu_request_euler();
// void imu_request_quat();
void IMU_UpdateData(uint8_t * pData);

#ifdef __cplusplus
}
#endif

#endif