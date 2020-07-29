#include <stm32f10x.h>  
#include "dac.h"
#include "bsp.h"
#include "dac.h"
#include <rtthread.h>

#include "vars.h"

															   
void  Dac_Init(void)
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOC	, ENABLE );	  

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_15 ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	CS_L_1;
	SCLK_L_1;
	DIN_L_1;

}				  


void csPinCtrl(u8 ch, u8 value)
{
	switch(ch)
	{
		case	0:
		{
			if(value)
				CS_H_1;
			else
				CS_L_1;
			break;
		}
		case	1:
		{	
			if(value)
				CS_H_2;
			else
				CS_L_2;
			break;
		}
		case	2:
		{	
			if(value)
				CS_H_3;
			else
				CS_L_3;
			break;
		}
		case	3:
		{	
			if(value)
				CS_H_4;
			else
				CS_L_4;
			break;
		}

		default:break;
	}
}
void dinPinCtrl(u8 ch, u8 value)
{
	switch(ch)
	{
		case	0:
		{
			if(value)
				DIN_H_1;
			else
				DIN_L_1;
			break;
		}
		case	1:
		{
			if(value)
				DIN_H_2;
			else
				DIN_L_2;
			break;
		}
		case	2:
		{
			if(value)
				DIN_H_3;
			else
				DIN_L_3;
			break;
		}
		case	3:
		{
			if(value)
				DIN_H_4;
			else
				DIN_L_4;
			break;
		}

		default:break;
	}
}

void sclkPinCtrl(u8 ch, u8 value)
{
	switch(ch)
	{
		case	0:
		{
			if(value)
				SCLK_H_1;
			else
				SCLK_L_1;
			break;
		}
		case	1:
		{
			if(value)
				SCLK_H_2;
			else
				SCLK_L_2;
			break;
		}
		case	2:
		{
			if(value)
				SCLK_H_3;
			else
				SCLK_L_3;
			break;
		}
		case	3:
		{
			if(value)
				SCLK_H_4;
			else
				SCLK_L_4;
			break;
		}

		default:break;
	}
}


static void dacDelay_us(int us)
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
/*//DAC TLC5615  10位DA
void dacOut(u8 ch,unsigned int data)
{	
	unsigned char i;
	u16	DAValue = 0;

	DAValue = data;

	//DAValue = getDacValue(ch);
	if(DAValue >= MAX_DAC_VALUE)
		DAValue = MAX_DAC_VALUE;


	
	
	DAValue <<= 6;	
	sclkPinCtrl(ch, VALUE_L);
	dacDelay_us(5);
	csPinCtrl(ch, VALUE_L);
	dacDelay_us(5);
	for(i = 0;i < 12;i++)
	{
		if(DAValue & 0x8000)
			dinPinCtrl(ch, VALUE_H);
		else
			dinPinCtrl(ch, VALUE_L);
		DAValue <<= 1;
		dacDelay_us(1);
		sclkPinCtrl(ch, VALUE_H);
		dacDelay_us(1);
		sclkPinCtrl(ch, VALUE_L);
		dacDelay_us(1);
	}
	csPinCtrl(ch, VALUE_H);
	dacDelay_us(1);
	sclkPinCtrl(ch, VALUE_L);
}
*/
void dacOut(u8 ch,unsigned int data)
{	
	unsigned char i;
	u16	DAValue = 0;

	DAValue = data;

	if(DAValue >= MAX_DAC_VALUE)
		DAValue = MAX_DAC_VALUE-1;


	
	
	DAValue <<= 4;	
	if(ch == TLV5625_OUTPUT_CHANNEL_A)
	{
		DAValue |= 0xc000;
	}
	else if(ch == TLV5625_OUTPUT_CHANNEL_B)
	{
		DAValue |= 0x4000;
	}
	sclkPinCtrl(0, VALUE_L);
	dacDelay_us(5);
	csPinCtrl(0, VALUE_L);
	dacDelay_us(5);
	for(i = 0;i < 16;i++)
	{
		if(DAValue & 0x8000)
			dinPinCtrl(0, VALUE_H);
		else
			dinPinCtrl(0, VALUE_L);
		DAValue <<= 1;
		dacDelay_us(1);
		sclkPinCtrl(0, VALUE_H);
		dacDelay_us(1);
		sclkPinCtrl(0, VALUE_L);
		dacDelay_us(1);
	}
	csPinCtrl(0, VALUE_H);
	dacDelay_us(1);
	sclkPinCtrl(0, VALUE_H);
}



