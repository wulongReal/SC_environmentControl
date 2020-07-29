/* Includes -----------------------通用的c语言头文件-------------------------------------------*/
#include <stdio.h>                                                                                            
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>
#include "Module.h"
#include "usart.h"
#include <rtthread.h>
#include "rtc.h"
#include "vars.h"
#include "app_task.h"
ALIGN(RT_ALIGN_SIZE)

//#include <stm32f10x.h>
////////////////////////////以下是需要移植的函数体以及包含具体移植平台的头文件//////////////////////////////////////////
//#include "Mcu_config.h"
//#include "Serial.h"
ALARM_TYPE   err_type[2] = 
{
	{FALSE,FALSE,FALSE,FALSE,FALSE,FALSE},
	{FALSE,FALSE,FALSE,FALSE,FALSE,FALSE},
};


/*
	喂狗函数，在每个函数里都会及时喂狗，防止程序卡死
*/
void feed_dog(void)
{
	IWDG_Feed();
	return;
}
/*
	延时函数，单位ms
*/

void delay_1ms(uint32_t delay_ms)
{
	//System_Delay_1ms(delay_ms);
	rt_thread_delay(delay_ms);
	return;
}
/*
	通过串口向模组发送指定长度的数据
*/
bool Send_Command_Data(uint8_t* pu8Data, uint16_t u16DataLen)
{
	//UART_Send_AtData_Count(pu8Data , u16DataLen);
	int i;

	for(i=0;i<u16DataLen;i++)
	  debug_uart_sendbyte(pu8Data[i]);
	
	return TRUE;
}
////////////////////////////////////////////////结束//////////////////////////////////////////////////////////////////////////

/*
全局变量，mcu与模组连接的串口接收缓存区 ： RecvBuffer
全局变量，mcu与模组连接的串口接收缓存区数据长度 ： u16RecvLen
可以在mcu与模组连接的串口接收中断函数里使用RecvBuffer接收数据，u16RecvLen累加接收数据的长度
*/
//uint8_t RecvBuffer[RECV_BUFFER_LEN];
//uint16_t u16RecvLen;

bool PDP_ActContext(void)	//Activate PDP context
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL, * pu16RecvLen2 = NULL;
	uint8_t send_temp[50] = "AT+MIPCALL=1,\"cmnet\"\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+MIPCALL=1,"cmnet"
	while(retry_times < 20)
	{
		Empty_Receive_Buffer();
		feed_dog();
		rt_thread_delay(500);
		feed_dog();
			
		retry_times++;

		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		pu16RecvLen2 = m_searchStr(RecvBuffer, "+MIPCALL: 1",u16RecvLen);
		if ((pu16RecvLen!=NULL) && (pu16RecvLen2!=NULL))
		{
			//uart485SendData("0K+MIPCALL,PDP_ActContext");
			return TRUE;
		}
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{	
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
				rt_thread_delay(500);
				feed_dog();

				feed_dog();				
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
			{
				//uart485SendData("ERROR+MIPCALL,PDP_ActContext");
				return FALSE;
			}
				
		}
		
	}

	return FALSE;
}
uint8_t PDP_QueryState(void)  //Query the state of PDP context
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t *pu16RecvLen1 = NULL , *pu16RecvLen2 = NULL;
	uint8_t send_temp[30] = "AT+MIPCALL?\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+MIPCALL?
	while(retry_times < 20)																		//20
	{
		feed_dog();
		rt_thread_delay(500);
		//feed_dog();
		//rt_thread_delay(500);
		retry_times++;
		pu16RecvLen1 = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		pu16RecvLen2 = m_searchStr(RecvBuffer, "+MIPCALL: 1," , u16RecvLen);		
		if ((pu16RecvLen1!=NULL) && (pu16RecvLen2!=NULL))
		{
			//uart485SendData("0K+MIPCALL: 1");
			return 2;
		}
			

		pu16RecvLen1 = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		pu16RecvLen2 = m_searchStr(RecvBuffer, "+MIPCALL: 0,0.0.0.0,0.0.0.0,0.0.0.0", u16RecvLen);			
		if ((pu16RecvLen1!=NULL) && (pu16RecvLen2!=NULL))
		{
			//uart485SendData("0K+MIPCALL: 0,0.0.0.0,0.0.0.0,0.0.0.0");
			return 1;
		}
			
				
		pu16RecvLen1 = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen1!=NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
				rt_thread_delay(500);
				feed_dog();

				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
			{
				//uart485SendData("0K+MIPCALL: ERROR");
				return 0;
			}
				
		}		
	}
	
	return 0;
}
bool PDP_Init(void)
{
	uint8_t state;

	
	state = PDP_QueryState();
	if (state == 1)
	{
		if (PDP_ActContext())
			return TRUE;
	}
	else 	if (state == 2)
		return TRUE;
	

	return FALSE;
}

bool pdp_unInit(void)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL , * pu16RecvLen2 = NULL;
	uint8_t send_temp[30] = "AT+MIPCALL=0\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+MIPCALL=0
	while(retry_times < 20)
	{
		feed_dog();
		delay_1ms(500);
		retry_times++;
		
		pu16RecvLen = m_searchStr(RecvBuffer, "+MIPCALL: 0",u16RecvLen);
		pu16RecvLen2 = m_searchStr(RecvBuffer, "OK",u16RecvLen);		
		if ((pu16RecvLen!=NULL) && (pu16RecvLen2!=NULL))
			return TRUE;

		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen!=NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
		delay_1ms(500);
		feed_dog();
		delay_1ms(500);
				feed_dog();
		delay_1ms(500);				
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;			
		}
	}
	
	return FALSE;
}
/*
the current status of tcp socket
-1: error
0 : this tcp socket is closed
1 : this tcp socket is open
*/
int8_t check_TcpState(uint8_t u8SocketID)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL , * pu16RecvLen2 = NULL;
	char temp_data[4] = {0};
	uint8_t send_temp[30] = "AT+MIPOPEN?\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+MIPOPEN?
	while(retry_times < 100)
	{
		feed_dog();
		delay_1ms(100);
		retry_times++;
		
		pu16RecvLen2 = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen2 != NULL)	
		{			
			temp_data[0] = '0'+u8SocketID;
			pu16RecvLen = m_searchStr(RecvBuffer, temp_data,u16RecvLen);
			if (pu16RecvLen!=NULL)
			{
				return 1;  //u8SocketID socket is open now
			}
			temp_data[0] = '0';
			pu16RecvLen = m_searchStr(RecvBuffer, temp_data,u16RecvLen);
			if (pu16RecvLen!=NULL)
			{
				return 0;  //u8SocketID socket is closed now
			}
		}			
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
		delay_1ms(500);
		feed_dog();
		delay_1ms(500);
				feed_dog();
		delay_1ms(500);		
				feed_dog();				
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				return -1;
		}			
	}
		
	return -1;		
}

bool TCP_SetMode(DATA_FORMAT u8DataFormat, RECV_DATA_MODE u8RecvDataMode, WHETHER_DELETE_PAC_WHEN_READ u8WhetherDelete)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	char send_temp[30] = {0};
	uint8_t i,j,k;
	
	if (u8DataFormat == ASCII_FORMAT)
		i = 0;
	else if (u8DataFormat == HEX_FORMAT)
		i = 1;
	else
		return FALSE;
	
	if (u8RecvDataMode == NOT_CACHE_RECV_DATA)
		j = 0;
	else if (u8RecvDataMode == CACHE_RECV_DATA)
		j = 1;
	else
		return FALSE;
	
	if (u8WhetherDelete == DELETE_PAC_WHEN_READ)
		k = 0;
	else if (u8WhetherDelete == NOT_DELETE_PAC_WHEN_READ)
		k = 1;
	else
		return FALSE;
	
	Empty_Receive_Buffer();
	sprintf(send_temp , "AT+MIPMODE=%d,%d,%d\r" , i,j,k);
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//AT+MIPMODE=0,1,0
	while(retry_times < 20)
	{
		feed_dog();		
		rt_thread_delay(100);feed_dog();//rt_thread_delay(500);feed_dog();		
		retry_times++;
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL)	 
		{
			//uart485SendData("MODE OK");
			return TRUE;
		}
			
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
				rt_thread_delay(500);
				feed_dog();
			
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//retry
			}
			else if (error_times >= 10)
			{
				//uart485SendData("MODE ERR");
				return FALSE;
			}
				
		}
	}
	
	return FALSE;
}

