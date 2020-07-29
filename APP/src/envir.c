#include "stdint.h"
#include "stdlib.h"
#include "envir.h"
#include "vars.h"
#include "rtc.h"
#include"DS1302.h"
#include <string.h>
#include "DataFlash.h"



CONTROLER_DEV_CFG_T		m_Controler_Dev_Cfg_t;	//��������������
CONTROLER_CTRL_CFG_T	m_Controler_Ctrl_Cfg_t;	//��������
CONTROLER_ALARM_CFG_T	m_Controler_Alarm_Cfg_t;//��������������
ROOM_INFO_CFG_T			m_Room_Cfg_t;			//������Ϣ���á���Ҫ������ʱ�����ʼ��
EQUIPMENT_RUN_INFO_T 	m_Equipment_RunInfo_t;	//��������״̬����ʱ����
DRVIE_BOARD_DATA_T		m_DriveBoard_Data_t;	//���������ݽṹ
CONTROLER_ALARM_PROC_T	m_Alarm_Proc_t;			//���ϴ���
CONTROLER_ALARM_STA_T	m_Alarm_Sta_t;			//����״̬




uint8_t rtcTime[6];

/*
���ʱ�������Ƿ�����
YY:20-255
MM:1-12
DD:1-30
hh:0-59
mm:0-59
ss:0-59
���أ�TRUE:ʱ����ȷ��FALSE:ʱ�����
*/
uint8_t Verify_RtcTime(uint8_t *pBuf)
{

	if(pBuf[1] <1 || pBuf[1] > 12)
		return FALSE;
	if(pBuf[2] <1 || pBuf[2] > 30)
		return FALSE;
	if(pBuf[3] <1 || pBuf[3] > 59)
		return FALSE;
	if(pBuf[4] <1 || pBuf[4] > 59)
		return FALSE;
	if(pBuf[5] <1 || pBuf[5] > 59)
		return FALSE;

	return TRUE;
}
//���ñ���
uint8_t Contrler_Cfg_Save(uint8_t cfgIndex)
{
	uint16_t size = 0;
	uint16_t num = 0;
	if(cfgIndex == CFG_INDEX_DEV_EN)
	{	
		size = sizeof(m_Controler_Dev_Cfg_t);
		num = Flash_Write(FLASH_CONTROLER_DEV_ENABLE_ADDR, (char *) &m_Controler_Dev_Cfg_t, size);
		if(num != size)
		{
			return FALSE;
		}
	}
	else if(cfgIndex == CFG_INDEX_CTRL_CFG)
	{
		size = sizeof(m_Controler_Ctrl_Cfg_t);
		num = Flash_Write(FLASH_CONTROLER_CTRL_CFG_ADDR, (char *) &m_Controler_Ctrl_Cfg_t, size);
		if(num != size)
		{
			return FALSE;
		}
	}
	else if(cfgIndex == CFG_INDEX_ALARM_CFG)
	{
		size = sizeof(m_Controler_Alarm_Cfg_t);
		num = Flash_Write(FLASH_CONTROLER_ALARM_CFG_ADDR, (char *) &m_Controler_Alarm_Cfg_t, size);
		if(num != size)
		{
			return FALSE;
		}
	}
	else if(cfgIndex == CFG_INDEX_ROOM_CFG)
	{
		size = sizeof(m_Room_Cfg_t);
		num = Flash_Write(FLASH_CONTROLER_ROOM_CFG_ADDR, (char *) &m_Room_Cfg_t, size);
		if(num != size)
		{
			return FALSE;
		}
	}
	return TRUE;
}
//����������Ϣ
uint8_t Ctroler_DevEnable_Init(void)
{
	char *buf;
	uint16_t size = sizeof(m_Controler_Dev_Cfg_t);
	uint16_t num;
	
	
	CONTROLER_DEV_CFG_T temp = {\
		{{0,1},{1,1},{2,0},{3,1},{4,0},{5,0},{6,0},{7,1},{8,1},{9,1},{10,1},{11,0}},\
		{{30,1,0},{31,5,0},{32,0,1},{33,0,0},{34,0,0},{35,0,0},{36,1,0},{37,0,0},{38,0,1}},\
		0,456,912,1368,1824,2280,2736,3192,3648,4104,\
		20,07,13,10,30,30\
		};

	buf =  malloc(size);
	if(RT_NULL == buf)
	{
		return FALSE;
	}
	num = Flash_Read(FLASH_CONTROLER_DEV_ENABLE_ADDR, buf, size);
	if(num != size)	
	{	
		if(buf) 	free(buf);
		return FALSE;
	}
	memcpy(&m_Controler_Dev_Cfg_t, buf, size);
	if(!Verify_RtcTime(m_Controler_Dev_Cfg_t.saveTime))
	{
		//memcpy(m_Controler_Dev_Cfg_t, temp, size);
		m_Controler_Dev_Cfg_t = temp;
		memcpy(m_Controler_Dev_Cfg_t.saveTime, rtcTime, sizeof(rtcTime));
		num = Flash_Write(FLASH_CONTROLER_DEV_ENABLE_ADDR, (char *) &m_Controler_Dev_Cfg_t, size);
		if(num != size)
		{
			if(buf) 	free(buf);
			return FALSE;
		}
			
	}
	
	if(buf) 	free(buf);
	return TRUE;
}
//�豸��������
uint8_t Ctroler_DevCtrlCfg_Init(void)
{
	
	char *buf;
	uint16_t size = sizeof(CONTROLER_CTRL_CFG_T);
	uint16_t num;
	
	

	TARGET_TEMP_INFO_T	targetTemperCfg = {{{1,320,50,10},{7,300,55,15},{14,280,60,10},{21,260,65,15},{28,240,70,10},{35,220,70,15},{42,200,70,20}},30 ,20,07,13,10,30,31};
	
	AirCtrlCfg_T	airCtrlCfg ={{\
			   {1,5, {{1, DEV_FRE_FAN, 0, 80, 15}, {0, DEV_FRE_FAN, 1, 80, 15}},{{1,DEV_FAN, 0,0},   {1,DEV_FAN, 1,0},   {1,DEV_FAN, 2,0},   {1,DEV_FAN, 3,0},   {1,DEV_FAN, 4,0}}},\
			   {2,10,{{1, DEV_FRE_FAN, 0, 100, 20},{0, DEV_FRE_FAN, 1, 80, 20}},{{1,DEV_FAN, 0,0},   {1,DEV_FAN, 1,0},   {1,DEV_FAN, 2,0},   {1,DEV_FAN, 3,0},   {1,DEV_FAN, 4,0}}},\
			   {3,15,{{1, DEV_FRE_FAN, 0, 0, 0},	{0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,5},{1,DEV_FAN, 1,0},    {1,DEV_FAN, 2,0},   {1,DEV_FAN, 3,0},   {1,DEV_FAN, 4,0}}},\
			   {4,20,{{1, DEV_FRE_FAN, 0, 0, 0},	{0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,5},{1,DEV_FAN, 1,10},  {1,DEV_FAN, 2,0},   {1,DEV_FAN, 3,0},   {1,DEV_FAN, 4,0}}},\
			   {5,25,{{1, DEV_FRE_FAN, 0, 0, 0},	{0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,5},{1,DEV_FAN, 1,10},  {1,DEV_FAN, 2,15}, {1,DEV_FAN, 3,0},   {1,DEV_FAN, 4,0}}},\
			   {6,30,{{1, DEV_FRE_FAN, 0, 0, 0},	{0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,5},{1,DEV_FAN, 1,10},  {1,DEV_FAN, 2,15}, {1,DEV_FAN, 3,20}, {1,DEV_FAN, 4,0}}},\
			   {7,35,{{1, DEV_FRE_FAN, 0, 0, 0},	{0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,5},{1,DEV_FAN, 1,10},  {1,DEV_FAN, 2,15}, {1,DEV_FAN, 3,20}, {1,DEV_FAN, 4,25}}}},\
			   30,30,20,07,13,10,30,32};
	
	HUMI_CTRL_CFG_T	humiCtrlCfg = {1,0x0900,0x1700,60,50,2,5,0,20,07,13,10,30,33};
	
	HEATER_CTRL_CFG_T heaterCtrlCfg = {0,0,0,20,07,13,10,30,34};
	
	LIGHT_CTRL_CFG_T lightCtrlCfg = {1,0x0900,0x2100,100,20,07,13,10,30,31};
	/*
	CONTROLER_CTRL_CFG_T initTemp={\
		{{{1,32,50,60},{7,30,55,60},{14,28,60,60},{21,26,65,60},{28,24,70,60},{35,22,70,90},{42,70,50,120}},300,0,20,07,13,10,30,31},
		{{\
			   {1,5, {{1, DEV_FRE_FAN, 0, 80, 150}, {0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,0},   {1,DEV_FAN, 1,0},   {1,DEV_FAN, 2,0},   {1,DEV_FAN, 3,0},   {1,DEV_FAN, 4,0}}},\
			   {2,10,{{1, DEV_FRE_FAN, 0, 100, 300},{0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,0},   {1,DEV_FAN, 1,0},   {1,DEV_FAN, 2,0},   {1,DEV_FAN, 3,0},   {1,DEV_FAN, 4,0}}},\
			   {3,15,{{1, DEV_FRE_FAN, 0, 0, 0},	{0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,150},{1,DEV_FAN, 1,0},    {1,DEV_FAN, 2,0},   {1,DEV_FAN, 3,0},   {1,DEV_FAN, 4,0}}},\
			   {4,20,{{1, DEV_FRE_FAN, 0, 0, 0},	{0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,150},{1,DEV_FAN, 1,150},  {1,DEV_FAN, 2,0},   {1,DEV_FAN, 3,0},   {1,DEV_FAN, 4,0}}},\
			   {5,25,{{1, DEV_FRE_FAN, 0, 0, 0},	{0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,150},{1,DEV_FAN, 1,150},  {1,DEV_FAN, 2,150}, {1,DEV_FAN, 3,0},   {1,DEV_FAN, 4,0}}},\
			   {6,30,{{1, DEV_FRE_FAN, 0, 0, 0},	{0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,150},{1,DEV_FAN, 1,150},  {1,DEV_FAN, 2,150}, {1,DEV_FAN, 3,150}, {1,DEV_FAN, 4,0}}},\
			   {7,35,{{1, DEV_FRE_FAN, 0, 0, 0},	{0, DEV_FRE_FAN, 1, 80, 150}},{{1,DEV_FAN, 0,150},{1,DEV_FAN, 1,150},  {1,DEV_FAN, 2,150}, {1,DEV_FAN, 3,150}, {1,DEV_FAN, 4,150}}}},\
			   300,300,20,07,13,10,30,32},\

		{1,0x0900,0x1700,60,50,2,5,0,20,07,13,10,30,33},\
		{0,0,0,20,07,13,10,30,34},\
		{1,0x0900,0x2100,20,07,13,10,30,31}\
		};
		*/


	buf =  malloc(size);
	if(RT_NULL == buf)
	{
		return FALSE;
	}
	num = Flash_Read(FLASH_CONTROLER_CTRL_CFG_ADDR, buf, size);
	if(num != size) 
	{
		if(buf) 	free(buf);
		return FALSE;
	}
	
	memcpy(&m_Controler_Ctrl_Cfg_t, buf, size);

	if(!Verify_RtcTime(m_Controler_Ctrl_Cfg_t.targetTemperCfg.saveTime))
	{
		//memcpy(&m_Controler_Ctrl_Cfg_t.targetTemperCfg, targetTemperCfg, sizeof(TARGET_TEMP_INFO_T));
		m_Controler_Ctrl_Cfg_t.targetTemperCfg = targetTemperCfg;
	}

	if(!Verify_RtcTime(m_Controler_Ctrl_Cfg_t.airCtrlCfg.saveTime))
	{
		//memcpy(&m_Controler_Ctrl_Cfg_t.airCtrlCfg, airCtrlCfg, sizeof(AirCtrlCfg_T));
		m_Controler_Ctrl_Cfg_t.airCtrlCfg = airCtrlCfg;
	}

	if(!Verify_RtcTime(m_Controler_Ctrl_Cfg_t.heaterCtrlCfg.saveTime))
	{
		//memcpy(&m_Controler_Ctrl_Cfg_t.heaterCtrlCfg, heaterCtrlCfg, sizeof(HEATER_CTRL_CFG_T));
		m_Controler_Ctrl_Cfg_t.heaterCtrlCfg = heaterCtrlCfg;
	}

	if(!Verify_RtcTime(m_Controler_Ctrl_Cfg_t.humiCtrlCfg.saveTim))
	{
		//memcpy(&m_Controler_Ctrl_Cfg_t.humiCtrlCfg, humiCtrlCfg, sizeof(HUMI_CTRL_CFG_T));
		m_Controler_Ctrl_Cfg_t.humiCtrlCfg = humiCtrlCfg;
	}

	if(!Verify_RtcTime(m_Controler_Ctrl_Cfg_t.lightCtrlCfg.saveTime))
	{
		//memcpy(&m_Controler_Ctrl_Cfg_t.lightCtrlCfg, lightCtrlCfg, sizeof(LIGHT_CTRL_CFG_T));
		m_Controler_Ctrl_Cfg_t.lightCtrlCfg = lightCtrlCfg;
	}

	
	num = Flash_Write(FLASH_CONTROLER_CTRL_CFG_ADDR, (char *) &m_Controler_Ctrl_Cfg_t, size);
	if(num != size)
	{
		if(buf) 	free(buf);
		return FALSE;
	}
		
	if(buf) 	free(buf);
	
	return TRUE;

}
//����������Ϣ
uint8_t Ctroler_AlarmCfg_Init(void)
{
	char *buf;
	uint16_t size = sizeof(CONTROLER_ALARM_CFG_T);
	uint16_t num;
	
	CONTROLER_ALARM_CFG_T  temp = {{{1,0,15,0,0,10},{1,1,15,0,0,10},{1,2,15,50,30,10},{1,3,15,80,00,10},{1,4,15,30,0,10}},20,07,13,10,30,39};

	
	buf =  malloc(size);
	if(RT_NULL == buf)
	{
		return FALSE;
	}
	num = Flash_Read(FLASH_CONTROLER_ALARM_CFG_ADDR, buf, size);
	if(num != size)	
	{
		if(buf) 	free(buf);
		return FALSE;
	}
		

	memcpy(&m_Controler_Alarm_Cfg_t, buf, size);
	if(!Verify_RtcTime(m_Controler_Alarm_Cfg_t.saveTime))
	{
		//memcpy(m_Controler_Alarm_Cfg_t, temp, sizeof(CONTROLER_ALARM_CFG_T));
		m_Controler_Alarm_Cfg_t = temp;
		memcpy(m_Controler_Alarm_Cfg_t.saveTime, rtcTime, sizeof(rtcTime));
		num = Flash_Write(FLASH_CONTROLER_ALARM_CFG_ADDR, (char *) &m_Controler_Alarm_Cfg_t, size);
		if(num != size)
		{
			if(buf) 	free(buf);
			return FALSE;
		}
	}

	if(buf) 	free(buf);
	
	return TRUE;
}

