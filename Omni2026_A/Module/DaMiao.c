#include "main.h"
#include "main_task.h"
#include "DaMiao.h"
#include "can.h"

DaMiao_Motor D_yaw;

static float P_MIN = -3.141593f;
static float P_MAX = 3.141593f;
static float V_MIN = -30.0f;
static float V_MAX = 30.0f;
static float KP_MIN = 0.0f;
static float KP_MAX = 500.0f;
static float KD_MIN = 0.0f;
static float KD_MAX = 5.0f;
static float T_MIN = -10.0f;
static float T_MAX = 10.0f;

static float uint_to_float(int x_int, float x_min, float x_max, int bits) {
    float span = x_max - x_min;
    float offset = x_min;
    return ((float) x_int) * span / ((float) ((1 << bits) - 1)) + offset;
}

static int float_to_uint(float x_float, float x_min, float x_max, int bits) {
    float span = x_max - x_min;
    float offset = x_min;
    return (int) ((x_float - offset) * ((float) ((1 << bits) - 1)) / span);
}

void Enable_Damiao_Motor(uint8_t which_can,uint32_t id)
{
  CAN_TxHeaderTypeDef tx_message;
  uint8_t txDATA[8];
  uint32_t send_mail_box;

  tx_message.StdId =id;
  tx_message.IDE  = CAN_ID_STD;
  tx_message.RTR  = CAN_RTR_DATA;
  tx_message.DLC  = 8;

  txDATA[0] = 0xFF;
  txDATA[1] = 0xFF;
  txDATA[2] = 0xFF;
  txDATA[3] = 0xFF;
  txDATA[4] = 0xFF;
  txDATA[5] = 0xFF;
  txDATA[6] = 0xFF;
  txDATA[7] = 0xFC;

  if(which_can==1){
    HAL_CAN_AddTxMessage(&hcan1, &tx_message, txDATA, &send_mail_box);
  }else if(which_can==2){
    HAL_CAN_AddTxMessage(&hcan2, &tx_message, txDATA, &send_mail_box);
  }
}

void Disable_Damiao_Motor(uint8_t which_can,uint32_t id)
{
  CAN_TxHeaderTypeDef tx_message;
  uint8_t txDATA[8];
  uint32_t send_mail_box;

  tx_message.StdId = id;
  tx_message.IDE  = CAN_ID_STD;
  tx_message.RTR  = CAN_RTR_DATA;
  tx_message.DLC  = 8;

  txDATA[0] = 0xFF;
  txDATA[1] = 0xFF;
  txDATA[2] = 0xFF;
  txDATA[3] = 0xFF;
  txDATA[4] = 0xFF;
  txDATA[5] = 0xFF;
  txDATA[6] = 0xFF;
  txDATA[7] = 0xFD;

  if(which_can==1){
    HAL_CAN_AddTxMessage(&hcan1, &tx_message, txDATA, &send_mail_box);
  }else if(which_can==2){
    HAL_CAN_AddTxMessage(&hcan2, &tx_message, txDATA, &send_mail_box);
  }
}

void Drive_Damiao_Motor_MIT(uint8_t which_can,uint32_t control_id,float _pos, float _vel, float _KP, float _KD, float _torq)
{
  uint16_t pos_tmp, vel_tmp, kp_tmp, kd_tmp, tor_tmp;
  pos_tmp = float_to_uint(_pos, P_MIN, P_MAX, 16);//注意所有调用的float_to_uint函数后三个参数均由调参软件上读出
  vel_tmp = float_to_uint(_vel, V_MIN, V_MAX, 12);    //如果更改会导致最后的解码错误
  kp_tmp = float_to_uint(_KP, KP_MIN, KP_MAX, 12);
  kd_tmp = float_to_uint(_KD, KD_MIN, KD_MAX, 12);
  tor_tmp = float_to_uint(_torq, T_MIN, T_MAX, 12);

  CAN_TxHeaderTypeDef tx_message;
  uint8_t txDATA[8];
  uint32_t send_mail_box;

  tx_message.StdId = control_id;
  tx_message.IDE  = CAN_ID_STD;
  tx_message.RTR  = CAN_RTR_DATA;
  tx_message.DLC  = 8;
  
  txDATA[0] = (pos_tmp >> 8);
  txDATA[1] = pos_tmp;
  txDATA[2] = (vel_tmp >> 4);
  txDATA[3] = ((vel_tmp & 0xF) << 4) | (kp_tmp >> 8);
  txDATA[4] = kp_tmp;
  txDATA[5] = (kd_tmp >> 4);
  txDATA[6] = ((kd_tmp & 0xF) << 4) | (tor_tmp >> 8);
  txDATA[7] = tor_tmp;

  if(which_can==1){
    HAL_CAN_AddTxMessage(&hcan1, &tx_message, txDATA, &send_mail_box);
  }else if(which_can==2){
    HAL_CAN_AddTxMessage(&hcan2, &tx_message, txDATA, &send_mail_box);
  }
}

void DaMiao_GetInfo(DaMiao_Motor * D_motor,uint8_t * canbuf_receive) {
    D_motor->state = (canbuf_receive[0]) >> 4;
    //ERR 表示故障，对应故障类型为：
    //8——超压；
    //9——欠压；
    //A——过电流；
    //B——MOS过温；
    //C——电机线圈过温；
    //D——通讯丢失；
    //E——过载；
    D_motor->pos_last = D_motor->pos; 
    D_motor->p_int = (canbuf_receive[1] << 8) | canbuf_receive[2];
    D_motor->v_int = (canbuf_receive[3] << 4) | (canbuf_receive[4] >> 4);
    D_motor->t_int = ((canbuf_receive[4] & 0xF) << 8) | canbuf_receive[5];
    D_motor->pos = uint_to_float(D_motor->p_int, P_MIN, P_MAX, 16); // (-12.5,12.5)
    D_motor->vel = uint_to_float(D_motor->v_int, V_MIN, V_MAX, 12); // (-30.0,30.0)
    D_motor->tor = uint_to_float(D_motor->t_int, T_MIN, T_MAX, 12); // (-10.0,10.0)
    D_motor->Tmos = (float) (canbuf_receive[6]);   //驱动上MOS的平均温度，单位℃
    D_motor->Tcoil = (float) (canbuf_receive[7]);  //电机内部线圈的平均温度，单位℃

    if((D_motor->pos-D_motor->pos_last)>3.141593f){
      D_motor->cirnum--;
    }else if((D_motor->pos-D_motor->pos_last)<-3.141593f){
      D_motor->cirnum++;
    }

    D_motor->accumulate_angle=D_motor->cirnum * 6.283186f + D_motor->pos;
}
