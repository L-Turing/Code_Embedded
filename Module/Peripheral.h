#ifndef __PERIPHERAL_H
#define __PERIPHERAL_H

#pragma pack(push, 1)

// Signal types for controlling LEDs and buzzer
#ifdef __cplusplus
#include <cstdint>

enum class type_signal
{
  LED_1_ON = 1,
  LED_2_ON,
  LED_3_ON,
  LED_4_ON,
  LED_5_ON,
  LED_6_ON,
  LED_7_ON,
  LED_8_ON,
  Buzzer_ON,
  LED_R_ON,
  LED_R_Breathe_ON,
  LED_G_ON,

  LED_1_OFF = LED_1_ON + 100,
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
  LED_G_OFF,
};

void SignalMark(type_signal sign);
#endif

#pragma pack(pop)

#endif
