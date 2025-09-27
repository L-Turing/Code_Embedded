#ifndef __Referee_H
#define __Referee_H

#ifdef __cplusplus
extern "C" {
#endif
#pragma once

#include "main.h"

#pragma pack(1)

#define LEN_HEADER 5  // 帧头长
#define LEN_CMDID 2   // 命令码长度
#define LEN_TAIL 2    // 帧尾CRC16

#define JUDGE_FRAME_HEADER 0xA5

// 命令码ID,用来判断接收的是什么数据
#define ID_game_status 0x0001               // 比赛状态数据
#define ID_game_result 0x0002               // 比赛结果数据
#define ID_game_robot_HP 0x0003             // 机器人血量数据
#define ID_event_data 0x0101                // 场地事件数据
#define ID_supply_projectile_action 0x0102  // 补给站动作标识数据
#define ID_referee_warning 0x0104           // 裁判警告数据
#define ID_dart_info 0x0105                 // 飞镖发射相关数据
#define ID_robot_status 0x0201              // 机器人性能体系数据
#define ID_power_heat_data 0x0202           // 实时底盘功率和枪口热量数据
#define ID_robot_pos 0x0203                 // 机器人位置数据
#define ID_buff 0x0204                      // 机器人增益数据
#define ID_air_support_data 0x0205          // 空中支援时间数据
#define ID_hurt_data 0x0206                 // 伤害状态数据
#define ID_shoot_data 0x0207                // 实时射击数据
#define ID_projectile_allowance 0x0208      // 允许发弹量
#define ID_rfid_status 0x0209               // 机器人 RFID 模块状态
#define ID_dart_client_cmd 0x020A           // 飞镖选手端指令数据
#define ID_ground_robot_position 0x020B     // 地面机器人位置数据
#define ID_radar_mark_data 0x020C           // 雷达标记进度数据
#define ID_sentry_info 0x020D               // 哨兵自主决策信息同步
#define ID_radar_info 0x020E                // 雷达自主决策信息同步
#define ID_robot_interaction_data 0x0301    // 机器人交互数据
#define ID_custom_robot_data 0x0302         // 自定义控制器与机器人交互数据
#define ID_map_command 0x0303               // 选手端小地图交互数据
#define ID_remote_control 0x0304            // 键鼠遥控数据
#define ID_map_robot_data 0x0305            // 选手端小地图接收雷达数据
#define ID_custom_client_data 0x0306        // 自定义控制器与选手端交互数据
#define ID_map_data 0x0307                  // 选手端小地图接收哨兵数据
#define ID_custom_info 0x0308               // 选手端小地图接收机器人数据

// 命令码数据段长,根据官方协议来定义长度
typedef enum
{
  LEN_game_status = 11,              // 0x0001
  LEN_game_result = 1,               // 0x0002
  LEN_game_robot_HP = 32,            // 0x0003
  LEN_event_data = 4,                // 0x0101
  LEN_supply_projectile_action = 4,  // 0x0102
  LEN_referee_warning = 3,           // 0x0104
  LEN_dart_info = 3,                 // 0x0105
  LEN_robot_status = 13,             // 0x0201
  LEN_power_heat_data = 16,          // 0x0202
  LEN_robot_pos = 12,                // 0x0203
  LEN_buff = 7,                      // 0x0204
  LEN_air_support_data = 2,          // 0x0205
  LEN_hurt_data = 1,                 // 0x0206
  LEN_shoot_data = 7,                // 0x0207
  LEN_projectile_allowance = 6,      // 0x0208
  LEN_rfid_status = 4,               // 0x0209
  LEN_dart_client_cmd = 6,           // 0x020A
  LEN_ground_robot_position = 40,    // 0x020B
  LEN_radar_mark_data = 6,           // 0x020C
  LEN_sentry_info = 4,               // 0x020D
  LEN_radar_info = 1,                // 0x020E
  LEN_robot_interaction_data = 118,  // 0x0301
  LEN_custom_robot_data = 30,        // 0x0302
  LEN_map_command = 11,              // 0x0303
  LEN_remote_control = 12,           // 0x0304
  LEN_map_robot_data = 10,           // 0x0305
  LEN_custom_client_data = 8,        // 0x0306
  LEN_map_data = 103,                // 0x0307
  LEN_custom_info = 34,              // 0x0308
} JudgeDataLength;

/* 自定义帧头 */
typedef struct
{
  uint8_t SOF;
  uint16_t DataLength;
  uint8_t Seq;
  uint8_t CRC8;

} xFrameHeader;

/* ID: 0x0001  Byte:  11    比赛状态数据 */
typedef struct
{
  uint8_t game_type : 4;
  uint8_t game_progress : 4;
  uint16_t stage_remain_time;
  uint64_t SyncTimeStamp;
} game_status_t;

/* ID: 0x0002  Byte:  1    比赛结果数据 */
typedef struct
{
  uint8_t winner;
} game_result_t;

/* ID: 0x0003  Byte:  32    机器人血量数据 */
typedef struct
{
  uint16_t red_1_robot_HP;
  uint16_t red_2_robot_HP;
  uint16_t red_3_robot_HP;
  uint16_t red_4_robot_HP;
  uint16_t red_5_robot_HP;
  uint16_t red_7_robot_HP;
  uint16_t red_outpost_HP;
  uint16_t red_base_HP;
  uint16_t blue_1_robot_HP;
  uint16_t blue_2_robot_HP;
  uint16_t blue_3_robot_HP;
  uint16_t blue_4_robot_HP;
  uint16_t blue_5_robot_HP;
  uint16_t blue_7_robot_HP;
  uint16_t blue_outpost_HP;
  uint16_t blue_base_HP;
} game_robot_HP_t;

/* ID: 0x0101  Byte:  4    场地事件数据 */
typedef struct
{
  uint32_t event_type;
} event_data_t;

/* ID: 0x0102  Byte:  3    补给站动作标识数据 */
typedef struct
{
  uint8_t reserved;
  uint8_t supply_robot_id;
  uint8_t supply_projectile_step;
  uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

/* ID: 0x0104  Byte:  3     裁判警告数据*/
typedef struct
{
  uint8_t level;
  uint8_t offending_robot_id;
  uint8_t count;
} referee_warning_t;

/* ID: 0x0105  Byte:  3     飞镖发射相关数据*/
typedef struct
{
  uint8_t dart_remaining_time;
  uint16_t dart_info;
} dart_info_t;

/* ID: 0X0201  Byte: 13    机器人性能体系数据 */
typedef struct
{
  uint8_t robot_id;
  uint8_t robot_level;
  uint16_t current_HP;
  uint16_t maximum_HP;
  uint16_t shooter_barrel_cooling_value;
  uint16_t shooter_barrel_heat_limit;
  uint16_t chassis_power_limit;
  uint8_t power_management_gimbal_output : 1;
  uint8_t power_management_chassis_output : 1;
  uint8_t power_management_shooter_output : 1;
} robot_status_t;

/* ID: 0X0202  Byte: 16    实时底盘功率和枪口热量数据 */
typedef struct
{
  uint16_t chassis_voltage;
  uint16_t chassis_current;
  float chassis_power;
  uint16_t buffer_energy;
  uint16_t shooter_17mm_1_barrel_heat;
  uint16_t shooter_17mm_2_barrel_heat;
  uint16_t shooter_42mm_barrel_heat;
} power_heat_data_t;

/* ID: 0x0203  Byte: 16    机器人位置数据 */
typedef struct
{
  float x;
  float y;
  float angle;
} robot_pos_t;

/* ID: 0x0204  Byte:  6    机器人增益数据 */
typedef struct
{
  uint8_t recovery_buff;
  uint8_t cooling_buff;
  uint8_t defence_buff;
  uint8_t vulnerability_buff;
  uint16_t attack_buff;
  uint8_t remaining_energy;
} buff_t;

/* ID: 0x0205  Byte:  2    空中支援时间数据 */
typedef struct
{
  uint8_t airforce_status;
  uint8_t time_remain;
} air_support_data_t;

/* ID: 0x0206  Byte:  1    伤害状态数据 */
typedef struct
{
  uint8_t armor_id : 4;
  uint8_t HP_deduction_reason : 4;
} hurt_data_t;

/* ID: 0x0207  Byte:  7    实时射击数据 */
typedef struct
{
  uint8_t bullet_type;
  uint8_t shooter_number;
  uint8_t launching_frequency;
  float initial_speed;
} shoot_data_t;

/* ID: 0x0208  Byte:  6     允许发弹量*/
typedef struct
{
  uint16_t projectile_allowance_17mm;
  uint16_t projectile_allowance_42mm;
  uint16_t remaining_gold_coin;
} projectile_allowance_t;

/* ID: 0x0209  Byte:  4     机器人 RFID 模块状态*/
typedef struct
{
  uint32_t rfid_status;
} rfid_status_t;

/* ID: 0x020A  Byte:  6     飞镖选手端指令数据*/
typedef struct
{
  uint8_t dart_launch_opening_status;
  uint8_t reserved;
  uint16_t target_change_time;
  uint16_t latest_launch_cmd_time;
} dart_client_cmd_t;

/* ID: 0x020B  Byte:  40     地面机器人位置数据*/
typedef struct
{
  float hero_x;
  float hero_y;
  float engineer_x;
  float engineer_y;
  float standard_3_x;
  float standard_3_y;
  float standard_4_x;
  float standard_4_y;
  float standard_5_x;
  float standard_5_y;
} ground_robot_position_t;

/* ID: 0x020C  Byte:  6     雷达标记进度数据*/
typedef struct
{
  uint8_t mark_hero_progress;
  uint8_t mark_engineer_progress;
  uint8_t mark_standard_3_progress;
  uint8_t mark_standard_4_progress;
  uint8_t mark_standard_5_progress;
  uint8_t mark_sentry_progress;
} radar_mark_data_t;

/* ID: 0x020D  Byte:  4     哨兵自主决策信息同步*/
typedef struct
{
  uint32_t sentry_info;
} sentry_info_t;

/* ID: 0x020E  Byte:  1     雷达自主决策信息同步*/
typedef struct
{
  uint8_t radar_info;
} radar_info_t;

/***************裁判系统数据********************/
/*
    机器人交互数据通过常规链路发送，其数据段包含一个统一的数据段头结构。
    数据段头结构包括内容 ID、发送者和接收者的 ID、内容数据段。机器人交互数据包的总长不超过 128 个字节，
    减去 frame_header、cmd_id 和 frame_tail 的 9 个字节以及数据段头结构的 6 个字节，
    故机器人交互数据的内容数据段最大为 112 个字节。
    每 1000 毫秒，英雄、工程、步兵、空中机器人、飞镖能够接收数据的上限为 3720 字节，
    雷达和哨兵机器人能够接收数据的上限为 5120 字节。

    机器人 ID 编号如下所示：
     1：红方英雄机器人
     2：红方工程机器人
     3/4/5：红方步兵机器人（与机器人 ID 3~5 对应）
     6：红方空中机器人
     7：红方哨兵机器人
     8：红方飞镖
     9：红方雷达
     10：红方前哨站
     11：红方基地
     101：蓝方英雄机器人
     102：蓝方工程机器人
     103/104/105：蓝方步兵机器人（与机器人 ID 3~5 对应）
     106：蓝方空中机器人
     107：蓝方哨兵机器人
     108：蓝方飞镖
     109：蓝方雷达
     110：蓝方前哨站
     111：蓝方基地

    选手端 ID 如下所示：
     0x0101：红方英雄机器人选手端
     0x0102：红方工程机器人选手端
     0x0103/0x0104/0x0105：红方步兵机器人选手端（与机器人 ID 3~5 对应）
     0x0106：红方空中机器人选手端
     0x016A：蓝方空中机器人选手端
     0x0165：蓝方英雄机器人选手端
     0x0166：蓝方工程机器人选手端
     0x0167/0x0168/0x0169：蓝方步兵机器人选手端（与机器人 ID 3~5 对应）
     0x8080：裁判系统服务器（用于哨兵和雷达自主决策指令）
*/

/* ID: 0x0301  Byte:  128     机器人交互数据*/
typedef struct
{
  uint16_t data_cmd_id;
  uint16_t sender_id;
  uint16_t receiver_id;
  uint8_t user_data[112];
} robot_interaction_data_t;
/*由于存在多个内容 ID，但整个 cmd_id 上行频率最大为 10Hz，请合理安排带宽。*/

/* ID: 0x  Byte:       */
typedef struct
{
  uint8_t delete_type;
  uint8_t layer;
} interaction_layer_delete_t;

/* ID: 0x  Byte:       */
typedef struct
{
  uint8_t figure_name[3];
  uint32_t operate_tpye : 3;
  uint32_t figure_tpye : 3;
  uint32_t layer : 4;
  uint32_t color : 4;
  uint32_t details_a : 9;
  uint32_t details_b : 9;
  uint32_t width : 10;
  uint32_t start_x : 11;
  uint32_t start_y : 11;
  uint32_t details_c : 10;
  uint32_t details_d : 11;
  uint32_t details_e : 11;
} interaction_figure_t;

/* ID: 0x  Byte:       */
typedef struct
{
  interaction_figure_t interaction_figure[2];
} interaction_figure_2_t;

/* ID: 0x  Byte:       */
typedef struct
{
  interaction_figure_t interaction_figure[5];
} interaction_figure_3_t;

/* ID: 0x  Byte:       */
typedef struct
{
  interaction_figure_t interaction_figure[7];
} interaction_figure_4_t;

/* ID: 0x  Byte:       */
// typedef struct
// {
//   //2024.05.07 有问题，别用
//   interaction_figure_t interaction_figure;
//   uint8_t data[30];
// } ext_client_custom_character_t;

/* ID: 0x  Byte:       */
typedef struct
{
  uint32_t sentry_cmd;
} sentry_cmd_t;

/* ID: 0x  Byte:       */
typedef struct
{
  uint8_t radar_cmd;
} radar_cmd_t;

/*
 * 为降低机器人串口接收设备的偶发不稳定性（例如：丢包）对花费金币的串口协议的影响，
 * 裁判系统串口协议中的 0x0303 协议的发送机制有所特殊处理，
 * 具体如下：选手端触发 1 次发送后，服务器将以100ms 的间隔向机器人额外发送 4 次，共 5 次。
 */
/* ID: 0x0303  Byte:  15     选手端小地图交互数据*/
typedef struct
{
  float target_position_x;
  float target_position_y;
  uint8_t cmd_keyboard;
  uint8_t target_robot_id;
  uint8_t cmd_source;
} map_command_t;

/* ID: 0x0305  Byte:  10      选手端小地图接收雷达数据*/
typedef struct
{
  uint16_t target_robot_id;
  float target_position_x;
  float target_position_y;
} map_robot_data_t;

/* ID: 0x0307  Byte:  103     选手端小地图接收哨兵数据*/
typedef struct
{
  uint8_t intention;
  uint16_t start_position_x;
  uint16_t start_position_y;
  int8_t delta_x[49];
  int8_t delta_y[49];
  uint16_t sender_id;
} map_data_t;

/* ID: 0x0308  Byte:  34     选手端小地图接收机器人数据*/
typedef struct
{
  uint16_t sender_id;
  uint16_t receiver_id;
  uint8_t user_data[30];
} custom_info_t;

/* ID: 0x0302  Byte:  30     自定义控制器与机器人交互数据*/
typedef struct
{
  uint8_t data[30];
} custom_robot_data_t;

/* ID: 0x0304  Byte:  12     键鼠遥控数据*/
typedef struct
{
  int16_t mouse_x;
  int16_t mouse_y;
  int16_t mouse_z;
  int8_t left_button_down;
  int8_t right_button_down;
  uint16_t keyboard_value;
  uint16_t reserved;
} remote_control_t;

/* ID: 0x0306  Byte:  8     自定义控制器与选手端交互数据*/
typedef struct
{
  uint16_t key_value;
  uint16_t x_position : 12;
  uint16_t mouse_left : 4;
  uint16_t y_position : 12;
  uint16_t mouse_right : 4;
  uint16_t reserved;
} custom_client_data_t;

extern game_status_t game_status;
extern game_result_t game_result;
extern game_robot_HP_t game_robot_HP;
extern event_data_t event_data;
extern ext_supply_projectile_action_t ext_supply_projectile_action;
extern referee_warning_t referee_warning;
extern dart_info_t dart_info;
extern robot_status_t robot_status;
extern power_heat_data_t power_heat_data;
extern robot_pos_t robot_pos;
extern buff_t buff;
extern air_support_data_t air_support_data;
extern hurt_data_t hurt_data;
extern shoot_data_t shoot_data;
extern projectile_allowance_t projectile_allowance;
extern rfid_status_t rfid_status;
extern dart_client_cmd_t dart_client_cmd;
extern ground_robot_position_t ground_robot_position;
extern radar_mark_data_t radar_mark_data;
extern sentry_info_t sentry_info;
extern radar_info_t radar_info;
extern robot_interaction_data_t robot_interaction_data;
extern map_command_t map_command;
extern map_robot_data_t map_robot_data;
extern map_data_t map_data;
extern custom_info_t custom_info;
extern custom_robot_data_t custom_robot_data;
extern remote_control_t remote_control;
extern custom_client_data_t custom_client_data;

void decode(uint8_t * rawData_t);

#ifdef __cplusplus
}
#endif
#endif
