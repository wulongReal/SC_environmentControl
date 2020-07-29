#include "app_task.h"
//#include "lcd.h"
#include "vars.h"
#include <rtthread.h>
#include "can.h"
#include "rs232.h"
//#include "TFTLED_Ref.h"
#include "adc.h"
#include "dac.h"

//#include "iic.h"
#include "envir.h"
#include "rtc.h"

#include "string.h"
#include "usart.h"
#include "lte720.h"
#include"DS1302.h"
#include "mbcrc.h"
#include "EP_MODBUS.h"





#ifdef __CC_ARM
  extern int Image$$RW_IRAM1$$ZI$$Limit;
#elif __ICCARM__
  #pragma section="HEAP"
#else
  extern int __bss_end;
#endif

#define	ESIM_TRANS_CNT	3
#define	ALARM_START		1
#define	ALARM_STOP		2

_timeCnt   cnt;
_uploadMes upload;
uint8_t	esimThreadCnt = 0;//鏈�澶?56




//====================操作系统各线程优先级==================================

#define thread_eSim_Prio                  3 
#define thread_ModbusMasterPoll_Prio      4
#define thread_SysMonitor_Prio            5
#define	thread_UART4COMM_Prio			 6
#define thread_DevRun_Prio            	  7
#define	thread_ADC_Prio					  8
#define	thread_DAC_Prio					  9
#define thread_CANCOMM_Prio               10
#define thread_RS232COMM_Prio             11





ALIGN(RT_ALIGN_SIZE)
//====================操作系统各线程堆栈====================================
#define	THREAD_ADC_EN 	1

static rt_uint8_t thread_SysMonitor_stack[512];
static rt_uint8_t thread_ModbusMasterPoll_stack[512];
#ifdef THREAD_ADC_EN
static rt_uint8_t thread_ADC_stack[256];
struct rt_thread thread_ADC;

#endif
#ifdef THREAD_DAC_EN
static rt_uint8_t thread_DAC_stack[256];
struct rt_thread thread_DAC;

#endif
#ifdef THREAD_CAN_EN
static rt_uint8_t thread_CANCOMM_stack[256];
struct rt_thread thread_CANCOMM;
#endif
static rt_uint8_t thread_RS232COMM_stack[1024];
static rt_uint8_t thread_DevRun_stack[1024];
static rt_uint8_t thread_eSim_stack[4096];
static rt_uint8_t thread_UART4COMM_stack[512];

struct rt_thread thread_SysMonitor;
struct rt_thread thread_ModbusMasterPoll;
struct rt_thread thread_RS232COMM;
struct rt_thread thread_DevRun;
struct rt_thread thread_eSim;
struct rt_thread thread_UART4;


static  rt_timer_t Timer_50MS, Timer_1S;

//char version[] = "V1.3.7";

#define REP_COUNT     2

__asm void SystemReset(void)
{
 MOV R0, #1           //; 
 MSR FAULTMASK, R0    //; 清除一切中断
 LDR R0, =0xE000ED0C  //;
 LDR R1, =0x05FA0004  //; 
 STR R1, [R0]         //; 清除软终端
 
deadloop
    B deadloop        //; 死循环
}


void thread_entry_SysMonitor(void* parameter)
{

	uint8_t i,j;
	uint8_t Err_Code;
	uint16_t sum = 0;
	uint8_t num = 0;



	POWER_SENSOR_OPEN;

	for(i = 0;i <3;i++)
	{
	
		BEEP_ON;rt_thread_delay(200);
		BEEP_OFF;rt_thread_delay(200);
	}

  	while (1)  
 	{   
	    rt_thread_delay(100);
	    IWDG_Feed(); //feed the dog
	    #ifndef TEST
	    //errorCode = eMBMasterReqReadInputRegister(1,3,2,RT_WAITING_FOREVER);
	    // 站点号 功能码 地址  长度  校验和 站点号 功能码 字节数 寄存器值H 寄存器值L  校验
	    // 01     03     00 00 00 01 84 0A  01     03     02     09        D0          BF 88
	    if (FLAGS_MODBUS1S)
		{
	      	FLAGS_MODBUS1S = FALSE;

	      	Rs485_threadCount = 0;


			for(i=0; i<DEV_NUM; i++)
			{ 
			  	rt_thread_delay(50);

	          	if((Dev_info[SEN_CurrentIdx].Dev_EN))
				{
					if(Dev_info[SEN_CurrentIdx].ID == EP_METER )
					{
						SEN_CurrentIdx++;
						continue;
					}
						
	            	for(j=0;j<REP_COUNT;j++)
					{ 
	              		Err_Code = MB_MRE_TIMEDOUT;
				
	              		Err_Code = eMBMasterReqReadHoldingRegister((Dev_info[SEN_CurrentIdx].ID),(Dev_info[SEN_CurrentIdx].Addr),(Dev_info[SEN_CurrentIdx].Lenth),RT_WAITING_FOREVER);  // RT_WAITING_NO
						if (Err_Code != MB_MRE_NO_ERR) 
						{    
	                		Err_Count[SEN_CurrentIdx]++;
	                		if (Err_Count[SEN_CurrentIdx] >8)
							{
	                  			Dev_info[SEN_CurrentIdx].Dev_sta=Err_Code;
	                  			Err_Count[SEN_CurrentIdx] =0;    //8 OR 0     160909
	                		}                
	              		}
						else 
						{ 
	                		Err_Count[SEN_CurrentIdx] = 0;
	                		Dev_info[SEN_CurrentIdx].Dev_sta = MB_MRE_NO_ERR;

							Dev_info[SEN_CurrentIdx].Data1=usMRegHoldBuf[SEN_CurrentIdx][Dev_info[SEN_CurrentIdx].Addr];
							if (Dev_info[SEN_CurrentIdx].Lenth>1)
								Dev_info[SEN_CurrentIdx].Data2=usMRegHoldBuf[SEN_CurrentIdx][Dev_info[SEN_CurrentIdx].Addr+1];	 

							if(SEN_CurrentIdx >= (TEMP1 - 1) && SEN_CurrentIdx <= (TEMP_OUTDOOR - 1))
							{
								if((Dev_info[SEN_CurrentIdx].Data1==0x2710)||(Dev_info[SEN_CurrentIdx].Data1==0))
								{
									Dev_info[SEN_CurrentIdx].Dev_sta = MB_MRE_REV_DATA;	
								}
							}
							/*
							if((Dev_info[SEN_CurrentIdx].Data1==0x2710)||(Dev_info[SEN_CurrentIdx].Data1==0))
							{
								if(!((Dev_info[SEN_CurrentIdx].ID == NH3_1)||(Dev_info[SEN_CurrentIdx].ID==NH3_2)||(Dev_info[SEN_CurrentIdx].ID==WATER_PRESU1)||(Dev_info[SEN_CurrentIdx].ID==WATER_PRESU2)))
									Dev_info[SEN_CurrentIdx].Dev_sta = MB_MRE_REV_DATA;//
							}									
							if (Dev_info[TEMP1-1].Dev_sta == MB_MRE_NO_ERR)
								FLAGS2_TMP_ORHM = FALSE;
							
	                		break;
							*/
	              		}
	            	}
					#if 1
					/*
					if (SEN_CurrentIdx==TEMP1-1)
					{
						if ((Dev_info[SEN_CurrentIdx].Dev_sta != MB_MRE_NO_ERR) ||(Dev_info[SEN_CurrentIdx].Data1<=0) ||(Dev_info[SEN_CurrentIdx].Data1>=6000))
						{ 
							if(Dev_info[HUMI1-1].Dev_sta==MB_MRE_NO_ERR)
							{ 
								FLAGS2_TMP_ORHM = TRUE;
								if((usMRegHoldBuf[HUMI1-1][1] > 0) && (usMRegHoldBuf[HUMI1-1][1] < TEMP_VALUE_MAX))
								{
									Dev_info[TEMP1-1].Data1=usMRegHoldBuf[HUMI1-1][1];
									Dev_info[TEMP1-1].Dev_sta = 0x10;//MB_MRE_NO_ERR;

								}
							}
							else
							{                                         //0119
								FLAGS2_TMP_ORHM = FALSE;
							}				
						}
					}
					*/
					#endif
	          	} 
				else
				{
					/*
					if (SEN_CurrentIdx==TEMP1-1)
					{
						FLAGS2_TMP_ORHM = FALSE;	
					}
					*/
	          	}
				
				/*
				if(SEN_CurrentIdx ==TEMP1 -1)
				{
					if(Dev_info[SEN_CurrentIdx].Dev_sta == MB_MRE_NO_ERR && Dev_info[SEN_CurrentIdx].Data1 != 0x2710)
					{
						curAvgTemp = Dev_info[SEN_CurrentIdx].Data1;
							if(curAvgTemp >= tempMax[0])
								tempMax[0] = curAvgTemp;
							else if(curAvgTemp < tempMin[0])
								tempMin[0] = curAvgTemp;
						if(tempMax[0] == 0)
							tempMax[0] = curAvgTemp;
						if(tempMin[0] == 0)
							tempMin[0] = curAvgTemp;
					}
						
				}
				if(SEN_CurrentIdx == HUMI1 - 1)
				{
					if(Dev_info[SEN_CurrentIdx].Dev_sta == MB_MRE_NO_ERR)
					{
						if(Dev_info[SEN_CurrentIdx].Data2 != 0x2710 && Dev_info[TEMP1 -1].Dev_sta != MB_MRE_NO_ERR)
						{
							curAvgTemp = Dev_info[SEN_CurrentIdx].Data2;
							if(curAvgTemp >= tempMax[0])
								tempMax[0] = curAvgTemp;
							else if(curAvgTemp < tempMin[0])
								tempMin[0] = curAvgTemp;
							
						if(tempMax[0] == 0)
							tempMax[0] = curAvgTemp;
						if(tempMin[0] == 0)
							tempMin[0] = curAvgTemp;
						}
						if(Dev_info[SEN_CurrentIdx].Data1 != 0x2710)
						{
							curAvgHumi = Dev_info[SEN_CurrentIdx].Data1;
							if(curAvgHumi >= humiMax[0])
								humiMax[0] = curAvgHumi;
							else if(curAvgHumi < humiMin[0])
								humiMin[0] = curAvgHumi;

							if(humiMax[0] == 0)
								humiMin[0] = curAvgHumi;
							if(humiMin[0] == 0)
								humiMin[0] = curAvgHumi;
						}
					}
				}
				*/
				SEN_CurrentIdx++;
		     
				if(SEN_CurrentIdx>=DEV_NUM)
				{
				    SEN_CurrentIdx = 0;
					goto MSENS_EXIT;
				}      


	        }
			
	    }
	    MSENS_EXIT:  
	      IWDG_Feed();
	    #endif
		//平均温度计算
		num = 0;
		sum = 0;
		for(i = 0;i < SUPPORT_SENSOR_TEMPER_NUM;i++)
		{
			if(Dev_info[TEMP1 - 1+i].Dev_EN)
			{
				if(Dev_info[TEMP1 - 1+i].Dev_sta == MB_MRE_NO_ERR && Dev_info[TEMP1 - 1+i].Data1 != 0x2710)
				{
					sum += Dev_info[TEMP1 - 1+i].Data1;
					num++;
				}
			}
		}

		//目标温度有效则刷新，否则不予刷新
		if(num > 0)
		{
			curAvgTemp = sum / num;
		}
		//else
		//	curAvgTemp = 0;
		
		//日最高温，最低温
		if(curAvgTemp >= curAvgTemp_DayMax)
		{
			curAvgTemp_DayMax = curAvgTemp;
		}
		else if(curAvgTemp <= curAvgTemp_DayMin)
		{
			curAvgTemp_DayMin = curAvgTemp;
		}

		//平均湿度计算
		num = 0;
		sum = 0;
		for(i = 0;i < SUPPORT_SENSOR_HUMI_NUM;i++)
		{
			if(Dev_info[HUMI1 - 1+i].Dev_EN)
			{
				if(Dev_info[HUMI1 - 1+i].Dev_sta == MB_MRE_NO_ERR && Dev_info[HUMI1 - 1+i].Data1  != 0x2710)
				{
					sum += Dev_info[HUMI1 - 1+i].Data1;
					num++;
				}
			}
		}
		if(num > 0)
		{
			curAvgHumi = sum / num;
		}
		//else
		//	curAvgHumi = 0;

		//日最高温，最低温
		if(curAvgHumi >= curAvgHumi_DayMax)
		{
			curAvgHumi_DayMax = curAvgHumi;
		}
		else if(curAvgTemp <= curAvgHumi_DayMin)
		{
			curAvgHumi_DayMin = curAvgHumi;
		}
		
		//电表读取
		if(Dev_info[EP_METER - 1].Dev_EN)
		{
			if(FLAGS_EP_RD_EN_10S)
			{
				FLAGS_EP_RD_EN_10S = FALSE;
				FLAGS_EP_RD_GET_SUC = FALSE;
				for(i =0;i < 5;i++)
				{
					RS485_EP_Get(Dev_info[EP_METER - 1].ID);
					FLAGS_EP_CMD_SEND = TRUE;
					rt_thread_delay(300 + i*10);
					if(FLAGS_EP_RD_GET_SUC == 1)
					{
						Err_Count[EP_METER -1] = 0;
						Dev_info[EP_METER - 1].Dev_sta = MB_MRE_NO_ERR;	
						break;
					}
				}
				FLAGS_EP_CMD_SEND = FALSE;
			}
		}
		/*
		for(i = 0;i < sensorCfg.tempSensorNum;i++)
		{
			if(((Dev_info[TEMP1 - 1 + i].Dev_sta == MB_MRE_NO_ERR) && (Dev_info[TEMP1 - 1 + i].Data1 > 0) && (Dev_info[TEMP1 - 1 + i].Data1 < TEMP_VALUE_MAX))|| (Dev_info[TEMP1 - 1 + i].Dev_sta == 0x10))
			{
				tempData[i] = Dev_info[TEMP1 - 1 + i].Data1 + sensorCfg.sensorAjust.tempAjustValue[i]*10;
			}
			else
			{
				tempData[i] = 0;
			}
		}
		//sensorCfg.humiSensorNum = 1;
		for(i = 0;i < sensorCfg.humiSensorNum;i++)
		{
			if(Dev_info[HUMI1 - 1 + i].Dev_sta == MB_MRE_NO_ERR)
			{
				humiData[i] = Dev_info[HUMI1 - 1 + i].Data1 + sensorCfg.sensorAjust.humiAjustValue[i]*10;
				if(humiData[i] > HUMI_VALUE_MAX)
				{
					humiData[i] = HUMI_VALUE_MAX;
				}
			}
			else
			{
				humiData[i] = 0;
			}
		}

		num =0;
		tmp = 0;
		for(i = 0;i < sensorCfg.tempSensorNum;i++)
		{
			if((Dev_info[TEMP1 - 1 + i].Dev_sta == MB_MRE_NO_ERR) || (Dev_info[TEMP1 - 1 + i].Dev_sta == 0x10))
			{
				if(tempMax[i] == 0) 
				{
					tempMax[i] = tempData[i];
				}
				if(tempMin[i] == 0) 
				{
					tempMin[i] = tempData[i];
				}
				if(tempData[i] >= tempMax[i])
				{
					tempMax[i] = tempData[i];;
				}
				else if(tempData[i] <= tempMin[i])
				{
					tempMin[i] = tempData[i];
				}
				tmp += tempData[i];;
				num++;
			}
		}
		if(num == 0)
		{
			avgTemper = 0;
		}
		else
		{
			avgTemper = tmp / num;
		}
		if(sensorCfg.tempSensorNum == 0)
		{
			avgTemper = 0;
			for(i = 0;i < TEMP_SENSOR_NUM_MAX;i++)
			{
				tempData[i] = 0;
				tempMax[i] = 0;
				tempMin[i] = 0;
			}
		}
		else
		{
			for(i = (sensorCfg.tempSensorNum);i < TEMP_SENSOR_NUM_MAX;i++)	
			{
				tempData[i] = 0;
				tempMax[i] = 0;
				tempMin[i] = 0;
			}
		}
		
		num =0;
		tmp = 0;
		for(i = 0;i < sensorCfg.humiSensorNum;i++)
		{
			if(Dev_info[HUMI1-1+i].Dev_sta == MB_MRE_NO_ERR)
			{
				if(humiMax[i] == 0)
				{
					humiMax[i] = humiData[i];
				}
				if(humiMin[i] == 0)
				{
					humiMin[i] = humiData[i];
				}
				if(humiData[i] >= humiMax[i])
				{
					humiMax[i] = humiData[i];
				}
				else if(humiData[i] <= humiMin[i])
				{
					humiMin[i] =  humiData[i];
				}
				tmp += humiData[i];;
				num++;
			}
		}
		if(num == 0)
		{
			avgHumi = 0;
		}
		else
		{
			avgHumi = tmp / num;
		}
		if(sensorCfg.humiSensorNum == 0)
		{
			avgHumi = 0;
			for(i = 0;i < HUMI_SENSOR_NUM_MAX;i++)
			{
				humiData[i] = 0;
				humiMax[i] = 0;
				humiMin[i] = 0;
			}
		}
		*/
  	}
}