bool TCP_Open(SOCKET_CHANNEL_ID u8SocketID , char* pRemoteIP, uint16_t u16RemotePort)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	char response[20] = "+MIPOPEN: 1,1";
	char send_temp[] = "AT+MIPOPEN=1,\"TCP\",\"122.112.219.87\",3724,7000\r\n";
	uint8_t i;
	//char temp;

	if (u8SocketID == SOCKET_CHANNEL_1)
		i = 1;
	else if (u8SocketID == SOCKET_CHANNEL_2)
		i = 2;
	else if (u8SocketID == SOCKET_CHANNEL_3)
		i = 3;
	else
		return FALSE;
	
	//temp = strlen(send_temp);
	Empty_Receive_Buffer();
	//sprintf(send_temp , "AT+MIPOPEN=%d,\"TCP\",\"%s\",%d\r" , i,pRemoteIP,u16RemotePort);
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	// AT+MIPOPEN=1,"TCP","183.131.13.85",12344
	
	sprintf(response , "+MIPOPEN: %d,1" , i);
	

	while(retry_times < 10)    //150
	{
		feed_dog();
		rt_thread_delay(500);
		feed_dog();

		retry_times++;

		pu16RecvLen = m_searchStr(RecvBuffer, response,u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			//uart485SendData("TCP OPEN 0K ");
			return TRUE;
		}
			
		
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
				rt_thread_delay(500);
				feed_dog();
	
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//retry
			}
			else if (error_times >= 50)
			{
				//uart485SendData("TCP OPEN ERROR");
				return FALSE;
			}
				
		}
	}
	
	return FALSE;
}

bool TCP_Send(SOCKET_CHANNEL_ID u8SocketID, DATA_FORMAT u8DataFormat , uint8_t* pSendData , uint16_t sendDataLen)
{
	uint16_t retry_times = 0 , retry_times2 = 0;
	uint8_t error_times = 0;	
	char send_temp[30] = {0};
	char sendData[1500] = {0};
	uint8_t* pu16RecvLen = NULL , * pu16RecvLen2 = NULL;
	uint8_t i , j;
	uint8_t sendEndFlag = 0x1a;
	
	if (u8SocketID == SOCKET_CHANNEL_1)
		i = 1;
	else if (u8SocketID == SOCKET_CHANNEL_2)
		i = 2;
	else if (u8SocketID == SOCKET_CHANNEL_3)
		i = 3;
	else
		return FALSE;
	
	sprintf(send_temp , "AT+MIPSEND=%d,%d\r" , i,sendDataLen);

	if (u8DataFormat == ASCII_FORMAT)
	{
		memcpy(sendData , pSendData , sendDataLen);
	}
	else if (u8DataFormat == HEX_FORMAT)
	{
		for(j=0; j<sendDataLen; j++)
			sprintf(&sendData[j*2] , "%02x" , pSendData[j]);
	}
	else
		return FALSE;
	
	Empty_Receive_Buffer();
	retry_times = 0 ;
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	// AT+MIPSEND=<Socket_ID>,<len>
	
	while(retry_times < 3000)
	{
		delay_1ms(5);
		retry_times++;
		pu16RecvLen = m_searchStr(RecvBuffer, ">",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			delay_1ms(50);
			Send_Command_Data((uint8_t*)sendData , strlen(sendData));
			delay_1ms(100);
			Send_Command_Data(&sendEndFlag , 1);

			retry_times2 = 0;
			while(retry_times2 < 3000)
			{
				retry_times2++;
				delay_1ms(5);
				pu16RecvLen = m_searchStr(RecvBuffer, "+MIPSEND: 1,",u16RecvLen);
				pu16RecvLen2 = m_searchStr(RecvBuffer, "OK",u16RecvLen);
				if ((pu16RecvLen!=NULL) && (pu16RecvLen2!=NULL))
					return TRUE;
			}
			if (retry_times2 >= 3000)
				return FALSE;
		}
		
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
		delay_1ms(500);
		feed_dog();
		delay_1ms(500);
				feed_dog();
		delay_1ms(500);		
				feed_dog();				
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}

	return FALSE;
}

bool TCP_MIPSTRS_Send(SOCKET_CHANNEL_ID u8SocketID, DATA_FORMAT u8DataFormat , uint8_t* pSendData , uint16_t sendDataLen)
{
	uint16_t retry_times = 0;
	uint8_t error_times = 0;	
	char send_temp[100] = {0};
	uint8_t* pu16RecvLen = NULL;
	uint8_t* rec_ok = NULL;
//	uint16_t i;
	
	if (sendDataLen > 1400)
		return FALSE;

	sprintf(send_temp , "AT+MIPSTRS=1,0,\"%s\"\r\n" , pSendData);
	
	
	Empty_Receive_Buffer();	
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//AT+MIPSTRS=1,1,"

	while(retry_times < 10)
	{	
		feed_dog();	
		rt_thread_delay(200);
		feed_dog();		
		rt_thread_delay(200);
		feed_dog();		

		retry_times++;
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen != NULL)
		{

			rec_ok = m_searchStr(RecvBuffer, "+MIPDATA:",u16RecvLen);
			if (rec_ok != NULL)
			{
				//uart485SendData("rec 0K");
				return TRUE;
			}
		}
			
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
		//	uart485SendData("TCP in error_times = %d",error_times);
			error_times++;
			if (error_times == 1)
			{
				rt_thread_delay(500);
				feed_dog();		
				rt_thread_delay(500);
				feed_dog();	

				Empty_Receive_Buffer();	
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//retry
			}
			else if (error_times >= 5)
			{
				//uart485SendData("TCP OPEN ERR");
				return FALSE;
			}
			
		}
		//uart485SendData("TRY AGAIN");
	}
//	uart485SendData("TCP DATA  ERR");
	return FALSE;
}

/*
{
	uint16_t retry_times = 0;
	uint8_t error_times = 0;	
	char send_temp[1500] = {0};
	uint8_t* pu16RecvLen = NULL;
	uint16_t i;
	
	if (sendDataLen > 1400)
		return FALSE;

	if (u8SocketID == SOCKET_CHANNEL_1)
		i = 1;
	else if (u8SocketID == SOCKET_CHANNEL_2)
		i = 2;
	else if (u8SocketID == SOCKET_CHANNEL_3)
		i = 3;
	else
		return FALSE;		
	
	if (u8DataFormat == ASCII_FORMAT) //ascii
	{
		sprintf(send_temp , "AT+MIPSTRS=%d,0,\"%s\"\r" , i,pSendData);
	}
	else if (u8DataFormat == HEX_FORMAT)  //hex
	{
		sprintf(send_temp , "AT+MIPSTRS=%d,1,\"" , i);
		for (i=0; i<sendDataLen; i++)
			sprintf(&send_temp[16+i*2] , "%02x" , pSendData[i]);
		send_temp[16+sendDataLen*2] = '"';
		send_temp[16+sendDataLen*2+1] = '\r';
	}
	else
		return FALSE;
	
	Empty_Receive_Buffer();	
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//AT+MIPSTRS=1,1,"
	while(retry_times < 3000)
	{
		feed_dog();		
		delay_1ms(500);
		retry_times++;
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen != NULL)
			return TRUE;

		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(500);	
				feed_dog();		
				delay_1ms(500);	
				feed_dog();	
				delay_1ms(500);	
				feed_dog();	
				Empty_Receive_Buffer();	
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}

	return FALSE;
}
*/