//����������Ϣ
uint8_t Room_Cfg_Init(void)
{
	 char  *buf;
	char testRtc[6]={20,7,20,8,00,00};
	uint16_t size = sizeof(ROOM_INFO_CFG_T);
	uint16_t num;
	
	ROOM_INFO_CFG_T  temp = {1,0x071E,0,200,30,20,07,13,10,30,39};

	
	buf =  malloc(size);
	if(RT_NULL == buf)
	{
		return FALSE;
	}
	num = Flash_Read(FLASH_CONTROLER_ROOM_CFG_ADDR, buf, size);
	if(num != size) 
	{
		if(buf) 	free(buf);
		return FALSE;
	}


	memcpy(&m_Room_Cfg_t, buf, size);
	if(!Verify_RtcTime(m_Room_Cfg_t.saveTime))
	{
		//memcpy(m_Room_Cfg_t, temp, sizeof(ROOM_INFO_CFG_T));
		m_Room_Cfg_t = temp;
		memcpy(m_Room_Cfg_t.saveTime, testRtc, sizeof(testRtc));
		num = Flash_Write(FLASH_CONTROLER_ROOM_CFG_ADDR, (char *) &m_Room_Cfg_t, size);
		if(num != size)
		{
			if(buf) 	free(buf);
			return FALSE;
		}
			
	}

	if(buf) 	free(buf);
	
	return TRUE;
}


/*
���ó�ʼ����
��������ʹ��ʹ��������´������������ʹ�ܡ�
*/


void Ctroler_Cfg_Refresh(void)
{
	uint8_t i = 0;
	uint8_t temp = 0;

	
	//����������ˢ��
	for(i = 0;i < DEV_NUM;i++)
	{
		Dev_info[i].Dev_EN = FALSE;
		Dev_info[i].Dev_sta = MB_MRE_TIMEDOUT;
	}
	if((SENSOR_TEMPER == (PERIPER_DEV_T)m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_TEMPER].type))
	{
		for(i = 0;i< SUPPORT_SENSOR_TEMPER_NUM;i++)
			Dev_info[TEMP1-1 + i].Dev_EN = 0;
		temp = m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_TEMPER].num;
		if(temp > 0)
		{
			for(i = 0 ;i < temp;i++)
				Dev_info[TEMP1-1 + i].Dev_EN = TRUE;
		}
	}

	if((SENSOR_HUIMI == (PERIPER_DEV_T)m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_HUIMI].type))
	{
		for(i = 0;i< SUPPORT_SENSOR_HUMI_NUM;i++)
			Dev_info[HUMI1-1 + i].Dev_EN = 0;
		temp = m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_HUIMI].num;
		if(temp > 0)
		{
			for(i = 0 ;i < temp;i++)
				Dev_info[HUMI1-1 + i].Dev_EN = TRUE;
		}
	}

	if((SENSOR_NH3 == (PERIPER_DEV_T)m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_NH3].type))
	{
		for(i = 0;i< SUPPORT_SENSOR_NH3_NUM;i++)
			Dev_info[NH3_1 -1 + i].Dev_EN = 0;
		temp = m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_NH3].num;
		if(temp > 0)
		{
			for(i = 0 ;i < temp;i++)
				Dev_info[NH3_1 -1 + i].Dev_EN = TRUE;
		}
	}
	if((SENSOR_PH == (PERIPER_DEV_T)m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_PH].type))
	{
		for(i = 0;i< SUPPORT_SENSOR_PH_NUM;i++)
			Dev_info[PH-1 + i].Dev_EN = 0;
		temp = m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_PH].num;
		if(temp > 0)
		{
			for(i = 0 ;i < temp;i++)
				Dev_info[PH-1 + i].Dev_EN = TRUE;
		}
	}
	if((SENSOR_WATER_PRESSURE == (PERIPER_DEV_T)m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_WATER_PRESSURE].type))
	{
		for(i = 0;i< SUPPORT_SENSOR_WATER_PRESSURE_NUM;i++)
			Dev_info[WATER_PRESU1-1 + i].Dev_EN = 0;
		temp = m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_WATER_PRESSURE].num;
		if(temp > 0)
		{
			for(i = 0 ;i < temp;i++)
				Dev_info[WATER_PRESU1-1 + i].Dev_EN = TRUE;
		}
	}
	if((SENSOR_POWER_METER == (PERIPER_DEV_T)m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_POWER_METER].type))
	{
		for(i = 0;i< SUPPORT_SENSOR_POWER_METER_NUM;i++)
			Dev_info[EP_METER -1 + i].Dev_EN = 0;
		temp = m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_POWER_METER].num;
		if(temp > 0)
		{
			for(i = 0 ;i < temp;i++)
				Dev_info[EP_METER -1 + i].Dev_EN = TRUE;
		}
	}
	if((SENSOR_WATER_PRESSURE == (PERIPER_DEV_T)m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_WATER_PRESSURE].type))
	{
		for(i = 0;i< SUPPORT_SENSOR_WATER_METER_NUM1;i++)
			Dev_info[WATER_METER-1 + i].Dev_EN = 0;
		temp = m_Controler_Dev_Cfg_t.sensorEnStruct[SENSOR_WATER_PRESSURE].num;
		if(temp > 0)
		{
			for(i = 0 ;i < temp;i++)
				Dev_info[WATER_METER-1 + i].Dev_EN = TRUE;
		}
	}

	//��������ˢ��
}

