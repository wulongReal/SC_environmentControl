#include "vars.h"
#include "platform.h"
//#include "lcd.h"
#include "string.h"
#include "dataflash.h"
#include"DS1302.h"


#include <stdlib.h>  
#include "stdint.h"

uint8_t jqidString[JQID_LEN_MAX]= "00100003";
COMMU_EN_T	m_CommuEn_T;						//通信使能
uint8_t UART4_RecvBuf[UART4_RECV_BUFFER_LEN] = {0};				//接收buf
uint8_t UART4_RecvByte;
uint8_t UART4_RecvLen;

uint8_t UART2_RecvByte;			//485传感器口接收byte
uint32_t EP_Value = 0;			//电表数值

uint8_t LTE_RecvBuffer[LTE_RECV_BUFFER_LEN];
uint16_t LTE_RecvLen;
uint8_t	LTE_RecvByte;
uint8_t LTE_RecvBuf_Busy = 0;//buffer写保护
uint8_t esimSumCheck = 0;
uint32_t jqid;

uint32_t inputBits = 0;					//IO输入
uint32_t outputBits = 0;				//IO输出
uint16_t dacValueOut[4] = {0};			//DAC输出
uint16_t adcValueIn[4] = {0};			//ADC输入


int16_t	curAvgTemp = 0;						//平均温度
int16_t curAvgTemp_DayMax = 0;				//日平均温度最大值
int16_t curAvgTemp_DayMin = 0;				//日平均温度最小值
int16_t	curAvgHumi = 0;						//平均湿度
int16_t curAvgHumi_DayMax = 0;				//日平均湿度最大值
int16_t curAvgHumi_DayMin = 0;				//日平均湿度最小值
uint16_t curElecPower_DayUseStart = 0;		//日耗电起始值
uint16_t curElecPower_DayUse = 0;			//日耗电量
uint16_t curWaterPower_DayUseStart = 0;		//日耗水量起始值
uint16_t curWaterPower_DayUse = 0;			//日耗水量



uint8_t curDayNum = 0;						//当前天数
uint8_t curDayLevel = 0;					//当前日龄级别
uint8_t curAirLevel = 0;					//当前通风级别
uint16_t curTargetTemp = 0;					//当前目标温度
uint8_t m_Is_PowerOff = 0;					//是否断电
uint8_t m_Is_LTE_Connect = FALSE;			//是否连接Web
uint8_t m_BatVol_Percent = 0;				//电池电量百分比
uint8_t m_Is_Charging = 0;					//是否在充电
uint8_t m_LTE_Rssi = 0;						//
uint8_t m_Is_Tcp_ConnectOk = FALSE;			//是否连接OK

uint8_t m_LightCtrl_En = FALSE;				//灯光控制使能
uint8_t m_Is_AlarmOccur = FALSE;			//是否产生故障
uint8_t m_Is_NeedRefresh_LCD_Cfg = TRUE;	//是否刷新LCD CFG

uint16_t softVersion = 150;//版本显示为1.00
uint16_t hardVersion = 101;//版本显示为1.00
uint16_t lcdSoftVersion = 141;//版本显示为1.00




volatile FLAGSINT FLAGS0;                 // 标志用于程序状态控制
volatile FLAGSINT FLAGS1;
volatile FLAGSINT FLAGS2;
volatile FLAGSINT FLAGS3;//LCD相关状态
volatile FLAGSINT FLAGS4;

uint16_t MOTOR_SPEED_REC[SPEED_FAN_NUM];//接收的变频器频率百分比
uint16_t adcValue[AD_INPUT_NUM];	//ADC读取值


uint8_t LIN_ProtBuf[MAX_LIN_DATANUMB];     // 输入输出缓冲
uint8_t recvDataBuf[MAX_LIN_DATANUMB];	//输入数据存放缓冲。防止处理过程中，其它数据将原有帧数据覆盖。
uint8_t  LIN_Length;
uint8_t  LIN_TmpSCIByte;
uint8_t  LIN_BufCounter;
uint8_t  LIN_StateFlags;
uint8_t  LIN_MsgDir;
uint8_t  LIN_Err;

uint8_t  Err_Count[DEV_NUM];               // RS485温度传感器读取错误计数