bool TCP_SetTransMode(SOCKET_CHANNEL_ID u8SocketID , TRANS_NODE u8TransMode , uint16_t thresholdLen)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;
	uint8_t* pu16RecvLen = NULL;
	char send_temp[100] = {0};
	uint8_t i , j;
	
	if (u8SocketID == SOCKET_CHANNEL_1)
		i = 1;
	else if (u8SocketID == SOCKET_CHANNEL_2)
		i = 2;
	else if (u8SocketID == SOCKET_CHANNEL_3)
		i = 3;
	else
		return FALSE;
	
	if (u8TransMode == CONFIRMATION_MODE)
		j = 1;
	else if (u8TransMode == TIMEOUT_MODE)
		j = 2;
	else if (u8TransMode == BUFFER_FULL_MODE)
		j = 3;	
	else if (u8TransMode == AUTOMATIC_MODE)
		j = 4;
	else
		return FALSE;
	
	Empty_Receive_Buffer();
	sprintf(send_temp , "AT+MIPTCFG=%d,%d,%d\r" , i,j,thresholdLen);
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//AT+MIPTCFG = 1,3,10
	while(retry_times < 6)
	{
		feed_dog();
		delay_1ms(500);
		retry_times++;
		
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL)		 
			return TRUE;

		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
		delay_1ms(500);
		feed_dog();
		delay_1ms(500);
				feed_dog();
		delay_1ms(500);		
				feed_dog();
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}
	
	return FALSE;			
}
bool TCP_EnterTransMode(SOCKET_CHANNEL_ID u8SocketID)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;
	uint8_t* pu16RecvLen1 = NULL , * pu16RecvLen2 = NULL;
	char send_temp[30] = {0};	
	char respBuff[30] = {0};
	uint8_t i;
	
	if (u8SocketID == SOCKET_CHANNEL_1)
		i = 1;
	else if (u8SocketID == SOCKET_CHANNEL_2)
		i = 2;
	else if (u8SocketID == SOCKET_CHANNEL_3)
		i = 3;
	else
		return FALSE;	
	
	Empty_Receive_Buffer();
	sprintf(send_temp , "AT+MIPTRSP=%d\r" , i);
	sprintf(respBuff , "+RMIPTRSP: %d,1" , i);
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	// AT+MIPTRSP=1
	while(retry_times < 3)
	{
		feed_dog();
		delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
		retry_times++;
		pu16RecvLen1 = m_searchStr(RecvBuffer, respBuff ,u16RecvLen);
		pu16RecvLen2 = m_searchStr(RecvBuffer, ">",u16RecvLen);		
		if ((pu16RecvLen1!=NULL) && (pu16RecvLen2!=NULL))
			return TRUE;

		pu16RecvLen1 = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen1 != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
		delay_1ms(500);
		feed_dog();
		delay_1ms(500);
				feed_dog();
		delay_1ms(500);		
				feed_dog();				
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}	
	
	return FALSE;
}
//+MIPTRSP: 1,100,4095
bool TCP_TransSendData(SOCKET_CHANNEL_ID u8SocketID , uint8_t* pData , uint16_t DataLen)
{
	uint8_t retry_times = 0;
	uint8_t* pu16RecvLen;
	char respBuff[30] = {0};
	uint8_t i;
	
	if (u8SocketID == SOCKET_CHANNEL_1)
		i = 1;
	else if (u8SocketID == SOCKET_CHANNEL_2)
		i = 2;
	else if (u8SocketID == SOCKET_CHANNEL_3)
		i = 3;
	else
		return FALSE;	
	
	Empty_Receive_Buffer();
	sprintf(respBuff , "+MIPTRSP: %d," , i);
	Send_Command_Data(pData , DataLen);
	while(retry_times < 30)
	{
		feed_dog();
		delay_1ms(300);
		retry_times++;
		pu16RecvLen = m_searchStr(RecvBuffer, respBuff,u16RecvLen);
		if (pu16RecvLen != NULL)
			return TRUE;
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
			return FALSE;
	}	
	
	return FALSE;
}

bool TCP_ExistTransMode(SOCKET_CHANNEL_ID u8SocketID)
{
	uint8_t retry_times = 0;
	uint8_t* pu16RecvLen = NULL;
	char send_temp[30] = {0};	
	char respBuff[30] = {0};
	uint8_t i;
	
	if (u8SocketID == SOCKET_CHANNEL_1)
		i = 1;
	else if (u8SocketID == SOCKET_CHANNEL_2)
		i = 2;
	else if (u8SocketID == SOCKET_CHANNEL_3)
		i = 3;
	else
		return FALSE;	
	
	Empty_Receive_Buffer();
	strcpy(send_temp , "+++");
	sprintf(respBuff , "+RMIPTRSP: %d,0" , i);
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	// +++
	while(retry_times < 30)
	{
		feed_dog();
		delay_1ms(300);
		retry_times++;
		pu16RecvLen = m_searchStr(RecvBuffer, respBuff ,u16RecvLen);
		if (pu16RecvLen != NULL)
			return TRUE;
	}	
	
	return FALSE;
}

bool TCP_Recv(uint8_t u8SocketID)
{
	return TRUE;
}
bool TCP_Close(SOCKET_CHANNEL_ID u8SocketID)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;
	uint8_t* pu16RecvLen = NULL , * pu16RecvLen2 = NULL;
	char response[20] = {0};
	char send_temp[30] = {0};
	uint8_t i;
	
	if (u8SocketID == SOCKET_CHANNEL_1)
		i = 1;
	else if (u8SocketID == SOCKET_CHANNEL_2)
		i = 2;
	else if (u8SocketID == SOCKET_CHANNEL_3)
		i = 3;
	else
		return FALSE;
	
	Empty_Receive_Buffer();
	sprintf(send_temp , "AT+MIPCLOSE=%d\r" , i);
	sprintf(response , "+MIPCLOSE: %d" , i);
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//AT+MIPCLOSE=1
	while(retry_times < 30)
	{
		feed_dog();
		delay_1ms(500);
		retry_times++;		
		pu16RecvLen = m_searchStr(RecvBuffer, response, u16RecvLen);
		pu16RecvLen2 = m_searchStr(RecvBuffer, "OK", u16RecvLen);		
		if ((pu16RecvLen!=NULL) && (pu16RecvLen2!=NULL))
			return TRUE;

		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen!=NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
		delay_1ms(500);
		feed_dog();
		delay_1ms(500);
				feed_dog();
		delay_1ms(500);			
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}

	return FALSE;	
}

uint8_t TCP_Query_UnReadPacksNum(void)
{
	uint16_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	char temp_data[4] = {0};
	uint8_t num;
	uint8_t send_temp[30] = "AT+MIPDQ\r";

	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+MIPDQ
	while(retry_times < 1000)
	{
		feed_dog();
		delay_1ms(10);
		retry_times++;
		
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL)
		{		 
			pu16RecvLen = m_searchStr(RecvBuffer, "+MIPDQ:",u16RecvLen);
			pu16RecvLen += 8;
			memset(temp_data,0,4);	
			num = 0;
			while(*pu16RecvLen != ',')
			{
				temp_data[num++] = *pu16RecvLen++;
				if (num >= 4)
					return 0;
			}
			num = atoi(temp_data);
			if (num > 60)
				return 0;
			return num;
		}
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{	
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
		delay_1ms(500);
		feed_dog();
		delay_1ms(500);
				feed_dog();
		delay_1ms(500);		
				feed_dog();				
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				return 0;
		}
	}
	
	return 0;
}

uint16_t TCP_Read_UnReadPacks(uint8_t *pReadDataBuff , uint16_t ReadDataMaxNum)
{
	uint16_t retry_times = 0;
	uint8_t error_times = 0;
	uint8_t* pu16RecvLen = NULL, * pu16RecvLen2 = NULL;
	uint8_t* pTempIndex;
	char temp_data[8] = {0};
	uint16_t num;
	uint16_t packNum;
	uint8_t send_temp[30] = "AT+MIPDR=0\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+MIPDR=0
	while(retry_times < 1000)
	{
		feed_dog();
		delay_1ms(10);
		retry_times++;
		pu16RecvLen = m_searchStr(RecvBuffer, "+MIPDR:",u16RecvLen);
		pu16RecvLen2 = m_searchStr(RecvBuffer, "OK",u16RecvLen);		
		if ((pu16RecvLen!=NULL) && (pu16RecvLen2!=NULL))//+MIPDR: 1,1,129,{"header"}
		{		 
			pu16RecvLen += 8;
			num = 0;
			pTempIndex = pu16RecvLen;
			while(1)
			{
				if ((pu16RecvLen-pTempIndex) > 20) 
					return 0;
				if (num >= 2)
					break;
				if (*pu16RecvLen++ == ',')
					num++;
			}
			memset(temp_data,0,8);	
			num = 0;
			while(*pu16RecvLen != ',')
			{
				temp_data[num++] = *pu16RecvLen++;
				if (num >= 5)
					return 0;
			}
			pu16RecvLen += 1;
			packNum = atoi(temp_data);
			if (packNum > ReadDataMaxNum)
				return 0;
			num = u16RecvLen-((uint8_t*)pu16RecvLen-RecvBuffer);			
			if (num >= packNum)
			{
				memcpy(pReadDataBuff , pu16RecvLen , packNum);		
				return packNum;
			}
		}
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
		delay_1ms(500);
		feed_dog();
		delay_1ms(500);
				feed_dog();
		delay_1ms(500);		
				feed_dog();				
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				return 0;
		}
	}

	return 0;
}
/*
uint16_t TCP_Read_MIPDATA(uint8_t *pReadDataBuff , uint16_t ReadDataMaxNum , uint32_t u32TimeOut)
{
	uint16_t retry_times = 0;
	uint8_t* pu16RecvLen = NULL;
	uint8_t* pTempIndex;
	char temp_data[8] = {0};
	uint16_t num;
	uint16_t packNum;
	
	while(retry_times < u32TimeOut)
	{
		feed_dog();		
		delay_1ms(1);
		retry_times++;	
		pu16RecvLen = m_searchStr(RecvBuffer, "+MIPDATA:",u16RecvLen);
		if (pu16RecvLen != NULL)//+MIPDATA: 1,136,
		{		 
			pu16RecvLen += 10;
			num = 0;
			pTempIndex = pu16RecvLen;
			while(1)
			{
				if ((pu16RecvLen-pTempIndex) > 20) 
					return 0;
				if (num >= 1)
					break;
				if (*pu16RecvLen++ == ',')
					num++;
			}
			memset(temp_data,0,8);	
			num = 0;
			while(*pu16RecvLen != ',')
			{
				temp_data[num++] = *pu16RecvLen++;
				if (num >= 5)
					return 0;
			}
			pu16RecvLen += 1;
			packNum = atoi(temp_data);
			packNum *= 2;
			if (packNum > ReadDataMaxNum)
				return 0;
			num = u16RecvLen-((uint8_t*)pu16RecvLen-RecvBuffer);			
			if (num >= packNum)
			{
				memcpy(pReadDataBuff , pu16RecvLen , packNum);		
				return packNum;
			}
		}
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
				return 0;
	}

	return 0;
}*/



