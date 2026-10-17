#include "Dri_I2C.h"

void Dri_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. 时钟使能GPIOA
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 2. 设置引脚模式，PA8(SDA), PA9(SCL)设置为通用开漏输出
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;   // 通用开漏输出
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// 发送起始信号
void Dri_I2C_Start(void)
{
    // 拉高SCL、拉高SDA、延时
    SCL_HIGH;
    SDA_HIGH;
    SCL_DELAY;

    // 拉低SDA，下降沿表示起始信号; 延时
    SDA_LOW;
    SCL_DELAY;

    // 拉低SCL，方便发送方修改SDA
    SCL_LOW;
    SCL_DELAY;
}

// 发送停止信号
void Dri_I2C_Stop(void)
{
    // 在拉高SCL之前，先拉低SDA （避免意外产生起始信号）
    SDA_LOW;

    // 拉高SCL并延时
    SCL_HIGH;
    SCL_DELAY;

    // SCL高电平期间拉高SDA，发起结束信号
    SDA_HIGH;
    SCL_DELAY;
}

// 发送一个字节的数据
void Dri_I2C_TransmitByte(uint8_t byte)
{
    // 分8次逐bit发送，先发高位
    for (uint8_t i = 0; i < 8; i++)
    {
        // 判断当前 byte 的最高位
        if (byte & 0x80)
        {
            SDA_HIGH;
        }
        else
        {
            SDA_LOW;
        }

        // 拉高时钟并延时，让接收方采样
        SCL_HIGH;
        SCL_DELAY;

        // byte 左移1位
        byte <<= 1;

        // 拉低SCL，好准备下一个要发的数据
        SCL_LOW;
        SCL_DELAY;
    }
}

// 接收一个字节的数据
uint8_t Dri_I2C_ReceiveByte(void)
{
    // 释放数据总线
    SDA_HIGH;

    // 定义变量，用于接收数据
    uint8_t byte = 0;

    // 分8次逐bit接收，先收高位
    for (uint8_t i = 0; i < 8; i++)
    {
        // 拉高SCL
        SCL_HIGH;

        // byte 左移1位，空出最低位用于接收当前的bit
        byte <<= 1;

        // 读SDA, 如果SDA是高电平
        if (SDA_READ)
        {
            byte |= 0x01;
        }

        // 延时
        SCL_DELAY;

        // 拉低SCL并延时，让发送方准备数据
        SCL_LOW;
        SCL_DELAY;
    }

    // 返回
    return byte;
}

// 发送一个ACK信号, 参数是0(ACK)或1(NACK)
void Dri_I2C_TransmitACK(uint8_t ack)
{
    if (ack)
    {
        // 发送NACK
        SDA_HIGH;
    }
    else
    {
        // 发送ACK
        SDA_LOW;
    }

    // 拉高时钟并延时
    SCL_HIGH;
    SCL_DELAY;

    // 高低时钟并延时
    SCL_LOW;
    SCL_DELAY;
}

// 接收一个ACK信号并返回
uint8_t Dri_I2C_ReceiveACK(void)
{
    // 定义变量保存收到的ack信号
    uint8_t ack = 0;

    // 释放数据总线
    SDA_HIGH;

    // 拉高SCL
    SCL_HIGH;

    // 读取SDA
    ack = SDA_READ ? 1 : 0;
    // 延时
    SCL_DELAY;

    // 拉低SCL并延时
    SCL_LOW;
    SCL_DELAY;

    return ack;
}

void Dri_I2C_WriteAddr(uint8_t slave_addr)
{
    Dri_I2C_TransmitByte(slave_addr);
    uint8_t ack = Dri_I2C_ReceiveACK();
    if (ack)
    {
        printf("I2C send slave address error");
        return;
    }
}

void Dri_I2C_WriteReg(uint8_t reg, uint8_t data)
{
    Dri_I2C_TransmitByte(reg);
    uint8_t ack = Dri_I2C_ReceiveACK();
    if (ack)
    {
        printf("I2C send register address error");
        return;
    }
    Dri_I2C_TransmitByte(data);
    ack = Dri_I2C_ReceiveACK();
    if (ack)
    {
        printf("I2C send register data error");
        return;
    }
}

uint8_t Dri_I2C_ReadReg(uint8_t slave_addr_read, uint8_t reg)
{
    Dri_I2C_Start();
    Dri_I2C_WriteAddr(slave_addr_read - 1);
    Dri_I2C_TransmitByte(reg);
    uint8_t ack = Dri_I2C_ReceiveACK();
    if (ack)
    {
        printf("I2C send register address error");
        return 0;
    }
    Dri_I2C_Start();
    Dri_I2C_WriteAddr(slave_addr_read);
    uint8_t data = Dri_I2C_ReceiveByte();
    Dri_I2C_TransmitACK(0);
    Dri_I2C_Stop();
    return data;
}