/*
�ϵ��ȡ������Ϣ������������Ϣ�����������豸������ȡͨ��������Ϣ,������������ˢ��ͨ�����á�
*/
uint8_t Ctroler_Init(void)
{
	uint8_t ret = TRUE;

	Ds1302_Readtime();
	rtcTime[0] = ds1302Data.year;
	rtcTime[1] = ds1302Data.month;
	rtcTime[2] = ds1302Data.day;
	rtcTime[3] = ds1302Data.hour;
	rtcTime[4] = ds1302Data.min;
	rtcTime[5] = ds1302Data.sec;
	
	//��flash�ж�ȡ����������Ϣ���������ʱ�䣬�������ʱ��Ϊ�Ƿ�ʱ�䣬����Ϊδ��������ã���ʼ�����á�
	ret = Ctroler_DevEnable_Init();
	
	//��flash�ж�ȡ��������������Ϣ���������ʱ�䣬�������ʱ��Ϊ�Ƿ�ʱ�䣬����Ϊδ��������ã���ʼ�����á�
	ret = Ctroler_DevCtrlCfg_Init();
	
	//��flash�ж�ȡ����������Ϣ���������ʱ�䣬�������ʱ��Ϊ�Ƿ�ʱ�䣬����Ϊδ��������ã���ʼ�����á�
	ret = Ctroler_AlarmCfg_Init();
	
	//��flash�ж�ȡ����������Ϣ��
	ret = Room_Cfg_Init();
	
	Ctroler_Cfg_Refresh();
	return ret;
}

/*
���룺��ǰRTCʱ�䡣uint8_t rtc[6]��hex��ʽ
�������ǰĿ���¶ȵ�������Ϣ
���㵱ǰʱ������������ʱ��Ĳ�ֵ�����㵱ǰ��������������ֵ��ȡĿ���¶ȣ���Сͨ�����
*/
struct TargetTemp_S Ctroler_Get_TargetTempCfg(uint8_t *pRtc)
{
	struct TargetTemp_S structTemp;


	return structTemp;
}


/*
���룺��ǰRTCʱ�䡣uint8_t rtc[6]��hex��ʽ
�������ǰ����������
���㵱ǰʱ������������ʱ��Ĳ�ֵ�����㵱ǰ��������
*/
#define	DAY_SECONDE_NUM	(60*60*24)

uint32_t	RTC_Get_Second(DS1302DATA rtcTime)
{
	uint16_t i = 0;
	uint64_t sec = 0;
	uint16_t year = 2000+ rtcTime.year;
	
	if(year < 1970)	
		return 0;

	for(i = 1970;i < year;i++)
	{
		if(Is_Leap_Year(year))
		{
			sec += 31622400;//���������
		}
		else
		{
			sec += 31536000;//ƽ�������
		}
	}

	rtcTime.month -= 1;
	for(i = 0;i < rtcTime.month;i++)//���·ݵ�������ӡ�
	{
		sec += (uint32_t)mon_table[i]*86400;
		if(Is_Leap_Year(year) && i == 1)
		{
			sec += 86400;	//����2�·�����һ�������
		}
	}
	sec += (uint32_t)(rtcTime.day - 1)*86400;
	sec += (uint32_t)(rtcTime.hour)*3600;
	sec += (uint32_t)rtcTime.min*60;
	sec += rtcTime.sec;

	return sec;
}

uint8_t Ctroler_Cal_RunDay(void)
{
		return TRUE;
}
//��ȡ��ǰ������������к�.����������0��ʼ
uint8_t Ctroler_Get_CurDayNum(void)
{
	

	//�����������õı���ʱ�䣬�Լ���ǰʱ�䣬����
	uint32_t tmp1 = 0;
	uint32_t tmp2 = 0;
	//uint32_t deltSeconde = 0;
	uint8_t nDays = 0;
	//uint16_t RefreshDaySec= 0;
	DS1302DATA ds1302Date_temp;
	
	
	ds1302Date_temp.year = m_Room_Cfg_t.saveTime[0];
	ds1302Date_temp.month = m_Room_Cfg_t.saveTime[1];
	ds1302Date_temp.day = m_Room_Cfg_t.saveTime[2];
	ds1302Date_temp.hour = m_Room_Cfg_t.saveTime[3];
	ds1302Date_temp.min = m_Room_Cfg_t.saveTime[4];
	ds1302Date_temp.sec = m_Room_Cfg_t.saveTime[5];
	tmp1 = RTC_Get_Second(ds1302Date_temp);
	tmp2 = RTC_Get_Second( ds1302Data);
	tmp2 -= tmp1;
	tmp2 /= DAY_SECONDE_NUM;
	nDays = (uint8_t)tmp2;
	//nDays= (uint8_t)(tmp2 - tmp1)/DAY_SECONDE_NUM;
	//�������ˢ��ʱ�䣬�����Ƿ�+1
	tmp1 = 0;
	//tmp1+=(m_Room_Cfg_t.runDaysRefreshTime >> 8)*60*60;
	//tmp1+=(m_Room_Cfg_t.runDaysRefreshTime & 0xff)*60;
	tmp1 = (m_Room_Cfg_t.runDaysRefreshTime >> 8)*60*60 + (m_Room_Cfg_t.runDaysRefreshTime & 0xff)*60;
	tmp2 = (ds1302Data.hour *60*60) + ds1302Data.min *60;
	if(tmp2 - tmp1)
	{
		nDays += 1;
	}
	
	return nDays;
}
//��ȡ��ǰ����ʱ���Ӧ��Ŀ���¶�����������ֵ��0��ʼ
uint8_t Ctroler_Get_DayNumLevel(uint8_t dayNum)
{
	uint8_t i = 0;

	for(i = (TARGET_TEMP_NUM - 1);i > 0;i--)
	{
		if(m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[i].dayNum == 0)
			continue;
		if(dayNum >= m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[i].dayNum)
		{
			return i;
		}
	}
	return 0;
}

