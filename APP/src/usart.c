/*
 * File    : usart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date       Author     Notes
 * 2009-01-05   Bernard    the first version
 * 2010-03-29   Bernard    remove interrupt Tx and DMA Rx mode
 */

#include <rthw.h>
#include <rtdevice.h>
#include "bsp.h"
#include "usart.h"
#include "vars.h"
#include "rs232.h"
#include <stdio.h>
#include "EP_MODBUS.h"
#include "string.h"

#define UART_ENABLE_IRQ(n)        NVIC_EnableIRQ((n))
#define UART_DISABLE_IRQ(n)       NVIC_DisableIRQ((n))


/*
// USART1 
#define UART1_GPIO_TX        GPIO_Pin_9
#define UART1_GPIO_RX        GPIO_Pin_10
#define UART1_GPIO           GPIOA
// USART1_REMAP 
#define UART1_GPIO_REMAP_TX  GPIO_Pin_6
#define UART1_GPIO_REMAP_RX  GPIO_Pin_7
#define UART1_GPIO_REMAP     GPIOB

// USART2 
#define UART2_GPIO_TX        GPIO_Pin_2
#define UART2_GPIO_RX        GPIO_Pin_3
#define UART2_GPIO           GPIOA


#define UART3_GPIO_TX        GPIO_Pin_10
#define UART3_GPIO_RX        GPIO_Pin_11
#define UART3_GPIO           GPIOB

#define UART4_GPIO_TX        GPIO_Pin_10
#define UART4_GPIO_RX        GPIO_Pin_11
#define UART4_GPIO           GPIOC

#define UART5_GPIO_TX        GPIO_Pin_12
#define UART5_GPIO_PORT_TX		GPIOC

#define UART5_GPIO_RX        GPIO_Pin_2
#define UART5_GPIO_PORT_RX		GPIOD
*/



//static  rt_timer_t Timer_Short, Timer_Long;

/* STM32 uart driver */
struct stm32_uart{
  USART_TypeDef* uart_device;
  IRQn_Type irq;
};

static rt_err_t stm32_configure(struct rt_serial_device *serial, struct serial_configure *cfg){
  struct stm32_uart* uart;
  USART_InitTypeDef USART_InitStructure;

  RT_ASSERT(serial != RT_NULL);
  RT_ASSERT(cfg != RT_NULL);

  uart = (struct stm32_uart *)serial->parent.user_data;

  USART_InitStructure.USART_BaudRate = cfg->baud_rate;

  if (cfg->data_bits == DATA_BITS_8){
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  } else if (cfg->data_bits == DATA_BITS_9) {
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  }

  if (cfg->stop_bits == STOP_BITS_1){
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
  } else if (cfg->stop_bits == STOP_BITS_2){
    USART_InitStructure.USART_StopBits = USART_StopBits_2;
  }

  if (cfg->parity == PARITY_NONE){
    USART_InitStructure.USART_Parity = USART_Parity_No;
  } else if (cfg->parity == PARITY_ODD) {
    USART_InitStructure.USART_Parity = USART_Parity_Odd;
  } else if (cfg->parity == PARITY_EVEN) {
    USART_InitStructure.USART_Parity = USART_Parity_Even;
  }

  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(uart->uart_device, &USART_InitStructure);

  /* Enable USART */
  USART_Cmd(uart->uart_device, ENABLE);
	
	/* enable interrupt */
  //USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);

  return RT_EOK;
}

static rt_err_t stm32_control(struct rt_serial_device *serial, int cmd, void *arg){
  struct stm32_uart* uart;

  RT_ASSERT(serial != RT_NULL);
  uart = (struct stm32_uart *)serial->parent.user_data;

  switch (cmd){
    /* disable interrupt */
  case RT_DEVICE_CTRL_CLR_INT:
    /* disable rx irq */
    UART_DISABLE_IRQ(uart->irq);
    /* disable interrupt */
    USART_ITConfig(uart->uart_device, USART_IT_RXNE, DISABLE);
    break;
    /* enable interrupt */
  case RT_DEVICE_CTRL_SET_INT:
    /* enable rx irq */
    UART_ENABLE_IRQ(uart->irq);
    /* enable interrupt */
    USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
    break;
  //default:   //170107
   // RED_BLINK();
  //  break;
  }
  return RT_EOK;
}

static int stm32_putc(struct rt_serial_device *serial, char c){
  struct stm32_uart* uart;

  RT_ASSERT(serial != RT_NULL);
  uart = (struct stm32_uart *)serial->parent.user_data;

  uart->uart_device->DR = c;
  while (!(uart->uart_device->SR & USART_FLAG_TC));

  return 1;
}

