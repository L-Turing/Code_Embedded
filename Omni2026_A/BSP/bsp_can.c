#include "bsp_can.h"

#include "DJIMotor.h"
#include "Peripheral.h"
#include "can.h"
#include "imu.h"
#include "string.h"

/**
 * @brief Initializes CAN2 peripheral and configures its filter.
 * This function sets up the CAN2 peripheral with a specific filter configuration
 * to receive messages from the CAN bus.
 */
void Can2_Init(void)
{
  CAN_FilterTypeDef can_filter_st;
  can_filter_st.FilterActivation = ENABLE;
  can_filter_st.FilterMode = CAN_FILTERMODE_IDLIST;
  can_filter_st.FilterScale = CAN_FILTERSCALE_16BIT;
  can_filter_st.FilterIdHigh = 0x201 << 5;
  can_filter_st.FilterIdLow = 0x202 << 5;
  can_filter_st.FilterMaskIdHigh = 0x203 << 5;
  can_filter_st.FilterMaskIdLow = 0x204 << 5;
  can_filter_st.FilterBank = 14;
  can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO1;

  HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);

  can_filter_st.FilterIdHigh = 0x123 << 5;
  can_filter_st.FilterIdLow = 0x14 << 5;
  can_filter_st.FilterMaskIdHigh = 0x13 << 5;
  can_filter_st.FilterBank = 15;
  HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);

  HAL_CAN_Start(&hcan2);
  HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);
}

/**
 * @brief Initializes CAN1 peripheral and configures its filter.
 * This function sets up the CAN1 peripheral with a specific filter configuration
 * to receive messages from the CAN bus.
 */
void Can1_Init(void)
{
  CAN_FilterTypeDef can_filter_st;
  can_filter_st.FilterActivation = ENABLE;
  can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
  can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
  can_filter_st.FilterIdHigh = 0x0000;
  can_filter_st.FilterIdLow = 0x0000;
  can_filter_st.FilterMaskIdHigh = 0x0000;
  can_filter_st.FilterMaskIdLow = 0x0000;
  can_filter_st.FilterBank = 0;
  can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
  can_filter_st.SlaveStartFilterBank = 14;
  HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);

  HAL_CAN_Start(&hcan1);
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/**
 * @brief Sends a message to control DJI motors via CAN.
 * This function constructs a CAN message with motor control data and sends it
 * through the specified CAN interface (CAN1 or CAN2).
 *
 * @param which_can The CAN interface to use (1 or 2).
 * @param control_id The control ID for the message.
 * @param motor1 The speed for motor 1.
 * @param motor2 The speed for motor 2.
 * @param motor3 The speed for motor 3.
 * @param motor4 The speed for motor 4.
 */
void CanSend_DJIMotor(
  uint8_t which_can, uint32_t control_id, int16_t motor1, int16_t motor2, int16_t motor3,
  int16_t motor4)
{
  CAN_TxHeaderTypeDef tx_message;
  uint8_t txDATA[8];
  uint32_t send_mail_box;

  tx_message.StdId = control_id;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 8;

  txDATA[0] = motor1 >> 8;
  txDATA[1] = motor1;
  txDATA[2] = motor2 >> 8;
  txDATA[3] = motor2;
  txDATA[4] = motor3 >> 8;
  txDATA[5] = motor3;
  txDATA[6] = motor4 >> 8;
  txDATA[7] = motor4;

  if (which_can == 1) {
    HAL_CAN_AddTxMessage(&hcan1, &tx_message, txDATA, &send_mail_box);
  }
  else if (which_can == 2) {
    HAL_CAN_AddTxMessage(&hcan2, &tx_message, txDATA, &send_mail_box);
  }
}

/**
 * @brief Sends a message with control data to the specified CAN interface.
 * This function constructs a CAN message with two 16-bit data fields and one float data field,
 * and sends it through the specified CAN interface (CAN1 or CAN2).
 *
 * @param which_can The CAN interface to use (1 or 2).
 * @param control_id The control ID for the message.
 * @param data1 The first 16-bit data value.
 * @param data2 The second 16-bit data value.
 * @param data3 The float data value.
 */
void CanSend_Message(
  uint8_t which_can, uint32_t control_id, uint16_t data1, uint16_t data2, float data3)
{
  CAN_TxHeaderTypeDef tx_message;
  uint8_t txDATA[8];
  uint32_t send_mail_box;

  tx_message.StdId = control_id;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 8;

  memcpy(txDATA, &data1, sizeof(uint16_t));
  memcpy(txDATA + 2, &data2, sizeof(uint16_t));
  memcpy(txDATA + 4, &data3, sizeof(float));

  if (which_can == 1) {
    HAL_CAN_AddTxMessage(&hcan1, &tx_message, txDATA, &send_mail_box);
  }
  else if (which_can == 2) {
    HAL_CAN_AddTxMessage(&hcan2, &tx_message, txDATA, &send_mail_box);
  }
}

/**
 * @brief Callback function for handling received messages in CAN RX FIFO 0.
 * This function is called when a message is pending in the RX FIFO 0 of CAN1.
 * It retrieves the message and processes it based on its standard ID.
 *
 * @param hcan Pointer to the CAN handle structure.
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
  CAN_RxHeaderTypeDef can1_rxheader;
  uint8_t rxdatafifo0[8];
  HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &can1_rxheader, rxdatafifo0);
  if (hcan == &hcan1) {
    switch (can1_rxheader.StdId) {
      case 0x209:
        GM6020_motor.motor_types = GM6020;
        GM6020_motor.motor_stdid = can1_rxheader.StdId;
        Get_Info_DJIMotor(&GM6020_motor, rxdatafifo0);
        break;
      case 0x201:
        motor_2006.motor_types = M2006;
        motor_2006.motor_stdid = can1_rxheader.StdId;
        Get_Info_DJIMotor(&motor_2006, rxdatafifo0);
        break;

      default:
        break;
    }
  }
}

/**
 * @brief Callback function for handling received messages in CAN RX FIFO 1.
 * This function is called when a message is pending in the RX FIFO 1 of CAN2.
 * It retrieves the message and processes it based on its standard ID.
 *
 * @param hcan Pointer to the CAN handle structure.
 */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
  CAN_RxHeaderTypeDef can2_rxheader;
  uint8_t rxdatafifo1[8];
  HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &can2_rxheader, rxdatafifo1);
  if (hcan == &hcan2) {
    switch (can2_rxheader.StdId) {
      case 0x201:
      case 0x202:
      case 0x203:
      case 0x204: {
        static uint8_t id = 0;
        id = can2_rxheader.StdId - 0x201;
        M3508_motor[id].motor_types = M3508;
        M3508_motor[id].motor_stdid = can2_rxheader.StdId;
        Get_Info_DJIMotor(&M3508_motor[id], rxdatafifo1);
        break;
      }
      case 0x13:
        IMU_UpdateData(rxdatafifo1);
        break;
      case 0x14:
        IMU_UpdateData(rxdatafifo1);
        break;

      default:
        break;
    }
  }
}