//�¶����߲�ֵ��ȡ
uint16_t Controler_Get_TargetTemper(uint8_t dayNum)
{
	uint8_t i = 0;
	uint16_t temp1  = 0, temp2 = 0,deltTemp = 0;
	uint8_t day1 = 0, day2 = 0, deltDay = 0;
	float date;
	uint16_t temp = 0;
	
	for(i = (TARGET_TEMP_NUM - 1);i > 0;i--)
	{
		if(m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[i].dayNum == 0)
			continue;
		if(dayNum >= m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[i].dayNum)
		{
			break;
		}
	}
	
	if(i == TARGET_TEMP_NUM-1)
	{
		return m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[i].targetTemp;
	}

	temp1 = m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[i].targetTemp;
	temp2 = m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[i + 1].targetTemp;
	deltTemp = temp1 - temp2;
	day1 = m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[i].dayNum;
	day2 = m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[i + 1].dayNum;
	deltDay = day2 - day1;
	if(dayNum == day1)
		return temp1;
	if(dayNum == day2)
		return temp2;
	
	date = deltTemp * 100 / deltDay;
	date *=  (day2 - dayNum);
	date += (temp2*100);
	temp = (uint16_t)date/100;
	//temp = temp2 + (day2 - dayNum)*(deltTemp / deltDay);
	return temp;
}
/*
���룺��ǰ�¶ȣ���ǰ��������������
�������ǰ��ͨ�缶��0xff������ͨ�硣other:ͨ�缶��0��ʼ
��ȡ��ǰ��ͨ����
*/
uint8_t Ctroler_Get_AirCtrlLevel(uint8_t dayLevel, uint16_t temper)
{
	uint8_t airLevel = 0;
	uint16_t targetTemper = 0;
	uint16_t curTemper = temper / 10;
	uint8_t i = 0;
	

	//��ȡĿ���¶�
	//targetTemper = m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[dayLevel].targetTemp;
	//����Ŀ���¶Ȼ�ȡ��ǰͨ�缶��
	if(curTemper <= curTargetTemp)
	{
		return 0xff;
	}
	for(i = CONTROL_LEVEL_MAX - 1;i > 0;i--)
	{
		if(m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[i].temperShift == 0)
			continue;
		targetTemper = curTargetTemp + m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[i].temperShift ;
		if(curTemper >= targetTemper)
		{
			return i;
		}
	}
	
	return airLevel;
}
//��Сͨ��׶μ����Ƶ���ͨ����
uint16_t Ctroler_Get_FrefanDacOut(uint8_t speed)
{
	uint16_t temp = 0;
	uint8_t i = 0;
	float k = 0;
	
	/*
	for(i = 0; i < 9;i++)
	{
		if(speed >= m_Controler_Dev_Cfg_t.freFanDacChart[i] && speed < m_Controler_Dev_Cfg_t.freFanDacChart[i+1] )
		{
			k = (m_Controler_Dev_Cfg_t.freFanDacChart[i+1] - m_Controler_Dev_Cfg_t.freFanDacChart[i])/10;
			k = (speed - i*10) *k;
			temp = m_Controler_Dev_Cfg_t.freFanDacChart[i] + k;
			return temp;
		}
	}
	*/
	if(speed > 0)
	{
		temp = 300+(speed*200/10);
		if(temp >2100)
			temp = 2100;
	}

	return temp;
	//return m_Controler_Dev_Cfg_t.freFanDacChart[9];
}


//ʪ������
uint8_t Ctroler_Get_ColdPad_Sta(uint16_t targetTemper)
{
	uint16_t enTime = m_Controler_Ctrl_Cfg_t.humiCtrlCfg.enTime;
	uint16_t disenTime = m_Controler_Ctrl_Cfg_t.humiCtrlCfg.disenTime;
	uint16_t curTime = (ds1302Data.hour << 8) + ds1302Data.min;
	uint16_t runSec = m_Controler_Ctrl_Cfg_t.humiCtrlCfg.runTime*60;
	uint16_t stopSec = m_Controler_Ctrl_Cfg_t.humiCtrlCfg.stopTime *60;

	uint16_t curTemp = curAvgTemp / 10;
	uint16_t curHumi = curAvgHumi / 10;
	static uint8_t coldPadRunFlag = FALSE;

	if(m_Controler_Ctrl_Cfg_t.humiCtrlCfg.en)
	{
		//�ж��Ƿ���ʪ�Ȳ���
		if(m_Controler_Ctrl_Cfg_t.humiCtrlCfg.compValue != 0)
		{
			if(curHumi >= m_Controler_Ctrl_Cfg_t.humiCtrlCfg.compValue)
			{
				m_Equipment_RunInfo_t.coldPadRunInfo.sta = DEV_STOP;
				m_Equipment_RunInfo_t.coldPadRunInfo.runCount = 0;
				m_Equipment_RunInfo_t.coldPadRunInfo.stopCount = 0;
				return TRUE;
			}
		}
		//�жϵ�ǰʱ���Ƿ�λ��ʪ����������
		if(curTime >= enTime && curTime <= disenTime)
		{
			//ʪ�������¶�ƫ�Ʊ��������á��ҿ���ʱ�䲻Ϊ0
			if(m_Controler_Ctrl_Cfg_t.humiCtrlCfg.runTempShift > 0 && m_Controler_Ctrl_Cfg_t.humiCtrlCfg.runTime > 0)
			{
				//�жϵ�ǰ�¶��Ƿ���Ҫʪ������
				if(DEV_RUN == m_Equipment_RunInfo_t.coldPadRunInfo.sta)
				{
					if(curTemp < (targetTemper + m_Controler_Ctrl_Cfg_t.humiCtrlCfg.stopTempShift))
					{
						m_Equipment_RunInfo_t.coldPadRunInfo.sta = DEV_STOP;
						m_Equipment_RunInfo_t.coldPadRunInfo.runCount = 0;
						m_Equipment_RunInfo_t.coldPadRunInfo.stopCount = 0;
						coldPadRunFlag = FALSE;
					}
					else
					{
						if(m_Equipment_RunInfo_t.coldPadRunInfo.runCount >= runSec)
						{
							m_Equipment_RunInfo_t.coldPadRunInfo.sta = DEV_STOP;
							m_Equipment_RunInfo_t.coldPadRunInfo.runCount = 0;
							m_Equipment_RunInfo_t.coldPadRunInfo.stopCount = 0;
						}
					}
				}
				else if(DEV_STOP == m_Equipment_RunInfo_t.coldPadRunInfo.sta)
				{
					if(curTemp >= (targetTemper + m_Controler_Ctrl_Cfg_t.humiCtrlCfg.runTempShift))
					{
						m_Equipment_RunInfo_t.coldPadRunInfo.sta = DEV_RUN;
						m_Equipment_RunInfo_t.coldPadRunInfo.runCount = 0;
						m_Equipment_RunInfo_t.coldPadRunInfo.stopCount = 0;
						coldPadRunFlag = TRUE;
					}
					else
					{
						if(coldPadRunFlag)
						{
							if(m_Equipment_RunInfo_t.coldPadRunInfo.stopCount >= stopSec)
							{
								m_Equipment_RunInfo_t.coldPadRunInfo.sta = DEV_RUN;
								m_Equipment_RunInfo_t.coldPadRunInfo.runCount = 0;
								m_Equipment_RunInfo_t.coldPadRunInfo.stopCount = 0;
							}
						}
						
					}
				}
			}
			else
			{
				m_Equipment_RunInfo_t.coldPadRunInfo.sta = DEV_STOP;
			}
		}
		else
		{
			m_Equipment_RunInfo_t.coldPadRunInfo.sta = DEV_STOP;
		}
	}
	return TRUE;
}
//����������
uint8_t Ctroler_Get_Heater_Sta(uint16_t targetTemp)
{
	uint16_t curTemp = curAvgTemp /  10;
	 
	if(m_Controler_Ctrl_Cfg_t.heaterCtrlCfg.en)
	{
		if(m_Controler_Ctrl_Cfg_t.heaterCtrlCfg.runTempShift > 0)
		{
			if(DEV_RUN == m_Equipment_RunInfo_t.heaterRunInfo.sta)
			{
				if(curTemp > (targetTemp - m_Controler_Ctrl_Cfg_t.heaterCtrlCfg.stopTempShift))
				{
					m_Equipment_RunInfo_t.heaterRunInfo.runCount = 0;
					m_Equipment_RunInfo_t.heaterRunInfo.stopCount = 0;
					m_Equipment_RunInfo_t.heaterRunInfo.sta = DEV_STOP;
				}
			}
			else if(DEV_STOP == m_Equipment_RunInfo_t.heaterRunInfo.sta)
			{
				if(curTemp <= (targetTemp - m_Controler_Ctrl_Cfg_t.heaterCtrlCfg.runTempShift))
				{
					m_Equipment_RunInfo_t.heaterRunInfo.runCount = 0;
					m_Equipment_RunInfo_t.heaterRunInfo.stopCount = 0;
					m_Equipment_RunInfo_t.heaterRunInfo.sta = DEV_RUN;
				}
			}
		}
	}
	
	return TRUE;
}
//�ƹ�����
uint8_t Ctroler_Get_Light_Sta(void)
{
	//uint16_t enTime = m_Controler_Ctrl_Cfg_t.lightCtrlCfg.enTime;
	//uint16_t disenTime =  m_Controler_Ctrl_Cfg_t.lightCtrlCfg.disenTime;
	uint16_t curTime = (ds1302Data.hour << 8) + ds1302Data.min;

	if(m_Controler_Ctrl_Cfg_t.lightCtrlCfg.en)
	{
		if(curTime >= m_Controler_Ctrl_Cfg_t.lightCtrlCfg.enTime && curTime <= m_Controler_Ctrl_Cfg_t.lightCtrlCfg.disenTime)
		{
			m_Equipment_RunInfo_t.lightRunInfo.sta = DEV_RUN;
		}
		else
		{
			m_Equipment_RunInfo_t.lightRunInfo.sta = DEV_STOP;
		}
	}


	
	return TRUE;
}