static int stm32_getc(struct rt_serial_device *serial){
  int ch;
  struct stm32_uart* uart;

  RT_ASSERT(serial != RT_NULL);
  uart = (struct stm32_uart *)serial->parent.user_data;

  ch = -1;
  if (uart->uart_device->SR & USART_FLAG_RXNE) {
    ch = uart->uart_device->DR & 0xff;
  }

  return ch;
}

static const struct rt_uart_ops stm32_uart_ops ={
  stm32_configure,
  stm32_control,
  stm32_putc,
  stm32_getc,
};

#if defined(RT_USING_UART1) || defined(RT_USING_REMAP_UART1)
/* UART1 device driver structure */
struct stm32_uart uart1 ={
  USART1,
  USART1_IRQn,
};
struct rt_serial_device serial1;

//USB_TTL
void USART1_IRQHandler(void){
  struct stm32_uart* uart;
  uart = &uart1;
  /* enter interrupt */
  rt_interrupt_enter();
  #if 1
  if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET) {
    //rt_hw_serial_isr(&serial1, RT_SERIAL_EVENT_RX_IND);
    /* clear interrupt */
    //LIN_TmpSCIStatus = LIN_SCSR1;
    //LIN_TmpSCIByte   = uart->uart_device->DR & 0x1FF;
    //LIN_RxCompleted();
    USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
  }
  //#if 0
  if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET) {
    /* clear interrupt */
    USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
  }
  if (USART_GetFlagStatus(uart->uart_device, USART_FLAG_ORE) == SET) {
    stm32_getc(&serial1);
		USART_ClearFlag(uart->uart_device, USART_FLAG_ORE);
  }
  #endif
  
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
/* UART1 device driver structure */
struct stm32_uart uart2 ={
  USART2,
  USART2_IRQn,
};
struct rt_serial_device serial2;
//rs485ä¼ æ„Ÿå™¨
void USART2_IRQHandler(void){
  struct stm32_uart* uart;

  uart = &uart2;
  /* enter interrupt */
  rt_interrupt_enter();
  if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET) {
  	if(FLAGS_EP_CMD_SEND)
  	{
  		UART2_RecvByte = uart->uart_device->DR & 0x1FF;
		RS485_EP_RecvProc();
	}
	else
   	 rt_hw_serial_isr(&serial2, RT_SERIAL_EVENT_RX_IND);
    /* clear interrupt */
    USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
  }
  if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET){
    /* clear interrupt */
    USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
  }
  if (USART_GetFlagStatus(uart->uart_device, USART_FLAG_ORE) == SET){
    stm32_getc(&serial2);
		USART_ClearFlag(uart->uart_device, USART_FLAG_ORE);
  }

  /* leave interrupt */
  rt_interrupt_leave();
}
/*
void debug_uart_sendbyte(unsigned char value)      //???
{
  USART_SendData(USART2,value);
  while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
}

*/

#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
/* UART3 device driver structure */
struct stm32_uart uart3 ={
  USART3,
  USART3_IRQn,
};
struct rt_serial_device serial3;




void debug_uart_sendbyte(unsigned char value)      //???
{
  USART_SendData(USART3,value);
  while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
}

void LTE_RecvData(uint8_t* data,uint16_t lenth)
{
	if(LTE_RecvBuf_Busy == TRUE)
		return;
	if(LTE_RecvLen < LTE_RECV_BUFFER_LEN)
	{
		
		LTE_RecvBuffer[LTE_RecvLen]=LTE_RecvByte;
		/*
		if(LTE_RecvByte == 0)
		{
			LTE_RecvByte = 0x35;
		}
		if(LTE_RecvBuffer[LTE_RecvLen] == 0)
		{
			LTE_RecvBuffer[LTE_RecvLen] = 0x35;
		}
		*/
		LTE_RecvLen++;
		
	}
	if(LTE_RecvLen >= LTE_RECV_BUFFER_LEN - 1)
			LTE_RecvLen = 0;
}
//LTE
void USART3_IRQHandler(void){
  struct stm32_uart* uart;

  uart = &uart3;

  /* enter interrupt */
  rt_interrupt_enter();
  if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET) {
	 LTE_RecvByte = uart->uart_device->DR &0X1FF;
	LTE_RecvData(LTE_RecvBuffer,LTE_RecvLen);
    USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
  }
  if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET) {
    /* clear interrupt */
    USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
  }
  if (USART_GetFlagStatus(uart->uart_device, USART_FLAG_ORE) == SET) {
    //stm32_getc(&serial3); 
		LTE_RecvByte = uart->uart_device->DR &0X1FF;
		USART_ClearFlag(uart->uart_device, USART_FLAG_ORE);
  }

  /* leave interrupt */
  rt_interrupt_leave();
}
#endif /* RT_USING_UART3 */



