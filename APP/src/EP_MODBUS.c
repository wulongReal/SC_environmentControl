#include "EP_MODBUS.h"
#include "vars.h"
#include "string.h"
#include "stdio.h"
#include "mbcrc.h"


void RS485_Sensor_SendByte(uint8_t value)
{
	USART_SendData(USART2,value);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
}
/**********************************
网上采购的ModbusRTU电表表。命令是04，因此使用单独读取方式
发送：1C 04 01 00 00 02 73 BA 
返回：1C 04 04 3D 70 A3 D7 02 5C 
*********************************/
uint8_t EPRecvData[10];
uint8_t EPDataRecvCount = 0;
uint8_t EPDataRecvFlag = 0;
FLOAT_TYPE_T floatData;


void EPMeterRecvClr(void)
{
	EPDataRecvFlag = WT_REC_IDLE;
	EPDataRecvCount = 0;
	memset(EPRecvData, 0, sizeof(EPRecvData));
}

void RS485_EP_Get(uint8_t	addr)
{
	uint8_t i = 0;
	uint16_t crc = 0;
	uint8_t	str[8] = {0x1c, 0x04, 0x01, 0x00, 0x00, 0x02, 0x73, 0xba };

	str[0] = addr;
	crc = usMBCRC16(str, 6);
	
	str[6] = crc & 0xff;
	str[7] = crc >> 8;
	RS485_SENSOR_SEND;
	for(i = 0;i < 8;i++)
		RS485_Sensor_SendByte(str[i]);
	//rt_thread_delay(5);
  	RS485_SENSOR_RECV;
	
}

//电量接收校验
void RS485_EP_RecvProc(void)
{
	uint16_t	crcValue = 0;
	uint16_t	recvCrc = 0;
	uint32_t 	temp = 0;

	
	if((UART2_RecvByte == 0x18) && EPDataRecvCount == 0)
	{
		EPRecvData[0] = 0x18;
		EPDataRecvCount = 1;
		EPDataRecvFlag = WT_REC_ID;
	}
	else if(EPDataRecvFlag == WT_REC_ID)
	{
		if(EPDataRecvCount == 1 && UART2_RecvByte == 0x04)
		{
			EPDataRecvCount = 2;
			EPDataRecvFlag = WT_REC_LEN;
			EPRecvData[1] = UART2_RecvByte;
		}
		else
		{
			EPMeterRecvClr();
		}
	}
	else if(EPDataRecvFlag == WT_REC_LEN)
	{
		EPRecvData[EPDataRecvCount++] = UART2_RecvByte;
		if(EPDataRecvCount >= 9)
		{
			crcValue = usMBCRC16(EPRecvData, 7);
			recvCrc = (EPRecvData[8] << 8) + EPRecvData[7];

			if(recvCrc == crcValue)
			{
				FLAGS_EP_RD_GET_SUC = 1;
				Dev_info[EP_METER - 1].Dev_sta = MB_MRE_NO_ERR;
				//elePowerData[0] = (EPRecvData[3] << 24) + (EPRecvData[4] << 16) + (EPRecvData[5] << 8) + EPRecvData[6];
				floatData.byte[3] = EPRecvData[3];
				floatData.byte[2] = EPRecvData[4];
				floatData.byte[1] = EPRecvData[5];
				floatData.byte[0] = EPRecvData[6];
				//elePowerData[0] = (int64_t)(floatData.f *100);
				temp = (uint32_t)(floatData.f *100);
				Dev_info[EP_METER - 1].Data1 = temp >> 16;
				Dev_info[EP_METER - 1].Data2 = temp & 0xffff;
				EPMeterRecvClr(); 
			}
			else
			{
				EPMeterRecvClr(); 
			}
		}
	}
}