// 0x04，读输入寄存器，   eMBFuncReadInputRegister，mbfuncinput.c，以及eMBRegInputCB函数
// 0x01，读取线圈状态，   eMBFuncReadCoils，mbfunccoils.c，以及eMBRegCoilsCB函数 
// 0x05，写单个线圈，     eMBFuncWriteCoil，同上 
// 0x0F，写多个线圈，     eMBFuncWriteMultipleCoils，同上 
// 0x11，报告从机标识，   eMBFuncReportSlaveID，mbfuncother.c，需要在初始化时进行设置 
// 0x06，写单个寄存器，   eMBFuncWriteHoldingRegister，mbfuncholding.c，需要eMBRegHoldingCB函数 
// 0x10，写多个寄存器，   eMBFuncWriteMultipleHoldingRegister，同上 
// 0x03，读保持寄存器，   eMBFuncReadHoldingRegister，同上 
// 0x17，读/写多个寄存器，eMBFuncReadWriteMultipleHoldingRegister，同上 
// 0x02，读输入状态，     eMBFuncReadDiscreteInputs，mbfuncdisc.c，需要eMBRegDiscreteCB函数 
void thread_entry_ModbusMasterPoll(void* parameter){
  #ifndef TEST    // 
  eMBMasterInit(MB_RTU, MODBUS_Port, RS485_SENSOR_BAUD_RATE,  MB_PAR_NONE);//MB_PAR_EVEN);
  eMBMasterEnable();
  while (1)  {
    Rs485REC_threadCount =0;
    eMBMasterPoll();
    rt_thread_delay(100);
  }
  #else    // test used slaver
  // eMBMode eMode, UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity
  eMBInit(MB_RTU, 0x01, MODBUS_Port, RS485_SENSOR_BAUD_RATE,  MB_PAR_NONE);
  eMBEnable();
  while (1) {    
    eMBPoll();
  }
  #endif
}



int8_t	targetClr(int8_t dayNumLevel)
{
	int8_t	i = 0;

	if(dayNumLevel < 0 || dayNumLevel > (DAY_LEVEL_MAX - 1))
	{
		return 0;
	}
	for(i = (DAY_LEVEL_MAX - 1);i >= dayNumLevel;i--)
	{
		tempModeCfg.targetTemp[i].dayCount = 0;
		tempModeCfg.targetTemp[i].temp = 0;
	}
	return 0;
}


int8_t	isTargetCfgRight2(int8_t	* errIndex)
{
	int8_t	i = 0;
	int8_t	flag = 0;	//是否获取到有效配置

	//如果第一行的日龄或者目标温度值不合理，则将所有配置清除
	if((0 == tempModeCfg.targetTemp[0].dayCount) || (0 == tempModeCfg.targetTemp[0].temp))
	{
		targetClr(0);
		return ERR_TARGET_FIST_ERR;
	}

	//日龄数值必须递增，否则从错误行开始往下清除
	for(i = (DAY_LEVEL_MAX - 1);i>0;i--)
	{
		if(0 == flag)
		{
			if(0  == tempModeCfg.targetTemp[i].dayCount)
			{
				continue;	
			}
		}
		else
		{
			if(0 == tempModeCfg.targetTemp[i].dayCount)
			{
				*errIndex = i;
				targetClr(i);
				return ERR_TARGET_DAY_NUM_ERR;
			}
		}
		flag = 1;
		if(tempModeCfg.targetTemp[i].dayCount <= tempModeCfg.targetTemp[i - 1].dayCount)
		{
			*errIndex = i;
			targetClr(i);
			return ERR_TARGET_DAY_NUM_ERR;
		}
	}

	//温度数值必须递减，否则从错误行开始往下清除
	flag = 0;
	for(i = (DAY_LEVEL_MAX - 1);i>0;i--)
	{
		if(0 == flag)
		{
			if(0  == tempModeCfg.targetTemp[i].temp)
			{
				continue;	
			}
		}
		else
		{
			if(0 == tempModeCfg.targetTemp[i].temp)
			{
				*errIndex = i;
				targetClr(i);
				return ERR_TARGET_DAY_NUM_ERR;
			}
		}
		flag = 1;
		if(tempModeCfg.targetTemp[i].temp > tempModeCfg.targetTemp[i - 1].temp)
		{
			*errIndex = i;
			targetClr(i);
			return ERR_TARGET_DAY_NUM_ERR;
		}
	}

	//加热值必须低于目标值,否则将加热值清除
	flag = 0;
	for(i = 0;i < DAY_LEVEL_MAX;i++)
	{
		if(tempModeCfg.targetTemp[i].temp != 0 && tempModeCfg.targetTemp[i].dayCount != 0)
		{
			if(tempModeCfg.dayCtrlMode[i].heatDevCfg.startParam > 0)
			{
				if(tempModeCfg.dayCtrlMode[i].heatDevCfg.startParam > tempModeCfg.targetTemp[i].temp)
				{
					tempModeCfg.dayCtrlMode[i].heatDevCfg.startParam = 0;
					flag = 1;
				}
			}
		}
		else
		{
			tempModeCfg.targetTemp[i].temp = 0;
			tempModeCfg.targetTemp[i].dayCount = 0;
			tempModeCfg.dayCtrlMode[i].heatDevCfg.startParam = 0;
		}
	}
	if(flag)
	{
		return ERR_TARGET_VALUE;	
	}
	//高温值必须高于目标值，低温值必须低于目标值，否则将对应值清除
	flag = 0;
	for(i = 0;i < DAY_LEVEL_MAX;i++)
	{
		if(tempModeCfg.targetTemp[i].temp != 0 && tempModeCfg.targetTemp[i].dayCount != 0)
		{
			if(tempModeCfg.targetTemp[i].high_temp > 0)
			{
				if(tempModeCfg.targetTemp[i].high_temp < tempModeCfg.targetTemp[i].temp)
				{
					tempModeCfg.targetTemp[i].high_temp = 0;
					flag = 1;
				}
			}
			if(tempModeCfg.targetTemp[i].low_temp > 0)
			{
				if(tempModeCfg.targetTemp[i].low_temp > tempModeCfg.targetTemp[i].temp)
				{
					tempModeCfg.targetTemp[i].low_temp = 0;
					flag = 1;
				}
			}
		}
		else
		{
			tempModeCfg.targetTemp[i].temp = 0;
			tempModeCfg.targetTemp[i].dayCount = 0;
			tempModeCfg.targetTemp[i].high_temp = 0;
			tempModeCfg.targetTemp[i].low_temp = 0;
		}
	}
	if(flag)
	{
		return ERR_TARGET_VALUE;	
	}

	return ERR_TARGET_NO;
}



int8_t	isTargetCfgRight(void)
{
	int8_t	i = 0;
	uint8_t j = 0;
	uint8_t	flag = 0;

	if(0 == tempModeCfg.targetTemp[0].dayCount)
	{
		
		return ERR_TARGET_FIST_ERR;
	}
		
	for(i = (DAY_LEVEL_MAX - 1);i > 0;i--)
	{
		if(0 == flag)
		{
			if(0 == tempModeCfg.targetTemp[i].dayCount)
				continue;
		}
		else
		{
			if(0 == tempModeCfg.targetTemp[i].dayCount)
				return i;
		}
		flag = 1;
		if(tempModeCfg.targetTemp[i].dayCount <= tempModeCfg.targetTemp[i - 1].dayCount)
		{
			return i;
		}
	}
	flag = 0;
	if(0 == tempModeCfg.targetTemp[0].temp)
	{
		return 0;
	}
		
	for(i = (DAY_LEVEL_MAX - 1);i > 0;i--)
	{
		if(0 == flag)
		{
			if(0 == tempModeCfg.targetTemp[i].temp)
				continue;
		}
		else
		{
			if(0 == tempModeCfg.targetTemp[i].temp)
				return (i);
		}
		flag = 1;
		if(tempModeCfg.targetTemp[i].temp >= tempModeCfg.targetTemp[i - 1].temp)
		{
			return (i);
		}
	}

	flag = 0;
	for(i = 0;i < DAY_LEVEL_MAX;i++)
	{
		if((tempModeCfg.targetTemp[i].dayCount != 0) && (tempModeCfg.targetTemp[i].dayCount != 0))
		{
			if(tempModeCfg.targetTemp[i].high_temp <= tempModeCfg.targetTemp[i].low_temp)
			{
				/*
				tempModeCfg.targetTemp[i].high_temp = tempModeCfg.targetTemp[i].temp + 300;
				if(tempModeCfg.targetTemp[i].low_temp >= 300)
					tempModeCfg.targetTemp[i].low_temp = tempModeCfg.targetTemp[i].low_temp - 300;
				else
						tempModeCfg.targetTemp[i].low_temp = 0;
				*/
				tempModeCfg.targetTemp[i].high_temp = 0;
				tempModeCfg.targetTemp[i].low_temp = 0;
			}
		}

		if(tempModeCfg.dayCtrlMode[i].heatDevCfg.startParam > 0)
		{
			if((tempModeCfg.dayCtrlMode[i].heatDevCfg.startParam >= tempModeCfg.targetTemp[i].temp))
			{
				for(j = 0;j < i;j++)
				{
					tempModeCfg.dayCtrlMode[i].heatDevCfg.startParam = 0;
				}
				return (i*10);
			}
		}
	
	}
	
	return -1;
}

int8_t	isTempStep(uint8_t	dayNum)
{
	uint8_t i = 0; 
	uint8_t	j = 0;
	uint8_t flag = 0;

	if(tempModeCfg.targetTemp[dayNum].dayCount == 0 || tempModeCfg.targetTemp[dayNum].temp == 0)
	{
		for(i = 0; i < TEMP_LEVEL_MAX;i++)
		{
			if(tempModeCfg.dayCtrlMode[dayNum].tempCtrlMode[j].tempMax > 0)
			{
				return i;	
			}
		}
		for(i = 0;i < TEMP_LEVEL_MAX;i++)
		{
			tempModeCfg.dayCtrlMode[dayNum].tempCtrlMode[j].tempMax = 0;
			for(j = 0;j < allDevMax;j++)	
			{
				tempModeCfg.dayCtrlMode[dayNum].tempCtrlMode[j].sta[j] = 0;
			}
		}
		return -1;
	}
	for(i = (TEMP_LEVEL_MAX - 1);i > 0;i--)
	{
		if(0 == flag)
		{
			if(tempModeCfg.dayCtrlMode[dayNum].tempCtrlMode[i].tempMax == 0)	
			{
				for(j = 0;j < devMax;j++)
				{
					tempModeCfg.dayCtrlMode[dayNum].tempCtrlMode[i].sta[j] = 0;
				}
				continue;
			}		
		}
		else
		{
			if(0 == tempModeCfg.dayCtrlMode[dayNum].tempCtrlMode[i].tempMax)
			{
				return i;
			}
		}
		flag = 1;
		if(tempModeCfg.dayCtrlMode[dayNum].tempCtrlMode[i].tempMax <= tempModeCfg.dayCtrlMode[dayNum].tempCtrlMode[i - 1].tempMax)
		{
			return i;
		}
	}

	return -1;
}

//清楚极值
/*
void	data_PeakValueClr(rtcTime_t curRtc)
{
	uint8_t	i;
	
	if(0 == curRtc.hour )
	{
		if(!FLAGS3_CLR_DAY_PEAK_VALUE)
		{
			for(i = 0;i < TEMP_SENSOR_NUM_MAX;i++)
			{
				tempMax[i] = 0;
				tempMin[i] = 0;
			}
			for(i = 0;i < HUMI_SENSOR_NUM_MAX;i++)
			{
				humiMax[i] = 0;
				humiMin[i] = 0;
			}
			FLAGS3_CLR_DAY_PEAK_VALUE = 1;
		}	
	}
	else
	{
		if(FLAGS3_CLR_DAY_PEAK_VALUE)	
		{
			FLAGS3_CLR_DAY_PEAK_VALUE = 0;
		}
	}
	
}
*/

uint8_t	devDayCfgClr(uint8_t dayNumLevel)
{
	uint8_t i = 0; 
	uint8_t	j = 0;
	uint8_t	k = 0;


	for(i = dayNumLevel;i < DAY_LEVEL_MAX;i++)
	{
		for(j = 0;j < TEMP_LEVEL_MAX;j++)
		{
			for(k = 0;k < allDevMax;k++)
			{
				tempModeCfg.dayCtrlMode[i].tempCtrlMode[j].sta[k] = 0;
			}
			tempModeCfg.dayCtrlMode[i].tempCtrlMode[j].tempMax = 0;
		}
		
	}

	return 0;
}

