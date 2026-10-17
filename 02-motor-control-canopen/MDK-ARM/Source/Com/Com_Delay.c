#include "Com_Delay.h"

/**
 * 延迟函数 - 延迟秒
*/
void Com_Delay_s( uint16_t s ) {
    HAL_Delay(s * 1000);
}
/**
 * 延迟函数 - 延迟毫秒
*/
void Com_Delay_ms( uint16_t ms ) {
    HAL_Delay(ms);
}
/**
 * 延迟函数 - 延迟微秒
*/
void Com_Delay_us( uint16_t us ) {
    // 72M
    // 10
    uint32_t delay = HAL_RCC_GetSysClockFreq() / 5000000 * us;
    while ( delay > 0 ) {
        __NOP(); // 1us
        delay--;
    }
}