/*
���ϴ�����
*/
void Controler_Alarm_Proc(void)
{
	uint8_t i = 0;
	uint8_t time[6] = {0};
	
	
	//�ϵ��ж�
	if(m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_POWER_OFF].en)
	{
		if(IS_POWER_OFF)
		{
			m_Alarm_Sta_t.alarmSta[ALARM_TYPE_POWER_OFF].sta = ALARM_STA_OCCUR;
			if(m_Alarm_Sta_t.alarmSta[ALARM_TYPE_POWER_OFF].confirmCnt >= m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_POWER_OFF].confirmTime)
			{
				m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_POWER_OFF].alarmSta = ALARM_STA_OCCUR;
				memcpy(m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_POWER_OFF].time, (uint8_t *)&ds1302Data,6);
				m_Alarm_Sta_t.alarmSta[ALARM_TYPE_POWER_OFF].confirmCnt = 0;
			}
		}
		else
		{
			m_Alarm_Sta_t.alarmSta[ALARM_TYPE_POWER_OFF].confirmCnt = 0;
			m_Alarm_Sta_t.alarmSta[ALARM_TYPE_POWER_OFF].sta = ALARM_STA_NO;
			if(ALARM_STA_OCCUR ==m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_POWER_OFF].alarmSta)
				memcpy(m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_POWER_OFF].time, (uint8_t *)&ds1302Data,6);
			m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_POWER_OFF].alarmSta = ALARM_STA_NO;
		}
	}
	//���±���
	if(m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_HIGH_TEMPER].en)
	{
		if(curAvgTemp > 0)
		{
			//δ����ʱ�����±��������δ�������£�����Ƿ���¼��Ƿ�����ȷ��ʱ�䡣
			if(m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_HIGH_TEMPER].alarmSta == ALARM_STA_NO)
			{
				if(curAvgTemp >= (curTargetTemp + m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_HIGH_TEMPER].alarmOccurShit)*10)
				{
					m_Alarm_Sta_t.alarmSta[ALARM_TYPE_HIGH_TEMPER].sta = ALARM_STA_OCCUR;
					if(m_Alarm_Sta_t.alarmSta[ALARM_TYPE_HIGH_TEMPER].confirmCnt >= m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_HIGH_TEMPER].confirmTime)
					{
						m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_HIGH_TEMPER].alarmSta = ALARM_STA_OCCUR;
						memcpy(m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_HIGH_TEMPER].time, (uint8_t *)&ds1302Data,6);
						m_Alarm_Sta_t.alarmSta[ALARM_TYPE_HIGH_TEMPER].confirmCnt = 0;
					}
				}
				else
				{
					m_Alarm_Sta_t.alarmSta[ALARM_TYPE_HIGH_TEMPER].confirmCnt = 0;
					m_Alarm_Sta_t.alarmSta[ALARM_TYPE_HIGH_TEMPER].sta = ALARM_STA_NO;
				}
			}
			else 
			{
				if(curAvgTemp < (curTargetTemp + m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_HIGH_TEMPER].alarmRecoverShit)*10)
				{
					m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_HIGH_TEMPER].alarmSta = ALARM_STA_NO;
				
					m_Alarm_Sta_t.alarmSta[ALARM_TYPE_HIGH_TEMPER].confirmCnt = 0;
					if(ALARM_STA_OCCUR ==m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_HIGH_TEMPER].alarmSta)
						memcpy(m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_HIGH_TEMPER].time, (uint8_t *)&ds1302Data,6);
					m_Alarm_Sta_t.alarmSta[ALARM_TYPE_HIGH_TEMPER].sta = ALARM_STA_NO;
				}
			}
		}
	}

	//���±���
	if(m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_LOW_TEMPER].en)
	{
		if(curAvgTemp > 0)
		{
			if(m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_LOW_TEMPER].alarmSta == ALARM_STA_NO)
			{
				if(curAvgTemp < (curTargetTemp - m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_LOW_TEMPER].alarmOccurShit)*10)
				{
					m_Alarm_Sta_t.alarmSta[ALARM_TYPE_LOW_TEMPER].sta = ALARM_STA_OCCUR;
					if(m_Alarm_Sta_t.alarmSta[ALARM_TYPE_LOW_TEMPER].confirmCnt >= m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_LOW_TEMPER].confirmTime)
					{
						m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_LOW_TEMPER].alarmSta = ALARM_STA_OCCUR;
						memcpy(m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_LOW_TEMPER].time, (uint8_t *)&ds1302Data,6);
						m_Alarm_Sta_t.alarmSta[ALARM_TYPE_LOW_TEMPER].confirmCnt = 0;
					}
				}
				else
				{
					m_Alarm_Sta_t.alarmSta[ALARM_TYPE_LOW_TEMPER].confirmCnt = 0;
					m_Alarm_Sta_t.alarmSta[ALARM_TYPE_LOW_TEMPER].sta = ALARM_STA_NO;
				}
			}
			else 
			{
				if(curAvgTemp >= (curTargetTemp - m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_LOW_TEMPER].alarmRecoverShit)*10)
				{
					m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_LOW_TEMPER].alarmSta = ALARM_STA_NO;
				
					m_Alarm_Sta_t.alarmSta[ALARM_TYPE_LOW_TEMPER].confirmCnt = 0;
					if(ALARM_STA_OCCUR ==m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_LOW_TEMPER].alarmSta)
						memcpy(m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_LOW_TEMPER].time, (uint8_t *)&ds1302Data,6);
					m_Alarm_Sta_t.alarmSta[ALARM_TYPE_LOW_TEMPER].sta = ALARM_STA_NO;
				}
			}
		}
			

	}

	//�豸���ϱ���
	//���벼�֣���Ƶ1����Ƶ2�����1-5������ ʪ�� �ƹ�
	if(m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_DEV_ERR].en)
	{
		if(m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_DEV_ERR].alarmSta == ALARM_STA_NO)
		{
			for(i = 0;i < 20;i++)
			{
				if(i < 2)
				{
					//��Ƶ��������жϣ�������ֶ������Զ�����ʱ����Զ������ź�>0����Ϊ������У��������������������Ϊ����
					if(inputBits & (0X01 << 2 * i)) 
					{
						if((m_DriveBoard_Data_t.sendData.dacOutValue[i] > 0) && !(inputBits & (0X02 << 2 * i)) )
						{
							m_Alarm_Sta_t.alarmSta[ALARM_TYPE_DEV_ERR].sta = ALARM_STA_OCCUR;
							if(m_Alarm_Sta_t.alarmSta[ALARM_TYPE_DEV_ERR].confirmCnt >= m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_DEV_ERR].confirmTime)
							{
								m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_DEV_ERR].alarmSta = ALARM_STA_OCCUR;
								m_Alarm_Sta_t.alarmSta[ALARM_TYPE_DEV_ERR].confirmCnt = 0;
								break;
							}
						}
						
					}
				}
				else if(inputBits & (0x01 << (2*i)) != inputBits & (0x02 << (2*i)))
				{
					m_Alarm_Sta_t.alarmSta[ALARM_TYPE_DEV_ERR].sta = ALARM_STA_OCCUR;
					if(m_Alarm_Sta_t.alarmSta[ALARM_TYPE_DEV_ERR].confirmCnt >= m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_DEV_ERR].confirmTime)
					{
						m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_DEV_ERR].alarmSta = ALARM_STA_OCCUR;
						m_Alarm_Sta_t.alarmSta[ALARM_TYPE_DEV_ERR].confirmCnt = 0;
						break;
					}
				}
			}
			m_Alarm_Proc_t.alarmPorc[ALARM_TYPE_DEV_ERR].alarmSta = ALARM_STA_NO;
			m_Alarm_Sta_t.alarmSta[ALARM_TYPE_DEV_ERR].confirmCnt = 0;	
		}
	}

	//���������ϣ�Ŀǰ������¶ȴ���������
	if(m_Controler_Alarm_Cfg_t.devAlarmCfg[ALARM_TYPE_SENSOR_ERR].en)
	{
		
	}
	
}