/**********************************
驱动板接收：
head ADDR cmd LEH_H LEN_L OUT1 OUT2 OUT3 OUT4 DA1H DA1L DA2H DA2L DA3H DA3L DA4H DA4L CRCH CRCL tail
head:0XAA  tail 0xbb
ADDR 默认为1，CMD默认为0，LEN为数据帧长度CRCH，CRCL为CRC字段前所有数据的CRC校验

驱动板发送：
head ADDR cmd LEN_H LEN_L IN1 IN2 IN3 IN4 AD1H AD1L AD2H AD2L AD3H AD3L AD4H AD4L CRCH CRCL tail 
*********************************/
/*
uint8_t EPRecvData[10];
uint8_t EPDataRecvCount = 0;
uint8_t m_DriveBoardRecvFlag = 0;

void RS485_TAG_SendByte(uint8_t value)
{
	USART_SendData(UART4,value);
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC) == RESET);
}

void DriveBoardClear(void)
{
	EPDataRecvFlag = WT_REC_IDLE;
	EPDataRecvCount = 0;
	memset(EPRecvData, 0, sizeof(EPRecvData));
}

void RS485_EP_Get(uint8_t	addr)
{
	uint8_t i = 0;
	uint16_t crc = 0;
	uint8_t	str[8] = {0x1c, 0x04, 0x01, 0x00, 0x00, 0x02, 0x73, 0xba };

	str[0] = addr;
	crc = usMBCRC16(str, 6);
	
	str[6] = crc & 0xff;
	str[7] = crc >> 8;
	RS485_SENSOR_SEND;
	for(i = 0;i < 8;i++)
		RS485_Sensor_SendByte(str[i]);
	//rt_thread_delay(5);
  	RS485_SENSOR_RECV;
	
}

//电量接收校验
void RS485_EP_RecvProc(void)
{
	uint16_t	crcValue = 0;
	uint16_t	recvCrc = 0;
	uint32_t 	temp = 0;

	
	if((UART2_RecvByte == 0x18) && EPDataRecvCount == 0)
	{
		EPRecvData[0] = 0x18;
		EPDataRecvCount = 1;
		EPDataRecvFlag = WT_REC_ID;
	}
	else if(EPDataRecvFlag == WT_REC_ID)
	{
		if(EPDataRecvCount == 1 && UART2_RecvByte == 0x04)
		{
			EPDataRecvCount = 2;
			EPDataRecvFlag = WT_REC_LEN;
			EPRecvData[1] = UART2_RecvByte;
		}
		else
		{
			EPMeterRecvClr();
		}
	}
	else if(EPDataRecvFlag == WT_REC_LEN)
	{
		EPRecvData[EPDataRecvCount++] = UART2_RecvByte;
		if(EPDataRecvCount >= 9)
		{
			crcValue = usMBCRC16(EPRecvData, 7);
			recvCrc = (EPRecvData[8] << 8) + EPRecvData[7];

			if(recvCrc == crcValue)
			{
				FLAGS_EP_RD_GET_SUC = 1;
				Dev_info[EP_METER - 1].Dev_sta = MB_MRE_NO_ERR;
				//elePowerData[0] = (EPRecvData[3] << 24) + (EPRecvData[4] << 16) + (EPRecvData[5] << 8) + EPRecvData[6];
				floatData.byte[3] = EPRecvData[3];
				floatData.byte[2] = EPRecvData[4];
				floatData.byte[1] = EPRecvData[5];
				floatData.byte[0] = EPRecvData[6];
				//elePowerData[0] = (int64_t)(floatData.f *100);
				temp = (uint32_t)(floatData.f *100);
				Dev_info[EP_METER - 1].Data1 = temp >> 16;
				Dev_info[EP_METER - 1].Data2 = temp && 0xffff;
				EPMeterRecvClr(); 
			}
			else
			{
				EPMeterRecvClr(); 
			}
		}
	}
}

*/
/**********************************
网上采购的PH,由于读取数据报错，这里使用单独读取
发送：1C 04 01 00 00 02 73 BA 
返回：1C 04 04 3D 70 A3 D7 02 5C 
*********************************/
/*
uint8_t EPRecvData[10];
uint8_t EPDataRecvCount = 0;
uint8_t EPDataRecvFlag = 0;
FLOAT_TYPE_T floatData;


void EPMeterRecvClr(void)
{
	EPDataRecvFlag = WT_REC_IDLE;
	EPDataRecvCount = 0;
	memset(EPRecvData, 0, sizeof(EPRecvData));
}

void RS485_EP_Get(uint8_t	addr)
{
	uint8_t i = 0;
	uint16_t crc = 0;
	uint8_t	str[8] = {0x1c, 0x04, 0x01, 0x00, 0x00, 0x02, 0x73, 0xba };

	str[0] = addr;
	crc = usMBCRC16(str, 6);
	
	str[6] = crc & 0xff;
	str[7] = crc >> 8;
	RS485_SENSOR_SEND;
	for(i = 0;i < 8;i++)
		RS485_Sensor_SendByte(str[i]);
	//rt_thread_delay(5);
  	RS485_SENSOR_RECV;
	
}

//电量接收校验
void RS485_EP_RecvProc(void)
{
	uint16_t	crcValue = 0;
	uint16_t	recvCrc = 0;
	uint32_t 	temp = 0;

	
	if((UART2_RecvByte == 0x18) && EPDataRecvCount == 0)
	{
		EPRecvData[0] = 0x18;
		EPDataRecvCount = 1;
		EPDataRecvFlag = WT_REC_ID;
	}
	else if(EPDataRecvFlag == WT_REC_ID)
	{
		if(EPDataRecvCount == 1 && UART2_RecvByte == 0x04)
		{
			EPDataRecvCount = 2;
			EPDataRecvFlag = WT_REC_LEN;
			EPRecvData[1] = UART2_RecvByte;
		}
		else
		{
			EPMeterRecvClr();
		}
	}
	else if(EPDataRecvFlag == WT_REC_LEN)
	{
		EPRecvData[EPDataRecvCount++] = UART2_RecvByte;
		if(EPDataRecvCount >= 9)
		{
			crcValue = usMBCRC16(EPRecvData, 7);
			recvCrc = (EPRecvData[8] << 8) + EPRecvData[7];

			if(recvCrc == crcValue)
			{
				FLAGS_EP_RD_GET_SUC = 1;
				Dev_info[EP_METER - 1].Dev_sta = MB_MRE_NO_ERR;
				//elePowerData[0] = (EPRecvData[3] << 24) + (EPRecvData[4] << 16) + (EPRecvData[5] << 8) + EPRecvData[6];
				floatData.byte[3] = EPRecvData[3];
				floatData.byte[2] = EPRecvData[4];
				floatData.byte[1] = EPRecvData[5];
				floatData.byte[0] = EPRecvData[6];
				//elePowerData[0] = (int64_t)(floatData.f *100);
				temp = (uint32_t)(floatData.f *100);
				Dev_info[EP_METER - 1].Data1 = temp >> 16;
				Dev_info[EP_METER - 1].Data2 = temp && 0xffff;
				EPMeterRecvClr(); 
			}
			else
			{
				EPMeterRecvClr(); 
			}
		}
	}
}
*/
