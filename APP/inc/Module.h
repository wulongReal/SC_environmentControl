#ifndef __MODULE_H_
#define __MODULE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "bsp.h"
#include "vars.h"


typedef enum
{
	ASCII_FORMAT = 0,
	HEX_FORMAT
}DATA_FORMAT;

typedef enum
{ 
	NOT_CACHE_RECV_DATA = 0,
	CACHE_RECV_DATA
}RECV_DATA_MODE;
	
typedef enum
{ 
	DELETE_PAC_WHEN_READ = 0,
	NOT_DELETE_PAC_WHEN_READ
}WHETHER_DELETE_PAC_WHEN_READ;

typedef enum
{
	SOCKET_CHANNEL_1 = 0,
	SOCKET_CHANNEL_2 = 1,
	SOCKET_CHANNEL_3 = 2
}SOCKET_CHANNEL_ID;

typedef enum
{
	CONFIRMATION_MODE = 0,
	TIMEOUT_MODE = 1,
	BUFFER_FULL_MODE = 2,
	AUTOMATIC_MODE = 3
}TRANS_NODE;

typedef enum
{
	HTTP_GET_METHOD = 0,
	HTTP_POST_METHOD = 1,
	HTTP_HEAD_METHOD = 2
}HTTP_METHOD;

typedef enum
{
	URL = 0,
	CONTENT_TYPE = 1,
	ACCEPT = 2,
	ACCEPT_LANGUAGE = 3,
	USER_AGENT = 4,
	REFERER = 5,
	CONNECTION = 6
}HTTP_PARA_NAME;

typedef struct _Cell_Info
{
	uint16_t u16Lac;  
	uint16_t u16CellID;
}Cell_Info;

/*
typedef struct upload_flag
{
	bool syncTime;  
	bool alarmMes;
	bool alarmCall;
	bool alarmBeep;
}mesUpload;*/
	/*

typedef struct  
{
	bool syncTime;  
	bool alarmMes;
	bool connect;
	bool alarmBeep;
	bool alarmCall;
	bool alarm;

}_uploadMes;


typedef struct  
{
	bool syncTime;  
	bool alarmMes;
	bool alarmCall;
	bool alarmBeep;

}_uploadMes;*/
typedef struct	
{
	bool LedEnable;	
	bool BeepEnable;
	bool MessageToNetEnable;
	bool MMSEnable;
	bool VoiceEnable;
	bool AcoustoEnable;

}ALARM_TYPE;
/*
typedef struct	
{
	bool Init;
	bool AlarmPowerCut;
	bool FunFault;

}ALARM_TYPE;*/





//////////////////////////////PDP context functions////////////////////////////////////////
bool PDP_Init(void);
bool pdp_unInit(void);
bool PDP_ActContext(void);
bool Module_Attach_Net(void);

////////////////////////////////Module Hardware///////////////////////////////////////
bool Module_Init(void);
bool Module_WarmReset(void);
bool Module_ColdReset(void);
bool Module_PowerDown(void);
bool Module_SetBaudrate(uint32_t u32Baudrate);//u32Baudrate:  Module BaudRate
bool Module_Sleep(void);
bool Module_Wakeup(void);
bool Module_ResponseFormat(void);
bool Module_Check_CPIN(void);
bool Module_Echo_Off(void);
bool Module_Echo_On(void);
bool Module_Set_Sleep(void);
bool Module_SleepWakeup_DSR(void);

/////////////////////////////TCP functions/////////////////////////////////////////////////
bool TCP_SetMode(DATA_FORMAT u8DataFormat, RECV_DATA_MODE u8RecvDataMode, WHETHER_DELETE_PAC_WHEN_READ u8WhetherDelete);
bool TCP_SetTransMode(SOCKET_CHANNEL_ID u8SocketID , TRANS_NODE u8TransMode , uint16_t thresholdLen);
int8_t check_TcpState(uint8_t u8SocketID);
bool TCP_Open(SOCKET_CHANNEL_ID u8SocketID , char* pRemoteIP, uint16_t u16RemotePort);
bool TCP_Send(SOCKET_CHANNEL_ID u8SocketID, DATA_FORMAT u8DataFormat , uint8_t* pSendData , uint16_t sendDataLen);
bool TCP_MIPSTRS_Send(SOCKET_CHANNEL_ID u8SocketID, DATA_FORMAT u8DataFormat , uint8_t* pSendData , uint16_t sendDataLen);
bool TCP_Recv(uint8_t u8SocketID);
bool TCP_Close(SOCKET_CHANNEL_ID u8SocketID);
bool TCP_Check_HeartBeat(uint8_t u8SocketID);
bool TCP_EnterTransMode(SOCKET_CHANNEL_ID u8SocketID);
bool TCP_TransSendData(SOCKET_CHANNEL_ID u8SocketID , uint8_t* pData , uint16_t DataLen);
bool TCP_ExistTransMode(SOCKET_CHANNEL_ID u8SocketID);
uint8_t TCP_Query_UnReadPacksNum(void);
uint16_t TCP_Read_UnReadPacks(uint8_t *pReadDataBuff , uint16_t ReadDataMaxNum);
uint16_t TCP_Read_MIPDATA(uint8_t *pReadDataBuff , uint16_t ReadDataMaxNum , uint32_t u32TimeOut);

////////////////////////////////Get Information///////////////////////////////////////
bool Module_Get_CellID(Cell_Info* pCellInfo);
bool Module_Get_SignalQuality(int8_t* pRawSignalNum);
bool Module_Get_Imsi(char* pImsi);
bool Module_Get_Imei(char* pImei);
bool Module_Get_Eid(char* pEid);

//////////////////////////////////HTTP SET//////////////////////////////////////////////
bool HTTP_Set_Method(HTTP_METHOD u8HttpMethod);
bool HTTP_Set_Para(HTTP_PARA_NAME u8HttpParaName , char* pParaValue);
bool HTTP_Post_AsciiData(char* pHttpSend , int16_t sendLen);
bool HTTP_Post_HexData(uint8_t* pHttpSend , int16_t sendLen);
bool HTTP_Activate(void);

//////////////////////////////////private functions////////////////////////////////////////////////////////
static uint8_t* find_comma(uint8_t* pSrc , uint8_t Index , uint16_t SrcLen);
static uint8_t* m_searchStr(uint8_t* pDest,const char* pSrc,uint16_t length);
static void Empty_Receive_Buffer(void);

//////////////////////////////////Transplant functions////////////////////////////////////////////////////
void feed_dog(void);
void delay_1ms(uint32_t delay_ms);
bool Send_Command_Data(uint8_t* pu8Data, uint16_t u16DataLen);

//**************
bool Module_softRest(void);

bool module_Init(void);
bool TCP_Read_DATA(uint8_t index);

bool tcpGetJqid(uint8_t index);

bool TCP_Read_Time(uint8_t index);
bool TCP_Read_ALARM_PARA(uint8_t index);
bool TCP_Read_ALARMEN(uint8_t index);
int myFunc(char *s, char *c, int n);



#endif