/*
��ȡ��ǰ�豸���������á�
*/
uint8_t Ctroler_Get_DevRunCfg()
{
	static uint8_t isInit = TRUE;
	static uint8_t lastDayNum = 0;
	static uint8_t lastAirLevel = 0;	
	uint16_t runSec = 0;
	uint16_t workCycle = 0;
	uint8_t speed = 0;
	
	uint8_t i = 0;


	Controler_Alarm_Proc();
	rt_thread_delay(50);
	
	curDayNum = Ctroler_Get_CurDayNum();
	curDayLevel = Ctroler_Get_DayNumLevel(curDayNum);
	//curTargetTemp = m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[curDayLevel].targetTemp;
	curTargetTemp = Controler_Get_TargetTemper(curDayNum);
	curAirLevel = Ctroler_Get_AirCtrlLevel(curDayLevel, curAvgTemp);



	if(lastDayNum != curDayNum  || lastAirLevel != curAirLevel)
	{
		isInit = TRUE;
		lastDayNum = curDayNum ;
		lastAirLevel = curAirLevel;
	}

	//��Сͨ��
	if(0XFF == curAirLevel)
	{
		
		speed = m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[dayNumLevel].minAirSpeed;
		runSec = m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[dayNumLevel].minAirRuntime;
		workCycle = m_Controler_Ctrl_Cfg_t.targetTemperCfg.minAirSupport_cycleTime;

		if(isInit)
		{
			if(speed > 0 && runSec > 0)
			{
				m_Equipment_RunInfo_t.freFanRunInfo[0].sta = DEV_RUN;
				//m_DriveBoard_Data_t.sendData.dacOutValue[0] = speed;
				m_Equipment_RunInfo_t.freFanRunInfo[0].runCount = 0;
				m_Equipment_RunInfo_t.freFanRunInfo[0].stopCount = 0;
				m_Equipment_RunInfo_t.freFanRunInfo[0].speed = speed;
			}
			isInit = FALSE;
		}
		else
		{
			if(DEV_RUN == m_Equipment_RunInfo_t.freFanRunInfo[0].sta)
			{
				if(m_Equipment_RunInfo_t.freFanRunInfo[0].runCount >= runSec)
				{
					m_Equipment_RunInfo_t.freFanRunInfo[0].sta = DEV_STOP;
					//m_DriveBoard_Data_t.sendData.dacOutValue[0] = 0;
					m_Equipment_RunInfo_t.freFanRunInfo[0].runCount = 0;
					m_Equipment_RunInfo_t.freFanRunInfo[0].stopCount = 0;
					m_Equipment_RunInfo_t.freFanRunInfo[0].speed = 0;
				}
			}
			else if(DEV_STOP == m_Equipment_RunInfo_t.freFanRunInfo[0].sta)
			{
				if(m_Equipment_RunInfo_t.freFanRunInfo[0].stopCount >= (workCycle - runSec))
				{
					m_Equipment_RunInfo_t.freFanRunInfo[0].sta = DEV_RUN;
					//m_DriveBoard_Data_t.sendData.dacOutValue[0] = speed;
					m_Equipment_RunInfo_t.freFanRunInfo[0].runCount = 0;
					m_Equipment_RunInfo_t.freFanRunInfo[0].stopCount = 0;
					m_Equipment_RunInfo_t.freFanRunInfo[0].speed = speed;
				}
			}
		}
		
	}
	else
	{
		//��Ƶ�������
		for(i = 0;i < FRE_FAN_NUM_MAX;i++)
		{
			if(m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[curAirLevel].freFanCfgArry[i].en)
			{
				speed = m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[curAirLevel].freFanCfgArry[i].speed;
				runSec = m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[curAirLevel].freFanCfgArry[i].runTime;
				workCycle = m_Controler_Ctrl_Cfg_t.airCtrlCfg.freFanCycleTime;
				if(isInit)
				{
					if(runSec > 0 && speed > 0)
					{
						m_Equipment_RunInfo_t.freFanRunInfo[i].speed = speed;
						m_Equipment_RunInfo_t.freFanRunInfo[i].runCount = 0;
						m_Equipment_RunInfo_t.freFanRunInfo[i].stopCount = 0;
						m_Equipment_RunInfo_t.freFanRunInfo[i].sta = DEV_RUN;
					}
					else
					{
						m_Equipment_RunInfo_t.freFanRunInfo[i].speed = 0;
						m_Equipment_RunInfo_t.freFanRunInfo[i].runCount = 0;
						m_Equipment_RunInfo_t.freFanRunInfo[i].stopCount = 0;
						m_Equipment_RunInfo_t.freFanRunInfo[i].sta = DEV_STOP;
					}
				}
				else
				{
					if(DEV_RUN == m_Equipment_RunInfo_t.freFanRunInfo[i].sta)
					{
						if(m_Equipment_RunInfo_t.freFanRunInfo[i].runCount >= runSec)
						{
							m_Equipment_RunInfo_t.freFanRunInfo[i].speed = 0;
							m_Equipment_RunInfo_t.freFanRunInfo[i].runCount = 0;
							m_Equipment_RunInfo_t.freFanRunInfo[i].stopCount = 0;
							m_Equipment_RunInfo_t.freFanRunInfo[i].sta = DEV_STOP;
						}
					}
					else if(DEV_STOP == m_Equipment_RunInfo_t.freFanRunInfo[i].sta)
					{
						if(m_Equipment_RunInfo_t.freFanRunInfo[i].runCount >= runSec)
						{
							m_Equipment_RunInfo_t.freFanRunInfo[i].speed = speed;
							m_Equipment_RunInfo_t.freFanRunInfo[i].runCount = 0;
							m_Equipment_RunInfo_t.freFanRunInfo[i].stopCount = 0;
							m_Equipment_RunInfo_t.freFanRunInfo[i].sta = DEV_RUN;
						}
					}
				}
			}
			else
			{
				m_Equipment_RunInfo_t.freFanRunInfo[i].sta = DEV_STOP;
				m_Equipment_RunInfo_t.freFanRunInfo[i].speed = 0;
			}
			
		}
		//��ͨ�������
		for(i = 0;i < FAN_NUM_MAX;i++)
		{
			if(m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[curAirLevel].fanCfgArry[i].en)
			{
				runSec = m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[curAirLevel].fanCfgArry[i].runTime;
				workCycle = m_Controler_Ctrl_Cfg_t.airCtrlCfg.fanCycleTim;

				if(isInit)
				{
					if(runSec > 0)
					{
						m_Equipment_RunInfo_t.fanDevRunInfo[i].runCount = 0;
						m_Equipment_RunInfo_t.fanDevRunInfo[i].stopCount = 0;
						m_Equipment_RunInfo_t.fanDevRunInfo[i].sta = DEV_RUN;
					}
					else
					{
						m_Equipment_RunInfo_t.fanDevRunInfo[i].runCount = 0;
						m_Equipment_RunInfo_t.fanDevRunInfo[i].stopCount = 0;
						m_Equipment_RunInfo_t.fanDevRunInfo[i].sta = DEV_STOP;

					}
				}		
				else
				{
					if(DEV_RUN == m_Equipment_RunInfo_t.fanDevRunInfo[i].sta)
					{
						if(m_Equipment_RunInfo_t.fanDevRunInfo[i].runCount >= runSec)
						{
							m_Equipment_RunInfo_t.fanDevRunInfo[i].runCount = 0;
							m_Equipment_RunInfo_t.fanDevRunInfo[i].stopCount = 0;
							m_Equipment_RunInfo_t.fanDevRunInfo[i].sta = DEV_STOP;
						}
					}
					else if(DEV_STOP == m_Equipment_RunInfo_t.fanDevRunInfo[i].sta)
					{
						if(m_Equipment_RunInfo_t.fanDevRunInfo[i].runCount >= (workCycle-runSec))
						{
							m_Equipment_RunInfo_t.fanDevRunInfo[i].runCount = 0;
							m_Equipment_RunInfo_t.fanDevRunInfo[i].stopCount = 0;
							m_Equipment_RunInfo_t.fanDevRunInfo[i].sta = DEV_RUN;
						}
					}
				}
			}
			else
			{
				m_Equipment_RunInfo_t.fanDevRunInfo[i].sta = DEV_STOP;
			}
		}
		isInit = FALSE;
	}

	Ctroler_Get_Heater_Sta(curTargetTemp);
	Ctroler_Get_ColdPad_Sta(curTargetTemp);
	if(!m_LightCtrl_En)
		Ctroler_Get_Light_Sta();
	for(i = 0;i < FRE_FAN_NUM_MAX;i++)
	{
		if(m_Equipment_RunInfo_t.freFanRunInfo[i].sta && m_Equipment_RunInfo_t.freFanRunInfo[i].speed > 0)
		{
			m_DriveBoard_Data_t.sendData.dacOutValue[i] = Ctroler_Get_FrefanDacOut(m_Equipment_RunInfo_t.freFanRunInfo[i].speed);
			
		}	
		else
			m_DriveBoard_Data_t.sendData.dacOutValue[i] = 0;
	}
	for(i = 0;i< FAN_NUM_MAX;i++)
	{
		if(m_Equipment_RunInfo_t.fanDevRunInfo[i].sta)
		{
			m_DriveBoard_Data_t.sendData.outBits |= (1 << i);
		}
		else
		{
			m_DriveBoard_Data_t.sendData.outBits &= ~(1 << i);
		}
	}
	if(m_Equipment_RunInfo_t.heaterRunInfo.sta)
		m_DriveBoard_Data_t.sendData.outBits |= (1 << (FAN_NUM_MAX));
	else
		m_DriveBoard_Data_t.sendData.outBits &= ~ (1 << (FAN_NUM_MAX));

	if(m_Equipment_RunInfo_t.coldPadRunInfo.sta)
		m_DriveBoard_Data_t.sendData.outBits |= (1 << (FAN_NUM_MAX + 1));
	else
		m_DriveBoard_Data_t.sendData.outBits &= ~(1 << (FAN_NUM_MAX + 1));

	if(m_Equipment_RunInfo_t.lightRunInfo.sta)
		m_DriveBoard_Data_t.sendData.outBits |= (1 << (FAN_NUM_MAX + 2));
	else
		m_DriveBoard_Data_t.sendData.outBits &= ~(1 << (FAN_NUM_MAX + 2));

	return TRUE;
}


//Vector gDevCondState[seasonMax][DAY_LINE_MAX];//�����������¶�����

//uint8_t	dayNum[DAY_LINE_MAX]= {1,14,28,42,49,0,0};//ÿ�������ж�Ӧ��ֵ��
//targetTemp_t	targetTemp[DAY_LINE_MAX] = {{1,28},{14,26},{28,24},{42,22},{49,22},{0,0},{0,0}};




