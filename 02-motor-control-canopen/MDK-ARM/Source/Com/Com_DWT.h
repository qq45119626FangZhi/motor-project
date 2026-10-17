#ifndef __DMT_H__
#define __DMT_H__

#include "stm32f407xx.h"

// DWT init
void Com_DWT_Init(void);

// 使用DWT延时time_us微秒
void Com_DWT_delay_us(uint32_t time_us);

#endif /* __DMT_H__ */
