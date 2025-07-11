#include "Referee.h"

#include "bsp_crc.h"
#include "cmsis_os.h"
#include "main.h"
#include "string.h"
#include "usart.h"

game_status_t game_status;
game_result_t game_result;
game_robot_HP_t game_robot_HP;
event_data_t event_data;
ext_supply_projectile_action_t ext_supply_projectile_action;
referee_warning_t referee_warning;
dart_info_t dart_info;
robot_status_t robot_status;
power_heat_data_t power_heat_data;
robot_pos_t robot_pos;
buff_t buff;
air_support_data_t air_support_data;
hurt_data_t hurt_data;
shoot_data_t shoot_data;
projectile_allowance_t projectile_allowance;
rfid_status_t rfid_status;
dart_client_cmd_t dart_client_cmd;
ground_robot_position_t ground_robot_position;
radar_mark_data_t radar_mark_data;
sentry_info_t sentry_info;
radar_info_t radar_info;
robot_interaction_data_t robot_interaction_data;
map_command_t map_command;
map_robot_data_t map_robot_data;
map_data_t map_data;
custom_info_t custom_info;
custom_robot_data_t custom_robot_data;
remote_control_t remote_control;
custom_client_data_t custom_client_data;
xFrameHeader FrameHeader;

void decode(uint8_t * rawData_t)
{
  uint16_t cmd_id;

  if (rawData_t == NULL) return;

  if (rawData_t[0] == JUDGE_FRAME_HEADER) {
    FrameHeader.DataLength = rawData_t[2] << 8 | rawData_t[1];
    if (Verify_CRC8_Check_Sum(rawData_t, LEN_HEADER)) {
      if (Verify_CRC16_Check_Sum(
            rawData_t, LEN_HEADER + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL)) {
        cmd_id = (rawData_t[6] << 8 | rawData_t[5]);
        switch (cmd_id) {
          case ID_game_status:
            memcpy(&game_status, rawData_t + 7, LEN_game_status);
            break;

          case ID_game_result:
            memcpy(&game_result, rawData_t + 7, LEN_game_result);
            break;

          case ID_game_robot_HP:
            memcpy(&game_robot_HP, rawData_t + 7, LEN_game_robot_HP);
            break;

          case ID_event_data:
            memcpy(&event_data, rawData_t + 7, LEN_event_data);
            break;

          case ID_supply_projectile_action:
            memcpy(&ext_supply_projectile_action, rawData_t + 7, LEN_supply_projectile_action);
            break;

          case ID_referee_warning:
            memcpy(&referee_warning, rawData_t + 7, LEN_referee_warning);
            break;

          case ID_dart_info:
            memcpy(&dart_info, rawData_t + 7, LEN_dart_info);
            break;

          case ID_robot_status:
            memcpy(&robot_status, rawData_t + 7, LEN_robot_status);
            break;

          case ID_power_heat_data:
            memcpy(&power_heat_data, rawData_t + 7, LEN_power_heat_data);
            break;

          case ID_robot_pos:
            memcpy(&robot_pos, rawData_t + 7, LEN_robot_pos);
            break;

          case ID_buff:
            memcpy(&buff, rawData_t + 7, LEN_buff);
            break;

          case ID_air_support_data:
            memcpy(&air_support_data, rawData_t + 7, LEN_air_support_data);
            break;

          case ID_hurt_data:
            memcpy(&hurt_data, rawData_t + 7, LEN_hurt_data);
            break;

          case ID_shoot_data:
            memcpy(&shoot_data, rawData_t + 7, LEN_shoot_data);
            break;

          case ID_projectile_allowance:
            memcpy(&projectile_allowance, rawData_t + 7, LEN_projectile_allowance);
            break;

          case ID_rfid_status:
            memcpy(&rfid_status, rawData_t + 7, LEN_rfid_status);
            break;

          case ID_dart_client_cmd:
            memcpy(&dart_client_cmd, rawData_t + 7, LEN_dart_client_cmd);
            break;

          case ID_ground_robot_position:
            memcpy(&ground_robot_position, rawData_t + 7, LEN_ground_robot_position);
            break;

          case ID_radar_mark_data:
            memcpy(&radar_mark_data, rawData_t + 7, LEN_radar_mark_data);
            break;

          case ID_sentry_info:
            memcpy(&sentry_info, rawData_t + 7, LEN_sentry_info);
            break;

          case ID_radar_info:
            memcpy(&radar_info, rawData_t + 7, LEN_radar_info);
            break;

          case ID_robot_interaction_data:
            memcpy(&robot_interaction_data, rawData_t + 7, LEN_robot_interaction_data);
            break;

          case ID_custom_robot_data:
            memcpy(&custom_robot_data, rawData_t + 7, LEN_custom_robot_data);
            break;

          case ID_map_command:
            memcpy(&map_command, rawData_t + 7, LEN_map_command);
            break;

          case ID_remote_control:
            memcpy(&remote_control, rawData_t + 7, LEN_remote_control);
            break;

          case ID_map_robot_data:
            memcpy(&map_robot_data, rawData_t + 7, LEN_map_robot_data);
            break;

          case ID_custom_client_data:
            memcpy(&custom_client_data, rawData_t + 7, LEN_custom_client_data);
            break;

          case ID_map_data:
            memcpy(&map_data, rawData_t + 7, LEN_map_data);
            break;

          case ID_custom_info:
            memcpy(&custom_info, rawData_t + 7, LEN_custom_info);
            break;

          default:
            break;
        }
      }
    }

    if (
      *(rawData_t + sizeof(xFrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL) == 0xA5) {
      // 如果一个数据包出现了多帧数据,则再次调用解析函数,直到所有数据包解析完毕
      decode(rawData_t + sizeof(xFrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL);
    }
  }
}