uint8_t  RS232Temp_Num;                    // RS232发送的温度传感的数量
uint8_t  RS232Hmd_Num;                     // RS232发送的湿度传感器数量
uint8_t  RS232DR_Num;                      // RS232发送的驱动板的数量
uint8_t  RS232PHV_Num;                     // RS232发送的PH值传感器数量
uint8_t  RS232WF_Num;                      // RS232发送的水流量传感器数量
uint8_t  RS232WP_Num;                      // RS232发送的水压力传感器数量
uint8_t  RS232TH_Num;                      // RS232发送的温湿度传感器数量
uint8_t  RS232NH_Num;                      // RS232发送的氨气传感器数量
uint8_t  RS232WG_Num;                      // RS232发送的称传感器数量
uint8_t  RS232RF_Num;                      // RS232发送的RFID传感器数量
uint8_t  RS232GRT_Num;                     // RS232发送的地面温度传感器数量
uint8_t  RS232CD_Num;                      // RS232发送的电流传感器数量    17/2/15

uint8_t	driveBoardNum;



uint16_t Rs485_threadCount;                // 161002 rs485数据采集进程测试
uint16_t Rs485REC_threadCount;             // 161013 rs485数据采集进程测试







uint8_t  RS232ERR_Cout;                    // RS232通信错误

uint8_t  RS232Send_Cout;                   // RS232发送计数 500ms
uint8_t  LeaTST_Count;                     // 离开调试模式计数
uint8_t  CoLED_Count;                      // 彩色LED计数
uint8_t  Ref_Count;                        // 刷新屏幕计数
uint8_t  SendCon_Count;                    // 发送配置信息回复
uint8_t  TstOV_Count;                      // 测试屏幕上电压完成发送计数






devModeCfg_t	devModeCfg[seasonMax];
alarmCfg_t		alarmCfg;
alarmCfg_t	alarmCfgTemp;

sensorCfg_t		sensorCfg;
sensorAjust_t	sensorAjustTemp;//杈撳叆鐨勬牎鍑嗗�?
uint16_t	noPressCnt = 0;


devInfo_t	devInfo;
devDebugCtrl_t	devDebugCtrl;

/////temp
devModeCfg_t	tempModeCfg;



rtcTime_t		curRtc;
uint16_t		lcdRecvAddr = 0;
uint16_t		lcdRecvData = 0;

uint8_t	curPicId = 0;
uint8_t	curTopMenumId = 0;//一级菜单ID
uint8_t	curSecMenumId = 0;//二级菜单ID
uint8_t	onlyRefreshData = 0;//仅更新PAGE内容，不切换LCD
int8_t	dayNumLevel = 0, tempStep = 0;



int16_t tempData[TEMP_SENSOR_NUM_MAX] = {0};
int16_t	humiData[HUMI_SENSOR_NUM_MAX] = {0};
int16_t tempMax[TEMP_SENSOR_NUM_MAX] = {0};
int16_t	tempMin[TEMP_SENSOR_NUM_MAX] = {0};
int16_t	humiMax[HUMI_SENSOR_NUM_MAX] = {0}, humiMin[HUMI_SENSOR_NUM_MAX] = {0};

uint8_t	lcdCtrlSaveId = 0;	//配置保存页面的ID
uint8_t	devSaveId = 0;//设备保存页面的ID


alarmRunSta_t	alarmRunSta;
devAlarmFlag_t	devAlarmFlag;

uint8_t	devRunSta[allDevMax];

//devIdInfo_t	devIdInfo;





uint8_t	jqidGetEn = TRUE;//
uint8_t	jqidEmpty = TRUE;//j
uint8_t	jqidGetForce = FALSE;
char isEsimConnect = false;
int esimLostCnt = 0;
char esimLostFlag = 0;
uint8_t esimConnectErrCnt = 0;
bool esimErr = 0;
bool isLTEConnetOk = TRUE;