bool Module_WarmReset(void)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[30] = "at+cfun=1,1\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//at+cfun=1,1
	while(retry_times < 10)
	{
		feed_dog();		
		feed_dog();
		delay_1ms(500);
		feed_dog();
		delay_1ms(500);
				feed_dog();
		delay_1ms(1000);feed_dog();
		delay_1ms(500);
		feed_dog();
		delay_1ms(500);
				feed_dog();
		delay_1ms(500);				
		retry_times++;
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL)
			return TRUE;

		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				feed_dog();
		delay_1ms(500);
		feed_dog();
		delay_1ms(500);
				feed_dog();
		delay_1ms(500);		
				feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}

	return FALSE;
}
/*
0: Digital format
1: Character format
*/
bool Module_ResponseFormat(void)
{
	uint8_t retry_times = 0;
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[30] = "ATV1\r";
	
	while(retry_times < 20)   //20
	{
		Empty_Receive_Buffer();
		Send_Command_Data(send_temp , strlen((char*)send_temp));	//ATV1  set module response type Character format
		feed_dog();
		rt_thread_delay(500);
		feed_dog();
		//rt_thread_delay(500);
		retry_times++;
		
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL){	
			//uart485SendData("ATV1 SUCCED");
			return TRUE;
		}
	}
	//uart485SendData("ATV1 ERRO!!");
	return FALSE;	
}

bool Module_softRest(void)
{
	uint8_t retry_times = 0;
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[30] = "AT+CFUN=1,1\r";
	
	while(retry_times < 5)   //20
	{
		Empty_Receive_Buffer();
		Send_Command_Data(send_temp , strlen((char*)send_temp));	//ATV1  set module response type Character format
		feed_dog();
		rt_thread_delay(500);
		feed_dog();
		//rt_thread_delay(500);
		retry_times++;
		
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL){	
			//uart485SendData("ATV1 SUCCED");
			return TRUE;
		}
	}
	//uart485SendData("ATV1 ERRO!!");
	return FALSE;	
}

bool Module_Check_CPIN(void)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[30] = "AT+CPIN?\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+CPIN?  Check whether the PIN needed
	while(retry_times < 10)
	{
		feed_dog();		
		delay_1ms(500);
		retry_times++;
		
		pu16RecvLen = m_searchStr(RecvBuffer, "READY",u16RecvLen);
		if (pu16RecvLen!=NULL)		 
			return TRUE;

		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{	
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
				continue;
			}
			else if (error_times >= 2)
				return FALSE;
		}
		pu16RecvLen = m_searchStr(RecvBuffer, "SIM PIN",u16RecvLen);
		if (pu16RecvLen!=NULL)			 	
			return FALSE;

		pu16RecvLen = m_searchStr(RecvBuffer, "SIM PUK",u16RecvLen);
		if (pu16RecvLen!=NULL)
			return FALSE;

		pu16RecvLen = m_searchStr(RecvBuffer, "SIM PIN2",u16RecvLen);
		if (pu16RecvLen!=NULL)		 		
			return FALSE;
		
		pu16RecvLen = m_searchStr(RecvBuffer, "SIM PUK2",u16RecvLen);
		if (pu16RecvLen!=NULL)	 	
			return FALSE;
		
		pu16RecvLen = m_searchStr(RecvBuffer, "BLOCK",u16RecvLen);
		if (pu16RecvLen!=NULL)	 
			return FALSE;		
	}
	
	return FALSE;
}

bool Module_Get_SignalQuality(int8_t* pRawSignalNum)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pIndex = NULL;
	int8_t rawSignalNum;
	//int8_t dBmSignalNum;
	int8_t signalnum = 0;
	char data[5];
	uint8_t num = 0 , i;
	uint8_t send_temp[20] = "AT+CSQ\r";
	//char rssiStr[20] = {0};
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+CSQ
	while(retry_times < 60)
	{
		feed_dog();		
		delay_1ms(100);
		retry_times++;
		pIndex = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pIndex!=NULL)
		{
			memset(data , 0 ,5);
			num = 0;
			pIndex = m_searchStr(RecvBuffer, ",",u16RecvLen);
			pIndex--;
			while(1)
			{
				if((*pIndex>='0') && (*pIndex<='9'))
				{
						pIndex--;
						num++;
				}
				else
				{
						pIndex++;
						break;
				}
				if (num > 3)
					goto signalerror;
			}
			for (i=0; i<num; i++)
				data[i] = *pIndex++;
			signalnum = atoi(data);
			if ((signalnum>99) || (signalnum<0))
				goto signalerror;
			rawSignalNum = signalnum;

			//signalnum = (signalnum<<1)-110; //将信号强度转化为dBm单位数值
			//dBmSignalNum = signalnum;
			
			*pRawSignalNum = rawSignalNum;
			//sprintf(rssiStr,"rssi : %d\n", *pRawSignalNum);
			//uart485SendData(rssiStr);
			return TRUE;
		}
		pIndex = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pIndex != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				goto signalerror;
		}
	}

signalerror:
	return FALSE;
}

bool Module_Get_Imsi(char* pImsi)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[30] = "AT+CIMI\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+CIMI
	while(retry_times < 10)
	{
		feed_dog();		
		delay_1ms(100);
		retry_times++;		
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL)
		{
			pu16RecvLen -= 19;
			memcpy(pImsi , pu16RecvLen , 15);
			return TRUE;
		}
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();//delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				goto getImsiError;
		}
	}

getImsiError:
	return FALSE;
}
bool Module_Get_Imei(char* pImei)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[20] = "AT+CGSN\r";
	
	retry_times = 0;
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+CGSN
	while(retry_times < 60)
	{
		delay_1ms(100);
		feed_dog();
		retry_times++;

		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL)
		{
			pu16RecvLen -= 19;
			memcpy(pImei , pu16RecvLen , 15);
			return TRUE;
		}
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(500);feed_dog();//delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				goto getImeiError;
		}			
	}

getImeiError:
	return FALSE;	
}
//+EID: "131502050201C0009067"
bool Module_Get_Eid(char* pEid)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[30] = "AT+EID?\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+EID?
	while(retry_times < 60)
	{
		feed_dog();		
		delay_1ms(100);
		retry_times++;

		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL)
		{
			pu16RecvLen = m_searchStr(RecvBuffer, "+EID:",u16RecvLen);
			if (pu16RecvLen == NULL)
				goto getEidError;
			pu16RecvLen += 6;
			if (*pu16RecvLen != '"')
				goto getEidError;
			pu16RecvLen++;
			if (*(pu16RecvLen+20) != '"')
				goto getEidError;	
			memcpy(pEid , pu16RecvLen , 20);		
			return TRUE;
		}
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp)); //retry
			}
			else if (error_times >= 2)
				goto getEidError;
		}			
	}

getEidError:
	return FALSE;	
}
bool Module_PowerDown(void)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[30] = "AT+CPOF\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+CPOF
	while(retry_times < 10)
	{
		feed_dog();		
		delay_1ms(2000);
		retry_times++;
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL)	
			return TRUE;
		
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));		//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}
	
	return FALSE;
}
bool Module_Echo_Off(void)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[30] = "ATE0\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//ATE0
	while(retry_times < 6)
	{
		feed_dog();
		delay_1ms(500);
		retry_times++;

		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL)		
			return TRUE;

		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}		
	}
	
	return FALSE;		
}
bool Module_Echo_On(void)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[10] = "ATE1\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));		//ATE1
	while(retry_times < 5)
	{
		feed_dog();
		rt_thread_delay(1000);
		retry_times++;
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL){
			//uart485SendData("ATE1 0K");
			return TRUE;
		}
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{	
			error_times++;
			if (error_times == 1)
			{
				rt_thread_delay(500);
				feed_dog();
				//rt_thread_delay(500);
				//feed_dog();
				//rt_thread_delay(500);
				//feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}
	//uart485SendData("ATE1 ERRO");
	return FALSE;	
}
bool Module_SetBaudrate(uint32_t u32Baudrate)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	char send_temp[50] = {0};
	
	Empty_Receive_Buffer();
	sprintf(send_temp , "AT+IPR=%d\r" , u32Baudrate);
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	// AT+IPR=115200
	while(retry_times < 6)
	{
		feed_dog();		
		delay_1ms(500);
		retry_times++;		
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen != NULL)	 
			return TRUE;

		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}
	
	return FALSE;		
}