uint8_t	cfgAutoCreate(void)
{
	uint8_t i, j,k;
	uint8_t	tempShiftValue[DAY_LEVEL_MAX - 1] = {0};

	//如果第一日龄的日龄天数或者目标温度为0，则不继续生成
	if((tempModeCfg.targetTemp[0].temp == 0) || (tempModeCfg.targetTemp[0].dayCount == 0))
	{
		return 0;
	}
	//如果N日龄项的日龄值或者目标温度为0，则不继续生成，并把其后边的配置清空
	for(i = 0;i < (DAY_LEVEL_MAX - 1);i++)
	{
		if((tempModeCfg.targetTemp[i].dayCount != 0) && (tempModeCfg.targetTemp[i].temp != 0))
		{
			if((tempModeCfg.targetTemp[i + 1].dayCount != 0) && (tempModeCfg.targetTemp[i + 1].temp != 0))
			{
				if((tempModeCfg.targetTemp[i + 1].temp <= tempModeCfg.targetTemp[i].temp) && (tempModeCfg.targetTemp[i + 1].temp > 0))
				{
					tempShiftValue[i] = tempModeCfg.targetTemp[i].temp - tempModeCfg.targetTemp[i+1].temp;
				}
				else
				{
					break;
					//return 0;
				}
			}
			else
			{
				devDayCfgClr(i + 1);
				break;
				//return 0;
			}
		}
		else
		{
			devDayCfgClr(i);
			//return 0;
			break;
		}
	}

	if(tempModeCfg.dayCtrlMode[0].tempCtrlMode[0].tempMax == 0)
	{
		return 0;
	}
	for(i = 0;i < (DAY_LEVEL_MAX - 1);i++)
	{
		if((tempModeCfg.targetTemp[i].dayCount == 0) || (tempModeCfg.targetTemp[i].temp == 0))
		{
			devDayCfgClr(i);
			break;
		}
		for(j = 0;j < TEMP_LEVEL_MAX;j++)
		{
			if(tempModeCfg.dayCtrlMode[i].tempCtrlMode[j].tempMax >= tempShiftValue[i])
			{
				tempModeCfg.dayCtrlMode[i + 1].tempCtrlMode[j].tempMax = tempModeCfg.dayCtrlMode[i].tempCtrlMode[j].tempMax - tempShiftValue[i];
			}
			for(k = 0;k < allDevMax;k++)
			{
				tempModeCfg.dayCtrlMode[i + 1].tempCtrlMode[j].sta[k] = tempModeCfg.dayCtrlMode[i].tempCtrlMode[j].sta[k];
			}
		}
	}

	return 0;
}


//LCD交互处理
void thread_entry_RS232COMM(void* parameter)
{
	uint8_t	 i = 0;
	//uint8_t  j = 0;
	//int8_t	errIndex = 0;
	
	//int16_t sta;
	int16_t	tmp = 0;
	uint32_t tempDate = 0;
	//rtcTime_t	curRtcTemp, startRtcTemp;
	//char debugStr[35]= {0};
	//static uint8_t	recvDataFlag = 0;
	//static uint8_t rtcTest = 0;
	static uint8_t cfg_Refres_cnt = 0;

	uint8_t roomName[]={0xB1,0xA3,0xD3,0xFD,0xC9,0xE1,0x31};
	uint8_t dateString[16] = {0x30,0x30,0x3A,0x30,0x30,0x20,0x32,0x30,0x32,0x30,0x2D,0x30,0x30,0x2D,0x30,0x30};
	uint8_t testCode[]="https://qianyu.cloud/ccd/login.htm?sbid=00100003";
	uint8_t testCode2[]={0xCC,0xEC};

	uint8_t recvCmdIndex = 0;
	uint8_t recvCmdLen = 0;
	uint16_t recvCmdAddr =0;
	uint16_t recvCmdKeyValue = 0;


  	rt_device_t write_device;
  	//rt_err_t result = RT_EOK;
  	// 查找系统中的串口1设备
  	write_device = rt_device_find("uart5");
  	if (write_device!= RT_NULL)  
  	{
    	rt_device_open(write_device, RT_DEVICE_OFLAG_WRONLY);//RT_DEVICE_OFLAG_RDWR);
  	}
	//打开LCD电源
	POWER_5V_OPEN;
	//rt_thread_delay(2000);
	/*
	目前主要是显示首页。
	目前显示内容：
	猪舍名称，时间，日期，电池电量，是否连上网路，信号强度
	温度，湿度，氨气，当日最大，最小值。
	当前目标温度，当前通风级别，运行天数，最小通风量。
	当前设备运行状态。
	*/
	lcd_show_txt(0X5088,testCode2,2);

	FLAGS_LCD_CFG_REF_5S = TRUE;
	/*
	while(1)
	{
		rt_thread_delay(100);
		
		lcd_show_index(0x5031, curAvgTemp/10);
					lcd_show_index(0x5032, curAvgTemp_DayMax/10);
					lcd_show_index(0x5033, curAvgTemp_DayMin/10);
		
					lcd_show_index(0x5041, curAvgHumi/10);
					lcd_show_index(0x5042, curAvgHumi_DayMax/10);
					lcd_show_index(0x5043, curAvgHumi_DayMin/10);
		
					if(Dev_info[NH3_1 - 1].Dev_EN)
					{
						if(Dev_info[NH3_1 - 1].Dev_sta == MB_MRE_NO_ERR)
						{
							lcd_show_index(0x5051, Dev_info[NH3_1 - 1].Data1);//氨气
						}
					}
					else
						lcd_show_index(0x5051, 0);//氨气
	}
	*/
	while(1)
	{
		rt_thread_delay(100);
		if(FLAGS_LCD_HAS_DATE)
		{
			FLAGS_LCD_HAS_DATE = FALSE;
			recvCmdIndex = recvDataBuf[3];

			recvCmdLen = recvDataBuf[6];
			
			if(recvCmdIndex == 0x83)
			{
				recvCmdAddr = (recvDataBuf[4] << 8) + recvDataBuf[5];
				recvCmdKeyValue = (recvDataBuf[7] << 8) + recvDataBuf[8];
				if(recvCmdAddr == 0x50ca)
				{
					if(recvCmdKeyValue == 1)
					{
						m_LightCtrl_En = TRUE;
						if(m_LightCtrl_En)
						{
							if(m_Equipment_RunInfo_t.lightRunInfo.sta)
							{
								m_DriveBoard_Data_t.sendData.outBits &= ~(1 << 7);
								m_Equipment_RunInfo_t.lightRunInfo.sta = DEV_STOP;
							}
							else
							{
								m_DriveBoard_Data_t.sendData.outBits |= (1 << 7);
								m_Equipment_RunInfo_t.lightRunInfo.sta = DEV_RUN;
							}
						}
						//灯光
						if(m_DriveBoard_Data_t.sendData.outBits & (1 << 7))
						{
							lcd_show_index(0x50c8, 1);
						}
						else 
							lcd_show_index(0x50c8, 0);
					}
					else if(recvCmdKeyValue == 2)
					{
						lcd_show_window_T5L(2);
					}
					else if(recvCmdKeyValue == 3)
					{
						for(i = 0;i < ALARM_TYPE_MAX;i++)
						{
							if(m_Alarm_Proc_t.alarmPorc[i].alarmSta == ALARM_STA_OCCUR)
							{
								if(i == 0)
								{
									lcd_show_window_T5L(3);
									break;
								}
								if(i == 2)
								{
									lcd_show_window_T5L(4);
									break;
								}
								if(i == 3)
								{
									lcd_show_window_T5L(5);
									break;
								}
							}
						}
					}
				}
			}
		}
	
		if(FLAGS_LCD_EH_REF_1S)
		{
			FLAGS_LCD_EH_REF_1S = FALSE;

			cfg_Refres_cnt++;
			if(cfg_Refres_cnt >= 10)
			{
				m_Is_NeedRefresh_LCD_Cfg = TRUE;
			}
			

			
		//刷新最大值最小值
		if(ds1302Data.hour == 0 && ds1302Data.min == 0)
		{
			curAvgTemp_DayMax = curAvgTemp_DayMin = curAvgTemp;
			curAvgHumi_DayMax = curAvgHumi_DayMin = curAvgHumi;

			//电量水量日消耗更新
		}
		else
		{
			if(curAvgTemp_DayMax == 0)	
				curAvgTemp_DayMax =curAvgTemp;
			if(curAvgTemp_DayMin == 0)
				curAvgTemp_DayMin =curAvgTemp;

			if(curAvgHumi_DayMax == 0)	
				curAvgHumi_DayMax =curAvgHumi;
			if(curAvgHumi_DayMin == 0)
				curAvgHumi_DayMin =curAvgHumi;
		}
		
			
			lcd_show_index(0x5031, curAvgTemp/10);
			lcd_show_index(0x5032, curAvgTemp_DayMax/10);
			lcd_show_index(0x5033, curAvgTemp_DayMin/10);

			lcd_show_index(0x5041, curAvgHumi/10);
			lcd_show_index(0x5042, curAvgHumi_DayMax/10);
			lcd_show_index(0x5043, curAvgHumi_DayMin/10);

			if(Dev_info[NH3_1 - 1].Dev_EN)
			{
				if(Dev_info[NH3_1 - 1].Dev_sta == MB_MRE_NO_ERR)
				{
					lcd_show_index(0x5051, Dev_info[NH3_1 - 1].Data1);//氨气
				}
			}
			else
				lcd_show_index(0x5051, 0);//氨气


			rt_thread_delay(50);
			//状态图标
			//报警图标
			for(i = 0;i < ALARM_TYPE_MAX;i++)
			{
				if(m_Alarm_Proc_t.alarmPorc[i].alarmSta == ALARM_STA_OCCUR)
				{
					lcd_show_index(0x5018, 0);
					break;
				}
				if(i == ALARM_TYPE_MAX - 1)
					lcd_show_index(0x5018, 1);
			}
			
			
			//传输图标
			lcd_show_index(0x5020, m_Is_LTE_Connect);
			//电池电量图标
			if(m_BatVol_Percent>= 80)
				lcd_show_index(0x5022, 5);
			else if(m_BatVol_Percent>= 60)
				lcd_show_index(0x5022, 4);
			else if(m_BatVol_Percent>= 40)
				lcd_show_index(0x5022, 3);
			else if(m_BatVol_Percent>= 20)
				lcd_show_index(0x5022, 2);
			else
				lcd_show_index(0x5022, 1);
			
			//信号强度图标
			if(m_LTE_Rssi == 0)
				lcd_show_index(0x5024, 1);
			else if(m_LTE_Rssi <= 2)
				lcd_show_index(0x5024, 1);
			else
			{
				tmp = ((m_LTE_Rssi -  2) / 7);
				tmp += 1;
				if(tmp >= 4)
					tmp = 4;
				lcd_show_index(0x5024, tmp);
			}

			
			
		
			rt_thread_delay(50);
			//变频风机运行
			if(m_DriveBoard_Data_t.sendData.dacOutValue[0] > 0)
			{
				lcd_show_index(0x5090, 1);
				if(0xff == curAirLevel)
					lcd_show_index(0x5092, m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[curDayLevel].minAirSpeed);
				else
					lcd_show_index(0x5092, m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[curAirLevel].freFanCfgArry[0].speed);
			}
			else
			{
				lcd_show_index(0x5090, 0);
				lcd_show_index(0x5092, 0);
			}
			//普通风机
			for(i = 0;i < 5;i++)
			{
				if(m_DriveBoard_Data_t.sendData.outBits & (1 << i))
				{
					lcd_show_index(0x50A0+ 4 * i, 1);
				}
				else
				{
					lcd_show_index(0x50A0+ 4 * i, 0);
				}
			}
			//加热
			if(m_DriveBoard_Data_t.sendData.outBits & (1 << 5))
			{
				lcd_show_index(0x50c0, 1);
			}
			else
			{
				lcd_show_index(0x50c0, 0);
			}
			//湿帘
			if(m_DriveBoard_Data_t.sendData.outBits & (1 << 6))
			{
				lcd_show_index(0x50c4, 1);
			}
			else
			{
				lcd_show_index(0x50c4, 0);
			}
			
			//灯光
			
			if(m_DriveBoard_Data_t.sendData.outBits & (1 << 7))
			{
				lcd_show_index(0x50c8, 1);
			}
			else
			{
				lcd_show_index(0x50c8, 0);
			}
			
			//耳标图标
			lcd_show_index(0x50d0, 1);
			//任务图标
			lcd_show_index(0x50d4, 1);
		}
		
		if(FLAGS_LCD_CFG_REF_5S)
		{
			FLAGS_LCD_CFG_REF_5S= FALSE;

				//猪舍名称
			lcd_show_txt(0x5000, roomName, sizeof(roomName));
			//时间日期
			dateString[0] = ds1302Data.hour / 10 + '0';
			dateString[1] = ds1302Data.hour % 10 + '0';
			dateString[3] = ds1302Data.min / 10 + '0';
			dateString[4] = ds1302Data.min % 10 + '0';
			dateString[11] = ds1302Data.month / 10 + '0';
			dateString[12] = ds1302Data.month % 10 + '0';
			dateString[14] = ds1302Data.day / 10 + '0';
			dateString[15] = ds1302Data.day % 10 + '0';
			lcd_show_txt(0x5010, dateString, sizeof(dateString));

			lcd_show_index(0X5080, curTargetTemp);//目标温度
			lcd_show_index(0X5081, curDayNum);//天数
			lcd_show_index(0X5082, curAirLevel+1);//通风级别
			lcd_show_index(0X5083, m_Controler_Ctrl_Cfg_t.targetTemperCfg.targetTempArry[curDayLevel].minAirSpeed);//最下通风量


				
			//PH  水压   电量  水量消耗
			lcd_show_index(0x5062, Dev_info[PH - 1].Data1 / 10);
			lcd_show_index(0x5066, Dev_info[WATER_PRESU1 - 1].Data1);
			tempDate = Dev_info[EP_METER - 1].Data1;
			tempDate <<= 16;
			curElecPower_DayUse = (tempDate + Dev_info[EP_METER - 1].Data2)*15;
			//curElecPower_DayUse = 123;
			lcd_show_index(0x506a, curElecPower_DayUse);

			tempDate = Dev_info[WATER_METER - 1].Data1;
			tempDate <<= 16;
			curWaterPower_DayUse = tempDate + Dev_info[WATER_METER - 1].Data2;
			//curWaterPower_DayUse = 456;
			lcd_show_index(0x5072, curWaterPower_DayUse);
			
			//二维码
			lcd_show_CodePic(0x5100, testCode, strlen((char*)testCode));

			//配置运行信息显示
			if(m_Is_NeedRefresh_LCD_Cfg)
			{
				m_Is_NeedRefresh_LCD_Cfg = FALSE;

				for(i = 0;i < 7;i++)
				{
					lcd_show_index(0x5210 + i, curTargetTemp + m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[i].temperShift);
					lcd_show_index(0x5220 + i, m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[i].freFanCfgArry[0].speed);
					lcd_show_index(0x5230 + i, m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[i].freFanCfgArry[0].runTime);
					lcd_show_index(0x5240 + i, m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[i].fanCfgArry[0].runTime);
					lcd_show_index(0x5250 + i, m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[i].fanCfgArry[1].runTime);
					lcd_show_index(0x5260 + i, m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[i].fanCfgArry[2].runTime);
					lcd_show_index(0x5270 + i, m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[i].fanCfgArry[3].runTime);
					lcd_show_index(0x5280 + i, m_Controler_Ctrl_Cfg_t.airCtrlCfg.airCtrllevel[i].fanCfgArry[4].runTime);
				}
				lcd_show_index(0x5290, m_Controler_Ctrl_Cfg_t.airCtrlCfg.freFanCycleTime);
				lcd_show_index(0x5291, m_Controler_Ctrl_Cfg_t.airCtrlCfg.fanCycleTim);	
			}

		}




		rt_thread_delay(100);
	}
	
	/*
	//主要是等待液晶屏启动
	while(1)
	{
		rt_thread_delay(1000);
		IWDG_Feed();
		if(FLAGS3_RTC_GET_SUC)
			break;
		if(i++ >= 5)
			break;
	}
	lcd_select_pic(PIC_ID_SIMPLE_DATA);
	lcd_driveBeep(200);

	RTC_Init();

	//RTC_Set(2017,6,21,13,46,55);  //设置时间	

  	while (1) 
	{
    	if (FLAGS0_RS232HAS_DAT)
		{

      		IWDG_Feed(); //feed the dog
      		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
      		SCI_RecData();
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
      		FLAGS2_CORECOMM =TRUE;
      		FLAGS0_RS232HAS_DAT = 0;

			recvDataFlag = 1;
			RS232ERR_Cout = 0;
    	}
    	IWDG_Feed(); //feed the dog


		
		if(FLAGS3_SAVE_TARGET)
		{
			sta = isTargetCfgRight2(&errIndex);
			//sta = 0;
			if(sta)
			{
				lcd_show_window(DATA_ERR_WINDOW);
			}
			else
			{
				devModeCfg[devInfo.curSeason] = tempModeCfg;
				saveModeCfgData(devInfo.curSeason);
				saveDevInfoData();
				lcd_show_window(SAVE_OK_WINDOW);
			}
			FLAGS3_SAVE_TARGET = 0;
		}
		
		
		if((FLAGS3_SAVE_CTRL_CFG) || (FLAGS3_SAVE_DEV_CFG))
		{
			if(lcdCtrlSaveId >= 1 && lcdCtrlSaveId <= DAY_LEVEL_MAX)
			{
				sta = isTempStep(lcdCtrlSaveId - 1);
				if(sta >= 0)
				{
					lcd_show_window(DATA_ERR_WINDOW);
					for(i = sta;i < TEMP_LEVEL_MAX;i++)
					{
						//xx devModeCfg[devInfo.curSeason].dayCtrlMode[lcdCtrlSaveId - 1].tempCtrlMode[i].tempMax = 0;
						tempModeCfg.dayCtrlMode[lcdCtrlSaveId - 1].tempCtrlMode[i].tempMax = 0;
						for(j = 0;j < devMax;j++)
						{
							//xx devModeCfg[devInfo.curSeason].dayCtrlMode[lcdCtrlSaveId - 1].tempCtrlMode[i].sta[j] = 0;
							tempModeCfg.dayCtrlMode[lcdCtrlSaveId - 1].tempCtrlMode[i].sta[j] = 0;
						}
					}
				}
				else
				{
					devModeCfg[devInfo.curSeason] = tempModeCfg;
					saveModeCfgData(devInfo.curSeason);
					lcd_show_window(SAVE_OK_WINDOW);
				}
			}
			
			FLAGS3_SAVE_CTRL_CFG = 0;
			FLAGS3_SAVE_DEV_CFG = 0;
		}
		

		if(FLAGS3_SAVE_DEV_CFG)
		{
			if(devSaveId)
			{
				saveAlarmCfgData();
			}
			FLAGS3_SAVE_DEV_CFG = 0;
		}
		if(FLAGS4_ALARM_ACT_GET_SUC)
		{
			FLAGS4_ALARM_ACT_GET_SUC = 0;
			for(i = 0; i< alarmEventMax;i++)
			{
				for(j = 0;j < alarmActMax;j++)
				{
					if((j == localBeep) || (j == pushMes))
					{
						alarmCfg.devAlarmCfg[i].actEn[j] = 1;
						continue;
					}
					alarmCfg.devAlarmCfg[i].actEn[j] = alarmCfgTemp.devAlarmCfg[i].actEn[j];
				}
			}
			//saveAlarmCfgData();
		}
		if(FLAGS3_SAVE_DEV_CFG)
		{
			//saveModeCfgData();
			FLAGS3_SAVE_DEV_CFG = 0;
		}
		if(FLAGS3_SAVE_MANAGER_CFG)
		{
			saveDevInfoData();
			saveAlarmCfgData();
			lcd_show_window(0x02);
			FLAGS3_SAVE_MANAGER_CFG = 0;
		}
		if(FLAGS4_JQID_GET_SUC)
		{
			FLAGS4_JQID_GET_SUC = 0;
			saveDevIdCfg();
		}
		if(FLAGS3_SAVE_SENSOR_AJUST)
		{
			saveSensorCfgData();
			lcd_show_window(0x02);
			FLAGS3_SAVE_SENSOR_AJUST = 0;
		}
		if(FLAGS3_SAVE_INSTALL)
		{
			saveSensorCfgData();
			lcd_show_window(0x02);
			FLAGS3_SAVE_INSTALL = 0;
		}

		//一旦修改日龄，则立刻重新计算日龄并显示
		if((FLAGS3_MODIFY_DAY_NUM) && (FLAGS3_RTC_GET_SUC))
		{
			devInfo.saveRtc = curRtc;
			FLAGS3_MODIFY_DAY_NUM = 0;
			FLAGS3_GET_CUR_DAY_NUM = 1;
		}

		if((FLAGS3_GET_CUR_DAY_NUM) && (FLAGS3_RTC_GET_SUC))
		{
			curRtcTemp = curRtc;
			data_PeakValueClr(curRtcTemp);
			startRtcTemp = devInfo.saveRtc;
			tmp = getCurDayNum(curRtcTemp,startRtcTemp);
			if(tmp <= 0)
			{
				devInfo.curDayNum = devInfo.saveDayNum;
			}
			else
			{
				
				if((devInfo.saveDayNum + tmp) > 999)
				{
					devInfo.curDayNum = 999;
				}
				else
				{
					devInfo.curDayNum = devInfo.saveDayNum + tmp;
				}
			}
			dayNumLevel = getDayItem2(devInfo.curSeason, devInfo.curDayNum);
			FLAGS3_GET_CUR_DAY_NUM = 0;
		}
		if(FLAGS4_IS_NEED_CFG_AUTO_CREATE)
		{
			FLAGS4_IS_NEED_CFG_AUTO_CREATE = 0;
			cfgAutoCreate();
			lcd_show_window(AUTO_CREATE_OK);
		}
		
    	if ((FLAGS3_REFRESH_LCD) || (recvDataFlag))
		{
      		IWDG_Feed(); //feed the dog
      		SCI_SendData(write_device);    
      		FLAGS3_REFRESH_LCD = 0;
			//debug_uart_sendbyte(0x55);
			recvDataFlag = 0;
			RTC_Get();
    	}

		rt_thread_delay(100);
  	}
	*/
}