/*
void initCondState()
{
    int i = 0;
	int j = 0;



    ///һ���ܱ�������ģʽ��ʼ��
    conditionRunner a1 = {{{0,27},{0,1}},{30,0,0,0,0,0,0}};
    conditionRunner a2 = {{{27,28},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner a3 = {{{28,29},{0,1}},{100,100,0,0,0,0,0}};

	
    ///һ���ܱ�������ģʽ��ʼ��
    conditionRunner b1 = {{{0,27},{0,1}},{60,0,0,0,0,0,0}};
    conditionRunner b2 = {{{27,28},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner b3 = {{{28,38},{0,1}},{100,1,0,0,0,0,0}};


    ///һ���ܱ����ļ�ģʽ��ʼ��
    conditionRunner d1 = {{{0,27},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner d2 = {{{27,28},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner d3 = {{{28,29},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner d4 = {{{29,30},{0,1}},{30,1,1,1,0,0,0}};
    conditionRunner d5 = {{{30,31},{0,1}},{100,1,1,1,1,0,0}};
    conditionRunner d6 = {{{31,32},{0,1}},{100,1,1,1,1,0,1}};
    conditionRunner d7 = {{{32,38},{0,1}},{100,1,1,1,1,1,1}};

	    ///�����ܱ�������ģʽ��ʼ��
    conditionRunner f1 = {{{0,25},{0,1}},{60,0,0,0,0,0,0}};
    conditionRunner f2 = {{{25,26},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner f3 = {{{26,27},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner f4 = {{{27,28},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner f5 = {{{28,38},{0,1}},{100,1,1,0,0,0,0}};


     ///�����ܱ�������ģʽ��ʼ��
    conditionRunner e1 = {{{0,25},{0,1}},{50,0,0,0,0,0,0}};
    conditionRunner e2 = {{{25,26},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner e3 = {{{26,38},{0,1}},{100,1,0,0,0,0,0}};


	    ///�����ܱ����ļ�ģʽ��ʼ��
    conditionRunner g1 = {{{0,25},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner g2 = {{{25,26},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner g3 = {{{26,27},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner g4 = {{{27,28},{0,1}},{100,1,1,1,0,0,0}};
    conditionRunner g5 = {{{28,29},{0,1}},{100,1,1,1,1,0,0}};
    conditionRunner g6 = {{{29,30},{0,1}},{100,1,1,1,1,0,1}};
    conditionRunner g7 = {{{30,38},{0,1}},{100,1,1,1,1,1,1}};

	    ///�����ܱ�������ģʽ��ʼ��
    conditionRunner h1 = {{{0,23},{0,1}},{60,0,0,0,0,0,0}};
    conditionRunner h2 = {{{23,24},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner h3 = {{{24,38},{0,1}},{100,1,0,0,0,0,0}};

	    ///�����ܱ�������ģʽ��ʼ��
    conditionRunner i1 = {{{0,23},{0,1}},{60,0,0,0,0,0,0}};
    conditionRunner i2 = {{{23,24},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner i3 = {{{24,25},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner i4 = {{{25,26},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner i5 = {{{26,38},{0,1}},{100,1,1,0,0,0,0}};

	    ///�����ܱ����ļ�ģʽ��ʼ��
    conditionRunner j1 = {{{0,23},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner j2 = {{{23,24},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner j3 = {{{24,25},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner j4 = {{{25,26},{0,1}},{100,1,1,1,0,0,0}};
    conditionRunner j5 = {{{26,27},{0,1}},{100,1,1,1,1,0,0}};
    conditionRunner j6 = {{{27,28},{0,1}},{100,1,1,1,1,0,1}};
    conditionRunner j7 = {{{28,38},{0,1}},{100,1,1,1,1,1,1}};

	     ///���ʶ���ģʽ��ʼ��
    conditionRunner k1 = {{{0,21},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner k2 = {{{21,22},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner k3 = {{{22,38},{0,1}},{100,1,0,0,0,0,0}};

	    ///���ʴ���ģʽ��ʼ��
    conditionRunner m1 = {{{0,21},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner m2 = {{{21,22},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner m3 = {{{22,23},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner m4 = {{{23,24},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner m5 = {{{24,38},{0,1}},{100,1,1,0,0,0,0}};


    ///�����ļ�ģʽ��ʼ��
    conditionRunner n1 = {{{0,21},{0,1}},{80,0,0,0,0,0,0}};
    conditionRunner n2 = {{{21,22},{0,1}},{100,0,0,0,0,0,0}};
    conditionRunner n3 = {{{22,23},{0,1}},{100,1,0,0,0,0,0}};
    conditionRunner n4 = {{{23,24},{0,1}},{100,1,1,1,0,0,0}};
    conditionRunner n5 = {{{24,25},{0,1}},{100,1,1,1,1,0,0}};
    conditionRunner n6 = {{{25,26},{0,1}},{100,1,1,1,1,0,1}};
    conditionRunner n7 = {{{26,38},{0,1}},{100,1,1,1,1,1,1}};

    for(i = 0; i < seasonMax;i++)
    {
    	for(j = 0;j < DAY_LINE_MAX;j++)
    	{
			gDevCondState[i][j] = VectorNew();
		}
        
    }

	  ///һ���ܱ�������ģʽ��ʼ��
    VectorPushBack(gDevCondState[winterMode][0],a1);  
    VectorPushBack(gDevCondState[winterMode][0],a2);  
    VectorPushBack(gDevCondState[winterMode][0],a3);  

	///һ���ܱ�������ģʽ��ʼ��
    VectorPushBack(gDevCondState[springMode][0],b1);  
    VectorPushBack(gDevCondState[springMode][0],b2);  
    VectorPushBack(gDevCondState[springMode][0],b3);  




	///һ���ܱ����ļ�ģʽ��ʼ��
    VectorPushBack(gDevCondState[summerMode][0],d1);  
    VectorPushBack(gDevCondState[summerMode][0],d2);  
    VectorPushBack(gDevCondState[summerMode][0],d3);  
    VectorPushBack(gDevCondState[summerMode][0],d4);  
    VectorPushBack(gDevCondState[summerMode][0],d5);  
    VectorPushBack(gDevCondState[summerMode][0],d6);  
    VectorPushBack(gDevCondState[summerMode][0],d7);  



	  ///�����ܱ�������ģʽ��ʼ��
    VectorPushBack(gDevCondState[winterMode][1],e1);  
    VectorPushBack(gDevCondState[winterMode][1],e2);  
    VectorPushBack(gDevCondState[winterMode][1],e3);  



	 ///�����ܱ�������ģʽ��ʼ��
    VectorPushBack(gDevCondState[springMode][1],f1);  
    VectorPushBack(gDevCondState[springMode][1],f2);  
    VectorPushBack(gDevCondState[springMode][1],f3);  
    VectorPushBack(gDevCondState[springMode][1],f4);  
    VectorPushBack(gDevCondState[springMode][1],f5); 




	 ///�����ܱ����ļ�ģʽ��ʼ��
    VectorPushBack(gDevCondState[summerMode][1],g1);  
    VectorPushBack(gDevCondState[summerMode][1],g2);  
    VectorPushBack(gDevCondState[summerMode][1],g3);  
    VectorPushBack(gDevCondState[summerMode][1],g4);  
    VectorPushBack(gDevCondState[summerMode][1],g5);  
    VectorPushBack(gDevCondState[summerMode][1],g6);  
    VectorPushBack(gDevCondState[summerMode][1],g7);  




	///�����ܱ�������ģʽ��ʼ��
    VectorPushBack(gDevCondState[winterMode][2],h1);  
    VectorPushBack(gDevCondState[winterMode][2],h2);  
    VectorPushBack(gDevCondState[winterMode][2],h3);  



	///�����ܱ�������ģʽ��ʼ��
    VectorPushBack(gDevCondState[springMode][2],i1);  
    VectorPushBack(gDevCondState[springMode][2],i2);  
    VectorPushBack(gDevCondState[springMode][2],i3);  
    VectorPushBack(gDevCondState[springMode][2],i4);  
    VectorPushBack(gDevCondState[springMode][2],i5); 




	///�����ܱ����ļ�ģʽ��ʼ��
    VectorPushBack(gDevCondState[summerMode][2],j1);  
    VectorPushBack(gDevCondState[summerMode][2],j2);  
    VectorPushBack(gDevCondState[summerMode][2],j3);  
    VectorPushBack(gDevCondState[summerMode][2],j4);  
    VectorPushBack(gDevCondState[summerMode][2],j5);  
    VectorPushBack(gDevCondState[summerMode][2],j6);  
    VectorPushBack(gDevCondState[summerMode][2],j7);  




	
	 ///���ʶ���ģʽ��ʼ��
    VectorPushBack(gDevCondState[winterMode][3],k1);  
    VectorPushBack(gDevCondState[winterMode][3],k2);  
    VectorPushBack(gDevCondState[winterMode][3],k3);  



	///���ʴ���ģʽ��ʼ��
    VectorPushBack(gDevCondState[springMode][3],m1);  
    VectorPushBack(gDevCondState[springMode][3],m2);  
    VectorPushBack(gDevCondState[springMode][3],m3);  
    VectorPushBack(gDevCondState[springMode][3],m4);  
    VectorPushBack(gDevCondState[springMode][3],m5); 

	///�����ļ�ģʽ��ʼ��
    VectorPushBack(gDevCondState[summerMode][3],n1);  
    VectorPushBack(gDevCondState[summerMode][3],n2);  
    VectorPushBack(gDevCondState[summerMode][3],n3);  
    VectorPushBack(gDevCondState[summerMode][3],n4);  
    VectorPushBack(gDevCondState[summerMode][3],n5);  
    VectorPushBack(gDevCondState[summerMode][3],n6);  
    VectorPushBack(gDevCondState[summerMode][3],n7);  
}
*/
//���ݵ�ǰ���䣬���㵱ǰ����λ�����ñ�����һ�У������У�
//���û�����ã��������������á�
//���룺curDay��������
//���أ���ǰ����λ�ڵ�������
/*
uint8_t	getDayItem(uint16_t curDay)
{
	int dayIndex = 0;
	int i = 0;
	
	for(i = 0;i < DAY_LINE_MAX;i++)
	{
		if(targetTemp[i].dayCount != 0)
		{
			if(curDay < targetTemp[i].dayCount)
			{
				dayIndex = (i - 1);
				break;
			}
			else
			{
				if(i == (DAY_LINE_MAX - 1))
				{
					dayIndex = (DAY_LINE_MAX - 1);
					break;
				}
			}
		}
		else
		{
			if(i == 0)

			{
				dayIndex = 0;
			}
			else
			{
				dayIndex = (i -1);
			}
			
			break;
		}
		
	}
    
	return dayIndex;
}
*/
/***************************************************************************************************************
����:�豸���еı���0 - 100���Ǳ�Ƶ�豸:0 �رգ�100 �򿪣���Ƶ�豸������аٷֱ�
�������:
mode ����ģʽ(������ģʽ)
curDay ��ǰ����ʱ��
dev  �豸����
evirParaN ������������
����:
0 -100 Ϊ�������أ�����100���س���

******************************************************************************************************************/
/*
uint8_t getDevState(uint8_t mode,uint16_t curDay,devSort dev,uint16_t envirPara[])
{
		int size = 0;
		int flag = 1;
		int i , j=0;
		int dayIndex = 0;
		uint8_t sta;

		conditionRunner condition = {0};

	
    if(mode >= seasonMax || dev >= devMax)
    {
        return 101;
    }

	dayIndex = getDayItem(curDay);
    size = VectorSize(gDevCondState[mode][dayIndex]);

    for(i = 0;i < size;i++)
    {
        condition = VectorGet(gDevCondState[mode][dayIndex], i);

        flag = 0;


		
			
		if((envirPara[j] >= condition.constraint[j][0]) && (envirPara[j] <= condition.constraint[j][1]))
		{
			sta = condition.state[dev];
			return sta;
		}
		if(i == (size - 1))
		{
			sta = condition.state[dev];
			return sta;
		}
    }

    return 101;
  
}

*/