bool Module_Set_Sleep(void)
{
	uint8_t retry_times = 0;	
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[20] = "AT+S32K=0\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+S32K=0
	while(retry_times < 10)
	{
		feed_dog();		
		delay_1ms(500);
		feed_dog();		
		
		retry_times++;
		
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL){
			//uart485SendData("AT+S32K=0 0K");
			return TRUE;
		}
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{	
			error_times++;
			if (error_times == 1)
			{
				rt_thread_delay(500);
				feed_dog();
				//rt_thread_delay(500);
				//feed_dog();
				//rt_thread_delay(500);
				//feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}
	//uart485SendData("AT+S32K ERRO!!");
	return FALSE;	
}
bool Module_SleepWakeup_DSR(void)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[20] = "AT&S=1\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT&S=1
	while(retry_times < 6)
	{
		feed_dog();		
		delay_1ms(500);
		retry_times++;
		
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL)
			return TRUE;

		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{	
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}
	
	return FALSE;	
}
bool Module_Attach_Net(void)
{
	uint8_t retry_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	uint8_t send_temp[20] = "at+cgatt?\r";
	
	while(retry_times < 100)   //30
	{
		Empty_Receive_Buffer();
		retry_times++;
		Send_Command_Data(send_temp , strlen((char*)send_temp));	//at+cgatt?
		feed_dog();
		rt_thread_delay(200);
		feed_dog();
		//rt_thread_delay(500);
		//feed_dog();

		//delay_1ms(500);


		pu16RecvLen = m_searchStr(RecvBuffer, "+CGATT: 1",u16RecvLen);
		if (pu16RecvLen!=NULL)
		{
			pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
			if (pu16RecvLen!=NULL){
				//uart485SendData("at+cgatt? 0K");
				return TRUE;
			}
		}
	}
	//uart485SendData("at+cgatt? ERRO!!");
	return FALSE;
}


//+CCED: 460,00,58b5,5027,20,7,16,99
bool  Module_Get_CellID(Cell_Info* pCellInfo)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;	
	uint8_t* pu16RecvLen = NULL;
	uint8_t* pTempIndex = NULL;
	char *stop;
	char temp_Data[10];
	uint32_t tempNum;
	uint16_t u16TempLac;  
	uint16_t u16TempCellID;
	uint8_t send_temp[20] = "AT+CCED=0,1\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data(send_temp , strlen((char*)send_temp));	//AT+CCED=0,1
	while(retry_times < 60)
	{
		feed_dog();
		delay_1ms(100);
		retry_times++;
		pu16RecvLen = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pu16RecvLen!=NULL)
		{
			pu16RecvLen = m_searchStr(RecvBuffer, "+CCED:",u16RecvLen);
			if (pu16RecvLen == NULL)
				goto celliderror;
			
			pu16RecvLen = find_comma(pu16RecvLen , 2 , 16);
			if (pu16RecvLen == NULL)
				goto celliderror;	
			pu16RecvLen++;
			pTempIndex = find_comma(pu16RecvLen , 1 , 6);
			if (pTempIndex == NULL)
				goto celliderror;				
			memset(temp_Data , 0 ,10);
			memcpy(temp_Data , pu16RecvLen , pTempIndex-pu16RecvLen);
			tempNum = strtol(temp_Data, &stop, 16);
			if (tempNum > 65535)
				goto celliderror;	
			u16TempLac = tempNum;	
			
			pTempIndex++;
			pu16RecvLen = pTempIndex;
			pTempIndex = find_comma(pu16RecvLen , 1 , 6);
			if (pTempIndex == NULL)
				goto celliderror;				
			memset(temp_Data , 0 ,10);
			memcpy(temp_Data , pu16RecvLen , pTempIndex-pu16RecvLen);
			tempNum = strtol(temp_Data, &stop, 16);
			if (tempNum > 65535)
				goto celliderror;	
			u16TempCellID = tempNum;
			
			pCellInfo->u16Lac = u16TempLac;
			pCellInfo->u16CellID = u16TempCellID;
			
			return TRUE;
		}
		pu16RecvLen = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pu16RecvLen != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();
				Empty_Receive_Buffer();
				Send_Command_Data(send_temp , strlen((char*)send_temp));	//retry
			}
			else if (error_times >= 2)
				goto celliderror;
		}
	}

celliderror:
	return FALSE;
}

////////////////////////////////////////////////HTTP///////////////////////////////////////////////////////////////
bool HTTP_Set_Method(HTTP_METHOD u8HttpMethod)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;
	uint8_t* pIndex = NULL;
	char send_temp[150] = {0};
	
	if (u8HttpMethod == HTTP_GET_METHOD)
		strcpy(send_temp , "AT+HTTPMETHOD=0\r");
	else if (u8HttpMethod == HTTP_POST_METHOD)
		strcpy(send_temp , "AT+HTTPMETHOD=1\r");
	else if (u8HttpMethod == HTTP_HEAD_METHOD)
		strcpy(send_temp , "AT+HTTPMETHOD=2\r");
	else
		return FALSE;
	
	Empty_Receive_Buffer();
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	// AT+HTTPMETHOD=1
	while(retry_times < 90)
	{
		feed_dog();
		delay_1ms(100);
		retry_times++;
		
		pIndex = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pIndex != NULL)
			break;
		pIndex = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pIndex != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//retry
			}
			else if (error_times >= 2)
				return FALSE;
		}		
	}		

	return FALSE;
}


bool HTTP_Set_Para(HTTP_PARA_NAME u8HttpParaName , char* pParaValue)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;
	uint8_t* pIndex = NULL;
	char send_temp[150] = {0};
	
	switch(u8HttpParaName)
	{
		case 0:
			sprintf(send_temp , "AT+HTTPPARA=\"uri\",\"%s\"\r" , pParaValue);
			break;
		case 1:
			sprintf(send_temp , "AT+HTTPPARA=\"Content_Type\",\"%s\"\r" , pParaValue);
			break;
		case 2:
			sprintf(send_temp , "AT+HTTPPARA=\"Accept\",\"%s\"\r" , pParaValue);
			break;		
		case 3:
			sprintf(send_temp , "AT+HTTPPARA=\"Accept_Language\",\"%s\"\r" , pParaValue);
			break;
		case 4:
			sprintf(send_temp , "AT+HTTPPARA=\"User_Agent\",\"%s\"\r" , pParaValue);
			break;
		case 5:
			sprintf(send_temp , "AT+HTTPPARA=\"Referer\",\"%s\"\r" , pParaValue);
			break;
		case 6:
			sprintf(send_temp , "AT+HTTPPARA=\"Connection\",\"%s\"\r" , pParaValue);
			break;
		default:
			return FALSE;
	}
	
	Empty_Receive_Buffer();
	retry_times = 0;
	error_times = 0;		
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));	//AT+HTTPPARA="uri","HTTP://183.131.13.85/command/getConfigurations"
	while(retry_times < 90)
	{
		feed_dog();
		delay_1ms(100);
		retry_times++;
		pIndex = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pIndex != NULL)
			return TRUE;
		pIndex = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pIndex != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));  //retry
			}
			else if (error_times >= 2)
				return FALSE;
		}
	}

	return FALSE;
}

static bool HTTP_Post_HexData_Single(uint8_t* pHttpSend , uint8_t sendLen , uint8_t beEnd)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;
	uint8_t* pIndex = NULL ;
	char sendData[500] = {0};
	uint8_t i;
	
	if (sendLen > 127)
		return FALSE;
	
	strcpy(sendData , "AT+HTTPHEXDATA=127,\"");
	for(i=0; i<sendLen; i++)
		sprintf(&sendData[20+i*2] , "%x" , pHttpSend[i]);
	if (beEnd == 0)
		strcat(&sendData[20+sendLen*2] , "\",0\r");
	else if (beEnd == 1)
		strcat(&sendData[20+sendLen*2] , "\",1\r");
	else
		return FALSE;
	
	Empty_Receive_Buffer();
	Send_Command_Data((uint8_t*)sendData , strlen(sendData));
	while(retry_times < 12)
	{
		feed_dog();		
		delay_1ms(500);
		retry_times++;
		pIndex = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pIndex != NULL)
			return TRUE;
		pIndex = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pIndex != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)sendData , strlen(sendData));
			}
			else if (error_times >= 2)
				return FALSE;			
		}
	}

	return FALSE;
}	

bool HTTP_Post_HexData(uint8_t* pHttpSend , int16_t sendLen)
{
	uint16_t i = 0;
	
	while(sendLen >127)
	{
		if (!HTTP_Post_HexData_Single(pHttpSend+i , 127 , 0))
			return FALSE;
		i += 127;
		sendLen -= 127;
	}
	
	if (!HTTP_Post_HexData_Single(pHttpSend+i , sendLen , 1))
		return FALSE;

	return TRUE;
}

