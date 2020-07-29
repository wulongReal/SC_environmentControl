/*----------------------------------------------------------------------------
 * Name:    CAN.h
 * Purpose: CAN interface for STM32
 * Version: V1.00
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef _CAN_H_
#define _CAN_H_

#include "platform.h"
#include "vars.h"
#include "stdint.h"

#define STANDARD_FORMAT  0
#define EXTENDED_FORMAT  1

#define DATA_FRAME       0
#define REMOTE_FRAME     1

#define READ_INPUT   0x01
#define RE_INPUT     0x11
#define WRITE_JDQ    0x02
#define READ_JDQ     0x03
#define RE_JDQ       0x13



typedef struct {
  unsigned int id;                 // 29 bit identifier
  unsigned char data[8];            // Data field
  unsigned char len;                // Length of data field in bytes
  unsigned char format;             // 0 - STANDARD, 1- EXTENDED IDENTIFIER
  unsigned char type;               // 0 - DATA FRAME, 1 - REMOTE FRAME
} CAN_msg;

/* Functions defined in module CAN.c */
void CAN_setup(void);
void CAN_init(void);
void CAN_start(void);
void CAN_waitReady(void);
void CAN_wrMsg(CAN_msg *msg);
void CAN_rdMsg(CAN_msg *msg);
void CAN_wrFilter(unsigned int id, unsigned char filter_type);

void CAN_testmode(unsigned int testmode);


//void CAN1_Init(void);
u8	canModeInit(u8	tsjw, u8 tbs2, u8 tbs1,u16 brp, u8	mode);

void CAN_Ctrol(void);

void CAN_TransmitData(uint8_t DataLength, uint8_t *Data_Buf, uint8_t Addr, uint8_t Cmd); 
uint8_t CAN_Recei_Data(uint8_t DataLength,uint8_t *Data_Buf,uint8_t Addr, uint8_t Cmd);

extern CAN_msg CAN_TxMsg;      // CAN messge for sending
extern CAN_msg CAN_RxMsg; // CAN message for receiving                                
extern unsigned int CAN_TxRdy;      // CAN HW ready to transmit a message
extern unsigned int CAN_RxRdy;      // CAN HW received a message

#endif // _CAN_H_
