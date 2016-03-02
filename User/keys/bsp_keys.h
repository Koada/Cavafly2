#ifndef __BSP_KEYS
#define __BSP_KEYS
#include "stm32f10x.h"

void Keys_Init(void);

#define KEY_ON	0
#define KEY_OFF	1
uint8_t Key_Scan(GPIO_TypeDef* GPIOx, u16 GPIO_Pin, uint8_t Down_state);

#endif
