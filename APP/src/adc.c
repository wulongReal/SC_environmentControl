#include "platform.h" 
#include "adc.h"
#include "bsp.h"

void ADC_init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	/* ADC1 registers reset ----------------------------------------------------*/
	ADC_DeInit(ADC1);                                   
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;        
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
	ADC_InitStructure.ADC_NbrOfChannel = 1;                
	ADC_Init(ADC1, &ADC_InitStructure);                    

	ADC_Cmd(ADC1, ENABLE);                                
	ADC_ResetCalibration(ADC1);                           
	while(ADC_GetResetCalibrationStatus(ADC1));         
	ADC_StartCalibration(ADC1);                           
	while(ADC_GetCalibrationStatus(ADC1));               
}


uint16_t Get_Adc(uint8_t ch){  
  unsigned int result=0;
  #if defined(PLATFORM_F103)
  unsigned char i;
  unsigned int adc_tmp;
  //����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
  
  ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_41Cycles5 );  //ADC1,ADCͨ��ch,�������˳��ֵ����Ϊ1,����ʱ��Ϊ239.5����              
  for(i=0;i<ADC_NUM;i++)  {//ADC_NUM 8
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);        //ʹ��ָ����ADC1�����ת����������   
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
    adc_tmp=ADC_GetConversionValue(ADC1);
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);//�ر�ָ����ADC���ת������
    result += adc_tmp;
  }
  result=result/ADC_NUM;   //ȡ8�β�����ƽ��ֵ
  #elif defined (PLATFORM_F405)
  #endif
  //result = result * 3300/4096;
  return result;  //����ƽ��ֵ
}
