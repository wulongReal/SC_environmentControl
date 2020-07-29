#include <stdio.h>                                                                                            
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>
#include "lte720.h"
#include "usart.h"
#include <rtthread.h>
#include "rtc.h"
#include "vars.h"
#include "app_task.h"
#include "mbcrc.h"

ALIGN(RT_ALIGN_SIZE)



void feed_dog(void)
{
	IWDG_Feed();
	return;
}

void delayMs(uint32_t delay_ms)
{
	rt_thread_delay(delay_ms);
	return;
}

bool LTE_SendBuf(uint8_t* pData, uint16_t len)
{
	int i;
	/*
	for(i=0;i<len;i++)
	  debug_uart_sendbyte(pData[i]);
	  */
	for(i =0 ;i < len;i++)
	{
		USART_SendData(USART3,pData[i]);
  		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
	}
	
	return TRUE;
}

void LTE_Clr_RecvBuf(void)
{
	 LTE_RecvBuf_Busy = TRUE;
	memset(LTE_RecvBuffer , 0 , LTE_RECV_BUFFER_LEN);
	LTE_RecvLen = 0;
	LTE_RecvBuf_Busy = FALSE;
}

uint8_t LTE_Send_AT_Command(uint8_t *cmd, uint8_t *ack, uint16_t waitTime)
{
  LTE_Clr_RecvBuf();
  LTE_SendBuf(cmd, strlen(cmd));

  while(--waitTime)
  {
    delayMs(100);
    if(strstr(LTE_RecvBuffer, ack)!= NULL)
    {
      return TRUE;
    }
    if(strstr(LTE_RecvBuffer, "ERROR") != NULL)
    {
      LTE_Clr_RecvBuf();
      LTE_SendBuf(cmd, strlen(cmd));
    }
  }
  return FALSE;
}

uint8_t Send_AT_Ask(uint8_t *cmd, uint8_t *ack, uint16_t waitTime)
{
  uint8_t cnt = 0;

  LTE_Clr_RecvBuf();
  LTE_SendBuf(cmd, strlen(cmd));
  while(--waitTime)
  {
    delayMs(200);
    if(strstr( LTE_RecvBuffer, ack)!= NULL)
    {
      return TRUE;
    }
    if(strstr(LTE_RecvBuffer, "ERROR") != NULL)
    {
      LTE_Clr_RecvBuf();
       LTE_SendBuf(cmd, strlen(cmd));
    }
    if(cnt++ >= 5)
    {
      cnt = 0;
      LTE_Clr_RecvBuf();
       LTE_SendBuf(cmd, strlen(cmd));
    }
    
  }
  return FALSE;
}

//source源字符串。dest目标字符串,len原字符串长度
uint8_t* findStr(uint8_t *source, uint8_t *dest, uint16_t len)
{
	uint16_t i = 0, j = 0;
	uint16_t destLen = strlen((char *)&dest);

	if(strlen((char *)&dest) > len)
		return 0;
	
	for(i = 0; i < len;i++)
	{
		if(source[i] == dest[0])
		{
			for(j = 0;j < destLen;j++)
			{
				if(source[i + j] != dest[j])
				{
					return NULL;
				}
			}
			return (source + i);
		}
	}

	return NULL;
}
uint8_t LTE_Send_Data_Ask(uint8_t *date, uint8_t *ack, uint16_t waitTime)
{
	LTE_Clr_RecvBuf();
  	LTE_SendBuf(date, strlen((char *)&date));
  	while(--waitTime)
	  {
	    delayMs(100);
	    if(findStr(LTE_RecvBuffer, ack,LTE_RecvLen)!= NULL)
	    {
	      return TRUE;
	    }
	    if(findStr(LTE_RecvBuffer, "ERROR",LTE_RecvLen) != NULL)
	    {
	      LTE_Clr_RecvBuf();
	      LTE_SendBuf(date, strlen((char *)&date));
	    }
	  }
  	return FALSE;
}



//1：连接OK，0：连接err
uint8_t LTE_Get_ConnectSta(void)
{
	if(!LTE_Send_AT_Command("AT+CIPSTATUS\r", "STATE: CONNECT OK",30))
	{
		return FALSE;
	}
	return TRUE;
}