static bool HTTP_Post_AsciiData_Single(char* pHttpSend , uint8_t sendLen , uint8_t beEnd)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;
	uint8_t* pIndex = NULL ;
	char sendData[500] = {0};
	
	if (sendLen > 255)
		return FALSE;
	
	sprintf(sendData , "AT+HTTPHEXDATA=255,\"%s\",%d\r" , pHttpSend , beEnd);
	Empty_Receive_Buffer();
	Send_Command_Data((uint8_t*)sendData , strlen(sendData));
	while(retry_times < 12)
	{
		feed_dog();		
		delay_1ms(500);
		retry_times++;
		pIndex = m_searchStr(RecvBuffer, "OK",u16RecvLen);
		if (pIndex != NULL)
			return TRUE;
		pIndex = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pIndex != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)sendData , strlen(sendData));
			}
			else if (error_times >= 2)
				return FALSE;			
		}
	}

	return FALSE;
}	

bool HTTP_Post_AsciiData(char* pHttpSend , int16_t sendLen)
{
	uint16_t i = 0;
	
	while(sendLen >255)
	{
		if (!HTTP_Post_AsciiData_Single(pHttpSend+i , 255 , 0))
			return FALSE;
		i += 255;
		sendLen -= 255;
	}

	if (!HTTP_Post_AsciiData_Single(pHttpSend+i , sendLen , 1))
		return FALSE;

	return TRUE;
}

bool HTTP_Activate(void)
{
	uint8_t retry_times = 0;
	uint8_t error_times = 0;
	uint8_t* pIndex = NULL;
	char send_temp[20] = "AT+HTTPACT\r";
	
	Empty_Receive_Buffer();
	Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));		//AT+HTTPACT
	while(retry_times < 80)
	{
		feed_dog();
		delay_1ms(100);
		retry_times++;
		pIndex = m_searchStr(RecvBuffer, "OK",u16RecvLen);	
		if (pIndex != NULL)
		{
			return TRUE;
		}
		pIndex = m_searchStr(RecvBuffer, "ERROR",u16RecvLen);
		if (pIndex != NULL)
		{
			error_times++;
			if (error_times == 1)
			{
				delay_1ms(1000);feed_dog();delay_1ms(1000);feed_dog();delay_1ms(500);;
				feed_dog();			
				Empty_Receive_Buffer();
				Send_Command_Data((uint8_t*)send_temp , strlen(send_temp));  //retry
			}
			else if (error_times >= 2)
				return 0;
		}
	}

	return 0;
}

/////////////////////////////private functions///////////////////////////////////////////
static uint8_t* find_comma(uint8_t* pSrc , uint8_t Index , uint16_t SrcLen)
{
	uint16_t i=0 ,j=0;
	
	if (pSrc == NULL)
		return NULL;
	if ((Index==0) || (SrcLen==0))
		return NULL;
	
	for(i=0,j=0; i<SrcLen; i++)
	{
		if (pSrc[i] == ',')
			j++;
		if (j == Index)
			return (pSrc+i);
	}
	
	return NULL;
}

static uint8_t* m_searchStr(uint8_t* pDest,const char* pSrc,uint16_t length)
{
	int n;
	int i;
	int  k;

	if(pDest==NULL || pSrc==NULL)
		return NULL;
	
	n = strlen(pSrc);

	if(length<n)
		return NULL;

	//length -= n;

	for(i=0;i<=length;i++)
	{
		if(pDest[i] == pSrc[0])
		{
			for(k=0;k<n;k++)
			{
				if(pDest[i+k] != pSrc[k])
					break;
			}

			if( k == n)
			{
				break;
			}
		}
		if(i >= RECV_BUFFER_LEN - 1)
		{
			return NULL;
		}
	}

	if(i >= (length ))
		return NULL;
	else
		return (pDest + i);
}


unsigned int m_searchStr_Locate(uint8_t* pDest,const char* pSrc,uint16_t length)
{
	int n;
	int i;
	int  k;

	if(pDest==NULL || pSrc==NULL)
		return NULL;
	
	n = strlen(pSrc);

	if(length<n)
		return NULL;

	//length -= n;

	for(i=0;i<=length;i++)
	{
		if(pDest[i] == pSrc[0])
		{
			for(k=0;k<n;k++)
			{
				if(pDest[i+k] != pSrc[k])
					break;
			}

			if( k == n)
			{
				break;
			}
		}
		if(i >= RECV_BUFFER_LEN - 1)
		{
			return 0;
		}
	}

	if(i >= length)
		return 0;
	else
		return i;
}


int myFunc( char *s, char *c, int n)
{
    int flag = 0;
    int index = 0;
    if (NULL == s)
    {
        return -1;
    }
    while (*s != '\0')
    {
        if (*s == *c)
        {
            ++flag;
            ++index;
            ++s;
            if (flag == n)
            {
                return index;
            }
        }
        ++s;
        ++index;
    }
    return index;
}


static void Empty_Receive_Buffer(void)
{
	memset(RecvBuffer , 0 , RECV_BUFFER_LEN);
	u16RecvLen = 0;
}

bool module_Init(void){
	DATA_FORMAT dataform=ASCII_FORMAT;
	RECV_DATA_MODE recmode=NOT_CACHE_RECV_DATA;
	WHETHER_DELETE_PAC_WHEN_READ det_pac=DELETE_PAC_WHEN_READ;
	char pImsi[15] = {0};
	int8_t rssi = 0;
	if(!Module_ResponseFormat())//设置模块返回值格式 ATV1\r
	{
		return FALSE;
	}

	//m_esimEcho = TRUE;
	
	if(!Module_Echo_On())//检查模块响应  ATE1\r
	{
		return FALSE;
	}
	
	if(!Module_Set_Sleep())//禁止模块进入省电模式  AT+S32K
	{
		return FALSE;
	}
	if(!Module_Get_Imsi(pImsi))
	{
		return FALSE;
	}
	if(! Module_Get_SignalQuality(&rssi))
	{
		return FALSE;

	}
	//登网操作
	if(!Module_Attach_Net())  //at+cgatt?\r
	{
		return FALSE;
	}
	
	if(!PDP_Init())        //AT+MIPCALL=0\r
	{
		return FALSE;
	}
	
	if(!TCP_SetMode(dataform,recmode,det_pac))
	{
		return FALSE;
	
	}
	
	return TRUE;
	
}

bool TCP_Read_DATA(uint8_t index)
{
	uint8_t* pu16RecvLen = NULL;
	char rec_ok[30] = "";
	sprintf(rec_ok , "hm+%d+0" ,index);
	
	pu16RecvLen = m_searchStr(RecvBuffer, rec_ok,u16RecvLen);
	if(pu16RecvLen !=NULL)
	{
		return TRUE;
	}
	return FALSE;
}

bool tcpGetJqid(uint8_t index)
{
	uint8_t* pu16RecvLen = NULL;
	char* indexStr = NULL;
	char rec_ok[30] = "";
	uint16_t startLocate = 0, stopLocate = 0;
	char jqidTemp[8] = {0};
	uint32_t jqidData = 0;
	//char site[3] =  {0};
	//uint8_t siteData = 0;
	char recvStr[30] = {0};
	uint8_t i;
	uint16_t strLen = 0;


	sprintf(rec_ok , "hm+%d+0" ,index);
	
	pu16RecvLen = m_searchStr(RecvBuffer, rec_ok,u16RecvLen);
	if(pu16RecvLen ==NULL)
	{
		return TRUE;
	}
	memcpy(recvStr,pu16RecvLen,sizeof(recvStr));
	indexStr = strstr(recvStr, "+end");
	if(indexStr == NULL)
	{
		//uart485SendData("err!, jqid get no end!\n");
		//uart485SendData(recvStr);
		return TRUE;
	}
	startLocate = myFunc(recvStr,"+",3);
	stopLocate = myFunc(recvStr,"+",4);

	if(stopLocate <= startLocate)
	{
		//uart485SendData("data check err1!");
		//uart485SendData(recvStr);
		return FALSE;
	}

	memset(jqidTemp,'0',sizeof(jqidTemp));
	strLen = stopLocate - startLocate;
	for(i = 0;i < (strLen - 1);i++)
	{
		jqidTemp[8 + 1 - strLen + i] = recvStr[startLocate + i];
	}
	

	for(i = 0;i < sizeof(jqidTemp);i++)
	{
		if((jqidTemp[i] < '0') || (jqidTemp[i] > '9')	)
		{
			//uart485SendData("jqid check err!");
			//uart485SendData(recvStr);
			return FALSE;
		}
	}

	for(i = 0;i < (strLen - 1);i++)
	{
		jqidData = jqidData * 10 + (recvStr[startLocate + i] - '0');
	}
	if(jqid != jqidData)
	{
		jqid = jqidData;
		devIdInfo.jqid = jqid;
		FLAGS4_JQID_GET_SUC = 1;
		//saveDevIdCfg();
		//printJqid();
	}
	

	/*
	startLocate = 0;
	stopLocate = 0;
	startLocate = myFunc(recvStr,"+",4);
	stopLocate = myFunc(recvStr,"+",5);

	if(stopLocate <= startLocate)
	{
		//uart485SendData("data check err2!");
		//uart485SendData(recvStr);
		return FALSE;
	}

	strLen = 0;
	strLen = stopLocate - startLocate;
	memset(site,'0',sizeof(site));
	for(i = 0;i < (strLen-1);i++)
	{
		site[3 + 1 - strLen + i] = recvStr[startLocate + i];
	}

	for(i = 0;i < sizeof(site);i++)
	{
		if((site[i] < '0') || (site[i] > '9')	)
		{
			//uart485SendData("site check err!");
			//uart485SendData(recvStr);
			return FALSE;
		}
	}
	for(i = 0;i < (strLen - 1);i++)
	{
		siteData = siteData*10 + (recvStr[startLocate + i] - '0');
	}
	*/
	
	return TRUE;
}

