#ifndef _MAIN_TASK_H
#define _MAIN_TASK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
void StartINS(void const * argument);

extern uint8_t * receive_usart_buffer[10];
extern uint8_t * receive_can_buffer[5];

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#endif

#endif
