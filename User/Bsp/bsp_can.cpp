#include "bsp_can.h"

#include <iostream>

#include "can.h"
#include "main.h"
#include "main_task.h"
#include "string.h"
#include "Motor.h"

uint8_t data_buffer[16][8] = { 0 };
/*
  L0~L7：can1   L8~L15:can2
    0:motor_yaw
    1:motor_ammunition

    8:motor_pitch
    9:motor_friwheel_left
    10:motor_friwheel_right
*/

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)  //CAN1
{
  CAN_RxHeaderTypeDef can_rxheader1;
  uint8_t rxDATA1[8];
  HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &can_rxheader1, rxDATA1);

  if (hcan == &hcan1) {//yaw 2006 板间通信
    if (motor_yaw.motor_feedback_id == can_rxheader1.StdId) {
      memcpy(data_buffer[0], rxDATA1, 8);
      motor_yaw.flag_connect = 1;
    }
    else if (motor_ammunition.motor_feedback_id == can_rxheader1.StdId) {
      memcpy(data_buffer[1], rxDATA1, 8);
      motor_ammunition.flag_connect = 1;
    }
  }
}


void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan)  //CAN2
{
  CAN_RxHeaderTypeDef can_rxheader2;
  uint8_t rxDATA2[8];
  HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &can_rxheader2, rxDATA2);

  if (hcan == &hcan2) { //pitch fri1 fri2
    if (motor_pitch.motor_feedback_id == can_rxheader2.StdId) {
      memcpy(data_buffer[8], rxDATA2, 8);
      motor_pitch.flag_connect = 1;
    }
    else if (motor_friwheel_left.motor_feedback_id == can_rxheader2.StdId) {
      memcpy(data_buffer[9], rxDATA2, 8);
      motor_friwheel_left.flag_connect = 1;
    }
    else if (motor_friwheel_right.motor_feedback_id == can_rxheader2.StdId) {
      memcpy(data_buffer[10], rxDATA2, 8);
      motor_friwheel_right.flag_connect = 1;
    }
  }

}

void Can1_Init(void)
{
  CAN_FilterTypeDef can_filter_st;
  can_filter_st.FilterActivation = ENABLE;
  can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
  can_filter_st.FilterScale = CAN_FILTERSCALE_16BIT;
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

void Can2_Init(void)
{
  CAN_FilterTypeDef can_filter_st;
  can_filter_st.FilterActivation = ENABLE;
  can_filter_st.FilterMode = CAN_FILTERMODE_IDLIST;
  can_filter_st.FilterScale = CAN_FILTERSCALE_16BIT;
  can_filter_st.FilterIdHigh = 0x201 << 5;
  can_filter_st.FilterIdLow = 0x202 << 5;
  can_filter_st.FilterMaskIdHigh = 0x205 << 5;
  can_filter_st.FilterMaskIdLow = 0x0000;
  can_filter_st.FilterBank = 14;
  can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO1;
  can_filter_st.SlaveStartFilterBank = 14;
  HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);

  HAL_CAN_Start(&hcan2);
  HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);
}

void CanSend(
  uint8_t which_can, Motor_Type_param type, uint32_t control_id, int16_t motor1, int16_t motor2,
  int16_t motor3, int16_t motor4)
{
  CAN_TxHeaderTypeDef tx_message;
  uint8_t txDATA[8];
  uint32_t send_mail_box;

  tx_message.StdId = control_id;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 8;

  if (type == DJI) {
    txDATA[0] = motor1 >> 8;
    txDATA[1] = motor1;
    txDATA[2] = motor2 >> 8;
    txDATA[3] = motor2;
    txDATA[4] = motor3 >> 8;
    txDATA[5] = motor3;
    txDATA[6] = motor4 >> 8;
    txDATA[7] = motor4;
  }
  else if (type == DM) {
    txDATA[0] = motor1;
    txDATA[1] = motor1 >> 8;
    txDATA[2] = motor2;
    txDATA[3] = motor2 >> 8;
    txDATA[4] = motor3;
    txDATA[5] = motor3 >> 8;
    txDATA[6] = motor4;
    txDATA[7] = motor4 >> 8;
  }

  if (which_can == 1) {
    HAL_CAN_AddTxMessage(&hcan1, &tx_message, txDATA, &send_mail_box);
  }
  else if (which_can == 2) {
    HAL_CAN_AddTxMessage(&hcan2, &tx_message, txDATA, &send_mail_box);
  }
}

void CanSendBoard(
  uint8_t which_can, uint32_t control_id, uint8_t* data)
{
  CAN_TxHeaderTypeDef tx_message;
  uint8_t txDATA[8];
  uint32_t send_mail_box;

  tx_message.StdId = control_id;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 8;

  memcpy(txDATA, data, 8);

  if (which_can == 1) {
    HAL_CAN_AddTxMessage(&hcan1, &tx_message, txDATA, &send_mail_box);
  }
  else if (which_can == 2) {
    HAL_CAN_AddTxMessage(&hcan2, &tx_message, txDATA, &send_mail_box);
  }
}