#if defined(RT_USING_UART4)
struct stm32_uart uart4 ={
  UART4,
  UART4_IRQn,
};
struct rt_serial_device serial4;


void UART4_SendDate(uint8_t *pBuf,uint16_t len)      
{
	uint16_t i = 0;

	for(i = 0;i <len;i++)
	{
		USART_SendData(UART4,pBuf[i]);
  		while(USART_GetFlagStatus(UART4,USART_FLAG_TC) == RESET);
	}
}

void UART4_EmptyBuf(void)
{
	memset(UART4_RecvBuf, 0, UART4_RECV_BUFFER_LEN);
	UART4_RecvLen = 0;
}
void UART4_RecvData(void)
{
	if(UART4_RecvLen < UART4_RECV_BUFFER_LEN)
	{
		
		UART4_RecvBuf[UART4_RecvLen]=UART4_RecvByte;
		UART4_RecvLen++;
	}	
}
//è€³æ ‡
void UART4_IRQHandler(void){
  struct stm32_uart* uart;
  uint8_t temp;

  uart = &uart4;

  /* enter interrupt */
  rt_interrupt_enter();
  if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET) {
  	
	  //esimRecByte = uart->uart_device->DR &0X1FF;
	 // eSim_RecData(RecvBuffer,u16RecvLen);
	 UART4_RecvByte = uart->uart_device->DR &0X1FF;
	  
	 UART4_RecvData();

    USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
  }
  if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET) {
    /* clear interrupt */
    USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
  }
  if (USART_GetFlagStatus(uart->uart_device, USART_FLAG_ORE) == SET) {
    //stm32_getc(&serial4);
		temp = uart->uart_device->DR &0X1FF;
		USART_ClearFlag(uart->uart_device, USART_FLAG_ORE);
  }

  /* leave interrupt */
  rt_interrupt_leave();
}
#endif 



#if defined(RT_USING_UART5)
struct stm32_uart uart5 ={
  UART5,
  UART5_IRQn,
};
struct rt_serial_device serial5;
//LCD
void UART5_IRQHandler(void){
  struct stm32_uart* uart;
   uint8_t temp;

  uart = &uart5;

  /* enter interrupt */
  rt_interrupt_enter();
  if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET) {
	//LIN_TmpSCIStatus = LIN_SCSR1;
	LIN_TmpSCIByte	 = uart->uart_device->DR & 0x1FF;
	LIN_RxCompleted();

    USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
  }
  if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET) {
    /* clear interrupt */
    USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
  }
  if (USART_GetFlagStatus(uart->uart_device, USART_FLAG_ORE) == SET) {
    
		temp = uart->uart_device->DR &0X1FF;
		USART_ClearFlag(uart->uart_device, USART_FLAG_ORE);
  }

  /* leave interrupt */
  rt_interrupt_leave();
}
#endif /* RT_USING_UART3 */


