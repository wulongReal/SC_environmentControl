#include"DS1302.h"
#include <rtthread.h>
#include "vars.h"
#include "bsp.h"



struct DS1302DATA ds1302Data = {0,0,0,0,0,0,0};
u8 ascii_time[7] = {0};     //保存ascii格式数据

u8 bcd_time[7] = {0};       //保存bcd码数据


void delayUs(int us)
{
    rt_uint32_t delta;
    rt_uint32_t current_delay;

        
    us = us * (SysTick->LOAD/(1000000/RT_TICK_PER_SECOND));

       
    delta = SysTick->VAL;

    
    do
    {
        if ( delta > SysTick->VAL )
            current_delay = delta - SysTick->VAL;
        else
   
            current_delay = SysTick->LOAD + delta - SysTick->VAL;
    } while( current_delay < us );
}

//14转为0X14
 u8 AsciiToBcd(u8 asciiData)
{
    u8 bcdData = 0;
    bcdData = (((asciiData/10)<<4)|((asciiData%10)));
    return bcdData;
}
 //0x14转为14
 u8 BcdToAscii(u8 bcdData)
{
    u8 asciiData = 0;
    asciiData = (((bcdData&0xf0)>>4)*10 + (bcdData&0x0f));
    return asciiData;
}

//IO口初始化
void Ds1302_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    //EN
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;      
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //CLK
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;      
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //IO
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1;      
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void DS1302_DAT_OUTPUT(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void DS1302_DAT_INPUT(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}


//读取一个字节的时序
u8 Ds1302_ReadByte(void)
{
    u8 i = 0, dat = 0;
    DS1302_DAT_INPUT();
    delayUs(5);
    for(i = 0; i <8; i++)
    {
        dat >>= 1;
        if(DS1302_DATIN == 1)dat |= 0x80;
        DS1302_CLK = 1;
        delayUs(2);
        DS1302_CLK = 0;
        delayUs(2);
    }
    return dat;
}

//写入一个字节的时序
void Ds1302_WriteByte(u8 dat)
{
    u8 i = 0, data = dat;
    DS1302_DAT_OUTPUT(); 
    DS1302_CLK = 0;
    delayUs(2);
    for(i = 0; i < 8; i++)
    {
        DS1302_DATOUT = data&0x01;
        delayUs(2);
        DS1302_CLK = 1;
        delayUs(2);
        DS1302_CLK = 0;
        data >>= 1;
    }
}

//写入一个寄存器
void Ds1302_Write(u8 address,u8 dat)
{
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
    Ds1302_WriteByte(address);
    Ds1302_WriteByte(dat);
    DS1302_CLK = 1;
    DS1302_RST = 0;
}

//单个写入时间
void Ds1302_Write_Time_Singel(u8 address,u8 dat)
{
    Ds1302_Write(DS1302_CONTROL_REG,0x00);  //取消写保护
    Ds1302_Write(address,dat);
    Ds1302_Write(DS1302_CONTROL_REG,0x80);  //打开写保护
}

//一次完成所有时间更新
//start当前时钟运行还是停止
void Ds1302_Write_Time_All(u8 start,DS1302DATA date)
{
    Ds1302_Write(DS1302_CONTROL_REG,0x00);      //取消写保护
    Ds1302_Write(DS1302_SEC_REG,(AsciiToBcd(date.sec)|start));
    Ds1302_Write(DS1302_MIN_REG,AsciiToBcd(date.min));
    Ds1302_Write(DS1302_HR_REG,AsciiToBcd(date.hour));
    Ds1302_Write(DS1302_DATE_REG,AsciiToBcd(date.day));
    Ds1302_Write(DS1302_MONTH_REG,AsciiToBcd(date.month));
    Ds1302_Write(DS1302_DAY_REG,AsciiToBcd(date.week));
    Ds1302_Write(DS1302_YEAR_REG,AsciiToBcd(date.year));
    Ds1302_Write(DS1302_CONTROL_REG,0x80);  //打开写保护
}


//读取一个字节
u8 Ds1302_Read(u8 address)
{
    u8 data = 0;
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
    Ds1302_WriteByte(address|0x01); //读取地址需要与0x01相或，最低为变成1
    data = Ds1302_ReadByte();
    DS1302_CLK = 1;
    DS1302_RST = 0;
    return data;
}

//读取时间的。返回值为十进制，例如0x14表示20
void Ds1302_Readtime(void)
{
    ds1302Data.sec = BcdToAscii(Ds1302_Read(DS1302_SEC_REG));  //秒
    ds1302Data.min = BcdToAscii(Ds1302_Read(DS1302_MIN_REG));  //分
    ds1302Data.hour = BcdToAscii(Ds1302_Read(DS1302_HR_REG));   //小时
    ds1302Data.day = BcdToAscii(Ds1302_Read(DS1302_DATE_REG)); //日
    ds1302Data.month = BcdToAscii(Ds1302_Read(DS1302_MONTH_REG)); //月
    ds1302Data.week = BcdToAscii(Ds1302_Read(DS1302_DAY_REG));  //星期几
    ds1302Data.year = BcdToAscii(Ds1302_Read(DS1302_YEAR_REG)); //年
}

void Ds1302_Readtime_BCD(void)
{
    ds1302Data.sec = (Ds1302_Read(DS1302_SEC_REG));  //秒
    ds1302Data.min = (Ds1302_Read(DS1302_MIN_REG));  //分
    ds1302Data.hour = (Ds1302_Read(DS1302_HR_REG));   //小时
    ds1302Data.day = (Ds1302_Read(DS1302_DATE_REG)); //日
    ds1302Data.month = (Ds1302_Read(DS1302_MONTH_REG)); //月
    ds1302Data.week = (Ds1302_Read(DS1302_DAY_REG));  //星期几
    ds1302Data.year = (Ds1302_Read(DS1302_YEAR_REG)); //年
}

/*
设置RTC时间
*/
void Ds1302_Settime(uint8_t isRunEn, DS1302DATA date)
{
	Ds1302_Write(DS1302_CONTROL_REG,0x00);      //取消写保护
    Ds1302_Write(DS1302_SEC_REG,(AsciiToBcd(date.sec)|isRunEn));
    Ds1302_Write(DS1302_MIN_REG,AsciiToBcd(date.min));
    Ds1302_Write(DS1302_HR_REG,AsciiToBcd(date.hour));
    Ds1302_Write(DS1302_DATE_REG,AsciiToBcd(date.day));
    Ds1302_Write(DS1302_MONTH_REG,AsciiToBcd(date.month));
    //Ds1302_Write(DS1302_DAY_REG,AsciiToBcd(date.week));
    Ds1302_Write(DS1302_YEAR_REG,AsciiToBcd(date.year));
    Ds1302_Write(DS1302_CONTROL_REG,0x80);  //打开写保护
}

