#include "bsp_can.h"

#include "can.h"
#include "main.h"
#include "main_task.h"
#include "string.h"

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
  CAN_RxHeaderTypeDef can_rxheader1;
  uint8_t rxDATA1[8];
  HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &can_rxheader1, rxDATA1);
  if (hcan == &hcan1) {
    memcpy(receive_can_buffer[1], rxDATA1, 8);
  }
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
  CAN_RxHeaderTypeDef can_rxheader2;
  uint8_t rxDATA2[8];
  HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &can_rxheader2, rxDATA2);
  if (hcan == &hcan2) {
    memcpy(receive_can_buffer[2], rxDATA2, 8);
  }
}

void Can_Filter_Init(void)
{
  CAN_FilterTypeDef can_filter_st;
  can_filter_st.FilterActivation = ENABLE;
  can_filter_st.SlaveStartFilterBank = 14;

  can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
  can_filter_st.FilterScale = CAN_FILTERSCALE_16BIT;
  can_filter_st.FilterIdHigh = 0x000;
  can_filter_st.FilterIdLow = 0x000;
  can_filter_st.FilterMaskIdHigh = 0x000;
  can_filter_st.FilterMaskIdLow = 0x000;
  can_filter_st.FilterBank = 0;
  can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
  HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);
  HAL_CAN_Start(&hcan1);
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

  can_filter_st.FilterMode = CAN_FILTERMODE_IDLIST;
  can_filter_st.FilterScale = CAN_FILTERSCALE_16BIT;
  can_filter_st.FilterIdHigh = 0x201 << 5;
  can_filter_st.FilterIdLow = 0x202 << 5;
  can_filter_st.FilterMaskIdHigh = 0x203 << 5;
  can_filter_st.FilterMaskIdLow = 0x204 << 5;
  can_filter_st.FilterBank = 14;
  can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO1;
  HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);
  HAL_CAN_Start(&hcan2);
  HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);
}

void Can_Msg_Send(uint8_t which_can, uint32_t send_id, const uint8_t * data)
{
  CAN_TxHeaderTypeDef tx_message;
  uint8_t txDATA[8];
  uint32_t send_mail_box;

  tx_message.StdId = send_id;
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