//获取rssi
uint8_t LTE_Get_RSSI(uint8_t *rssi)
{
	char *ptr = NULL;
	char *ptr1 = NULL;
	char buf[10] = {0};

	
	if(!LTE_Send_AT_Command("AT+CSQ\r", "OK", 30))
	{
		return FALSE;
	}

	ptr = strstr(LTE_RecvBuffer, "+CSQ:");
	if(ptr != NULL)
	{
		ptr1 = strchr(ptr, ',');
		if(ptr1 != NULL)
		{
			memcpy(buf, ptr+5, ptr1-ptr-5);
			*rssi = atoi(buf);
		}
	}
	else
		return FALSE;
	
	return TRUE;
}


//1：连接OK，0：连接错误
uint8_t  LTE_Init(void)
{
	uint8_t ret = TRUE;

	if(!LTE_Send_AT_Command("AT\r", "OK",30))
	{
		return FALSE;
	}
	if(!LTE_Send_AT_Command("ATE0\r", "OK",30))
	{
		return FALSE;
	}

	if(!LTE_Send_AT_Command("AT+CSQ\r", "OK", 50))
	{
		return FALSE;
	}

	//if(!LTE_Send_AT_Command("AT+CREG\r", "+CREG: 0,1", 200))
	{
	//	return FALSE;
	}
	if(!Send_AT_Ask("AT+CGATT?\r", "+CGATT: 1", 100))
	{
		return FALSE;
	}
	if(!LTE_Send_AT_Command("AT+CIPMUX=0\r", "OK", 50))
	{
		return FALSE;
	}
	if(!LTE_Send_AT_Command("AT+CIPQSEND=1\r", "OK", 50))
	{
		return FALSE;
	}
	if(!LTE_Send_AT_Command("AT+CSTT=\"CMNET\"\r", "OK", 50))
	{
		return FALSE;
	}
	if(!LTE_Send_AT_Command("AT+CIICR\r", "OK", 30))
	{
		return FALSE;
	}
	if(!LTE_Send_AT_Command("AT+CIFSR\r", ".", 30))
	{
		return FALSE;
	}
	
	//if(!LTE_Send_AT_Command("AT+CIPSTART=\"TCP\",\"qianyu.qicp.vip\",16845\r", "CONNECT OK", 200))
	if(!LTE_Send_AT_Command("AT+CIPSTART=\"TCP\",\"47.114.169.91\",1113\r", "CONNECT OK", 50))	
	{
		return FALSE;
	}

	/*
	if(!TCP_Open(SOCKET_CHANNEL_1, "122.112.219.87\0", 3734))
	{
		return FALSE;
	}
	*/
	return TRUE;
}

uint8_t LTE_TCP_Close(void)
{
	//if(LTE_Get_ConnectSta())
	{
		if(!LTE_Send_AT_Command("AT+CIPCLOSE\r", "OK",200))
		{
			return FALSE;
		}	
	}
	
	return TRUE;
}
void LTE_Start(void)
{
	LTE_RESET_DIS;
	POWER_4V_OPEN;
	//pwkey
	/*
	LTE_PWKEY_HGIH;
	delayMs(2000);
	LTE_PWKEY_LOW;
	delayMs(2000);
	*/
}
//发送关机命令，重启模组，硬重启
void LTE_Reboot(void)
{
	LTE_Send_AT_Command("AT+CPOWD=1\r", "OK", 30);
	POWER_4V_OFF;
	delayMs(1000);
	POWER_4V_OPEN;
	delayMs(3000);
}

//重启TCP链接，软重启
uint8_t LTE_Close_Net(void)
{
	uint8_t ret  = FALSE;

	ret  = LTE_Send_AT_Command("AT+CIPCLOSE=1\r", "CLOSE OK", 30);
	ret  = LTE_Send_AT_Command("AT+CIPSHUT\r", "SHUT OK", 30); 
	return ret;
}
void LTE_Restart(void)
{
	LTE_TCP_Close();
	POWER_4V_OFF;
	delayMs(1000);
	LTE_Start();
	delayMs(5000);
}


