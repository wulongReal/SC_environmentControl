/*
 * File      : usart.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

#ifndef __USART_H__
#define __USART_H__



void stm32_hw_usart_init(void);
void USART_Config(uint32_t baud);
//void Read_FloorHeat(void);
//bool FloorHeat_Scm(uint8_t cmd,uint8_t *data,uint8_t len);
//void LIN_RxCompleted(void);
void debug_uart_sendbyte(unsigned char value);
void debug_uart1_sendbyte(unsigned char value);


void UART4_SendDate(uint8_t *pBuf,uint16_t len);
void UART4_EmptyBuf(void);


#endif