/*
void	alarmProcFunc(void)
{
	uint8_t i = 0;
	int16_t	curTemp = avgTemper / 10;
	int16_t	curHumi = avgHumi /10;
	uint8_t	sensorErrFlag  = 0;

	if(curTemp <= 0)
		return;

	//i = IS_POWER_OFF();
	//power off alarm check
	//if((alarmCfg.devAlarmCfg[powerOffAlarm].en) && (IS_POWER_OFF()))
	{
		devAlarmFlag.alarmFlag[powerOffAlarm] = alarmAccur;
	}
	//else
	{
		devAlarmFlag.alarmFlag[powerOffAlarm] = alarmNo;
		alarmRunSta.devAlarmSta[powerOffAlarm].sta = alarmNo;
	}
	//high temp alarm check
	if((alarmCfg.devAlarmCfg[highTempAlarm].en) && (tempModeCfg.targetTemp[dayNumLevel].high_temp > 0))
	{
		if(curTemp > (devModeCfg[devInfo.curSeason].targetTemp[dayNumLevel].high_temp + 5))
		{
			devAlarmFlag.alarmFlag[highTempAlarm] = alarmAccur;
		}
		else if(curTemp < (devModeCfg[devInfo.curSeason].targetTemp[dayNumLevel].high_temp - 5))
		{
			devAlarmFlag.alarmFlag[highTempAlarm] = alarmNo;
		}
	}
	else
	{
		devAlarmFlag.alarmFlag[highTempAlarm] = alarmNo;
		alarmRunSta.devAlarmSta[highTempAlarm].sta = alarmNo;
	}

	//low temp alarm check
	if((alarmCfg.devAlarmCfg[lowTempAlarm].en) && (devModeCfg[devInfo.curSeason].targetTemp[dayNumLevel].low_temp > 0))
	{
		if(curTemp > (devModeCfg[devInfo.curSeason].targetTemp[dayNumLevel].low_temp + 5))
		{
			devAlarmFlag.alarmFlag[lowTempAlarm] = alarmNo;
		}
		if(curTemp < (devModeCfg[devInfo.curSeason].targetTemp[dayNumLevel].low_temp - 5))
		{
			devAlarmFlag.alarmFlag[lowTempAlarm] = alarmAccur;
		}
	}
	else
	{
		devAlarmFlag.alarmFlag[lowTempAlarm] = alarmNo;
		alarmRunSta.devAlarmSta[lowTempAlarm].sta = alarmNo;
	}

	//high humi alarm check
	if((alarmCfg.devAlarmCfg[highHumiAlarm].en) && (alarmCfg.devAlarmCfg[highHumiAlarm].condition > 0))
	{
		if(curHumi > alarmCfg.devAlarmCfg[highHumiAlarm].condition )
		{
			devAlarmFlag.alarmFlag[highHumiAlarm] = alarmAccur;
		}
		if(curHumi < (alarmCfg.devAlarmCfg[highHumiAlarm].condition - 5))
		{
			devAlarmFlag.alarmFlag[highHumiAlarm] = alarmNo;
		}
	}
	else
	{
		devAlarmFlag.alarmFlag[highHumiAlarm] = alarmNo;
		alarmRunSta.devAlarmSta[highHumiAlarm].sta = alarmNo;
	}
	//sensor err alarm check 
	if(alarmCfg.devAlarmCfg[sensorErrAlarm].en)
	{
		for(i = 0;i < sensorCfg.tempSensorNum;i++)
		{
			if(Dev_info[TEMP1 - 1 + i].Dev_sta != MB_MRE_NO_ERR)	
			{
				sensorErrFlag = 1;	
			}
			else
			{
				sensorErrFlag = 0;
			}
			if(sensorErrFlag)
				break;
		}
		if(!sensorErrFlag)
		{
			for(i = 0;i < sensorCfg.humiSensorNum;i++)
			{
				if(Dev_info[HUMI1 - 1 + i].Dev_sta != MB_MRE_NO_ERR)	
				{
					sensorErrFlag = 1;	
				}
				else
				{
					sensorErrFlag = 0;
				}
				if(sensorErrFlag)
					break;
			}
		}
		
		
		if(sensorErrFlag)
		{
			devAlarmFlag.alarmFlag[sensorErrAlarm] = alarmAccur;	
		}
		else
		{
			devAlarmFlag.alarmFlag[sensorErrAlarm] = alarmNo;
			alarmRunSta.devAlarmSta[sensorErrAlarm].sta = alarmNo;
		}
	}
	else
	{
		devAlarmFlag.alarmFlag[sensorErrAlarm] = alarmNo;
		alarmRunSta.devAlarmSta[sensorErrAlarm].sta = alarmNo;
	}

	

	for(i = 0;i < alarmEventMax;i++)
	{
		if(alarmRunSta.devAlarmSta[i].sta == alarmAccur)
		{
			//GPIO_ResetBits(FAN16_OUT);
			ALARM_BEEP_RUN;
			break;
		}
		else
		{
			if(i == alarmEventMax-1)
			{
				//GPIO_SetBits(FAN16_OUT);
				ALARM_BEEP_OFF;
			}
		}
	}
	
}
void	coldDevRun(void)
{
	if(devModeCfg[devInfo.curSeason].coldDevCfg.startShift != 0)
	{
		if(!devRunSta[coldDev])
		{
			if(devModeCfg[devInfo.curSeason].coldDevCfg.stopParam > 0)
			{
				if(((avgHumi/10) < devModeCfg[devInfo.curSeason].coldDevCfg.stopParam) && ((avgTemper/10) >= (devModeCfg[devInfo.curSeason].targetTemp[dayNumLevel].temp + devModeCfg[devInfo.curSeason].coldDevCfg.stopShift)))
				{
					devRunSta[coldDev] = 1;
					//devRunFunc(coldDev, devRunSta[coldDev]);
				}

			}
			else
			{
				if((avgTemper/10) >= (devModeCfg[devInfo.curSeason].targetTemp[dayNumLevel].temp + devModeCfg[devInfo.curSeason].coldDevCfg.startShift))
				{
					devRunSta[coldDev] = 1;
					//devRunFunc(coldDev, devRunSta[coldDev]);
				}
			}
		}
		else
		{
			if(devModeCfg[devInfo.curSeason].coldDevCfg.stopParam > 0)
			{
				if(((avgHumi/10) >= devModeCfg[devInfo.curSeason].coldDevCfg.stopParam) || ((avgTemper/10) <= (devModeCfg[devInfo.curSeason].targetTemp[dayNumLevel].temp - devModeCfg[devInfo.curSeason].coldDevCfg.stopShift)))
				{
					devRunSta[coldDev] = 0;
					//devRunFunc(coldDev, devRunSta[coldDev]);
				}
			}
			else
			{
				if((avgTemper/10) <= (devModeCfg[devInfo.curSeason].targetTemp[dayNumLevel].temp - devModeCfg[devInfo.curSeason].coldDevCfg.stopShift))
				{
					devRunSta[coldDev] = 0;
				}
			}
		}
		
	}
	else
	{
		devRunSta[coldDev] = 0;
	}
	//devRunFunc(coldDev, devRunSta[coldDev]);
}

void	heatDevRun(void)
{
	if(devModeCfg[devInfo.curSeason].dayCtrlMode[dayNumLevel].heatDevCfg.startParam > 0)
	{
		if(!devRunSta[heatDev])	
		{
			if((avgTemper/10) < devModeCfg[devInfo.curSeason].dayCtrlMode[dayNumLevel].heatDevCfg.startParam)
			{
				devRunSta[heatDev] = 1;	
			}
		}
		else
		{
			if((avgTemper/10) > (devModeCfg[devInfo.curSeason].targetTemp[dayNumLevel].temp + devModeCfg[devInfo.curSeason].heatStopShift))
			{
				devRunSta[heatDev] = 0; 
			}

		}
	}
	else
	{
		devRunSta[heatDev] = 0;
	}
	//devRunFunc(heatDev, devRunSta[heatDev]);
}
*/
void thread_entry_DevRun(void* parameter)
{
	while(1)
	{
		
		//故障检查
		//刷新天数，目标温度
		//根据当前温度刷新通风级别，并运行通风级别设置
		//加热，制冷，灯光，料线，小窗，幕帘控制
		
		Ctroler_Get_DevRunCfg();
		rt_thread_delay(200);
	}
}
/*
void thread_entry_DevRun(void* parameter)
{
	uint16_t sta;
	uint8_t	i = 0;
	//uint8_t num = 0;
	//int16_t	tmp = 0;
	dayNumLevel = 0;
	tempStep = 0;
	

	while(1)
	{
		RTC_Get();
		alarmProcFunc();

		IWDG_Feed();
		if(FLAGS3_DEBUG_ENTER)
		{
			
			for(i = 0;i < (uint8_t)devMax;i++)
			{
				devRunFunc((devSort)i, devDebugCtrl.devAction[i]);
				rt_thread_delay(100);
			}
			devRunFunc(coldDev, devDebugCtrl.coldDevAction);
			devRunFunc(heatDev, devDebugCtrl.heatDevAction);
		}
		else
		{
			tempStep = getRunStep(devInfo.curSeason, dayNumLevel, avgTemper);
			for(i = 0;i < (uint8_t)devMax;i++)
			{
				
				//sta = getDevState2(devInfo.curSeason, devInfo.curDayNum, (devSort)i,(Dev_info[TEMP1-1].Data1/10);
				//sta = 1;
				sta = devModeCfg[devInfo.curSeason].dayCtrlMode[dayNumLevel].tempCtrlMode[tempStep].sta[i];
				//yy sta = tempModeCfg.dayCtrlMode[dayNumLevel].tempCtrlMode[tempStep].sta[i];
				devRunFunc((devSort)i, sta);
						
				rt_thread_delay(500);
			}
			coldDevRun();
			heatDevRun();
			rt_thread_delay(1000);
			IWDG_Feed();
			rt_thread_delay(1000);
			IWDG_Feed();
		}
		rt_thread_delay(1000);
		IWDG_Feed();
		
	}
}

*/