static void Uart_NVIC_Configuration(struct stm32_uart* uart, uint8_t	preemptionPriority, uint8_t	subPriority){
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = uart->irq;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = preemptionPriority;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPriority;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void stm32_hw_usart_init(void){
  struct stm32_uart* uart;
  struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

  //RCC_Configuration();
  //GPIO_Configuration();

#if defined(RT_USING_UART1) || defined(RT_USING_REMAP_UART1)
  uart = &uart1;
  config.baud_rate = USB_TTL_BAUND_RATE;

  serial1.ops  = &stm32_uart_ops;
  serial1.config = config;
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ClearFlag(USART1,USART_FLAG_TXE);
  
  Uart_NVIC_Configuration(&uart1, 0, 1);

  /* register UART1 device */
  rt_hw_serial_register(&serial1, "uart1",
              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX ,
              uart);
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
  uart = &uart2;
  config.baud_rate = RS485_SENSOR_BAUD_RATE;

  serial2.ops  = &stm32_uart_ops;
  serial2.config = config;

  Uart_NVIC_Configuration(&uart2, 0, 0);

  /* register UART1 device */
  rt_hw_serial_register(&serial2, "uart2",
              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
              uart);
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
  uart = &uart3;

  config.baud_rate = LTE_BAUD_RATE;

  serial3.ops  = &stm32_uart_ops;
  serial3.config = config;
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_ClearFlag(USART3,USART_FLAG_TXE);

  Uart_NVIC_Configuration(&uart3, 0, 2);

  /* register UART1 device */
  rt_hw_serial_register(&serial3, "uart3",
              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
              uart);
#endif /* RT_USING_UART3 */

#if defined(RT_USING_UART4)
  uart = &uart4;

  config.baud_rate = RS485_TAG_BAUD_RATE;

  serial4.ops  = &stm32_uart_ops;
  serial4.config = config;
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
  USART_ClearFlag(UART4,USART_FLAG_TXE);

  Uart_NVIC_Configuration(&uart4, 0, 3);

  rt_hw_serial_register(&serial4, "uart4",
              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
              uart);
#endif 
#if defined(RT_USING_UART5)
  uart = &uart5;

  config.baud_rate = LCD_BAUD_RATE;

  serial5.ops  = &stm32_uart_ops;
  serial5.config = config;
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
  USART_ClearFlag(UART5,USART_FLAG_TXE);

  Uart_NVIC_Configuration(&uart5, 0, 4);

  rt_hw_serial_register(&serial5, "uart5",
              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
              uart);
#endif 


}
/*
// µØÅ¯Í¨ÐÅÊý¾Ý Ð£Ñéº¯Êý
uint8_t checksum(uint8_t *arr,uint8_t lenth){
  int i=0;
  uint8_t check=0; 
  uint16_t checksum=0;
  for (i = 0; i < lenth; i++)
    checksum +=arr[i];
  check=checksum & 0xff ^ 0xa5;
  return check;
}
//½ÓÊÕµØÅ¯·µ»Ø     ½ÓÊÕÊý¾ÝÍê³Éºó£¬×¼±¸·¢ËÍ¸øºËÐÄ°å
void Read_FloorHeat(void)
{
    if(Rs485_byte==0x50&&Rs485_Count==0){
      Rs485_Databuf[0]=0x50;
      Rs485_Count=1;
      RS485_StateFlag = RS_REC_READY;
      //FLAGS2_DN_50MS=FALSE;  //¿ªÆô¼ÆÊ±
    }else if(RS485_StateFlag==RS_REC_READY){
      if(Rs485_Count==1&&Rs485_byte==0x10){
        Rs485_Databuf[Rs485_Count++]=0x10;
      }else if(Rs485_Count==2&&Rs485_byte==0){
        Rs485_Databuf[Rs485_Count++]=0x00;
        RS485_StateFlag=RS_REC_ID;
      }
      else{
        Rs485_Count=0;
      }        
    }else if(RS485_StateFlag==RS_REC_ID){
      //Rs485_Count++;
      Rs485_Databuf[Rs485_Count++]=Rs485_byte;
      if(Rs485_Count==11)
        RS485_StateFlag=RS_REC_DATA;
    }else if(RS485_StateFlag==RS_REC_DATA){
        if(Rs485_byte==checksum(Rs485_Databuf,Rs485_Count)){
          Rs485_Databuf[Rs485_Count]=Rs485_byte;
          RS485_StateFlag=RS_REC_END;
          FLAGS2_RS232_SED=TRUE;
          Rs485_Count=0;
        }
        else{
          RS485_StateFlag=RS_REC_ERRO;
          Rs485_Count=0;
        }
    }
}



//µØÅ¯Í¨ÐÅ
//·¢ËÍµØÅ¯Ö¸Áî
bool FloorHeat_Scm(uint8_t cmd,uint8_t *data,uint8_t len){
  
  uint8_t i;
  uint8_t num[20]={0};
  uint8_t lenth=0;
  num[0]=cmd;
  num[1]=0x10;
  num[2]=0x00;
  lenth=len+3;
  for(i=0;i<len;i++){
    num[i+3]=data[i];
  }
  num[lenth]=checksum(num,lenth);
  DIR_OUT();
  for(i=0;i<lenth+1;i++){
    debug_uart_sendbyte(num[i]);
  }
  rt_thread_delay(5);
  DIR_IN();
  return TRUE;  
}
*/
//
void USART_Config(uint32_t baud)
{
  USART_InitTypeDef USART_InitStructure; 
  USART_Cmd(USART3, DISABLE);
  USART_InitStructure.USART_BaudRate =baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3, &USART_InitStructure); 
  USART_Cmd(USART3, ENABLE);
  
}



