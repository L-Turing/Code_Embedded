#ifndef __PERIPHERAL_H
#define __PERIPHERAL_H

#pragma pack(push, 1)
#ifdef __cplusplus
extern "C" {
#endif

// Signal types for controlling LEDs and buzzer
typedef enum
{
  LED_G_ON = 0,
  LED_1_ON = 1,
  LED_2_ON = 2,
  LED_3_ON = 3,
  LED_4_ON = 4,
  LED_5_ON = 5,
  LED_6_ON = 6,
  LED_7_ON = 7,
  LED_8_ON = 8,
  Buzzer_ON = 9,
  LED_R_ON = 10,
  LED_R_Breathe_ON = 11,

  LED_G_OFF,
  LED_1_OFF,
  LED_2_OFF,
  LED_3_OFF,
  LED_4_OFF,
  LED_5_OFF,
  LED_6_OFF,
  LED_7_OFF,
  LED_8_OFF,
  Buzzer_OFF,
  LED_R_OFF,
  LED_R_Breathe_OFF,

} type_signal;

void SignalMark(type_signal sign);

#ifdef __cplusplus
}
#endif
#pragma pack(pop)

#endif
