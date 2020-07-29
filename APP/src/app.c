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
  //下面是给各模块开启时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
                         RCC_APB2Periph_GPIOC |    \
                         RCC_APB2Periph_GPIOD |    \
                         RCC_APB2Periph_GPIOE |    \
                         RCC_APB2Periph_ADC1 |     \
                         RCC_APB2Periph_AFIO,
                         ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);     /* CAN Periph clock enable */
  //RCC_APB1PeriphClockCmd(  RCC_APB1Periph_SPI2,  ENABLE );//SPI2时钟使能
  //配置ADC转换时钟
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

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;        // 设置SPI工作模式:设置为主SPI
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;    //设置SPI的数据大小:SPI发送接收8位帧结构
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;          // 选择了串行时钟的稳态:时钟悬空高
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;         // 数据捕获于第二个时钟沿
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;            // NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;    //定义波特率预分频的值:波特率预分频值为16
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;   //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
  SPI_InitStructure.SPI_CRCPolynomial = 7;             // CRC值计算的多项式
  SPI_Init(SPI2, &SPI_InitStructure);                  // 根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
  SPI_Cmd(SPI2, ENABLE); //使能SPI外设    
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
	SENSOR_EN	PB8			//传感器供电控制。1：使能。0：失能
	PWRKEY		PB13
	UART2_DE	PA1			//RS485传感器口。DE
	BEEP		PA8
	5V_EN		PB9
	4V_EN		PB3
	UART4_EN	PA15		//RS485对外口。DE
	
	IN:
	MODULE_STATUS 	PB14
	NET_STATUS		PB15
	CHARGEING		PB7		//充电状态。0：未充电。		1：充电中
	CHARG_DONE		PB6		//充电完成。0：充电中。1：完成
	PLUG_DE			PB5		//电池在位检测，设为上拉输入。1：在外
	断电检测			PC13

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

 //*******************初始化独立看门狗*************************************
//函数定义: void IWDG_Configuration(void) 
//描    述：初始化独立看门狗
//入口参数：无
//出口参数：无
//备    注：分频因子=4*2^prer.但最大值只能是256!时间计算(大概):Tout=40K/((4*2^prer)*rlr)值   2S超时
//Editor：liuqh 2013-1-16  Company: BXXJS
//********************************************** *********************
//static void IWDG_Configuration(void){
void IWDG_Configuration(void){
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//使能对IWDG->PR和IWDG->RLR的写
  IWDG_SetPrescaler(IWDG_Prescaler_256);//128分频，装载2600，则复位超时时间为(128*5000)/40k = 16S
  IWDG_SetReload(5000);
  IWDG_ReloadCounter();
  IWDG_Enable();    
}
//*******************喂独立看门狗*************************************
//函数定义: void IWDG_Feed(void)
//描    述：初始化独立看门狗
//入口参数：无
//出口参数：prer:分频数:0~7(只有低3位有效!)，rlr:重装载寄存器值:低11位有效.
//备    注：分频因子=4*2^prer.但最大值只能是256!时间计算(大概):Tout=40K/((4*2^prer)*rlr)值
//Editor：liuqh 2013-1-16  Company: BXXJS
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

  //  TODO  方便调试，暂时注释看门狗，正式发布时需要打开
 //IWDG_Configuration();
}
#if 0
//****************************防超时程序********************************
//函数定义: uint8_t AvoidTimeout(uint32_t TimeOfTimeout,uint32_t Period,uint8_t (*DetectCondition)())
//描    述：在TimeOfTimeout时间内，每Period时间检测一次DetectCondition()返回的值是否有效
//入口参数：TimeOfTimeout：防超时总时间（单位：systick）
//          Period       ：每Period时间检测一次，即时间因子（单位：systick）
//          (*DetectCondition)()：检测条件，等于ConditionValue则条件满足，检测结束，否则延时Period时间继续检测
//          ConditionValue      ；条件成立的值
//出口参数：0：在TimeOfTimeout时间内，检测到条件成立
//          1：在TimeOfTimeout时间内，没有检测到条件成立
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


//************************************延时函数**************************************
//函数定义: void Delay(vu32 nCount)
//入口参数：nCount ：延时函数中，循环的次数
//出口参数：无
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
  RCC_DeInit();/*将外设RCC寄存器重设为缺省值 */
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);/*设置外部高速晶振（HSE） HSE晶振打开(ON)*/
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if (HSEStartUpStatus == SUCCESS) { /*等待HSE起振,  SUCCESS：HSE晶振稳定且就绪*/
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);/*设置AHB时钟(HCLK)RCC_SYSCLK_Div1——AHB时钟 = 系统时*/
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); /*设置高速AHB时钟(PCLK2)RCC_HCLK_Div1——APB2时钟 = HCLK*/
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);/*设置低速AHB时钟(PCLK1)RCC_HCLK_Div2——APB1时钟 = HCLK / 2*/
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);  //xtal @16Mhz /*设置PLL时钟源及倍频系数*/
    /* Enable PLL */
    RCC_PLLCmd(ENABLE);/*使能PLL */
    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {/*检查指定的RCC标志位(PLL准备好标志)设置与否*/
    }
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);/*设置系统时钟（SYSCLK） */
    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08) { /*0x08：PLL作为系统时钟 */
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