//从接收buffer，
uint8_t findByte(uint8_t destByte, uint8_t ms)
{
	uint16_t i = 0;
	
	while(ms--)
	{
		delayMs(100);
		for(i = 0;i < LTE_RecvLen;i++)
		{
			if(LTE_RecvBuffer[i] == destByte)
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}
uint8_t LTE_SendDataToWeb(uint8_t *pData, uint16_t len)
{
	uint8_t ret = 0;
  	uint8_t endStr[1] = {0x1a};
	uint8_t sendOkStr[20] = {0}; 
	uint8_t sendDateStr[20] = {0};
	uint8_t cnt = 50;

	sprintf(sendOkStr, "DATA ACCEPT:%d", len);
	sprintf(sendDateStr, "AT+CIPSEND=%d\r",len);


	//定长发送
	if(LTE_Send_AT_Command(sendDateStr, ">", 30))
	{
		LTE_Clr_RecvBuf();
		LTE_SendBuf(pData, len);
		while(cnt--)
		{
			delayMs(100);
			if(strstr(LTE_RecvBuffer, sendOkStr))
			{
				return TRUE;
			}
		}
	}

	/*
	//变长发送
	if(LTE_Send_AT_Command("AT+CIPSEND\r\n", ">", 50))
	{
		LTE_Clr_RecvBuf();
		LTE_SendBuf(pData, len);
		delayMs(100);
		if(LTE_Send_Data_Ask(endStr, "DATA ACCEPT:", 50))
		{
			return TRUE;
		}
	}
	*/
  	
  	return FALSE;
}

uint8_t LTE_Recv_Check(uint8_t *pData)
{
	//包结构校验

	//len校验

	//crc校验

	//命令解析

	return TRUE;
}

uint8_t Commu_Recv_Proc(uint8_t cmd )
{
	uint8_t cmdRd = 0;
	uint16_t len = 0;
	uint16_t crc = 0;
	uint16_t temp = 0;
	uint16_t i = 0,j = 0;
	uint8_t jqid[8] = {0};
	uint8_t *ptr = NULL;
	uint8_t recvBuf[300] = {0};
	uint8_t ret  = 0;

	//查找包头
	for(i = 0;i < LTE_RecvLen;i++)
	{
		if(LTE_RecvBuffer[i] == 0xa5)
		{
			break;
		}
		if(i == LTE_RecvLen - 1)
			return FALSE;
	}
	len = (LTE_RecvBuffer[i+10] <<8) + LTE_RecvBuffer[i + 11];
	if(len > LTE_RecvLen) 
		 return FALSE;
	//取出数据包
	memcpy(recvBuf, &LTE_RecvBuffer[i], len);
	//校验数据包
	for(i = 0;i < 8;i++)
		jqid[i] = recvBuf[1+i];
	ret = strncmp(jqid, jqidString,8);
	if(ret)
	{
		return FALSE;
	}
	cmdRd = recvBuf[9];
	if(cmdRd != cmd)
		return FALSE;
	crc = usMBCRC16(recvBuf, len - 2);
	temp = recvBuf[len - 1];
	temp = temp << 8;
	temp += recvBuf[len - 2];
	//temp = recvBuf[len - 1] <<8 + recvBuf[len - 2];
	if(crc != temp)
		return FALSE;
	
	//取出数据并进行处理
	if(cmd == COMMU_CMD_UPLOAD_VER)
	{
		return TRUE;
	}
	else if(cmd == COMMU_CMD_UPLOAD_ALARM)
	{
		return TRUE;
	}
	else if(cmd == COMMU_CMD_UPLOAD_RUN_INFO)
	{
		return TRUE;
	}
	else if(cmd == COMMU_CMD_UPLOAD_SENSOR_DATE)
	{
		return TRUE;
	}
	else if(cmd == COMMU_CMD_SYS_TARGET_TEMPER)
	{
		TARGET_TEMP_INFO_T tempCtlr_s  = {0};
		if(recvBuf[12] == 0)
			return TRUE;
		else if(recvBuf[12] == 1)
		{
			m_Is_NeedRefresh_LCD_Cfg = TRUE;
			memcpy((uint8_t *)&tempCtlr_s, recvBuf+13, sizeof(tempCtlr_s));
			for(i = 0;i < TARGET_TEMP_NUM;i++)
			{
				tempCtlr_s.targetTempArry[i].targetTemp = BigtoLittle16(tempCtlr_s.targetTempArry[i].targetTemp);
				tempCtlr_s.targetTempArry[i].minAirRuntime = BigtoLittle16(tempCtlr_s.targetTempArry[i].minAirRuntime);
			}
			tempCtlr_s.minAirSupport_cycleTime = BigtoLittle16(tempCtlr_s.minAirSupport_cycleTime);
			m_Controler_Ctrl_Cfg_t.targetTemperCfg = tempCtlr_s;
			Contrler_Cfg_Save(CFG_INDEX_CTRL_CFG);
		}
	}
	else if(cmd == COMMU_CMD_SYS_AIR_CTRL)
	{
		AirCtrlCfg_T tempCtlr_s  = {0};
		if(recvBuf[12] == 0)
			return TRUE;
		else if(recvBuf[12] == 1)
		{
		m_Is_NeedRefresh_LCD_Cfg = TRUE;
			memcpy((uint8_t *)&tempCtlr_s, recvBuf+13, sizeof(tempCtlr_s));
			for(i = 0;i < CONTROL_LEVEL_MAX;i++)
			{
				for(j = 0; j < FRE_FAN_NUM_MAX;j++)
				{
					tempCtlr_s.airCtrllevel[i].freFanCfgArry[j].runTime = BigtoLittle16(tempCtlr_s.airCtrllevel[i].freFanCfgArry[j].runTime);
				}
				for(j = 0; j < FAN_NUM_MAX;j++)
				{
					tempCtlr_s.airCtrllevel[i].fanCfgArry[j].runTime = BigtoLittle16(tempCtlr_s.airCtrllevel[i].fanCfgArry[j].runTime);
				}
			}
			tempCtlr_s.freFanCycleTime = BigtoLittle16(tempCtlr_s.freFanCycleTime);
			tempCtlr_s.fanCycleTim = BigtoLittle16(tempCtlr_s.fanCycleTim);

			m_Controler_Ctrl_Cfg_t.airCtrlCfg = tempCtlr_s;
			Contrler_Cfg_Save(CFG_INDEX_CTRL_CFG);
		}
	}
	else if(cmd == COMMU_CMD_SYS_HEATER_CTRL)
	{
		HEATER_CTRL_CFG_T tempCtlr_s  = {0};
		if(recvBuf[12] == 0)
			return TRUE;
		else if(recvBuf[12] == 1)
		{
		m_Is_NeedRefresh_LCD_Cfg = TRUE;
			memcpy((uint8_t *)&tempCtlr_s, recvBuf+13, sizeof(tempCtlr_s));
			m_Controler_Ctrl_Cfg_t.heaterCtrlCfg = tempCtlr_s;
			Contrler_Cfg_Save(CFG_INDEX_CTRL_CFG);
		}
	}
	else if(cmd == COMMU_CMD_SYS_COLDPAD_CTRL)
	{
		HUMI_CTRL_CFG_T tempCtlr_s  = {0};
		if(recvBuf[12] == 0)
			return TRUE;
		else if(recvBuf[12] == 1)
		{
		m_Is_NeedRefresh_LCD_Cfg = TRUE;
		
			memcpy((uint8_t *)&tempCtlr_s, recvBuf+13, sizeof(tempCtlr_s));
			tempCtlr_s.enTime = BigtoLittle16(tempCtlr_s.enTime);
			tempCtlr_s.disenTime = BigtoLittle16(tempCtlr_s.disenTime);
			m_Controler_Ctrl_Cfg_t.humiCtrlCfg = tempCtlr_s;
			Contrler_Cfg_Save(CFG_INDEX_CTRL_CFG);
		}
	}
	else if(cmd == COMMU_CMD_SYS_LIGHT_CTRL)
	{
		LIGHT_CTRL_CFG_T tempCtlr_s  = {0};
		if(recvBuf[12] == 0)
			return TRUE;
		else if(recvBuf[12] == 1)
		{
		m_Is_NeedRefresh_LCD_Cfg = TRUE;
			memcpy((uint8_t *)&tempCtlr_s, recvBuf+13, sizeof(tempCtlr_s));
			tempCtlr_s.enTime = BigtoLittle16(tempCtlr_s.enTime);
			tempCtlr_s.disenTime = BigtoLittle16(tempCtlr_s.disenTime);
			tempCtlr_s.targetLux = BigtoLittle16(tempCtlr_s.targetLux);
			m_Controler_Ctrl_Cfg_t.lightCtrlCfg = tempCtlr_s;
			Contrler_Cfg_Save(CFG_INDEX_CTRL_CFG);
		}
	}
	return TRUE;
}

