#ifndef __COM_DELAY_H__
#define __COM_DELAY_H__

#include "stm32f4xx_hal.h"

/**
 * 延迟函数 - 延迟秒
*/
void Com_Delay_s( uint16_t s );
/**
 * 延迟函数 - 延迟毫秒
*/
void Com_Delay_ms( uint16_t ms );
/**
 * 延迟函数 - 延迟微秒
*/
void Com_Delay_us( uint16_t us );

#endif /* __COM_DELAY_H__ */
