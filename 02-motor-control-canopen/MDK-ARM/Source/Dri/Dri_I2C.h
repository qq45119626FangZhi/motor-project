#ifndef __DRI_I2C_H__
#define __DRI_I2C_H__

#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f4xx.h"
#include "main.h"
#include "stdio.h"
#include "Com_DWT.h"

// PA8是SDA，PA9是SCL
#define I2C_SCL_PIN GPIO_PIN_9
#define I2C_SDA_PIN GPIO_PIN_8
#define I2C_GPIO_PORT GPIOA

// 宏定义 SDA拉高或拉低
#define SDA_HIGH I2C_GPIO_PORT->BSRR = I2C_SDA_PIN
#define SDA_LOW I2C_GPIO_PORT->BSRR = (uint32_t)I2C_SDA_PIN << 16U

// 宏定义 SCL拉高或拉低
#define SCL_HIGH I2C_GPIO_PORT->BSRR = I2C_SCL_PIN
#define SCL_LOW I2C_GPIO_PORT->BSRR = (uint32_t)I2C_SCL_PIN << 16U

// 宏定义 读SDA
#define SDA_READ (I2C_GPIO_PORT->IDR & I2C_SDA_PIN)

// 宏定义 延时5us
#define SCL_DELAY Com_DWT_delay_us(5)

void Dri_I2C_Init(void);                 // 初始化函数
void Dri_I2C_Start(void);                // 发送起始信号
void Dri_I2C_Stop(void);                 // 发送停止信号
void Dri_I2C_TransmitByte(uint8_t byte); // 发送一个字节的数据
uint8_t Dri_I2C_ReceiveByte(void);       // 接收一个字节的数据
void Dri_I2C_TransmitACK(uint8_t ack);   // 发送一个ACK信号, 参数是0(ACK)或1(NACK)
uint8_t Dri_I2C_ReceiveACK(void);        // 接收一个ACK信号并返回

void Dri_I2C_WriteAddr(uint8_t slave_addr);                    // 写从机地址
void Dri_I2C_WriteReg(uint8_t reg, uint8_t data);              // 写一个寄存器
uint8_t Dri_I2C_ReadReg(uint8_t slave_addr_read, uint8_t reg); // 读一个寄存器

#endif /* __DRI_I2C_H__ */