//���ݵ�ǰ���䣬���㵱ǰ����λ�����ñ�����һ�У������У�
//���û�����ã��������������á�
//���룺curDay��������
//���أ���ǰ����λ�ڵ�������
/*
uint8_t	getDayItem2(uint8_t seasonMode,uint16_t curDay)
{
	int dayIndex = 0;
	int i = 0;
	
	for(i = 0;i < DAY_LINE_MAX;i++)
	{
		if(devModeCfg[seasonMode].dayCtrlMode[i].dayNum != 0)
		{
			if(curDay < devModeCfg[seasonMode].dayCtrlMode[i].dayNum)
			{
				if(i == 0)
				{
					return 0;
				}
				dayIndex = i-1;
				break;
			}
			else if(curDay == devModeCfg[seasonMode].dayCtrlMode[i].dayNum)
			{
				dayIndex = i;
				break;
			}
			else
			{
				if(i == (DAY_LINE_MAX - 1))
				{
					dayIndex = (DAY_LINE_MAX - 1);
					break;
				}
			}
		}
		else
		{
			if(i == 0)
			{
				dayIndex = 0;
			}
			else
			{
				dayIndex = (i -1);
			}
			
			break;
		}
		
	}
    
	return dayIndex;
}

uint8_t getDevState2(uint8_t seasonMode,uint16_t curDay,devSort dev,uint16_t	temp)
{
	uint8_t	i = 0;
	//uint8_t	j = 0;
	uint8_t	dayIndex = 0;
	//int16_t	tmp = 0;

	dayIndex = getDayItem2(seasonMode,curDay);

	//tmp = temp / 10;
	if(temp <= devModeCfg[seasonMode].dayCtrlMode[dayIndex].tempCtrlMode[0].tempMax)
	{
		return devModeCfg[seasonMode].dayCtrlMode[dayIndex].tempCtrlMode[0].sta[dev];
	}
	for(i  = (TEMP_LEVEL_MAX - 1);i >0;i--)
	{
		if(devModeCfg[seasonMode].dayCtrlMode[dayIndex].tempCtrlMode[i].tempMax != 0)
		{
			if(temp >= devModeCfg[seasonMode].dayCtrlMode[dayIndex].tempCtrlMode[i].tempMax)
			{
				return devModeCfg[seasonMode].dayCtrlMode[dayIndex].tempCtrlMode[i].sta[dev];
			}
		}
	}
	return devModeCfg[seasonMode].dayCtrlMode[dayIndex].tempCtrlMode[0].sta[dev];
}

int8_t	getRunStep(uint8_t seasonMode,uint8_t	dayIndex,int16_t		temp)
{
	uint8_t i =0;
	int16_t tmp = temp;

	tmp = tmp/10;
	if(tmp <= devModeCfg[seasonMode].dayCtrlMode[dayIndex].tempCtrlMode[0].tempMax)
	{
		return 0;
	}
	for(i  = (TEMP_LEVEL_MAX - 1);i >0;i--)
	{
		if(devModeCfg[seasonMode].dayCtrlMode[dayIndex].tempCtrlMode[i].tempMax != 0)
		{
			if(tmp >= devModeCfg[seasonMode].dayCtrlMode[dayIndex].tempCtrlMode[i].tempMax)
			{
				return i;
			}
		}
	}
	return 0;
}
*/
/*
void	devRunFunc(devSort devType, uint8_t	sta)
{
	switch(devType)
	{
		case	VariFreqFan1:
		{
			//frequencyFanValue[0] = sta;
			MOTOR_SPEED_REC[0] = sta;
			break;
		}//
		
		case	VariFreqFan2:
		{
			//frequencyFanValue[1] = sta;
			MOTOR_SPEED_REC[1] = sta;
			break;
		}
		
		case	Fan1:
		{
			if(!sta)	GPIO_SetBits(FAN1_OUT);
			else	GPIO_ResetBits(FAN1_OUT);
			break;
		}
		case	Fan2:
		{
			if(!sta)	GPIO_SetBits(FAN2_OUT);
			else	GPIO_ResetBits(FAN2_OUT);
			break;
		}
		case	Fan3:
		{
			if(!sta)	GPIO_SetBits(FAN3_OUT);
			else	GPIO_ResetBits(FAN3_OUT);
			break;
		}
		case	Fan4:
		{
			if(!sta)	GPIO_SetBits(FAN4_OUT);
			else	GPIO_ResetBits(FAN4_OUT);
			break;
		}
		case	Fan5:
		{
			if(!sta)	GPIO_SetBits(FAN5_OUT);
			else	GPIO_ResetBits(FAN5_OUT);
			break;
		}
		
		case	Fan6:
		{
			if(!sta)	GPIO_SetBits(FAN6_OUT);
			else	GPIO_ResetBits(FAN6_OUT);
			break;
		}
		//
		//case	Fan7:
		//{
		//	if(!sta)	GPIO_SetBits(FAN7_OUT);
		//	else	GPIO_ResetBits(FAN7_OUT);
		//	break;
		//}
		//
		case	ventWindow:
		{
			//
			///if(!sta)	GPIO_SetBits(FAN8_OUT);
			//else	GPIO_ResetBits(FAN8_OUT);
			//break;
			//
			//break;
		}
		//
		//case	hotDev:
		//{
		//	if(!sta)	GPIO_SetBits(FAN9_OUT);
		//	else	GPIO_ResetBits(FAN9_OUT);
		//	break;
		//}
		//
		case	coldDev:
		{
			if(!sta)	GPIO_SetBits(FAN7_OUT);
			else	GPIO_ResetBits(FAN7_OUT);
			break;
		}
		case	heatDev:
		{
			if(!sta)	GPIO_SetBits(FAN8_OUT);
			else	GPIO_ResetBits(FAN8_OUT);
			break;
		}
		default:break;
	}
}
*/

/*
//��ȡRTCʱ���Ӧ���롣	
uint32_t	getRtcSec(rtcTime_t rtcTime)
{
	uint16_t i = 0;
	uint32_t sec = 0;
		
	if(rtcTime.year < 1970 || rtcTime.year > 2099)	
		return 0;

	for(i = 1970;i < rtcTime.year;i++)
	{
		if(Is_Leap_Year(rtcTime.year))
		{
			sec += 31622400;//���������
		}
		else
		{
			sec += 31536000;//ƽ�������
		}
	}
	rtcTime.month -= 1;
	for(i = 0;i < rtcTime.month;i++)//���·ݵ�������ӡ�
	{
		sec += (uint32_t)mon_table[i]*86400;
		if(Is_Leap_Year(rtcTime.year) && i == 1)
		{
			sec += 86400;	//����2�·�����һ�������
		}
	}
	sec += (uint32_t)(rtcTime.day - 1)*86400;
	sec += (uint32_t)(rtcTime.hour)*3600;
	sec += (uint32_t)rtcTime.min*60;
	sec += rtcTime.sec;

	return sec;
}


//��ȡ����RTCʱ����������
int16_t getCurDayNum(rtcTime_t	curTime, rtcTime_t startTime)
{
	uint32_t	startSec = 0;
	uint32_t	curSec = 0;
	uint32_t	temp = 0;
	uint16_t		dayNum  = 0;

	startSec = getRtcSec(startTime);
	curSec = getRtcSec(curTime);

	if(startSec ==0 || curSec ==0 )
	{
		return -1;
	}

	if(startSec >= curSec)
	{
		return -1;
	}
	temp = curSec - startSec;
	dayNum = temp/DAY_SECONDE_NUM;

	if(dayNum > 999)
	{
		dayNum = 999;
	}
	return dayNum;
}

*/