//主要用于获取电池电量
void thread_entry_ADC(void* parameter)
{
  	uint16_t ADc[2];
	float temp = 0;
	
	
	FLAGS_ADC_RD_3S = TRUE;

  	ADC_init();
  	while(1)
	{
		
		if (FLAGS_ADC_RD_3S)
		{
		  	IWDG_Feed(); //feed the dog
		  	ADc[0] = Get_Adc(ADC_CH1);    // ADC_DEBUG
			//ADc[1] = Get_Adc(ADC_CH2); 	
			
		  	FLAGS_ADC_RD_3S = 0;  
			temp = ADc[0]*4.4*3.3/4096;
			temp -= 9;		//9-12V为电池电量区间
			temp /= 3;
			temp *= 100;
			if(temp >= 100)
				temp = 100;
			m_BatVol_Percent = (uint8_t)temp;
		}
		
		rt_thread_delay(500);
	}
}


//暂时不用
void thread_entry_DAC(void* parameter)
{
	//int i = 0;
	//uint32_t data = 0;
	
	Dac_Init();
	
	
 	while(1)
	{
	/*
		for(i = 0;i <SPEED_FAN_NUM ;i++)
		{
			MOTOR_SPEED_REC[1] = MOTOR_SPEED_REC[0];
			//if(MOTOR_SPEED_REC[i] >= 100)
			//MOTOR_SPEED_REC[i] = 50;
			data = (MOTOR_SPEED_REC[i] * MAX_DAC_VALUE);
			
			if(i == 0)
				data = data*0.83/100;
			else if(i == 1)
				data = data*0.93/100;
			//data = data/100;
			if(FLAGS4_DAC_5V_10V_SELECT == DA_OUT_MAX_5V)
			{
				data >>= 1;
			}
			data >>= 1;
			dacOut(i,(unsigned int)data);
			rt_thread_delay(200);
		}
		*/
		rt_thread_delay(2000);
	}		

}

bool uploadAlarmEn()
{
/*
	char send_temp[50] ={0};
	bool ret = TRUE;
	uint8_t check =0;
	uint8_t i =0;

	//uart485SendData("upload alarm en\n");
	check = (jqid % 10) + 14;
	check = check % 10;
	esimSumCheck = check;
	sprintf(send_temp , "hm+%d+14+%d+end",jqid,check);
	//uart485SendData(send_temp);
	//uart485SendData("\n");
	for(i = 0; i < 5; i++)
	{IWDG_Feed();
		ret = TCP_MIPSTRS_Send(SOCKET_CHANNEL_1, HEX_FORMAT, (uint8_t*)send_temp ,strlen(send_temp));
		if(ret)
		{
			ret =  TCP_Read_ALARMEN(14);
			if(ret)
			{
				//uart485SendData("upload alarm en SUC\n");
				return TRUE;
			}		     
		}
	}
	
	return ret;
*/
}
/*

bool uploadAlarmPara()
{
	char send_temp[50] ={0};
	bool ret = FALSE;
	uint8_t i=0;
	uint8_t check =0;

	uart485SendData("upload alarm para \n");
	check = (jqid % 10) + 18;
	check = check % 10;
	esimSumCheck = check;
	sprintf(send_temp , "hm+%d+18+%d+end",jqid,check);
		uart485SendData(send_temp);
	uart485SendData("\n");
	for(i = 0; i < 5; i++)
	{IWDG_Feed();
		ret = TCP_MIPSTRS_Send(SOCKET_CHANNEL_1, HEX_FORMAT, (uint8_t*)send_temp ,strlen(send_temp));
		if(ret)
		{
			ret =  TCP_Read_ALARM_PARA(18);
			if(ret)
			{
				uart485SendData("upload alarm para SUC\n");
				return TRUE; 
			}
			   
		}
	}
	return ret;

}
*/





uint8_t alarmCheck(uint8_t index)
{
    uint8_t errType =0;

	switch(index)
	{
		case	powerOffAlarm:
		{
			errType =appEventAlarmPowerCut;
			break;
		}
		case	highTempAlarm:
		{
			errType =appEventAlarmOverTemp;
			break;
		}
		case	lowTempAlarm:
		{
			errType =appEventAlarmLowTemp;
			break;
		}
		case	tempDiffAlarm:
		{
			errType =appEventHighTempDiff;
			break;
		}
		case	highHumiAlarm:
		{
			errType =appEventHighHumidity;
			break;
		}
		case	sensorErrAlarm:
		{
			errType =appEventTempSensorFault;
			break;
		}
		default:
			break;
	}
	return errType;
}
	
unsigned char actMess(uint8_t index,uint8_t state)
{
/*
	char send_temp[50] ={0};
	
	bool ret = FALSE;
	uint8_t check =0;
	uint8_t errType =0;
	uint8_t i = 0;

	
	errType = alarmCheck(index);



	check = (jqid % 10) + 5 + (errType % 10) + 0 + (state %10);
	check =check%10; 
	esimSumCheck = check;
	
	sprintf(send_temp , "hm+%d+15+%d+0+%d+%d+end" , jqid,errType,state,check);
	
	for(i = 0; i < ESIM_TRANS_CNT; i++)
	{
		ret = TCP_MIPSTRS_Send(SOCKET_CHANNEL_1, HEX_FORMAT, (uint8_t*)send_temp ,strlen(send_temp));
		if(ret)
		{
			ret =  TCP_Read_DATA(15);
			
			if(ret)
			{

				return TRUE;
			}
		}
	}
	return ret;*/

	//return TRUE;
}
unsigned char actCall(uint8_t index)
{
/*
	char send_temp[50] ={0};
	bool ret = FALSE;
	uint8_t check =0;
	uint8_t errType =0;
	uint8_t i =0;


	errType = alarmCheck(index);


	
	check = (jqid % 10) + 7 + (errType % 10) + 0 ;
	check =check%10; 
	esimSumCheck = check;
		
	sprintf(send_temp , "hm+%d+17+%d+0+%d+end",jqid,errType,check);

	for(i = 0; i < ESIM_TRANS_CNT; i++)
	{
		ret = TCP_MIPSTRS_Send(SOCKET_CHANNEL_1, HEX_FORMAT, (uint8_t*)send_temp ,strlen(send_temp));
		if(ret)
		{
			ret =  TCP_Read_DATA(17);
			
			if(ret)
			{

				return TRUE;
			}		
		}
	}
	
	return ret;*/
}
	
unsigned char actAcousto(uint8_t index,uint8_t state)
{
/*
	char send_temp[50] ={0};
	bool ret = FALSE;
	uint8_t check =0;
	uint8_t errType =0;
	uint8_t i =0;

	
	errType = alarmCheck(index);




	check = (jqid % 10) + 6 + (errType % 10) + 0 + (state %10);
	check =check%10; 
	esimSumCheck = check;
	sprintf(send_temp , "hm+%d+16+%d+0+%d+%d+end" , jqid,errType,state,check);
	

	for(i = 0; i < ESIM_TRANS_CNT; i++)
	{
		ret = TCP_MIPSTRS_Send(SOCKET_CHANNEL_1, HEX_FORMAT, (uint8_t*)send_temp ,strlen(send_temp));
		if(ret)
		{
			ret =  TCP_Read_DATA(16);
			if(ret)
			{

				return TRUE;
			}	   
		}
	}
	
	return ret;*/
	//return TRUE;
}
	
	
unsigned char actPush(uint8_t index,uint8_t state)
{
/*
	char send_temp[50] ={0};
	bool ret = FALSE;
	uint8_t check =0;
	uint8_t errType =0;
	uint8_t i =0;
	char rtcTime[20] ={0};

	
	errType = alarmCheck(index);


	
	sprintf(rtcTime , "%4d%2d%2d%2d%2d%2d",curRtc.year,curRtc.month,curRtc.day,curRtc.hour,curRtc.min,curRtc.sec);
	for(i = 0; i < 14;i++)
	{
		if(rtcTime[i] == 0x20)
		{
			rtcTime[i] = '0';
		}
	}
	
	check = (jqid % 10) + 9 + (errType % 10) + 0+(state %10) +(rtcTime[13]-'0')%10;
	check =check%10;
	esimSumCheck = check;
	sprintf(send_temp , "hm+%d+19+%d+0+%d+%s+%d+end" , jqid,errType,state,rtcTime,check);
	

	
	for(i = 0; i < ESIM_TRANS_CNT; i++)
	{
		ret = TCP_MIPSTRS_Send(SOCKET_CHANNEL_1, HEX_FORMAT, (uint8_t*)send_temp ,strlen(send_temp));
		if(ret)
		{
			ret =  TCP_Read_DATA(19);
			if(ret)
			{

				return TRUE;
			}	
		}
	}	
	
	return ret;*/
}
	
	
bool uploadTemp(void)
{
/*
	char send_temp[50] ={0};
	bool ret = TRUE;
	//TEST////
	int i=0;
	
	char rtcTime[20] ={0};
	uint8_t check =0;
	uint16_t temp = 0;
	uint8_t sensorNum = 0;

	//curRtc.year= 2018;curRtc.month=02;curRtc.day=27;curRtc.hour = 12;curRtc.min = 0;curRtc.sec = 0;

	for(sensorNum = 0;sensorNum < sensorCfg.tempSensorNum;sensorNum++)
	{
		sprintf(rtcTime , "%4d%2d%2d%2d%2d%2d",curRtc.year,curRtc.month,curRtc.day,curRtc.hour,curRtc.min,curRtc.sec);
		for(i = 0; i < 14;i++)
		{
			if(rtcTime[i] == 0x20)
			{
				rtcTime[i] = '0';
			}
		}
		if(((Dev_info[TEMP1-1 + sensorNum].Dev_sta == MB_MRE_NO_ERR) || (Dev_info[TEMP1-1+ sensorNum].Dev_sta == 0X10)) && (tempData[sensorNum] > 0) )
		{
			//temp = Dev_info[TEMP1-1+ sensorNum].Data1/10;
			temp = tempData[sensorNum]/10;	

		
			check = (jqid % 10) + 7 + sensorNum + 1 + (temp % 10)+ (rtcTime[13]-'0')%10;
			check = check % 10;
			sprintf(send_temp , "hm+%d+7+%d+%d+%s+%d+end",jqid,(sensorNum + 1),temp,rtcTime,check);
			
	
			IWDG_Feed();
			for(i = 0; i < ESIM_TRANS_CNT; i++)
			{
				ret = TCP_MIPSTRS_Send(SOCKET_CHANNEL_1, HEX_FORMAT, (uint8_t*)send_temp ,strlen(send_temp));
				if(ret)
				{
					ret =  TCP_Read_DATA(7);
					if(ret)
						break;
				}
				if(i == (ESIM_TRANS_CNT - 1))
				{
					if(!ret)
					{
						return FALSE;
					}
				}
			}
		}
		else
		{
		
		}
	}
		
	return ret;*/
}
	
bool uploadHumidity(void)
{
/*
	char send_temp[50] ={0};
	bool ret = TRUE;
	uint8_t i=0;
	//int  sensorId =1;
	char rtcTime[20] ={0};
	uint8_t check =0;
	uint16_t temp = 0;
	uint8_t sensorNum = 0;
	IWDG_Feed();

	for(sensorNum = 0;sensorNum < sensorCfg.humiSensorNum;sensorNum++)
	{
		sprintf(rtcTime , "%4d%2d%2d%2d%2d%2d",curRtc.year,curRtc.month,curRtc.day,curRtc.hour,curRtc.min,curRtc.sec);
		for(i = 0; i < 14;i++)
		{
			if(rtcTime[i] == 0x20)
			{
				rtcTime[i] = '0';
			}
		}
				
		if(((Dev_info[HUMI1-1 + sensorNum].Dev_sta == MB_MRE_NO_ERR) || (Dev_info[HUMI1-1+ sensorNum].Dev_sta == 0X10)) && (humiData[sensorNum] > 0) )
		{
			//temp = Dev_info[HUMI1-1+ sensorNum].Data1/10;
			temp = humiData[sensorNum]/10;
						
			check = (jqid % 10) + 8 + sensorNum + 1 + (temp % 10)+ (rtcTime[13]-'0')%10;
			check = check % 10;
			sprintf(send_temp , "hm+%d+8+%d+%d+%s+%d+end",jqid,(sensorNum + 1),temp,rtcTime,check);

			IWDG_Feed();
			for(i = 0; i < ESIM_TRANS_CNT; i++)
			{
				ret = TCP_MIPSTRS_Send(SOCKET_CHANNEL_1, HEX_FORMAT, (uint8_t*)send_temp ,strlen(send_temp));
				if(ret)
				{
					ret =  TCP_Read_DATA(8);
					if(ret)
					{
					
						break;
					}
						
				}
				if(i == (ESIM_TRANS_CNT - 1))
				{
					if(!ret)
					{
						return FALSE;
					}
				}
			}
		}
		else
		{
		}

	}
	
	return ret;*/
}

