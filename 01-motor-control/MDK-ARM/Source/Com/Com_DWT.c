#include "Com_DWT.h"

/**
 * @brief 定时器DWT， 调试专用，延迟微秒
 * 
 */
void Com_DWT_Init(void)
{
    // 使能 DWT 外设
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // DWT 计数器清零
    DWT->CYCCNT = 0;
    // 使能 DWT 计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void Com_DWT_delay_us(uint32_t xus)
{
    DWT->CYCCNT = 0;
    uint32_t target_cnt = (uint32_t)(xus * (SystemCoreClock / 1000000));
    while (DWT->CYCCNT < target_cnt)
    {
        __NOP();
    }
}
