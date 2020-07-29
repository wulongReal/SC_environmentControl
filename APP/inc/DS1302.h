#include "stm32f10x.h"


#ifndef __DS1302_H
#define __DS1302_H

extern u8 ascii_time[7];     //保存ascii格式数据

extern u8 bcd_time[7];       //保存bcd码数据

typedef struct DS1302DATA
{
    u8 year;    //年
    u8 month;   //月
    u8 day;     //日
    u8 hour;    //时
    u8 min;     //分
    u8 sec;     //秒
    u8 week;    //周
}DS1302DATA;

extern struct DS1302DATA ds1302Data;



#define DS1302_RST      PBout(2)
#define DS1302_CLK      PBout(0)
#define DS1302_DATIN    PBin(1)
#define DS1302_DATOUT   PBout(1)

//#define DS1302_DAT_INPUT()     {GPIOB->CRH &= 0XFFFF0FFF;GPIOB->CRH|=8<<12;}
//#define DS1302_DAT_OUTPUT()    {GPIOB->CRH &= 0XFFFF0FFF;GPIOB->CRH|=3<<12;}

//芯片寄存器地址定义 定义的写地址，读需要+1
#define DS1302_SEC_REG			0x80		//秒数据地址
#define DS1302_MIN_REG			0x82		//分数据地址
#define DS1302_HR_REG			0x84		//时数据地址
#define DS1302_DATE_REG			0x86		//日数据地址
#define DS1302_MONTH_REG		0x88		//月数据地址
#define DS1302_DAY_REG			0x8a		//星期几数据地址
#define DS1302_YEAR_REG			0x8c		//年数据地址
#define DS1302_CONTROL_REG		0x8e		//写保护寄存器地址
#define DS1302_CHARGER_REG		0x90 		//涓流充电寄存器			 
#define DS1302_CLKBURST_REG		0xbe             //脉冲串寄存器
#define DS1302_RAM_BASE                 0X30            //RAM基础地址

#define CLOCKSTOP       0X80
#define CLOCKSTART      0X00


void Ds1302_Gpio_Init(void);
void Ds1302_Write_Time_All(u8 start,DS1302DATA date);
void Ds1302_Settime(uint8_t isRunEn, DS1302DATA date);

void Ds1302_Readtime(void);
void Ds1302_Readtime_BCD(void);


u8 AsciiToBcd(u8 asciiData);
 u8 BcdToAscii(u8 bcdData);	



#endif