uint8_t Sensor_GetTypeId(uint8_t sensorId,uint8_t *type, uint8_t *id)
{
	uint8_t i = 0;

	switch(sensorId)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			*type = SENSOR_TEMPER;
			*id = sensorId;
			break;
			
		case 6:
			*type = SENSOR_HUIMI;
			*id = 0;
			break;		
		case 7:
			*type = SENSOR_HUIMI;
			*id = 1;
			break;		
		case 8:
			*type = SENSOR_HUIMI;
			*id = 2;
			break;	
			
		case 9:
			*type = SENSOR_OUT_DOOR_TEMPER;
			*id = 0;
			break;

		case 10:
			*type = SENSOR_NH3;
			*id = 0;
			break;
		case 11:
			*type = SENSOR_NH3;
			*id = 1;
			break;
			
			
			
		case 12:
			*type = SENSOR_WIEGHT;
			*id = 0;
			break;
		case 13:
			*type = SENSOR_WIEGHT;
			*id = 1;
			break;

		case 14:
			*type = SENSOR_WIEGHT;
			*id = 2;
			break;

		case 15:
			*type = SENSOR_WIEGHT;
			*id = 3;
			break;

		case 16:
			*type = SENSOR_WATER_PRESSURE;
			*id = 0;
			break;
		case 17:
			*type = SENSOR_WATER_PRESSURE;
			*id = 1;
			break;		

		case 18:
			*type = SENSOR_PH;
			*id = 0;
			break;
		case 19:
			*type = SENSOR_CO2;
			*id = 0;
			break;
		case 20:
			*type = SENSOR_AIR_PRESSURE;
			*id = 0;
			break;
		case 21:
			*type = SENSOR_LUX;
			*id = 0;
			break;
		/*
		case 22:
			*type = SENSOR_WATER_F;
			*id = 1;
			break;
		*/
		case 23:
			*type = SENSOR_POWER_METER;
			*id = 0;
			break;

		case 24:
			*type = SENSOR_WATER_METER;
			*id = 0;
			break;
		default:
			break;
	}
}


void Commu_Pack_SensorData(uint8_t *pDate, uint16_t *nLen)
{
	uint16_t len = 0;
	uint8_t i = 0;
	uint8_t type, id;
	uint32_t temp = 0;
	
	struct SENSOR_DATA_S sensorTemp = {0};


	for(i = 0;i < DEV_NUM;i++)
	{
		if(Dev_info[i].Dev_EN)
		{
			Sensor_GetTypeId(i, &type, &id);
			sensorTemp.type = type;
			sensorTemp.id = id;
			if(Dev_info[i].ID == EP_METER || Dev_info[i].ID == WATER_METER)
			{
				temp = (Dev_info[i].Data1 << 16) + Dev_info[i].Data2;
				sensorTemp.value = BigtoLittle32(temp);
			}
			else
			{
				temp = Dev_info[i].Data1;
				sensorTemp.value = BigtoLittle32(temp);
			}
			
			sensorTemp.sta = Dev_info[i].Dev_sta;
			memcpy(pDate, (uint8_t *)&sensorTemp, sizeof(sensorTemp));
			pDate += sizeof(struct SENSOR_DATA_S);
			len+=  sizeof(struct SENSOR_DATA_S);
		}
	}
	*nLen = len;
}
void Commu_Pack_Cfg(uint8_t cmd, uint8_t *pDate, uint16_t *nLen)
{
	uint16_t len = 0;
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t type, id;
	uint32_t temp = 0;

	if(cmd == COMMU_CMD_SYS_TARGET_TEMPER)
	{
		TARGET_TEMP_INFO_T tempCtlr_s = m_Controler_Ctrl_Cfg_t.targetTemperCfg;
		for(i = 0;i < TARGET_TEMP_NUM;i++)
		{
			tempCtlr_s.targetTempArry[i].targetTemp = BigtoLittle16(tempCtlr_s.targetTempArry[i].targetTemp);
			tempCtlr_s.targetTempArry[i].minAirRuntime = BigtoLittle16(tempCtlr_s.targetTempArry[i].minAirRuntime);
		}
		tempCtlr_s.minAirSupport_cycleTime = BigtoLittle16(tempCtlr_s.minAirSupport_cycleTime);
		
		memcpy(pDate, (uint8_t *)&tempCtlr_s, sizeof(tempCtlr_s));
		*nLen = sizeof(tempCtlr_s);
	}
	else if(cmd == COMMU_CMD_SYS_AIR_CTRL)
	{
		AirCtrlCfg_T tempCtlr_s  = m_Controler_Ctrl_Cfg_t.airCtrlCfg;
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
		memcpy(pDate, (uint8_t *)&tempCtlr_s, sizeof(tempCtlr_s));
		*nLen = sizeof(tempCtlr_s);
	}
	else if(cmd == COMMU_CMD_SYS_HEATER_CTRL)
	{
		HEATER_CTRL_CFG_T tempCtlr_s  = m_Controler_Ctrl_Cfg_t.heaterCtrlCfg;
		
		memcpy(pDate, (uint8_t *)&tempCtlr_s, sizeof(tempCtlr_s));
		*nLen = sizeof(tempCtlr_s);
	}
	else if(cmd == COMMU_CMD_SYS_COLDPAD_CTRL)
	{
		HUMI_CTRL_CFG_T tempCtlr_s  = m_Controler_Ctrl_Cfg_t.humiCtrlCfg;
		tempCtlr_s.enTime = BigtoLittle16(tempCtlr_s.enTime);
		tempCtlr_s.disenTime = BigtoLittle16(tempCtlr_s.disenTime);
		memcpy(pDate, (uint8_t *)&tempCtlr_s, sizeof(tempCtlr_s));
		*nLen = sizeof(tempCtlr_s);
	}
	else if(cmd == COMMU_CMD_SYS_COLDPAD_CTRL)
	{
		LIGHT_CTRL_CFG_T tempCtlr_s  = m_Controler_Ctrl_Cfg_t.lightCtrlCfg;
		tempCtlr_s.enTime = BigtoLittle16(tempCtlr_s.enTime);
		tempCtlr_s.disenTime = BigtoLittle16(tempCtlr_s.disenTime);
		tempCtlr_s.targetLux = BigtoLittle16(tempCtlr_s.targetLux);
		memcpy(pDate, (uint8_t *)&tempCtlr_s, sizeof(tempCtlr_s));
		*nLen = sizeof(tempCtlr_s);
	}
	else if(cmd == COMMU_CMD_SYS_LIGHT_CTRL)
	{
		LIGHT_CTRL_CFG_T tempCtlr_s  = m_Controler_Ctrl_Cfg_t.lightCtrlCfg;
		tempCtlr_s.enTime = BigtoLittle16(tempCtlr_s.enTime);
		tempCtlr_s.disenTime = BigtoLittle16(tempCtlr_s.disenTime);
		tempCtlr_s.targetLux = BigtoLittle16(tempCtlr_s.targetLux);
		memcpy(pDate, (uint8_t *)&tempCtlr_s, sizeof(tempCtlr_s));
		*nLen = sizeof(tempCtlr_s);
	}
	else if(cmd == COMMU_CMD_SYS_ALARM_CFG)
	{
		CONTROLER_ALARM_PROC_T tempCtrl_s = {0};
		memcpy((uint8_t *)&tempCtrl_s, (uint8_t *)&m_Controler_Alarm_Cfg_t, sizeof(tempCtrl_s));
		*nLen = sizeof(tempCtrl_s);
	}
	else if(cmd == COMMU_CMD_SYS_DEV_EN_CFG)
	{
		CONTROLER_DEV_CFG_T tempCtrl_s = {0};
		memcpy((uint8_t *)&tempCtrl_s, (uint8_t *)&m_Controler_Dev_Cfg_t, sizeof(tempCtrl_s));
		*nLen = sizeof(tempCtrl_s);
	}
}
void Commu_Pack_AlarmInfo(struct ALARM_INFO_S info, uint8_t *pDate, uint16_t *nLen)
{
	uint16_t len = 0;
	uint8_t i = 0;

	memcpy(pDate, (uint8_t *)&info, sizeof(struct ALARM_INFO_S));
	pDate += sizeof(struct ALARM_INFO_S);
	len+=  sizeof(struct ALARM_INFO_S);
	*nLen = len;
}

uint8_t Commu_test(uint8_t cmd, uint8_t *pDate, uint16_t nLen)
{
	uint8_t sendBuf[300] = {0};
	uint8_t *ptr = NULL;
	uint16_t crc = 0;
	uint8_t i = 0;
	uint8_t ret = 0;
	
	if(nLen == 0)
		return;

	ptr = &sendBuf[0];
	*ptr = 0xa5;
	ptr++;
	memcpy(ptr, jqidString, JQID_LEN_MAX);
	ptr += JQID_LEN_MAX;
	*ptr = cmd;
	ptr++;
	*ptr = (nLen+14) >> 8;
	ptr++;
	*ptr = (nLen + 14) & 0xff;
	ptr++;
	memcpy(ptr, pDate, nLen);
	ptr+= nLen;
	
	crc = usMBCRC16(sendBuf, nLen +12);
	*ptr = crc & 0xff;

	ptr++;
	*ptr = crc >> 8;

	for(i = 0;i < 3;i++)
	{
		ret = LTE_SendDataToWeb(sendBuf, nLen+14);
		if(ret)
		{
			//如果是上传温度，不做返回校验
			if(cmd == COMMU_CMD_UPLOAD_SENSOR_DATE)
				return TRUE;
			//发送成功后,检测返回中是否有目标数据，最多等待3S
			ret = findByte(0XA5, 30);
			if(ret)
			{
				ret = Commu_Recv_Proc(cmd);
				if(ret)
					return TRUE;
			}
			else 
				return FALSE;
		}
		else
			return FALSE;
	}

	return FALSE;
}
/*
这个函数单独作为报警处理
*/
uint8_t  Commu_Alarm_Proc(void)
{
	uint16_t len = 0;
	uint8_t i = 0;
	uint8_t ret = 0; 
	uint8_t time[6] = {0};
	struct ALARM_INFO_S alarmInfo = {0};

	

	for(i = ALARM_TYPE_START;i < ALARM_TYPE_MAX;i++)
	{
		if(m_Controler_Alarm_Cfg_t.devAlarmCfg[i].en)
		{
			if(m_Alarm_Proc_t.alarmPorc[i].alarmSta == ALARM_STA_OCCUR)
			{
				if(m_Alarm_Proc_t.alarmPorc[i].alarmActSta == ALARM_ACT_PENDING)
				{
					alarmInfo.type = i;
					alarmInfo.devType = 0;
					alarmInfo.id = 0;
					alarmInfo.sta = m_Alarm_Proc_t.alarmPorc[i].alarmSta == ALARM_STA_OCCUR ? 1:2;
					memcpy(alarmInfo.time ,m_Alarm_Proc_t.alarmPorc[i].time, 6);
					ret = Commu_test(COMMU_CMD_UPLOAD_ALARM, (uint8_t *)&alarmInfo, sizeof(struct ALARM_INFO_S));
					if(ret)
					{
						m_Alarm_Proc_t.alarmPorc[i].alarmActSta = 	ALARM_ACT_DONE;
					}
					else 
						return FALSE;
				}
			}
			else
			{
				if(m_Alarm_Proc_t.alarmPorc[i].alarmActSta == ALARM_ACT_DONE)
				{
					alarmInfo.type = i;
					alarmInfo.devType = 0;
					alarmInfo.id = 0;
					alarmInfo.sta = m_Alarm_Proc_t.alarmPorc[i].alarmSta == ALARM_STA_OCCUR ? 1:2;
					memcpy(alarmInfo.time ,m_Alarm_Proc_t.alarmPorc[i].time, 6);
					ret = Commu_test(COMMU_CMD_UPLOAD_ALARM, (uint8_t *)&alarmInfo, sizeof(struct ALARM_INFO_S));
					if(ret)
					{
						m_Alarm_Proc_t.alarmPorc[i].alarmActSta = 	ALARM_ACT_PENDING;
					}
					else
						return FALSE;
				}
			}
		}
	}

	return TRUE;
}


uint8_t CommuPack_Pack(void)
{
	uint8_t dateBuf[300] = {0};
	uint8_t sendBuf[320] = {0};
	uint16_t nLen = 0;
	uint8_t ret = FALSE;
	uint8_t i = 0;
	//uint8_t commuFlag = 0,commuCmd = 0;



	if(m_CommuEn_T.Commu_Upload_Sensor)
	{
		Commu_Pack_SensorData(dateBuf,&nLen);
		ret = Commu_test(COMMU_CMD_UPLOAD_SENSOR_DATE, dateBuf, nLen);
		if(ret)
		{
			m_CommuEn_T.Commu_Upload_Sensor = FALSE;
			//return TRUE;
		}
		else
			return FALSE;
		
	}
	if(m_CommuEn_T.Commu_Sys_TargetCfg)
	{
		Commu_Pack_Cfg(COMMU_CMD_SYS_TARGET_TEMPER, dateBuf, &nLen);
		ret = Commu_test(COMMU_CMD_SYS_TARGET_TEMPER, dateBuf, nLen);
		if(ret)
		{
			m_CommuEn_T.Commu_Sys_TargetCfg = FALSE;
			//return TRUE;
		}
		else
			return FALSE;
	}
	if(m_CommuEn_T.Commu_Sys_AirCtrlCfg)
	{
		Commu_Pack_Cfg(COMMU_CMD_SYS_AIR_CTRL, dateBuf, &nLen);
		ret = Commu_test(COMMU_CMD_SYS_AIR_CTRL, dateBuf, nLen);
		if(ret)
		{
			m_CommuEn_T.Commu_Sys_AirCtrlCfg = FALSE;
			//return TRUE;
		}
		else
			return FALSE;
	}
	if(m_CommuEn_T.Commu_Sys_HeaterCfg)
	{
		Commu_Pack_Cfg(COMMU_CMD_SYS_HEATER_CTRL, dateBuf, &nLen);
		ret = Commu_test(COMMU_CMD_SYS_HEATER_CTRL, dateBuf, nLen);
		if(ret)
		{
			m_CommuEn_T.Commu_Sys_HeaterCfg = FALSE;
			//return TRUE;
		}
		else
			return FALSE;
	}
	if(m_CommuEn_T.Commu_Sys_WaterPadCfg)
	{
		Commu_Pack_Cfg(COMMU_CMD_SYS_COLDPAD_CTRL, dateBuf, &nLen);
		ret = Commu_test(COMMU_CMD_SYS_COLDPAD_CTRL, dateBuf, nLen);
		if(ret)
		{
			m_CommuEn_T.Commu_Sys_WaterPadCfg = FALSE;
			//return TRUE;
		}
		else
			return FALSE;
	}
	if(m_CommuEn_T.Commu_Sys_Light_Cfg)
	{
		Commu_Pack_Cfg(COMMU_CMD_SYS_LIGHT_CTRL, dateBuf, &nLen);
		ret = Commu_test(COMMU_CMD_SYS_LIGHT_CTRL, dateBuf, nLen);
		if(ret)
		{
			m_CommuEn_T.Commu_Sys_Light_Cfg = FALSE;
			//return TRUE;
		}
		else
			return FALSE;
	}

	return TRUE;
}


