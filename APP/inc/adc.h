#ifndef __ADC_H__  
#define __ADC_H__  
 
#include <rtthread.h>  

#define   ADC_CH1                    ADC_Channel_4
#define   ADC_CH2                    ADC_Channel_13

#define   SAMPLING_TIME_CK           ADC_SampleTime_7Cycles5
#define   ADC_NUM    8

void ADC_init(void);

uint16_t Get_Adc(uint8_t ch);
  
#endif  