struct Device485_Info Dev_info[]={   //ID Addr lenth（寄存器） data1 data2 sta EN
  // name
  {TEMP1       ,0 ,1,0,0,MB_MRE_NO_ERR,1},   // 1  1   //读出单位℃，2位小数
  {TEMP2       ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 2  2
  {TEMP3       ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 3  3
  {TEMP4       ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 4  4
  {TEMP5       ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 5  5
  {TEMP6       ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 6  6
  {HUMI1       ,0 ,2,0,0,MB_MRE_NO_ERR,1},   // 7  7	//读出单位%RH，2位小数
  {HUMI2       ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 8  8
  {HUMI3       ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 9  9
  {TEMP_OUTDOOR,0 ,1,0,0,MB_MRE_NO_ERR,0},  // 0a 10
  {NH3_1       ,0 ,1,0,0,MB_MRE_NO_ERR,1},   // 0b 11  	//读出单位ppm，（0-100ppm量程无小数）
  {NH3_2       ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 0c 12
  {WEIGH_1     ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 0d 13		
  {WEIGH_2     ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 0e 14
  {WEIGH_3     ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 0f 15
  {WEIGH_4     ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 10 16
  {WATER_PRESU1,4 ,1,0,0,MB_MRE_NO_ERR,1},   // 11 17	//读出单位Kpa，无小数点
  {WATER_PRESU2,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 12 18
  {PH          ,0 ,1,0,0,MB_MRE_NO_ERR,1},   // 13 19	//读出单位PH，2位小数
  {CO2         ,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 14 20
  {AIR_PRESSURE,0 ,1,0,0,MB_MRE_NO_ERR,0},   // 15 21
  {LIGHT_LUX   ,0 ,1,0,0,MB_MRE_NO_ERR,0},	 // 16 22
  {WATER_FLOW   ,0 ,1,0,0,MB_MRE_NO_ERR,0},	 // 17 23	
  {EP_METER   ,0 ,2,0,0,MB_MRE_NO_ERR,1},	 // 18 24	//浮点输出,发送到web使用hex，2位小数
  {WATER_METER   ,1 ,2,0,0,MB_MRE_NO_ERR,1}, // 19 25	//读出单位m3，2位小数
};
uint8_t  SEN_CurrentIdx;                   // 当前处理的传感器号
uint8_t  SEN_Count;                        // 传感器的总数量

#ifdef BOOT

uint8_t App_CheckappChecksum(void){
  return 0;
}

void Checksum_Err(void) {
  for(;;){
    //__RESET_WATCHDOG(); 
  }
}
#endif

void Change_Sen(unsigned char flag)
{  
	byte i;
  
  	SEN_Count = 0;// 使能的传感器的总数量
  	for(i=0;i<DEV_NUM-1;i++)
	{
    	if (1==Dev_info[i].Dev_EN)
      		SEN_Count++;  
  	}
	/*
  	if (flag == 1)
	{
    	//RS232Temp_Num = TEMP_NUM;
    	RS232Temp_Num = 0;
    	for(i=TEMP1;i<=TEMP6;i++)
		{
      		if (1==Dev_info[i-1].Dev_EN)
        		RS232Temp_Num++;
    	}    
    	RS232TH_Num   = 0;
    	for(i=TEMP_HUMI1;i<=TEMP_HUMI3;i++){
      if (1==Dev_info[i-1].Dev_EN)
        RS232TH_Num++;
    }
    //RS232NH_Num   = NH_NUM;
    */
    RS232NH_Num   = 0;
    for(i=NH3_1;i<=NH3_2;i++)
	{
      	if (1==Dev_info[i-1].Dev_EN)
        	RS232NH_Num++;
    }
    RS232WG_Num   = 0;
    for(i=WEIGH_1;i<=WEIGH_4;i++)
	{
      if (1==Dev_info[i-1].Dev_EN)
        RS232WG_Num++;
    }
    
    RS232WF_Num   = 0;
    if (1==Dev_info[WATER_FLOW-1].Dev_EN)
        RS232WF_Num++;

    RS232WP_Num   = 0;
    for(i=WATER_PRESU1;i<=WATER_PRESU2;i++)
	{
      	if (1==Dev_info[i-1].Dev_EN)
        	RS232WP_Num++;
    }

    RS232PHV_Num  = 0;
	if (1==Dev_info[PH-1].Dev_EN)
        RS232PHV_Num++;

    RS232Hmd_Num  = 0;
    for(i=HUMI1;i<=HUMI3;i++)
	{
      if (1==Dev_info[i-1].Dev_EN)
        RS232Hmd_Num++;
    }
}
/*
uint8_t	flashInit(void)
{
	char buf[PAGE_SIZE] = {0};
	char flag[1] = {0};
	uint8_t	i =0;

	Flash_Read(FLASH_INIT_FLAG_SAVE_ADDR, flag, 1);
	if(flag[0] == 0xff)
	{
		for(i = 0;i < FLASH_SAVE_PAGE_NUM;i++)
		{
			Flash_Write(FLASH_DEV_INFO_SAVE_ADRR+i*PAGE_SIZE, buf, PAGE_SIZE);
			IWDG_Feed();
		}
		flag[0] = 0;
		Flash_Write(FLASH_INIT_FLAG_SAVE_ADDR, flag, 1);
	}
	return 0;
}
*/
/*
uint8_t	flashInit(void)
{
	char buf[PAGE_SIZE] = {0};
	//char flag[1] = {0};
	uint8_t	i =0;

	//Flash_Read(FLASH_ID_CFG_SAVE_ADDR, flag, 1);
	//if(flag[0] == 0xff)
	//{
		for(i = 0;i < FLASH_SAVE_PAGE_NUM;i++)
		{
			Flash_Write(FLASH_DEV_INFO_SAVE_ADRR+i*PAGE_SIZE, buf, PAGE_SIZE);
			IWDG_Feed();
		}
		//flag[0] = 0;
		//Flash_Write(FLASH_ID_CFG_SAVE_ADDR, flag, 1);
	//}
	return 0;
}
uint8_t	saveDevIdCfg(void)
{
	char *buf = NULL;
	uint16_t size = sizeof(devIdInfo_t);

	if(size > PAGE_SIZE)	
		return 1;
	buf = malloc(size);
	memcpy(buf, &devIdInfo, size);
	Flash_Write(FLASH_ID_CFG_SAVE_ADDR, buf, size);
	free(buf);
	return 0;
}

uint8_t	getDevIdCfg(void)
{
	char *buf = NULL;
	uint16_t	size = sizeof(devIdInfo_t);

	if(size > PAGE_SIZE)
		return 1;
	buf = malloc(size);
	if(buf == NULL)
		return 1;

	Flash_Read(FLASH_ID_CFG_SAVE_ADDR, buf, size);
	memcpy(&devIdInfo,buf,size);
	free(buf);
	if(devIdInfo.isInitOk == 0XFF) 
	{
		flashInit();
		devIdInfo.isInitOk = 0;
		if(devIdInfo.jqid == 0xffffffff)
		{
			devIdInfo.jqid = 0;
			memset(devIdInfo.imeiCode, 0, sizeof(devIdInfo.imeiCode));
			jqidGetEn = TRUE;
		}
		saveDevIdCfg();	
		return 0;
	}
	if(devIdInfo.jqid == 0xffffffff)
	{
		devIdInfo.jqid = 0;
		memset(devIdInfo.imeiCode, 0, sizeof(devIdInfo.imeiCode));
		jqidGetEn = TRUE;
	}

	if(devIdInfo.rebootCnt == 0xff)
		devIdInfo.rebootCnt = 0;
	return 0;
}

uint8_t	getDevInfoData(void)
{
	char *buf = NULL;
	uint16_t size = sizeof(devInfo_t);

	if(size > PAGE_SIZE)	
		return 1;
	buf = malloc(size);
	if(buf == NULL)
		return 1;
	Flash_Read(FLASH_DEV_INFO_SAVE_ADRR, buf,size);
	memcpy(&devInfo, buf, size);
	free(buf);
	return 0;
}
uint8_t	saveDevInfoData(void)
{
	char *buf = NULL;
	uint16_t size = sizeof(devInfo_t);

	if(size > PAGE_SIZE)	
		return 1;
	buf = malloc(size);
	memcpy(buf, &devInfo, size);
	Flash_Write(FLASH_DEV_INFO_SAVE_ADRR, buf, size);
		free(buf);
	return 0;
}

uint8_t	getSensorCfgData(void)
{
	char *buf = NULL;
	uint16_t	size = sizeof(sensorCfg_t);

	if(size > PAGE_SIZE)	
		return 1;
	buf = malloc(size);
	Flash_Read(FLASH_SENSOR_CFG_SAVE_ADDR, buf,size);
	memcpy(&sensorCfg, buf, size);
	free(buf);
	return 0;
}
uint8_t	saveSensorCfgData(void)
{
	char *buf = NULL;
	uint16_t size = sizeof(sensorCfg);

	if(size > PAGE_SIZE)	
		return 1;
	buf = malloc(size);
	memcpy(buf, &sensorCfg, size);
	Flash_Write(FLASH_SENSOR_CFG_SAVE_ADDR, buf, size);
	free(buf);
	return 0;
}

uint8_t	getAlarmCfgData(void)
{
	char *buf = NULL;
	//uint8_t	i = 0;
	
	uint16_t size = sizeof(alarmCfg);

	if(size > PAGE_SIZE)	
		return 1;
	buf = malloc(size);
	Flash_Read(FLASH_ALARM_CFG_SAVE_ADDR, buf,size);
	memcpy(&alarmCfg, buf, size);
	free(buf);


	
	return 0;
}
uint8_t	saveAlarmCfgData(void)
{
	char *buf = NULL;
	//uint8_t	i = 0;
	uint16_t size = sizeof(alarmCfg);

	if(size > PAGE_SIZE)	
		return 1;
	buf = malloc(size);
	memcpy(buf, &alarmCfg, size);
	Flash_Write(FLASH_ALARM_CFG_SAVE_ADDR, buf, size);
	free(buf);

	return 0;
}
uint8_t	getModeCfgData(void)
{
	char *buf = NULL;
	uint8_t i = 0;
	uint16_t size = sizeof(devModeCfg_t);

	if(size > PAGE_SIZE)	
		return 1;
	buf = malloc(size);
	for(i = 0;i < SEASON_MAX;i++)
	{
		memset(buf, 0, size);
		Flash_Read(FLASH_MODE_CFG_SAVE_ADDR_1 + i*PAGE_SIZE, buf,size);
		memcpy(&devModeCfg[i], buf, size);
	}
	free(buf);

	if(devInfo.curSeason > (uint8_t)winterMode)
	{
		devInfo.curSeason = springMode;
	}
	tempModeCfg = devModeCfg[devInfo.curSeason];
	//memcpy(&tempModeCfg, &devModeCfg[devInfo.curSeason], sizeof(devModeCfg_t));
	return 0;
}
uint8_t	saveModeCfgData(uint8_t	season)
{
	char *buf = NULL;
	uint16_t size = sizeof(devModeCfg_t);

	if(size > PAGE_SIZE)	
		return 1;
	buf = malloc(size);
	memcpy(buf, &devModeCfg[season], size);
	Flash_Write(FLASH_MODE_CFG_SAVE_ADDR_1+season*PAGE_SIZE, buf, size);
	free(buf);
	return 0;
}

*/

void Init_Vars(void)
{
  	uint8_t i;
		DS1302DATA date = {20,07,27,23,33,00,1};
		
		
  	FLAGS0.LONG = 0;
  	FLAGS1.LONG = 0;
	


  	for(i=0;i<4;i++)
	{
	    //Hmd_RS485[i]  = 0;
	    //Hmd_Sta[i]    = 0;
	    //TEHD_RS485[i] = 0;
	    //PHV_RS485[i]  = 0;
	    //PHV_Sta[i]    = 0;
	    //WF_RS485[i]   = 0;
	    //WF_Sta[i]     = 0;
	    //WP_RS485[i]   = 0;
	    //WP_Sta[i]    = 0;
	    //NH3_RS485[i]  = 0;
	    //NH3_Sta[i] = 0;
	    //ColorLED_FRCR[i] =0;//160816// 收到的彩色LED的显示信息
	}


  	for(i=0;i<8;i++)
  	{
	    //Driver_RELSta[i] = 0;
	    //Driver_Input[i] = 0;
	    //#if defined(TRA_PRO)
	    //Driver_RELSta1[i] = 0;
	    //Driver_Input1[i] = 0;
	    //#endif
  	}
	for (i=0;i<10;i++)
	{
		//Temp_RS485[i] = 0;
		//Temp_Sta[i]   = 0;
		Err_Count[i] = 0;
		//DriverInf_FRCR[i] = 0;
		//#if defined(TRA_PRO)
		//DriverInf_FRCR1[i] = 0;
		//#endif
	}
  	//ADC_Cout  = 0;
  	RS232Send_Cout = 0;
  	RS232ERR_Cout  = 0;
  	//FANDR_Cout     = 0;
  	//CAN1ERR_Cout    = 0;
	//CAN2ERR_Cout    = 0;
  	Ref_Count  = 0;
  	//CoreRstCount =0;
  	//CoreStCount  =0;
  


  	SEN_CurrentIdx = 0;
  	Change_Sen(1);
  	Rs485_threadCount = 0;
 	Rs485REC_threadCount =0;
 

	for(i = 0;i < DEV_NUM;i++)
	{
		Dev_info[i].Data1 = 0;
		Dev_info[i].Dev_sta = MB_MRE_TIMEDOUT;
	}

  	//FLAGS3_GET_RTC_SEND = 1;


	/*
  	getDevIdCfg();

  	getDevInfoData();
  	getSensorCfgData();
  	getAlarmCfgData();
  	getModeCfgData();
	*/

	Ds1302_Gpio_Init();
	//Ds1302_Write_Time_All(1,date);
	Ctroler_Init();
}