void thread_entry_eSim(void* parameter)
{
	int state = 0;
	static uint8_t commuFailCnt = 0;
	

	//重启模组
	//LTE_Restart();
	//CommuPack_Pack();
	LTE_Reboot();
	while(1)
	{
		
		IWDG_Feed();
		rt_thread_delay(100);

		if(FLAGS_LTE_GET_RSSI_10S)
		{
			FLAGS_LTE_GET_RSSI_10S = FALSE;
			LTE_Get_RSSI(&m_LTE_Rssi);
		}

		
		if(FLAGS_LTE_GET_CON_STA_10S)
		{
			FLAGS_LTE_GET_CON_STA_10S = FALSE;
			state = LTE_Get_ConnectSta();
			if(state)
			{
				commuFailCnt = 0;
			}
			else
			{
				commuFailCnt++;
			}
		}
		
		if(!m_Is_LTE_Connect)
		{
			state = LTE_Init();
			if(state)
			{
				m_Is_LTE_Connect = TRUE;
				commuFailCnt = 0;
			}
			else
			{
				commuFailCnt++;
			}
		}
		if(commuFailCnt >= 3)
		{
			LTE_Close_Net();
			goto _restartRun;
		}
		if(commuFailCnt >= 5)
		{
			m_Is_LTE_Connect = FALSE;
			LTE_Reboot();
			commuFailCnt = 0;
			goto _restartRun;
		}
		
		if(m_Is_LTE_Connect)
		{
			
			if(!Commu_Alarm_Proc())
			{
				//m_Is_LTE_Connect = FALSE;
				commuFailCnt++;
				goto 
					_restartRun;
				
			}
			else 
			{
				commuFailCnt = 0;
			}
			
			if(!CommuPack_Pack())
			{
				//m_Is_LTE_Connect = FALSE;
				commuFailCnt++;
				goto 
					_restartRun;
			}
			else
			{
				commuFailCnt = 0;
			}	
		}
		
		/*
		if(commuFailCnt >= 3)
		{
			commuFailCnt = 0;
			m_Is_LTE_Connect = FALSE;
			
			
		}

		if(FLAGS_LTE_GET_RSSI_10S)
		{
			FLAGS_LTE_GET_RSSI_10S = FALSE;
			LTE_Get_RSSI(&m_LTE_Rssi);
		}
		
		if(!m_Is_LTE_Connect) 
		{
			LTE_Restart();
		}
		if(!m_Is_LTE_Connect)
		{
			state = LTE_Init();
			if(state)
			{
				m_Is_LTE_Connect = TRUE;
			}
			else
			{
				m_Is_LTE_Connect = FALSE;
				goto _restartRun;
			}
		}
		
		
		
		if(m_Is_LTE_Connect)
		{
			
			if(!Commu_Alarm_Proc())
			{
				//m_Is_LTE_Connect = FALSE;
				commuFailCnt++;
				goto 
					_restartRun;
				
			}
			else 
			{
				commuFailCnt = 0;
			}
			
			if(!CommuPack_Pack())
			{
				//m_Is_LTE_Connect = FALSE;
				commuFailCnt++;
								goto 
					_restartRun;
			}
			else
			{
				commuFailCnt = 0;
			}
			
		}
		*/
		/*
		if(FLAGS_LTE_GET_CONNECT_STA)
		{
			FLAGS_LTE_GET_CONNECT_STA = FALSE;
			m_Is_Tcp_ConnectOk = LTE_Get_ConnectSta();
		}
		*/
		
		_restartRun:
			rt_thread_delay(100);
	}
	
}


