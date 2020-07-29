// ****** (C) COPYRIGHT 2012-2016 Huimv Technology Co., Ltd ***********************
// * File Name          : APP.C
// * Author             : tanny
// * Date First Issued  : 16/07/05
// * Description        : Main program body.
// ********************************************************************************
// ********************************************************************************
 
// ********************************************************************************

#include "app_task.h"
#include <bsp.h>
#include <rthw.h>
#include <rtthread.h>
#include "usart.h"
#include "gpio.h"
#include "vars.h"




void rt_hw_board_init(){
  BSP_Init();
  
  stm32_hw_pin_init();

  stm32_hw_usart_init();
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the RCC.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void RCC_Configuration(void){
  #if defined(PLATFORM_F103)
  //�����Ǹ���ģ�鿪��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
                         RCC_APB2Periph_GPIOC |    \
                         RCC_APB2Periph_GPIOD |    \
                         RCC_APB2Periph_GPIOE |    \
                         RCC_APB2Periph_ADC1 |     \
                         RCC_APB2Periph_AFIO,
                         ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);     /* CAN Periph clock enable */
  //RCC_APB1PeriphClockCmd(  RCC_APB1Periph_SPI2,  ENABLE );//SPI2ʱ��ʹ��
  //����ADCת��ʱ��
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //72/6=12MHz
  
#if defined(RT_USING_UART1)
  /* Enable UART clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
#elif defined(RT_USING_REMAP_UART1)
  /* Enable UART clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
#endif

#ifdef RT_USING_UART2
  /* Enable UART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
#endif /* RT_USING_UART2 */

#ifdef RT_USING_UART3
  /* Enable UART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
#endif /* RT_USING_UART3 */
#ifdef RT_USING_UART4
  /* Enable UART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
#endif 
#ifdef	RT_USING_UART5
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
  #elif defined (PLATFORM_F405)  


  #endif
}
/*
void TFTSPI_Init(void){
  // TFT
  // SPI_CS PB12; SPI_SCK PB13; SPI_MISO PB14; SPI_MOSI PB15     SPI2
  // SPI_DC PA8;  RESET PA15
  SPI_InitTypeDef  SPI_InitStructure;

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;        // ����SPI����ģʽ:����Ϊ��SPI
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;    //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;          // ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;         // ���ݲ����ڵڶ���ʱ����
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;            // NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;    //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ16
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;   //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
  SPI_InitStructure.SPI_CRCPolynomial = 7;             // CRCֵ����Ķ���ʽ
  SPI_Init(SPI2, &SPI_InitStructure);                  // ����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
  SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����    
}
*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configer NVIC
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void NVIC_Configuration(void){
  //NVIC_InitTypeDef NVIC_InitStructure;
  //#if defined(PLATFORM_F103)
#ifdef  VECT_TAB_RAM
  // Set the Vector Table base location at 0x20000000
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  // VECT_TAB_FLASH  
  // Set the Vector Table base location at 0x08000000
  #ifdef BOOT
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x02400);
  #else
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  #endif
#endif
  //#elif defined (PLATFORM_F405)
  
  //#endif
   //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/


static void GPIO_Configuration(void){
  #if defined(PLATFORM_F103)
  GPIO_InitTypeDef GPIO_InitStructure;
  

    /*
    OUT:	
	LTE_RESET	PB12		
	SENSOR_EN	PB8			//������������ơ�1��ʹ�ܡ�0��ʧ��
	PWRKEY		PB13
	UART2_DE	PA1			//RS485�������ڡ�DE
	BEEP		PA8
	5V_EN		PB9
	4V_EN		PB3
	UART4_EN	PA15		//RS485����ڡ�DE
	
	IN:
	MODULE_STATUS 	PB14
	NET_STATUS		PB15
	CHARGEING		PB7		//���״̬��0��δ��硣		1�������
	CHARG_DONE		PB6		//�����ɡ�0������С�1�����
	PLUG_DE			PB5		//�����λ��⣬��Ϊ�������롣1������
	�ϵ���			PC13

	AD:
	BAT_ADC		PA4
	

	OTHER:
	
  */

	//out
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_15;   
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_15);
	

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_8|GPIO_Pin_9 | GPIO_Pin_12  | GPIO_Pin_13;                
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_8|GPIO_Pin_9 | GPIO_Pin_12  | GPIO_Pin_13);
	BEEP_OFF;


	//in


	GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 

	GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 

	GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	

    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);


	//ADC
	GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);



  #endif

  
	/* Configure CAN pin: RX */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure CAN pin: TX */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
    #if defined(RT_USING_UART1)
    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_RX;
    GPIO_Init(UART1_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_TX;
    GPIO_Init(UART1_GPIO, &GPIO_InitStructure);
  #elif defined(RT_USING_REMAP_UART1)
    /* Configure USART Remap Rx/tx PIN */
    GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_REMAP_RX;
    GPIO_Init(UART1_GPIO_REMAP, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_REMAP_TX;
    GPIO_Init(UART1_GPIO_REMAP, &GPIO_InitStructure);
  #endif /* RT_USING_UART1 */

  #if defined(RT_USING_UART2)
    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_RX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_TX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);
  #endif /* RT_USING_UART2 */

  #if defined(RT_USING_UART3)
    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART3_GPIO_RX;
    GPIO_Init(UART3_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART3_GPIO_TX;
    GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
  #endif /* RT_USING_UART3 */
  #if defined(RT_USING_UART4)
    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART4_GPIO_RX;
    GPIO_Init(UART4_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART4_GPIO_TX;     // 
    GPIO_Init(UART4_GPIO, &GPIO_InitStructure);
  #endif /* RT_USING_UART4 */
    #if defined(RT_USING_UART5)
    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART5_GPIO_RX;
    GPIO_Init(UART5_GPIO_PORT_RX, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART5_GPIO_TX;     // 
    GPIO_Init(UART5_GPIO_PORT_TX, &GPIO_InitStructure);
  #endif /* RT_USING_UART4 */
  #elif defined (PLATFORM_F405)
 

  #endif

}

 //*******************��ʼ���������Ź�*************************************
//��������: void IWDG_Configuration(void) 
//��    ������ʼ���������Ź�
//��ڲ�������
//���ڲ�������
//��    ע����Ƶ����=4*2^prer.�����ֵֻ����256!ʱ�����(���):Tout=40K/((4*2^prer)*rlr)ֵ   2S��ʱ
//Editor��liuqh 2013-1-16  Company: BXXJS
//********************************************** *********************
//static void IWDG_Configuration(void){
void IWDG_Configuration(void){
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//ʹ�ܶ�IWDG->PR��IWDG->RLR��д
  IWDG_SetPrescaler(IWDG_Prescaler_256);//128��Ƶ��װ��2600����λ��ʱʱ��Ϊ(128*5000)/40k = 16S
  IWDG_SetReload(5000);
  IWDG_ReloadCounter();
  IWDG_Enable();    
}
//*******************ι�������Ź�*************************************
//��������: void IWDG_Feed(void)
//��    ������ʼ���������Ź�
//��ڲ�������
//���ڲ�����prer:��Ƶ��:0~7(ֻ�е�3λ��Ч!)��rlr:��װ�ؼĴ���ֵ:��11λ��Ч.
//��    ע����Ƶ����=4*2^prer.�����ֵֻ����256!ʱ�����(���):Tout=40K/((4*2^prer)*rlr)ֵ
//Editor��liuqh 2013-1-16  Company: BXXJS
//*******************************************************************
void IWDG_Feed(void){
  IWDG_ReloadCounter();//reload                         
}


/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void  SysTick_Configuration(void){
  RCC_ClocksTypeDef  rcc_clocks;
  rt_uint32_t         cnts;

  RCC_GetClocksFreq(&rcc_clocks);

  cnts = (rt_uint32_t)rcc_clocks.HCLK_Frequency / RT_TICK_PER_SECOND;

  SysTick_Config(cnts);
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}
/**
 * This is the timer interrupt service routine.
 *
 */
void rt_hw_timer_handler(void){
  /* enter interrupt */
  rt_interrupt_enter();

  rt_tick_increase();

  /* leave interrupt */
  rt_interrupt_leave();
}


void  BSP_Init (void){
  RCC_Configuration();
  NVIC_Configuration();
  SysTick_Configuration();
  GPIO_Configuration();  

  //  TODO  ������ԣ���ʱע�Ϳ��Ź�����ʽ����ʱ��Ҫ��
 //IWDG_Configuration();
}
#if 0
//****************************����ʱ����********************************
//��������: uint8_t AvoidTimeout(uint32_t TimeOfTimeout,uint32_t Period,uint8_t (*DetectCondition)())
//��    ������TimeOfTimeoutʱ���ڣ�ÿPeriodʱ����һ��DetectCondition()���ص�ֵ�Ƿ���Ч
//��ڲ�����TimeOfTimeout������ʱ��ʱ�䣨��λ��systick��
//          Period       ��ÿPeriodʱ����һ�Σ���ʱ�����ӣ���λ��systick��
//          (*DetectCondition)()���������������ConditionValue���������㣬��������������ʱPeriodʱ��������
//          ConditionValue      ������������ֵ
//���ڲ�����0����TimeOfTimeoutʱ���ڣ���⵽��������
//          1����TimeOfTimeoutʱ���ڣ�û�м�⵽��������
//**********************************************************************
uint8_t AvoidTimeout(uint32_t TimeOfTimeout,uint32_t Period,uint8_t (*DetectCondition)(),uint8_t ConditionValue){
  uint32_t LastTimeLocal, CurTimeLocal;
  uint8_t ConditionValueLocal;
  LastTimeLocal = rt_tick_get();
  CurTimeLocal  =  LastTimeLocal;
  while(CurTimeLocal - LastTimeLocal < TimeOfTimeout)  {   
    CurTimeLocal = rt_tick_get();
    ConditionValueLocal = DetectCondition();
    if (ConditionValueLocal == ConditionValue) return 0;
    rt_thread_delay(Period);
  }  
  return 1;
} 


//************************************��ʱ����**************************************
//��������: void Delay(vu32 nCount)
//��ڲ�����nCount ����ʱ�����У�ѭ���Ĵ���
//���ڲ�������
//**********************************************************************************
void Delay(vu32 nCount){
  for(; nCount!= 0;nCount--);
}
#endif


void assert_failed(u8* file, u32 line){
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)  {
  }
}

void PLL_Configuration(void) {
  ErrorStatus HSEStartUpStatus;

  /* RCC system reset(for debug purpose) */
  RCC_DeInit();/*������RCC�Ĵ�������Ϊȱʡֵ */
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);/*�����ⲿ���پ���HSE�� HSE�����(ON)*/
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if (HSEStartUpStatus == SUCCESS) { /*�ȴ�HSE����,  SUCCESS��HSE�����ȶ��Ҿ���*/
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);/*����AHBʱ��(HCLK)RCC_SYSCLK_Div1����AHBʱ�� = ϵͳʱ*/
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); /*���ø���AHBʱ��(PCLK2)RCC_HCLK_Div1����APB2ʱ�� = HCLK*/
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);/*���õ���AHBʱ��(PCLK1)RCC_HCLK_Div2����APB1ʱ�� = HCLK / 2*/
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);  //xtal @16Mhz /*����PLLʱ��Դ����Ƶϵ��*/
    /* Enable PLL */
    RCC_PLLCmd(ENABLE);/*ʹ��PLL */
    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {/*���ָ����RCC��־λ(PLL׼���ñ�־)�������*/
    }
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);/*����ϵͳʱ�ӣ�SYSCLK�� */
    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08) { /*0x08��PLL��Ϊϵͳʱ�� */
    }
  }
}

int main(void){
  rt_uint32_t UNUSED level;

  /* disable interrupt first */
  level = rt_hw_interrupt_disable();

  /* init system setting */

  //SystemInit();
  PLL_Configuration();
 

  /* startup RT-Thread RTOS */
  rtthread_startup();

  return 0;
}


