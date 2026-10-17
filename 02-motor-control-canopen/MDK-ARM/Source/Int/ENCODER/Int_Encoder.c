#include "Int_Encoder.h"
#include "Dri_I2C.h"

// 编码器I2C地址
#define ENCODER_I2C_ADDR 0x06
#define ENCODER_I2C_ADDR_WRITE (ENCODER_I2C_ADDR << 1)
#define ENCODER_I2C_ADDR_READ (ENCODER_I2C_ADDR << 1 | 0x01)

#define ENCODER_ABZ_RES_H 0x30 // 只用低两位
#define ENCODER_ABZ_RES_L 0x31

/**
 * @brief 这是编码器分辨率
 *
 */
void __Int_Encoder_Set_Resolution(uint16_t res) {
   // I2C
    // 配置引脚
    Dri_I2C_Init();

    // 1. 写寄存器
    //   读取寄存器的值
    //  
    uint8_t reg_data = Dri_I2C_ReadReg(ENCODER_I2C_ADDR_READ, ENCODER_ABZ_RES_H);

    uint8_t res_l = res & 0xFF;
    uint8_t res_h = res >> 8; // 11
    // 01010101 +  11 =>  01010111
    for (uint8_t i = 0; i < 2; i++)
    {
        if ( (res_h & ( 1 << i )) == 0 ) {
            reg_data &= ~(1 << i);
        } else {
            reg_data |= (1 << i);
        }
    }

    Dri_I2C_Start();
    Dri_I2C_WriteAddr(ENCODER_I2C_ADDR_WRITE);
    Dri_I2C_WriteReg(ENCODER_ABZ_RES_L, res_l);
    Dri_I2C_Stop();  

    Dri_I2C_Start();
    Dri_I2C_WriteAddr(ENCODER_I2C_ADDR_WRITE);
    Dri_I2C_WriteReg(ENCODER_ABZ_RES_H, reg_data);
    Dri_I2C_Stop();  

    // 2. 编程密钥
    Dri_I2C_Start();
    Dri_I2C_WriteAddr(ENCODER_I2C_ADDR_WRITE);
    Dri_I2C_WriteReg(0x09, 0xB3);
    Dri_I2C_Stop();  

    // 3. 编程命令
    Dri_I2C_Start();
    Dri_I2C_WriteAddr(ENCODER_I2C_ADDR_WRITE);
    Dri_I2C_WriteReg(0x0A, 0x05);
    Dri_I2C_Stop();  

    // 4. 编程
    HAL_Delay(1000);

    // 5. 重新上电
    HAL_GPIO_WritePin(ENCODER_1_EN_GPIO_Port, ENCODER_1_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(ENCODER_1_EN_GPIO_Port, ENCODER_1_EN_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

    uint8_t reg_h = Dri_I2C_ReadReg(ENCODER_I2C_ADDR_READ, ENCODER_ABZ_RES_H);
    uint8_t reg_l = Dri_I2C_ReadReg(ENCODER_I2C_ADDR_READ, ENCODER_ABZ_RES_L);

    printf("%d\n", (((reg_h & 0x03) << 8) | reg_l));
}

/**
 * @brief 初始化编码器
 *
 */
void Int_Encoder_Init(void)
{
  // 开始使能
  HAL_GPIO_WritePin(ENCODER_1_EN_GPIO_Port, ENCODER_1_EN_Pin, GPIO_PIN_SET);

  // 配置工作模式I2C
  HAL_GPIO_WritePin(ENCODER_1_MODE_GPIO_Port, ENCODER_1_MODE_Pin, GPIO_PIN_SET);

  // 使用I2C修改编码器配置
  __Int_Encoder_Set_Resolution(0x3FF);

  // 需要将引脚工作模式修改会最初状态
  HAL_TIM_Encoder_MspInit(&htim1);

  // 设置工作模式ABZ
  HAL_GPIO_WritePin(ENCODER_1_MODE_GPIO_Port, ENCODER_1_MODE_Pin, GPIO_PIN_RESET);
}

void Int_Encoder_Start(void) { HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL); }

void Int_Encoder_Stop(void) { HAL_TIM_Encoder_Stop(&htim1, TIM_CHANNEL_ALL); }