/*
比较理想的话，使用modbusRTU，驱动板作为从机。但是时间比较紧这里就不再做地址分配了
自定协议如下：
驱动板接收：
head ADDR cmd LEH_H LEN_L OUT1 OUT2 OUT3 OUT4 DA1H DA1L DA2H DA2L DA3H DA3L DA4H DA4L CRCH CRCL tail
head:0XAA  tail 0xbb
ADDR 默认为1，CMD默认为0，LEN为数据帧长度CRCH，CRCL为CRC字段前所有数据的CRC校验

驱动板发送：
head ADDR cmd LEN_H LEN_L IN1 IN2 IN3 IN4 AD1H AD1L AD2H AD2L AD3H AD3L AD4H AD4L CRCH CRCL tail
*/
uint8_t UART4_Recv_Proc(void)
{
	uint16_t temp = 0;
	uint16_t len = 0;
	uint16_t crc = 0;
	uint8_t i = 0;
	uint8_t j = 0;

	for(i = 0;i <UART4_RecvLen;i++)
	{
		if(UART4_RecvBuf[i] == 0XAA)
		{
			break;
		}
	}
	if(i == UART4_RecvLen)
		goto __exit;
	
	if(UART4_RecvLen > 8)
	{
		if(UART4_RecvBuf[i] == 0xaa)
		{
			if(UART4_RecvBuf[i+1] == 1)
			{
				if(UART4_RecvBuf[i+2] == 0)
				{
					len = (UART4_RecvBuf[i+3] << 8) + UART4_RecvBuf[i+4];
					if(len == 19)
					{
						crc = usMBCRC16(UART4_RecvBuf + i,17);
						temp = (UART4_RecvBuf[i + 16] << 8) + UART4_RecvBuf[i + 17];
						if(crc == temp)
						{	
							inputBits = (UART4_RecvBuf[i + 5] << 24) + (UART4_RecvBuf[i + 6] << 16) + (UART4_RecvBuf[i + 7] << 8) + UART4_RecvBuf[i + 8];
							for(j = 0;j < 4;j++)
							{
								adcValueIn[j] = (UART4_RecvBuf[9+i+2*j] << 8) + UART4_RecvBuf[10+i+2*j];
							}
							UART4_EmptyBuf();
							return 1;
						}
					}
				}
			}
		}
	}

	__exit:
	UART4_EmptyBuf();
	
	return 0;

}
void thread_entry_UART4(void* parameter)
{	
	uint8_t sendBuf[19] = {0xaa,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	uint16_t crc;
	uint8_t i = 0;
	
	while(1)
	{
		if(FLAGS_DRIVE_1S)
		{
			
			FLAGS_DRIVE_1S = FALSE;
			sendBuf[4] = 19;
			crc = usMBCRC16(sendBuf,17);
			sendBuf[17] = crc & 0xff;
			sendBuf[18] = crc >> 8;
			UART_485_EN_SEND;
			outputBits = m_DriveBoard_Data_t.sendData.outBits;
			memcpy(dacValueOut, m_DriveBoard_Data_t.sendData.dacOutValue, 4);
			sendBuf[5] = outputBits >> 24;
			sendBuf[6] = outputBits >> 16;
			sendBuf[7] = outputBits >> 8;
			sendBuf[8] = outputBits & 0xff;
			for(i = 0;i <4;i++)
			{
				sendBuf[9 + 2*i] = dacValueOut[i] >> 8;
				sendBuf[10 + 2*i] = dacValueOut[i] & 0XFF;
			}
			
			/*
			UART_485_EN_SEND;
			sendBuf[0]= curDayNum;
			sendBuf[1] = curDayLevel;
			sendBuf[2] = curTargetTemp >> 8;
			sendBuf[3] = curTargetTemp;
			sendBuf[4] = (curAvgTemp /10) >> 8;
			sendBuf[5] = (curAvgTemp /10);
			
			sendBuf[6] = curAirLevel;
			sendBuf[7]= m_DriveBoard_Data_t.sendData.dacOutValue[0] >>8;
			sendBuf[8]= m_DriveBoard_Data_t.sendData.dacOutValue[0];
			sendBuf[9] = m_DriveBoard_Data_t.sendData.outBits >> 8;
			sendBuf[10] =  m_DriveBoard_Data_t.sendData.outBits;
			*/
			
			UART4_SendDate(sendBuf, 20);
			UART_485_EN_RECV;
			rt_thread_delay(300);
			UART4_Recv_Proc();
		}
		rt_thread_delay(500);
	}
}


#define   CAN_LEN        8
#define   CAN_ERRDEL     5

void thread_entry_CANCOMM(void* parameter){   // CAN_MES
  //uint8_t i,j,k;
  //uint8_t Data[10] = {0};

  
 // CAN1_Init();
 //pre = 4, 250K
 //pre = 8, 125K
 canModeInit(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,8,CAN_Mode_Normal);
  rt_thread_delay(2000);


	//can标准帧数据8字节。
	//主机命令：4个字节代表4个DA输出，剩余4个字节可表示32个开关输出
	//从机命令
  	while(1)
	{
		rt_thread_delay(100);
  	}

		/*
	while(1)
	{
		//canSendMsg(buf, 8);
		 rt_thread_delay(2000);
	}	
	*/
	/*
  while(1)
  {
		
		sendSensorData(TEMP1);
		sendSensorData(TEMP2);
		sendSensorData(TEMP3);
		sendSensorData(HUMI1);
		
		
		//CAN_Recei_Data(CAN_LEN,Data,0x01,WRITE_JDQ);
			
			//CAN_Recei_Data(CAN_LEN,Data,0x01,READ_JDQ);
			
			
			//CAN_Recei_Data(CAN_LEN,Data,0x01,RE_JDQ);
		
		//LinMemCopy(can_data_buffer, Data,CAN_LEN);
  }
  */

  while(1){
    rt_thread_delay(400);
  }
}

#define TO_BASE                 50
#define TO_COUNT_200MS          (200ul  / TO_BASE)

#define TO_COUNT_500MS          (500ul  / TO_BASE)
#define TO_COUNT_1000MS         (1000ul  / TO_BASE)      // 200
#define TO_COUNT_3000MS         (3000ul  / TO_BASE)      // 200
#define TO_COUNT_5000MS         (5000ul  / TO_BASE) 


static void Timer50MS_OUT(void* parameter){
	static uint8_t	runLedCnt = 0;	
	static uint8_t	refreshLcd = 0;
	//static uint8_t  getLcdIdCnt = 0;
	static uint8_t	adcGetCnt = 0;

	EX_WDT_FEED_TOG();
	
  RS232Send_Cout++;
  if(RS232Send_Cout>=TO_COUNT_500MS){  
    //FLAGS0_SEND500MS = 1;
	//FLAGS3_REFRESH_LCD = 1;
    //FLAGS2_CANSEND =1;
    RS232Send_Cout=0;
  }

	if(refreshLcd++ >= TO_COUNT_5000MS)
	{
		//FLAGS3_REFRESH_LCD_2 = 1;
		refreshLcd = 0;
	}

  if(runLedCnt++ >= TO_COUNT_500MS)
  {
	runLedCnt = 0;
	RUN_LED_BLINK();
  }
	/*
  if(getLcdIdCnt++ >= TO_COUNT_1000MS)
  {
		FLAGS3_GET_LCD_ID_SEND = 1;
		getLcdIdCnt = 0;
  }
  */
	if(adcGetCnt++ >= 20)
	{
		//FLAGS0_ADC = 1;
		adcGetCnt = 0;
	}
}
#define TO_BASE2                1000
#define TO_RS232ERR_5S          (4000ul   / TO_BASE2)
#define TO_CANERR_5S            (4000ul   / TO_BASE2)
#define TO_TIM_5S               (2000ul   / TO_BASE2)
#define TO_ADC_4S               (1000ul   / TO_BASE2) 
#define TO_LETST_10S            (8000ul   / TO_BASE2) 
#define TO_REF_1S               (1000ul   / TO_BASE2)
#define TO_REF_2S               (2000ul   / TO_BASE2) 
#define TO_REF_20S              (20000ul  / TO_BASE2)
#define TO_FANDR_10S            (5000ul   / TO_BASE2)
#define TO_REST_15MIN           (900000ul / TO_BASE2)   // 15*60  
#define TO_START_5S             (5000ul   / TO_BASE2)
#define TO_DN_2MIN              (120000ul / TO_BASE2)
#define TO_REST_1MIN            (60000ul  / TO_BASE2)
#define TO_SETMOTOR_15S         (15000ul  / TO_BASE2)
#define	TO_TIMER_10S			(10000ul  / TO_BASE2)
#define	TO_5S					(5000ul   / TO_BASE2)
#define	TO_10S					(10000ul   / TO_BASE2)
#define	TO_20S					(20000ul   / TO_BASE2)
#define	TO_30S					(30000ul   / TO_BASE2)
#define TO_1MIN           		(60000ul   / TO_BASE2)
#define TO_2MIN           		(120000ul   / TO_BASE2)
#define TO_3MIN           		(180000ul   / TO_BASE2)
#define TO_5MIN           		(300000ul   / TO_BASE2)

void getBatChargeSta(void)
{
	//CHARGEING		PB7		//充电状态。0：未充电。		1：充电中
	//CHARG_DONE		PB6		//充电完成。0：充电中。1：完成
	//PLUG_DE			PB5		//电池在位检测，设为上拉输入。1：在外	

	uint8_t isCharge = 0;
	uint8_t isChargeOver = 0;
	uint8_t isOn = 0;
	uint8_t isPowerOff;

	
	isOn = IS_BAT_ON;
	isCharge = IS_BAT_CHARGING;
	isChargeOver = IS_BAT_CHARGE_DONE;
	isPowerOff = IS_POWER_OFF;

	m_Is_Charging = IS_BAT_CHARGING;
}

static void Timer1S_OUT(void* parameter){
	
	static uint16_t	getDayNumCnt = 0;
	static uint8_t	getLcdPicId = 0;
	static uint8_t	esimSupendFlag = 0;
	static uint8_t	ehDataRefresh = 0;

	

	static uint8_t	errConfirmCnt[alarmEventMax] = {0};

	static uint8_t testCnt = 0;
	uint8_t i = 0;

	static uint8_t cnt_EP_Rd_10S = 0;//电表定时读取
	static uint8_t cnt_ADC_Rd_3S= 0;	//ADC定时读取
	static uint8_t cnt_Sensor_Upload_30S = 0;	//传感器数据上传
	static uint8_t cnt_CtrlCfg_Upload_30S = 0;	//控制配置上传
	static uint8_t cnt_CtrlEn_Upload_30S = 0;	//外设数量上传

	static uint8_t cnt_LCD_Cfg_Refresh_5S = 0;	//液晶屏配置刷新
	static uint8_t cnt_LCD_Date_Refresh_2S = 0;

	static uint8_t cnt_LTE_Get_RSSI_10S = 0;	//获取RSSI
	static uint8_t cnt_LTE_Get_CONNECT_STA_10S = 0;	//获取连接状态
	
	static COMMU_EN_T cnt_Commu = {0};
	static uint8_t cnt_Err_Confirm[ALARM_TYPE_MAX] = {0};


	getBatChargeSta();
	Ds1302_Readtime();

	//外设运行计数
	for(i = 0;i < FRE_FAN_NUM_MAX;i++)
	{
		m_Equipment_RunInfo_t.freFanRunInfo[i].runCount++;
		m_Equipment_RunInfo_t.freFanRunInfo[i].stopCount++;
	}
	for(i = 0;i < FAN_NUM_MAX;i++)
	{
		m_Equipment_RunInfo_t.fanDevRunInfo[i].runCount++;
		m_Equipment_RunInfo_t.fanDevRunInfo[i].stopCount++;
	}
	m_Equipment_RunInfo_t.heaterRunInfo.runCount++;
	m_Equipment_RunInfo_t.heaterRunInfo.stopCount++;
	m_Equipment_RunInfo_t.coldPadRunInfo.runCount++;
	m_Equipment_RunInfo_t.coldPadRunInfo.stopCount++;
	

	//故障报警计数
	for(i = 0;i < ALARM_TYPE_MAX;i++)
	{
		m_Alarm_Sta_t.alarmSta[i].confirmCnt++;
	}
	
	FLAGS_MODBUS1S = TRUE;
	FLAGS_DRIVE_1S = TRUE;
	if(cnt_LCD_Date_Refresh_2S++ > 2)
	{
		cnt_LCD_Date_Refresh_2S = 0;
		FLAGS_LCD_EH_REF_1S = TRUE;
	}
	
	if(cnt_LCD_Cfg_Refresh_5S++ > 10)
	{
		cnt_LCD_Cfg_Refresh_5S = 0;
		FLAGS_LCD_CFG_REF_5S = TRUE;
	}
	
	 //电表读取
	if(cnt_EP_Rd_10S++ >= 10)
	{
		cnt_EP_Rd_10S = 0;
		FLAGS_EP_RD_EN_10S = TRUE;
	}

	//ADC读取
	if(cnt_ADC_Rd_3S++ >= 10)
	{
		cnt_ADC_Rd_3S = 0;
		FLAGS_ADC_RD_3S = TRUE;
	}
	//传感器上传频率
	if(cnt_Sensor_Upload_30S++ >= 10)
	{
		cnt_Sensor_Upload_30S = 0;
		m_CommuEn_T.Commu_Upload_Sensor = TRUE;
	}
	//配置同步
	if(cnt_CtrlCfg_Upload_30S++ >= 30)
	{
		cnt_CtrlCfg_Upload_30S = 0;
		//m_CommuEn_T.Commu_Upload_DevEn =  TRUE;
		//m_CommuEn_T.Commu_Sys_AlarmCfg= TRUE;
		
		m_CommuEn_T.Commu_Sys_TargetCfg = TRUE;
		m_CommuEn_T.Commu_Sys_AirCtrlCfg = TRUE;
		m_CommuEn_T.Commu_Sys_HeaterCfg = TRUE;
		m_CommuEn_T.Commu_Sys_WaterPadCfg = TRUE;
		m_CommuEn_T.Commu_Sys_Light_Cfg = TRUE;
		
	}
		
	
	//LTE获取RSSI
	if(cnt_LTE_Get_RSSI_10S++ >= 10)
	{
		cnt_LTE_Get_RSSI_10S = 0;
		FLAGS_LTE_GET_RSSI_10S = TRUE;
	}
	if(cnt_LTE_Get_CONNECT_STA_10S++ >= 10)
	{
		cnt_LTE_Get_CONNECT_STA_10S = 0;
		FLAGS_LTE_GET_CON_STA_10S = TRUE;
	}

	
	
	if(ehDataRefresh++ >= TO_REF_2S)
	{
		ehDataRefresh = 0;
		//FLAGS4_EH_DATA_REFRESH = 1;
	}
	
  // 161002 华腾修改
  Rs485_threadCount++;
  if (Rs485_threadCount>=10){     // 300->30  170106
    rt_thread_suspend(&thread_SysMonitor);
    rt_thread_resume(&thread_SysMonitor);
    Rs485_threadCount = 0;
  }
  // ************ 161013 ***************************
  Rs485REC_threadCount++;
  if (Rs485REC_threadCount>=10){     //150->10 170106
    rt_thread_suspend(&thread_ModbusMasterPoll);
    rt_thread_resume(&thread_ModbusMasterPoll);
    Rs485REC_threadCount = 0;
  }
  // *************************************************
 

  RS232ERR_Cout++;
  if (RS232ERR_Cout>=TO_30S)
  {
    //FLAGS0_RS232COMERR =TRUE;
	rt_thread_suspend(&thread_RS232COMM);
	rt_thread_resume(&thread_RS232COMM);
    RS232ERR_Cout =0;
  }
  
  Ref_Count++;

/*
  CAN1ERR_Cout++;
  if (CAN1ERR_Cout>=TO_CANERR_5S){
    FLAGS0_CAN1COMERR =1;
    CAN1ERR_Cout =0;
  }

	//#if defined(TRA_PRO)
	if(RS232DR_Num == 2)
	{
		CAN2ERR_Cout++;
		if (CAN2ERR_Cout>=TO_CANERR_5S)
		{
			FLAGS0_CAN2COMERR =1;
			CAN2ERR_Cout =0;
		}
	}
	else
	{
		CAN2ERR_Cout = 0;
		FLAGS0_CAN2COMERR	=	0;
	}
	//#endif
	*/



	/*
	if(getLcdPicId++ >= TO_5S)
	{
		FLAGS3_GET_LCD_ID_SEND = 1;
		getLcdPicId = 0;
	}
	if(getDayNumCnt++ >= TO_10S)
	{
		getDayNumCnt = 0;
		FLAGS3_GET_CUR_DAY_NUM = 1;
		FLAGS3_GET_LCD_ID_SEND = 0;
	}
*/
	if(esimLostFlag)
	{
		esimThreadCnt++;
	}
	else
	{
		esimThreadCnt = 0;
	}

	 if(esimSupendFlag)
	  {
		  rt_thread_resume(&thread_eSim);
			esimSupendFlag = 0;
	  }
	  if(esimThreadCnt>=TO_3MIN)
	  {
		  esimLostFlag = TRUE;
		  rt_thread_suspend(&thread_eSim);
		  //rt_schedule();
		  //rt_thread_delay(10);
		  esimSupendFlag = 1;
		  //rt_thread_resume(&thread_eSim);
		  esimThreadCnt = 0;
		 // devIdInfo.rebootCnt++;
		  //saveDevIdCfg();
		  //SystemReset();
	  }


	 
	  	 
	
	  esimConnectErrCnt++;
	  if(esimConnectErrCnt >= 60)
	  {
		  esimConnectErrCnt = 0;
		  esimErr = TRUE;
	  }
	

	/*
	esimLostCnt++;
	cnt.min1min++;
 	if(cnt.min1min >= TO_10S)
 	{
		cnt.min1min =0;
		upload.temp = TRUE;
		upload.humidity = TRUE;
		//upload.alarmEnable = TRUE;
		//upload.alarmPara = TRUE;
	  	//upload.humidity = TRUE;
		//upload.nh3 = TRUE;
		//upload.floorHeat = TRUE;
		upload.getRssi= TRUE;
		//upload.syncTime = TRUE;
		//upload.nh3 = TRUE;
	}
	cnt.min2min++;
	if(cnt.min2min >= TO_2MIN)
	{
		upload.nh3 = TRUE;	
		cnt.min2min = 0;
	}
	cnt.min3min++;
	if(cnt.min3min >= TO_30S)
	{
		cnt.min3min= 0;
		upload.alarmEnable = TRUE;
		upload.alarmPara = TRUE;	
		upload.syncTime = TRUE;
	}
	
	cnt.min5min++;
	if(cnt.min5min >= TO_1MIN)
	{
		cnt.min5min= 0;
		
		
		upload.getRemoteJqid = TRUE;
	}

	//power off alarm confirm
	if(devAlarmFlag.alarmFlag[powerOffAlarm])
	{
		if(errConfirmCnt[powerOffAlarm]++ >= alarmCfg.devAlarmCfg[powerOffAlarm].param/2)
		{
			errConfirmCnt[powerOffAlarm] = 0;
			alarmRunSta.devAlarmSta[powerOffAlarm].sta = alarmAccur;
		}
	}
	else
	{
		errConfirmCnt[powerOffAlarm] = 0;
		alarmRunSta.devAlarmSta[powerOffAlarm].sta = alarmNo;
	}


	//hight temper alarm confirm
	if(devAlarmFlag.alarmFlag[highTempAlarm])
	{
		if(errConfirmCnt[highTempAlarm]++ >= alarmCfg.devAlarmCfg[highTempAlarm].param/2)
		{
			errConfirmCnt[highTempAlarm] = 0;
			alarmRunSta.devAlarmSta[highTempAlarm].sta = alarmAccur;
		}
	}
	else
	{
		errConfirmCnt[highTempAlarm] = 0;
		alarmRunSta.devAlarmSta[highTempAlarm].sta = alarmNo;
	}

	//low temper alarm confirm
	if(devAlarmFlag.alarmFlag[lowTempAlarm])
	{
		if(errConfirmCnt[lowTempAlarm]++ >= alarmCfg.devAlarmCfg[lowTempAlarm].param/2)
		{
			errConfirmCnt[lowTempAlarm] = 0;
			alarmRunSta.devAlarmSta[lowTempAlarm].sta = alarmAccur;
		}
	}
	else
	{
		errConfirmCnt[lowTempAlarm] = 0;
		alarmRunSta.devAlarmSta[lowTempAlarm].sta = alarmNo;
	}
	//high humi alarm confirm 
	if(devAlarmFlag.alarmFlag[highHumiAlarm])
	{
		if(errConfirmCnt[highHumiAlarm]++ >= alarmCfg.devAlarmCfg[highHumiAlarm].param/2)
		{
			errConfirmCnt[highHumiAlarm] = 0;
			alarmRunSta.devAlarmSta[highHumiAlarm].sta = alarmAccur;
		}
	}
	else
	{
		errConfirmCnt[highHumiAlarm] = 0;
		alarmRunSta.devAlarmSta[highHumiAlarm].sta = alarmNo;
	}
	//sensor err alarm confirm
	if(devAlarmFlag.alarmFlag[sensorErrAlarm])
	{
		if(errConfirmCnt[sensorErrAlarm]++ >= alarmCfg.devAlarmCfg[sensorErrAlarm].param/2)
		{
			errConfirmCnt[sensorErrAlarm] = 0;
			alarmRunSta.devAlarmSta[sensorErrAlarm].sta = alarmAccur;
		}
	}
	else
	{
		errConfirmCnt[sensorErrAlarm] = 0;
		alarmRunSta.devAlarmSta[sensorErrAlarm].sta = alarmNo;
	}

	if(noPressCnt++ > TO_3MIN)
	{
		noPressCnt	= 0;
		FLAGS3_NO_PRESS = 1;
	}
	*/
}



int rt_application_init(void){
    /* 创建定时器1 */
  Timer_50MS = rt_timer_create("timer", /* 定时器名字是timer1 */
                           Timer50MS_OUT, /* 超时时回调的处理函数*/
                           RT_NULL, /* 超时函数的入口参数*/
                           TO_BASE, /* 定时长度，以OS Tick为单位，即10个OS Tick */
                           RT_TIMER_FLAG_PERIODIC); /* 周期性定时器*/
  /* 启动定时器*/
  if (Timer_50MS != RT_NULL) 
    rt_timer_start(Timer_50MS);
  
   Timer_1S = rt_timer_create("timer1", /* 定时器名字是timer1 */
                           Timer1S_OUT, /* 超时时回调的处理函数*/
                           RT_NULL, /* 超时函数的入口参数*/
                           TO_BASE2, /* 定时长度，以OS Tick为单位，即10个OS Tick */
                           RT_TIMER_FLAG_PERIODIC); /* 周期性定时器*/
  /* 启动定时器*/
  if (Timer_1S != RT_NULL) 
    rt_timer_start(Timer_1S);
  

  
  rt_thread_init(&thread_SysMonitor, "SysMonitor", thread_entry_SysMonitor,
      RT_NULL, thread_SysMonitor_stack, sizeof(thread_SysMonitor_stack),
      thread_SysMonitor_Prio, 5);
  rt_thread_startup(&thread_SysMonitor);
      
  rt_thread_init(&thread_ModbusMasterPoll, "MBMasterPoll",
      thread_entry_ModbusMasterPoll, RT_NULL, thread_ModbusMasterPoll_stack,
      sizeof(thread_ModbusMasterPoll_stack), thread_ModbusMasterPoll_Prio,
      5);
  rt_thread_startup(&thread_ModbusMasterPoll);
  #if 1
  rt_thread_init(&thread_DevRun, "DevRun",
      thread_entry_DevRun, RT_NULL, thread_DevRun_stack,
      sizeof(thread_DevRun_stack), thread_DevRun_Prio,
      5);
  rt_thread_startup(&thread_DevRun);
  #endif
  #ifdef THREAD_CAN_EN
  rt_thread_init(&thread_CANCOMM, "CANCOMM",
      thread_entry_CANCOMM, RT_NULL, thread_CANCOMM_stack,
      sizeof(thread_CANCOMM_stack), thread_CANCOMM_Prio,
      5);
  rt_thread_startup(&thread_CANCOMM);
  #endif 
  #ifdef THREAD_ADC_EN
  rt_thread_init(&thread_ADC, "ADC",
      thread_entry_ADC, RT_NULL, thread_ADC_stack,
      sizeof(thread_ADC_stack), thread_ADC_Prio,
      5);
  rt_thread_startup(&thread_ADC); 
  #endif

   #ifdef THREAD_DAC_EN
  rt_thread_init(&thread_DAC, "DAC",
      thread_entry_DAC, RT_NULL, thread_DAC_stack,
      sizeof(thread_DAC_stack), thread_DAC_Prio,
      5);
  rt_thread_startup(&thread_DAC); 
  #endif
  
  #if 1
  rt_thread_init(&thread_RS232COMM, "RS232COMM",
      thread_entry_RS232COMM, RT_NULL, thread_RS232COMM_stack,
      sizeof(thread_RS232COMM_stack), thread_RS232COMM_Prio,
      5);
  rt_thread_startup(&thread_RS232COMM); 
  #endif

   #if 1
  rt_thread_init(&thread_eSim, "ESIM",
      thread_entry_eSim, RT_NULL, thread_eSim_stack,
      sizeof(thread_eSim_stack), thread_eSim_Prio,
      5);
  rt_thread_startup(&thread_eSim); 
  #endif

  #if 1
  rt_thread_init(&thread_UART4, "UART4",
  thread_entry_UART4, RT_NULL, thread_UART4COMM_stack,
  sizeof(thread_UART4COMM_stack), thread_UART4COMM_Prio,
  10);
  rt_thread_startup(&thread_UART4);
	#endif
    
  return 0;
}


//**************************初始化RT-Thread函数*************************************
//函数定义: void rtthread_startup(void)
//入口参数：无
//出口参数：无
//**********************************************************************************
void rtthread_startup(void){
  /* init board */
  rt_hw_board_init();
  Init_Vars();

  /* show version */
  //rt_show_version();
  /* init tick */
  rt_system_tick_init();

  /* init kernel object */
  rt_system_object_init();

  /* init timer system */
  rt_system_timer_init();

#ifdef RT_USING_HEAP
#ifdef __CC_ARM
  rt_system_heap_init((void*)&Image$$RW_IRAM1$$ZI$$Limit, (void*)STM32_SRAM_END);
#elif __ICCARM__
  rt_system_heap_init(__segment_end("HEAP"), (void*)STM32_SRAM_END);
#else
  /* init memory system */
  rt_system_heap_init((void*)&__bss_end, (void*)STM32_SRAM_END);
#endif
#endif

  /* init scheduler system */
  rt_system_scheduler_init();

  /* init all device */
  rt_device_init_all();

  /* init application */
  rt_application_init();

#ifdef RT_USING_FINSH
  /* init finsh */
  finsh_system_init();
  finsh_set_device("uart1");
#endif

  /* init timer thread */
  rt_system_timer_thread_init();

  /* init idle thread */
  rt_thread_idle_init();

  /* Add CPU usage to system */
  cpu_usage_init();

  /* start scheduler */
  rt_system_scheduler_start();

  /* never reach here */
  return;
}