/*
bool TCP_Read_Time(uint8_t index)
{
	uint8_t* pu16RecvLen = NULL;
	char getTime[31] = {0};
	char debugStr[100] = {0};
	char* index1 = NULL;
	u16 year = 0;
	u8 mon = 0;
	u8 day = 0;
	u8 hour = 0;
	u8 min = 0;
	u8 sec = 0;
	char i = 0;
	//bool ret =FALSE;
	
	pu16RecvLen = m_searchStr(RecvBuffer, "+end",u16RecvLen);
	if(pu16RecvLen == NULL)
	{
		//uart485SendData("err, get time no end!\n");
		return TRUE;
	}
	pu16RecvLen = m_searchStr(RecvBuffer, "hm+1+",u16RecvLen);
	if(pu16RecvLen == NULL)
	{
		//uart485SendData("err, get time no start!\n");
		return TRUE;
	}
	//cp data
	memset(getTime, 0, sizeof(getTime));
	memcpy(getTime,pu16RecvLen,(sizeof(getTime)-1));

	//check data tail
	index1 = NULL;
	index1 = strstr(getTime, "+end");
	if (index1 == NULL)
	{
		//uart485SendData("err, get time no end 2!\n");
		//uart485SendData(getTime);
		return TRUE;
	}

	//check data head
	index1 = NULL;
	index1 = strstr(getTime, "hm+1+");
	if (index1 == NULL)
	{
		//uart485SendData("err, get time no start 2!\n");
		//uart485SendData(getTime);
		return TRUE;
	}

	year = (getTime[5]-'0')*1000+ (getTime[6]-'0')*100 +(getTime[7]-'0')*10 + (getTime[8]-'0');
	for(i = 0;i < 4;i++)
	{
		if((getTime[5 + i] < '0') || (getTime[5 + i] > '9'))
		{
			//uart485SendData("ssss");
			//uart485SendData(getTime);
			//uart485SendData("ssss");
			//uart485SendData("\nyear err\n");
				
			return TRUE;
		}
	}
	mon  = (getTime[9]-'0')*10  + (getTime[10]-'0');
	if(((getTime[9] < '0') || (getTime[9] > '1')) || ((getTime[10] < '0') || (getTime[10] > '9')))
	{
		//uart485SendData("ssss");
		//uart485SendData(getTime);
		//uart485SendData("ssss");
		//uart485SendData("yyyy");
		//uart485SendData("\nmon err\n");
		return TRUE;
	}
	day  = (getTime[11]-'0')*10 + (getTime[12]-'0');
	if(((getTime[11] < '0') || (getTime[11] > '3')) || ((getTime[10] < '0') || (getTime[10] > '9')))
	{
		//uart485SendData("ssss");
		//uart485SendData(getTime);
		//uart485SendData("ssss");
		//uart485SendData("yyyy");
		//uart485SendData("\nday err\n");
		return TRUE;
	}

	hour = (getTime[13]-'0')*10 + (getTime[14]-'0');
	if(((getTime[13] < '0') || (getTime[13] > '2')) || ((getTime[14] < '0') || (getTime[14] > '9')))
	{
		//uart485SendData("ssss");
		//uart485SendData(getTime);
		//uart485SendData("ssss");
		//uart485SendData("yyyy");
		//uart485SendData("\nhour err\n");
		return TRUE;
	}
	min  = (getTime[15]-'0')*10  + (getTime[16]-'0');
	if(((getTime[15] < '0') || (getTime[13] > '6')) || ((getTime[14] < '0') || (getTime[14] > '9')))
	{
		uart485SendData("ssss");
		uart485SendData(getTime);
		uart485SendData("ssss");
		uart485SendData("yyyy");
		uart485SendData("\nmin err\n");
		return TRUE;
	}
	sec  = (getTime[17]-'0')*10  + (getTime[18]-'0');
	if(((getTime[17] < '0') || (getTime[17] > '6')) || ((getTime[18] < '0') || (getTime[18] > '9')))
	{
		uart485SendData("ssss");
		uart485SendData(getTime);
		uart485SendData("ssss");
		uart485SendData("yyyy");
		uart485SendData("\nsec err\n");
		return TRUE;
	}

	sprintf(debugStr, "old year = %d, month = %d, day = %d, hour = %d, min = %d, sec = %d\n", calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
	uart485SendData(debugStr);
	uart485SendData("\n");
	memset(debugStr,0, sizeof(debugStr));
	sprintf(debugStr, "new year = %d, month = %d, day = %d, hour = %d, min = %d, sec = %d\n", year,mon,day,hour,min,sec);
	uart485SendData(debugStr);
	uart485SendData("\n");
	RTC_Set(year,mon,day,hour,min,sec);
		
	return TRUE;
}
*/

