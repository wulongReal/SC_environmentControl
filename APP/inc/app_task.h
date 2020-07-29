/*********************************************************************************************************
//                             NCLUDE FILES
*********************************************************************************************************/
#ifndef APP_TASK_H
#define APP_TASK_H

#include <stdlib.h>
#include <stdio.h>
#include <rthw.h>  
#include <rtthread.h>
#include "platform.h"
#include "bsp.h"
#include "delay_conf.h"
#include "cpuusage.h"
#include "user_mb_app.h"

/*******************************************************************************************************/
//                           DEFINES                                                                   
/*******************************************************************************************************/




extern uint16_t   ucSDiscInBuf[]  ;
extern uint16_t   ucSCoilBuf[]    ;
extern uint16_t   usSRegInBuf[]   ;
extern uint16_t   usSRegHoldBuf[] ;



 

typedef struct  
{
	uint16_t syncTime;  
	uint16_t temp;  
	uint16_t alarmEnable;	
	uint16_t alarmPara;	
	uint16_t humidity;
	uint16_t nh3;
	uint16_t floorHeat;
	uint16_t min1min;
	uint16_t min2min;
	uint16_t min3min;
	uint16_t min5min;
}_timeCnt;

enum 
{
	appEventInit = 0,			
	appEventAlarmPowerCut = 1,			
	appEventAlarmFunFault,			
	appEventAlarmOverTemp,			
	appEventTempSensorFault,			
	appEventHumidSensorFault,			
	appEventAmmoniaSensorFault, 		
	appEventPHSensorFault,			
	appEventWaterPressereSensorFault,			
	appEventAlarmLowTemp,			
	appEventHighTempDiff,			
	appEventHighHumidity,			
	appEventAmmoniaHigh,			
	appEventDisconnect, 		
	appEventGetWeightFail  ,			
	appEventEntranceDoorFault,			
	appEventRevolutionDoorFault,			
	appEventSeparationDoorFault,			
	appEventPigNotLeave,			
	appEventDetecteSensor,			
	appEventConfigErr,			
	appEventUploadErr,//21			
};


#define	ERR_TARGET_NO		0X00
#define	ERR_TARGET_FIST_ERR		0X10
#define	ERR_TARGET_DAY_NUM_ERR	0X20
#define	ERR_TARGET_VALUE		0X30

void rtthread_startup(void);


#endif
