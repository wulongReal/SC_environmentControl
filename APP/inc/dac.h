#ifndef __DAC_H
#define __DAC_H	
#include "stm32f10x.h"
#include "bsp.h"




#define	CS_H_1	GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define	DIN_H_1	GPIO_SetBits(GPIOC,GPIO_Pin_10)
#define	SCLK_H_1	GPIO_SetBits(GPIOA,GPIO_Pin_15)

#define	CS_L_1	GPIO_ResetBits(GPIOA,GPIO_Pin_8)
#define	DIN_L_1	GPIO_ResetBits(GPIOC,GPIO_Pin_10)
#define	SCLK_L_1 GPIO_ResetBits(GPIOA,GPIO_Pin_15)

#define	CS_H_2	GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define	DIN_H_2	GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define	SCLK_H_2	GPIO_SetBits(GPIOB,GPIO_Pin_5)

#define	CS_L_2	GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define	DIN_L_2	GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define	SCLK_L_2 GPIO_ResetBits(GPIOB,GPIO_Pin_5)


#define	CS_H_3	GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define	DIN_H_3	GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define	SCLK_H_3	GPIO_SetBits(GPIOB,GPIO_Pin_7)

#define	CS_L_3	GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define	DIN_L_3	GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define	SCLK_L_3 GPIO_ResetBits(GPIOB,GPIO_Pin_7)




#define	CS_H_4	GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define	DIN_H_4	GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define	SCLK_H_4	GPIO_SetBits(GPIOB,GPIO_Pin_9)

#define	CS_L_4	GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define	DIN_L_4	GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define	SCLK_L_4 GPIO_ResetBits(GPIOB,GPIO_Pin_9)





#define	VALUE_L		0
#define	VALUE_H		1


#define	MIN_DAC_VALUE	5


#define	TLV5625_EN
#ifdef	TLV5625_EN
#define	TLV5625_OUTPUT_CHANNEL_A	0
#define	TLV5625_OUTPUT_CHANNEL_B	1
#define	MAX_DAC_VALUE				256
#elif	TLC5615_EN
#define	MAX_DAC_VALUE		1024
#endif



void Dac_Init(void);
void csPinCtrl(u8 ch, u8 value);
void dinPinCtrl(u8 ch, u8 value);
void sclkPinCtrl(u8 ch, u8 value);
u16 getDacValue(u8 ch);
void dacOut(u8 ch,unsigned int data);




#endif 

