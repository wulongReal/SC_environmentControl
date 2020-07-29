#ifndef _IIC_H_
#define _IIC_H_
#include "usart.h"
#include "platform.h"

//typedef uint8_t bool;

#define WRITE_MASK  0xFE
#define READ_MASK  0x01

/************************
*  SDA-->PC13(输出)  *
*  SCL-->PC14(输出)    *
************************/

//I2C输入端口定义
#define SDA     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)
//I2C输出端口定义
#define SDA_0   GPIO_ResetBits(GPIOC , GPIO_Pin_13)
#define SDA_1   GPIO_SetBits(GPIOC, GPIO_Pin_13)
#define SCL_0   GPIO_ResetBits(GPIOC, GPIO_Pin_14)
#define SCL_1   GPIO_SetBits(GPIOC, GPIO_Pin_14)



#define SDA_H SDA_1
#define SDA_L SDA_0
#define SCL_H SCL_1
#define SCL_L SCL_0



#define SCLread     GPIOC->IDR & GPIO_Pin_14
#define SDAread     GPIOC->IDR & GPIO_Pin_13

//Measure Relative Humidity, Hold Master Mode 0xE5
//Measure Relative Humidity, No Hold Master Mode 0xF5
//Measure Temperature, Hold Master Mode 0xE3
//Measure Temperature, No Hold Master Mode 0xF3
//Read Temperature Value from Previous RH Measurement 0xE0
//Reset 0xFE
//Write RH/T User Register 1 0xE6
//Read RH/T User Register 1 0xE7
//Write Heater Control Register 0x51
//Read Heater Control Register 0x11
//Read Electronic ID 1st Byte 0xFA 0x0F
//Read Electronic ID 2nd Byte 0xFC 0xC9
//Read Firmware Revision 0x84 0xB8

#define READ0HUMI_HOLD   0xE5
#define READ0HUMI_NOHOLD 0xF5
#define READ0TEMP_HOLD   0xE3
#define READ0TEMP_NOHOLD 0xF3
#define READPRE_TEMP     0xE0
#define REST             0xFE
#define WR_USR_REG       0xE6
#define RE_USR_REG       0xE7
#define WR_HEAT_REG      0x51
#define RE_HEAT_REG      0x11
#define RE_ELCID_1B      0xFA0F
#define RE_ELCID_2B      0xFCC9
#define RE_FIR_REV       0x84B8
/********************************************
*函数功能：初始化硬件I2C2接口          *
*参数：无                  *
*返回值：无                  *
********************************************/
enum ENUM_IIC_REPLY{
  IIC_NACK=0,
  IIC_ACK=1
};
enum ENUM_IIC_BUS_STATE{
  IIC_READY=0,
  IIC_BUS_BUSY=1,
  IIC_BUS_ERROR=2
};

/************************************************************
*函数功能：通过I2C2接口向从机写数据函数            *
*参数：addr:从机的设备地址，buff:数据缓冲区，length:数据长度  *
*返回值：无                          *
************************************************************/
void I2C_Write_Data(uint8_t addr,uint8_t *buff,u16 length);

/************************************************************
*函数功能：通过I2C从从机读数据函数              *
*参数：addr:从机的设备地址，buff:数据缓冲区，length:数据长度  *
*返回值：无                          *
************************************************************/
void I2C_Read_Data(uint8_t addr,uint8_t *buff,u16 length);
void si7020_function(void);



#endif