bool TCP_Read_ALARMEN(uint8_t index)
{
	char* index1 = NULL;
	char tmp[200]  = {0};
	
	//char cpNum = 0;
	uint8_t alarmTypePlace[15]={0};
	uint8_t i=0;
	uint8_t j=0;
	int ret=0;
	//uint8_t num=0;
	//int recINdex =0;
	char rec_ok[20] = {0};

	uint8_t* pu16RecvLen = NULL;

	//ALARM_CFG	tempCfg[ALARM_CFG_NUM];
	
	
	pu16RecvLen = m_searchStr(RecvBuffer, "+MIPDATA:",u16RecvLen);
	if(pu16RecvLen == NULL)
	{
		//uart485SendData("err, get en no MIPDATA!\n");
		return FALSE;
	}
	//cp data
	memcpy(tmp, pu16RecvLen, (sizeof(tmp) - 1));

	//check head
	sprintf(rec_ok , "hm+%d+%d" ,jqid,index);
	index1 = NULL;
	index1 =  strstr(tmp, rec_ok);
	if (index1 == NULL)
	{
		//uart485SendData("err, get en no start!\n");
		//uart485SendData(tmp);
		return TRUE;
	}
	
	//check tail
	index1 = NULL;
	index1 = strstr(tmp, "+end");
	if (index1 == NULL)
	{
		//uart485SendData("err, get en no end!\n");
		//uart485SendData(tmp);
		return TRUE;
	}

	for(i = 0; i < 15 ; i++)
	{
		ret = myFunc(tmp,"(",i+1);
		if(index <= 0)
		{
			//uart485SendData("find err\n\n");
			return TRUE;
		}
		if(ret < 7)
		{
			//num = i;
			//uart485SendData("locate err\n\n");
			return TRUE;
		}
		else
		{
			alarmTypePlace[i] = ret;
		}
	}
	/*
	//tempCfg[0].en  = (tmp[alarmTypePlace[0] - 2] - '0') -1;
	alarmCftTemp.devAlarmCfg[powerOffAlarm].actEn[acousto] = (tmp[alarmTypePlace[0]  + 0] - '0') -1;
	alarmCftTemp.devAlarmCfg[powerOffAlarm].actEn[message]= (tmp[alarmTypePlace[0]  + 1] - '0') -1;
	alarmCftTemp.devAlarmCfg[powerOffAlarm].actEn[callVoice] = (tmp[alarmTypePlace[0]  + 2] - '0') -1;
		
	//tempCfg[1].en  = (tmp[alarmTypePlace[3] - 2]- '0') -1;  
	alarmCftTemp.devAlarmCfg[highTempAlarm].actEn[acousto] = (tmp[alarmTypePlace[3]  + 0] - '0') -1;
	alarmCftTemp.devAlarmCfg[highTempAlarm].actEn[message] = (tmp[alarmTypePlace[3]  + 1] - '0') -1;
	alarmCftTemp.devAlarmCfg[highTempAlarm].actEn[callVoice] = (tmp[alarmTypePlace[3]  + 2] - '0') -1;

	//tempCfg[2].en  = (tmp[alarmTypePlace[4] - 2]- '0')-1;  
	alarmCftTemp.devAlarmCfg[lowTempAlarm].actEn[acousto] = (tmp[alarmTypePlace[4]  + 0] - '0') -1;
	alarmCftTemp.devAlarmCfg[lowTempAlarm].actEn[message] = (tmp[alarmTypePlace[4]  + 1] - '0') -1;
	alarmCftTemp.devAlarmCfg[lowTempAlarm].actEn[callVoice] = (tmp[alarmTypePlace[4]  + 2] - '0') -1;
	
	//tempCfg[3].en  = (tmp[alarmTypePlace[2] - 2]- '0')-1; 
	alarmCftTemp.devAlarmCfg[sensorErrAlarm].actEn[acousto] = (tmp[alarmTypePlace[2]  + 0] - '0') -1;
	alarmCftTemp.devAlarmCfg[sensorErrAlarm].actEn[message] = (tmp[alarmTypePlace[2]  + 1] - '0') -1;
	alarmCftTemp.devAlarmCfg[sensorErrAlarm].actEn[callVoice] = (tmp[alarmTypePlace[2]  + 2] - '0') -1;

	for(i = 0;i < alarmEventMax;i++)
	{
		if((alarmCftTemp.devAlarmCfg[i].actEn[acousto] != 0) && (alarmCftTemp.devAlarmCfg[i].actEn[acousto] != 1))	
		{
			return TRUE;
		}
		if((alarmCftTemp.devAlarmCfg[i].actEn[message] != 0) && (alarmCftTemp.devAlarmCfg[i].actEn[acousto] != 1))	
		{
			return TRUE;
		}
		if((alarmCftTemp.devAlarmCfg[i].actEn[callVoice] != 0) && (alarmCftTemp.devAlarmCfg[i].actEn[acousto] != 1))	
		{
			return TRUE;
		}
	}
	*/
		//tempCfg[0].en  = (tmp[alarmTypePlace[0] - 2] - '0') -1;
	alarmCfgTemp.devAlarmCfg[powerOffAlarm].actEn[acousto] = (tmp[alarmTypePlace[0]  + 0] - '0') -1;
	alarmCfgTemp.devAlarmCfg[powerOffAlarm].actEn[callVoice]= (tmp[alarmTypePlace[0]  + 1] - '0') -1;
	alarmCfgTemp.devAlarmCfg[powerOffAlarm].actEn[message] = (tmp[alarmTypePlace[0]  + 2] - '0') -1;
		
	//tempCfg[1].en  = (tmp[alarmTypePlace[3] - 2]- '0') -1;  
	alarmCfgTemp.devAlarmCfg[highTempAlarm].actEn[acousto] = (tmp[alarmTypePlace[3]  + 0] - '0') -1;
	alarmCfgTemp.devAlarmCfg[highTempAlarm].actEn[callVoice] = (tmp[alarmTypePlace[3]  + 1] - '0') -1;
	alarmCfgTemp.devAlarmCfg[highTempAlarm].actEn[message] = (tmp[alarmTypePlace[3]  + 2] - '0') -1;

	//tempCfg[2].en  = (tmp[alarmTypePlace[4] - 2]- '0')-1;  
	alarmCfgTemp.devAlarmCfg[lowTempAlarm].actEn[acousto] = (tmp[alarmTypePlace[4]  + 0] - '0') -1;
	alarmCfgTemp.devAlarmCfg[lowTempAlarm].actEn[callVoice] = (tmp[alarmTypePlace[4]  + 1] - '0') -1;
	alarmCfgTemp.devAlarmCfg[lowTempAlarm].actEn[message] = (tmp[alarmTypePlace[4]  + 2] - '0') -1;
	
	//tempCfg[3].en  = (tmp[alarmTypePlace[2] - 2]- '0')-1; 
	alarmCfgTemp.devAlarmCfg[sensorErrAlarm].actEn[acousto] = (tmp[alarmTypePlace[2]  + 0] - '0') -1;
	alarmCfgTemp.devAlarmCfg[sensorErrAlarm].actEn[callVoice] = (tmp[alarmTypePlace[2]  + 1] - '0') -1;
	alarmCfgTemp.devAlarmCfg[sensorErrAlarm].actEn[message] = (tmp[alarmTypePlace[2]  + 2] - '0') -1;

	//check if cfg is right
	for(i = 0;i < alarmEventMax;i++)
	{
		for(j = 0;j < alarmActMax;j++)
		{
			if(alarmCfgTemp.devAlarmCfg[i].actEn[j] != 0 && alarmCfgTemp.devAlarmCfg[i].actEn[j] != 1)
			{
				return TRUE;
			}
		}	
	}
	//check if need save
	for(i = 0;i < alarmEventMax;i++)
	{
		for(j = 0;j < alarmActMax;j++)
		{
			if((j == localBeep) || (j == pushMes))
			{
				continue;
			}
			if(alarmCfgTemp.devAlarmCfg[i].actEn[j] != alarmCfg.devAlarmCfg[i].actEn[j])
			{
				FLAGS4_ALARM_ACT_GET_SUC = 1;
				return TRUE;
			}
		}	
	}
	
	
	return TRUE;
}
/*
bool TCP_Read_ALARM_PARA(uint8_t index)
{
	char* pu16RecvLen = NULL;
	char rec_ok[20] = "";
	uint8_t i=0;
	uint16_t highTemp =0 ;
	uint16_t lowTemp =0 ;
	uint8_t* index1 = NULL;
	//uint8_t ParaLenth = 0; 
	//int recINdex = 0; 
	uint8_t plus1 = 0; 
	uint8_t plus2 = 0; 
	uint8_t plus3 = 0; 
	char tmp[80];
	char debugStr[30] =  {0};
	static char alarmIndexChange = 0;
	

	index1 = m_searchStr(RecvBuffer, "MIPDATA:",u16RecvLen);
	if(index1 == NULL)
	{
		uart485SendData("err, get index no MIPDATA!\n");
		return FALSE;
	}
	
	//cp data
	memcpy(tmp, index1, (sizeof(tmp) - 1));

	//check head
	sprintf(rec_ok , "hm+%d+%d+" ,jqid,index);
	pu16RecvLen= NULL;
	pu16RecvLen =strstr(tmp,rec_ok);
	if (pu16RecvLen == NULL)
	{
		uart485SendData("err, get index no start!\n");
		uart485SendData(tmp);
		return TRUE;
	}
	//check tail
	pu16RecvLen = NULL;
	pu16RecvLen = strstr(tmp, "+end");
	if(pu16RecvLen == NULL)
	{
		uart485SendData("err, get index no end !\n");
		uart485SendData(tmp);
		return TRUE;
	}
		
	plus1 = myFunc(tmp,"+",3);
	plus2 = myFunc(tmp,"+",4);
	if((plus1 <= 0) || (plus2 <= 0) || (plus2 <= plus1))
	{
		memset(debugStr, 0, sizeof(debugStr));
		sprintf(debugStr, "1plus1 = %d, plus2 = %d\n\n", plus1, plus2);
		uart485SendData(debugStr);
		return TRUE;
	}
	for(i = plus1; i < (plus2-1); i++)
	{
		highTemp = (highTemp*10) + (tmp[i]-'0');
		if((tmp[i]-'0') > 9 || (tmp[i]-'0') < 0)
		{
			uart485SendData("ssss");
			uart485SendData(tmp);
			uart485SendData("ssss");
			uart485SendData("yycyy");
			return TRUE;
		}
	}
	plus3 = myFunc(tmp,"+",5);
	if((plus3 <= 0) || (plus2 <= 0) || (plus3 <= plus2))
	{
		memset(debugStr, 0, sizeof(debugStr));
		sprintf(debugStr, "2plus2 = %d, plus3 = %d\n\n", plus2, plus3);
		uart485SendData(debugStr);
		return TRUE;
	}
	for(i = plus2 ; i < (plus3-1); i++)
	{
		lowTemp = (lowTemp*10) + (tmp[i]-'0');			
		if((tmp[i]-'0') > 9 || (tmp[i]-'0') < 0)
		{
			uart485SendData("ssss");
			uart485SendData(tmp);
			uart485SendData("ssss");
			uart485SendData("yyyy");
			return TRUE;
		}
 	}
	if((highTemp > 1000) || (highTemp&0x1000)||(lowTemp > 1000) || (lowTemp&0x1000))
	{
		uart485SendData("index data Err\n");
		uart485SendData("ssss");
		uart485SendData(tmp);
		uart485SendData("ssss");
		uart485SendData("yyyy");
		return TRUE;
	}
	if((highTemp != alarmIndex.tempH) || (lowTemp != alarmIndex.tempL))
	{	
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "change old temph = %d, templ = %d, new temph = %d, templ = %d\n", alarmIndex.tempH,alarmIndex.tempL,highTemp, lowTemp);
		uart485SendData(tmp);
		//if(alarmIndexChange++ >= 2)
		
		
		
		
		
		
		
		
		
		{
			uart485SendData("\n\n\n save index ok\n\n\n");
			alarmIndex.tempH = highTemp ;
			alarmIndex.tempL = lowTemp;  
			sendAlarmCfg(0);
			alarmCfgWr();
			alarmIndexChange = 0;
		}
	}
return TRUE ;
}
*/


