#ifndef ENVIR_H  
#define ENVIR_H  


//#include "vector.h"

#pragma pack(push)
#pragma pack(1)

typedef enum
{
	CFG_INDEX_DEV_EN = 0,		//外设使能配置
	CFG_INDEX_CTRL_CFG,			//控制配置
	CFG_INDEX_ALARM_CFG,		//报警配置
	CFG_INDEX_ROOM_CFG			//栏舍配置
}CFG_INDEX_T;

#define	PAGE_SIZE_LARGE		0X800 
//存放控制器外设使能,外设配置
#define	FLASH_CONTROLER_DEV_ENABLE_ADDR		0X0803E000

//存放控制器通风配置
#define	FLASH_CONTROLER_CTRL_CFG_ADDR		0X0803E800

//存放控制器报警配置
#define	FLASH_CONTROLER_ALARM_CFG_ADDR		0X0803F000

//存放控制器栏舍配置。
#define	FLASH_CONTROLER_ROOM_CFG_ADDR		0X0803F800


/*
一、配置信息
1、环境控制功能相关参数
	温度曲线，最小通风，通风级别，外设设备控制参数
2、报警配置
	报警项使能，上限值报警，下限值报警，报警恢复差值，报警方式
3、外设支持的配置信息
	传感器配置信息，如校准等、外设设备支持配置，控制方式等

二、运行信息
1、通风相关信息
2、传感器状态、数值。
3、外设设备状态、数值
4、告警状态
5、控制器状态。电量，信号量，栏舍状态等
*/

#define	TARGET_TEMP_NUM	7//目标温度曲线点数
#define	CONTROL_LEVEL_MAX	7	//通风级别个数
#define	FRE_FAN_NUM_MAX		2
#define	FAN_NUM_MAX			5

#define SUPPORT_SENSOR_TEMPER_NUM			6
#define	SUPPORT_SENSOR_HUMI_NUM				3
#define	SUPPORT_SENSOR_NH3_NUM				2
#define	SUPPORT_SENSOR_WATER_PRESSURE_NUM	1
#define	SUPPORT_SENSOR_PH_NUM				1
#define	SUPPORT_SENSOR_POWER_METER_NUM		1
#define SUPPORT_SENSOR_WATER_METER_NUM1		1

#define	SUPPORT_DEV_FRE_FAN_NUM				2
#define	SUPPORT_DEV_FAN_NUM					5
#define	SUPPORT_DEV_COLD_PAD_NUM			1
#define	SUPPORT_DEV_HEATER_NUM				1
#define	SUPPORT_DEV_LIGHT_NUM				1

//外设，传感器类型
typedef enum
{
	SENSOR_TEMPER = 0,					//0 室内温度
	SENSOR_HUIMI,						//1 湿度
	SENSOR_OUT_DOOR_TEMPER,				//2 室外温度
	SENSOR_NH3,							//3 氨气
	SENSOR_CO2,							//4 co2
	SENSOR_LUX,							//5 光照
	SENSOR_AIR_PRESSURE,				//6 气压
	SENSOR_POWER_METER,					//7 电表
	SENSOR_WATER_METER,					//8 水表
	SENSOR_PH,							//9 PH
	SENSOR_WATER_PRESSURE,				//10 水压
	SENSOR_WIEGHT,						//11 称重
	SENSOR_TYPE_MAX,

	DEV_TYPE_START = 30,		
	DEV_FRE_FAN = DEV_TYPE_START,		//30 变频
	DEV_FAN,							//31 普通风机
	DEV_WATER_PAD,						//32 湿帘
	DEV_HEATER,							//33 加热
	DEV_AIR_CELL,						//34 通风小窗
	DEV_AIR_CURTAIN,					//35 卷帘
	DEV_LIGHT,							//36 灯光
	DEV_STOCK_LINE,						//37 料线
	DEV_LOCAL_ALARM,					//38 本地报警
	DEV_TYPE_MAX
}PERIPER_DEV_T;


typedef struct
{
	uint64_t outBits;			//64位输出，bit0表示第一路输出
	uint16_t dacOutValue[4];	//4路DAC输出，此处为DAC值。
}DRIVE_BOARD_OUT_T;

typedef struct 
{
	uint64_t inputBits;			//64位输入。bit0代表第一路输入
	uint16_t adcInputValue[4];	//4路ADC输入。此处为ADC值
}DRIVE_BOARD_IN_T;
typedef struct 
{
	DRIVE_BOARD_OUT_T sendData;
	DRIVE_BOARD_IN_T  recvData;
}DRVIE_BOARD_DATA_T;

//传感器、外设使能情况
typedef	enum
{
	vFan_1 = 0,
	vFan_2,
	fan1,
	fan2,
	fan3,
	fan4,
	fan5,
	heater,
	waterPad,
	airWindow,
	airCurtain,
	light,
	stockLin_1,
	stockLin_2,
	devSortMax
}DEV_SORT;
typedef struct
{
	uint8_t type;
	uint8_t typeIndex;
	uint8_t sta;
}DEV_STRUCT_T;

//通风相关配置

struct sensorEnStruct_S
{
	uint8_t type;
	uint8_t num;
};
struct devEnStruct_S
{
	uint8_t type;
	uint8_t num;
	uint8_t index;		
};

//设备能力。
typedef struct 
{
	/*
	uint8_t freFanNum;
	uint8_t fanNum;
	uint8_t heaterNum;
	uint8_t waterPadNum;
	uint8_t lightNum;
	*/

	struct sensorEnStruct_S sensorEnStruct[SENSOR_TYPE_MAX];
	struct devEnStruct_S devEnStruct[DEV_TYPE_MAX - DEV_TYPE_START];
	uint16_t freFanDacChart[10];//变频风机0-100%通风量对应的DA值
	uint8_t saveTime[6];
}CONTROLER_DEV_CFG_T;


//目标温度配置信息。6Byte
struct TargetTemp_S
{
	uint8_t dayNum;		//天数
	uint16_t targetTemp;//目标温度
	uint8_t minAirSpeed;//最小通风风速
	uint16_t minAirRuntime;//最小通风运行时间。单位S
};

//目标温度配置曲线表。6*7+2=44Byte
typedef struct 
{
	struct TargetTemp_S targetTempArry[TARGET_TEMP_NUM];	//目标温度曲线，最小通风曲线
	uint16_t minAirSupport_cycleTime;				//循环周期时间。0-600S。0-10min
	//uint8_t outTemperCompValue;						//温度是否使能。
	uint8_t saveTime[6];
}TARGET_TEMP_INFO_T;
//变频风机运行配置数据结构。6Byte
struct FRE_FAN_RUN_CFG_S
{
	uint8_t en;				//使能
	uint8_t type;			//类型
	uint8_t id;				//序号
	uint8_t speed;			//速度。0-100%
	uint16_t runTime;		//运行时间。单位S。0-600S
};
//普通风机运行配置数据结构。5Byte
struct FAN_RUN_CFG_S
{
	uint8_t en;				//使能
	uint8_t type;			//类型
	uint8_t id;				//序号
	uint16_t runTime;		//运行时间。单位S。0-600S
};
//通风档位设置配置结构。1+1+6*2+5*5=39Byte
typedef struct 
{
	uint8_t levelId;					//通风级别。1-X
	uint8_t temperShift;				//温度偏移℃。*10。123=12.3℃
	struct FRE_FAN_RUN_CFG_S freFanCfgArry[2];	//变频风机运行配置信息。
	struct FAN_RUN_CFG_S	fanCfgArry[5];		//普通风机运行配置信息
}AirCtrlLevelCfg_T;

//通风级别设置。39*7+2+2=277Byte
typedef struct 
{
	AirCtrlLevelCfg_T airCtrllevel[CONTROL_LEVEL_MAX];//
	uint16_t freFanCycleTime;			//变频风机循环时间。0-600S
	uint16_t fanCycleTim;				//普通风机循环时间。0-600S
	uint8_t saveTime[6];
}AirCtrlCfg_T;

//湿度控制配置数据结构。9Byte
typedef struct 
{
	uint8_t en;					//使能
	uint16_t enTime;			//湿帘运行起始时间。0X0930=09:30
	uint16_t disenTime;			//湿帘运行停止时间。0X1730=17:30
	uint8_t runTempShift;		//湿帘开启温度偏移℃。55=5.5℃。目标温度+该偏移
	uint8_t stopTempShift;		//湿帘关闭温度偏移℃。50=5.0℃。目标温度+该偏移
	uint8_t runTime;			//运行时间Min。
	uint8_t stopTime;			//停止时间Min。
	uint8_t compValue;			//湿度补偿值。整数。0-100。0表示无效
	uint8_t saveTim[6];
}HUMI_CTRL_CFG_T;

//加热控制配置数据结构。3Byte
typedef struct 
{
	uint8_t en;					//使能
	uint8_t runTempShift;		//加热开启温度偏移℃。55=5.5℃。目标温度-该偏移
	uint8_t stopTempShift;		//加热开启温度偏移℃。50=5.0℃。目标温度-该偏移
	uint8_t saveTime[6];
}HEATER_CTRL_CFG_T;


struct LIGHT_RUN_CFG_S
{
	uint16_t enTime;			//湿帘运行起始时间。0X0930=09:30
	uint16_t disenTime;			//湿帘运行停止时间。0X1730=17:30
	uint16_t targetLux;			//目标亮度。lux
};

//灯光配置。7Byte
typedef	struct
{
	uint8_t en;					//使能
	//struct LIGHT_RUN_CFG_S lightCtrlCfg;
	uint16_t enTime;			//湿帘运行起始时间。0X0930=09:30
	uint16_t disenTime;			//湿帘运行停止时间。0X1730=17:30
	uint16_t targetLux;			//目标亮度。lux
	uint8_t saveTime[6];
}LIGHT_CTRL_CFG_T;
//料线配置。暂不考虑
typedef	struct
{
	uint8_t a;
}STROCK_LINE_CTRL_CFG_T;

//控制器外设控制。44+277+9+3+7=340
typedef	struct
{
	
	TARGET_TEMP_INFO_T	targetTemperCfg;
	
	AirCtrlCfg_T	airCtrlCfg;
	
	HUMI_CTRL_CFG_T	humiCtrlCfg;
	
	HEATER_CTRL_CFG_T heaterCtrlCfg;
	
	LIGHT_CTRL_CFG_T lightCtrlCfg;

}CONTROLER_CTRL_CFG_T;

typedef struct 
{
	uint8_t speed;
	uint8_t sta;
	uint16_t runCount;
	uint16_t stopCount;
}FRE_FAN_RUN_INFO_S;
typedef struct 
{
	uint8_t sta;
	uint16_t runCount;
	uint16_t stopCount;
}FAN_RUN_INFO_S;

//设备运行信息
typedef enum
{
	DEV_STOP = 0,
	DEV_RUN
}EQUIPMENT_STA_T;
	
typedef	struct
{
	FRE_FAN_RUN_INFO_S freFanRunInfo[2];
	FAN_RUN_INFO_S 	fanDevRunInfo[5];
	FAN_RUN_INFO_S	heaterRunInfo;
	FAN_RUN_INFO_S coldPadRunInfo;
	FAN_RUN_INFO_S	lightRunInfo;
}EQUIPMENT_RUN_INFO_T;


//报警类型
typedef enum
{
	ALARM_TYPE_START = 0,
	ALARM_TYPE_POWER_OFF = 0,	//0 断电
	ALARM_TYPE_DEV_ERR,			//1 设备姑杭
	ALARM_TYPE_HIGH_TEMPER,		//2	高温
	ALARM_TYPE_LOW_TEMPER,		//3 低温
	ALARM_TYPE_SENSOR_ERR,		//4	传感器故障
	ALARM_TYPE_MAX
}ALARM_TYPE_T;

struct DEV_ALARM_CFG_S
{
	uint8_t en;			//报警使能
	uint8_t type;		//报警类型
	uint8_t alarmAct;	//报警方式。bit0:本地，bit1：短信，bit2：电话，bit3：声光。（平台推送设备端不考虑报警方式）
	uint8_t alarmOccurShit;	//报警偏移
	uint8_t alarmRecoverShit;	//恢复偏移
	uint8_t confirmTime;	//确认时间
};

typedef	struct
{
	struct DEV_ALARM_CFG_S devAlarmCfg[ALARM_TYPE_MAX];
	uint8_t saveTime[6];
}CONTROLER_ALARM_CFG_T;
//故障状态
typedef	enum
{
	ALARM_STA_NO = 0, 
	ALARM_STA_RECOVER = ALARM_STA_NO,
	ALARM_STA_OCCUR
}ALARM_STA_T;
//故障处理状态，故障是否已经上报
typedef	enum
{
	ALARM_ACT_PENDING= 0,//故障动作尚未处理
	ALARM_ACT_DONE		//故障动作已处理。
}ALARM_STA_ACT_T;
//组件故障处理数据结构
typedef struct
{
	ALARM_STA_T alarmSta;
	ALARM_STA_ACT_T alarmActSta;
	uint8_t time[6];				//发生时间
}DEV_ALARM_STA_T;
//控制器故障处理数据结构
typedef struct
{
	DEV_ALARM_STA_T alarmPorc[ALARM_TYPE_MAX];
}CONTROLER_ALARM_PROC_T;


//临时报警状态数据。用于指示是否满足报警
typedef struct 
{
	uint8_t sta;
	uint8_t confirmCnt;
}ALARM_PROC_TEMP_T;
typedef struct 
{
	ALARM_PROC_TEMP_T alarmSta[ALARM_TYPE_MAX];
}CONTROLER_ALARM_STA_T;
/*
//报警配置
//考虑到每个传感器的均可能通信故障，上下限阈值报警，每个设备单独分配空间
//断电。4ByteCONTROLER_ALARM_CFG_T
struct POWER_OFF_ALARM_CFG_S
{
	uint8_t en;				//使能
	uint8_t type;			//报警类型
	uint8_t confirmTime;	//确认时间。单位S。0-255S
	uint8_t alarmMode;		//告警方式。bit0:本地，bit1：短信，bit2：电话，bit3：声光。（平台推送设备端不考虑）
};
//设备故障。5Byte
struct EQUIPMENT_ALARM_CFG_S
{
	uint8_t en;				//使能
	uint8_t type;			//报警类型
	uint8_t id;				//序号
	uint8_t confirmTime;	//确认时间。单位S。0-255S
	uint8_t alarmMode;		//告警方式。bit0:本地，bit1：短信，bit2：电话，bit3：声光。（平台推送设备端不考虑）
};
//传感器类报警配置。10Byte
struct SENSOR_ALARM_CFG_S
{
	uint8_t en;				//使能
	uint8_t type;			//
	uint16_t uperLimt;		//上限阈值
	uint16_t lowerLimt;		//下限阈值
	uint16_t recoverShit;	//阈值恢复偏移值
	uint8_t confirmTime;	//确认时间。单位S。0-255S
	uint8_t alarmMode;		//告警方式。bit0:本地，bit1：短信，bit2：电话，bit3：声光。（平台推送设备端不考虑）
};


#define	SENSOR_TYPE_NUM		3//传感器类型数量。由平台下发的外设设备获取

typedef	struct
{
	struct POWER_OFF_ALARM_CFG_S powerOffAlarmCfg;
	struct EQUIPMENT_ALARM_CFG_S equipmentAlarmCfg[2+5+3];//变频风机+普通风机+加热+制冷+灯光
	struct SENSOR_ALARM_CFG_S sensorAlarmCfg[SENSOR_TYPE_NUM];//
}ALARM_CFG_T;
*/

//与驱动板的通信内容
/*
发送
//GPIO输出
//DA的输出值
接收
GPIO输入
AD采集值
*/

//栏舍配置
typedef struct 
{
	//uint8_t jqid[8];					//jqid
	uint8_t setDays;					//运行天数。1-256
	//uint8_t curAirLevel;				//当前通风级别
	uint16_t runDaysRefreshTime;		//目标温度，运行天数刷新时间。0X930=09:30
	uint8_t isEmptyRoom;				//是否为空舍
	//uint8_t restartRunDays;				//重置运行天数
	uint16_t animalNum;					//动物数量
	uint16_t avgWeight;					//平均体重
	uint8_t saveTime[6];
}ROOM_INFO_CFG_T;


struct CONTROLER_LOCATE_S
{
	uint8_t latitude[10];
	uint8_t longitude[10];
};
//控制器状态数据类型
typedef	struct 
{
	uint8_t powerPercent;		//电量,0-100%
	uint8_t isCharging;			//是否在充电。0：未充电，1：充电中，2：已充满
	uint8_t isPowerOff;			//是否断电。0：未断电，1：断电
	uint8_t signalQuality;		//信号强度。0-100
	uint8_t alarmSta;			//告警状态。0：未告警，1：告警
	//uint8_t isTransData;		//数据传输状态。0：未传输，1：上传中
	//struct CONTROLER_LOCATE_S locate;			//定位信息
}CONTROLER_HARDWARE_STA_T;
struct DEV_INFO_S
{
	uint8_t en;
	uint8_t type;
	uint8_t id;
	uint8_t sta;
};
//设备运行信息。
typedef struct
{
	//栏舍相关信息。	
	uint8_t dayNum;				//运行天数
	uint8_t airCtrlLevel;		//通风级别
	uint8_t MinAirPercent;		//最小通风量
	uint8_t roomStartTime[6];	//栏舍配置更新时间

	//控制器相关信息
	CONTROLER_HARDWARE_STA_T	controlerInfo;
	//外设信息
	
}CONTROLER_RUN_INFO_T;

typedef	struct
{
	CONTROLER_HARDWARE_STA_T controlerHardwareSta;
	//ROOM_INFO_T	roomInfo;
}CONTROLER_INFO_T;
//版本信息
typedef struct 
{
	uint8_t motherboardHardwareVersion[10];	//主板硬件版本
	uint8_t attachboardHardwareVersion[10];	//附加板硬件版本
	uint8_t lcdVer[10];						//液晶版本
	uint8_t motherboardSoftwareVersion[10];	//主板软版本
	uint8_t attachboardSoftwareVersion[10];	//附加板软件版本
}CTRL_VER_T;

//控制器ID相关信息
#define DEV_SERIAL_CODE_LEN 8
#define	LTE_IMEI_LEN		15
#define	LTE_CCID_LEN		20
#define	LTE_IMSI_LEN		15
#define	LET_MODEL_NAME_LEN	10

typedef struct 
{
	//uint8_t devSerialCode[DEV_SERIAL_CODE_LEN+1];//机器id
	uint8_t imeiCode[LTE_IMEI_LEN];							
	uint8_t ccidCode[LTE_CCID_LEN];			
	uint8_t imsiCode[LTE_IMSI_LEN];							
	uint8_t modelName[LET_MODEL_NAME_LEN];
}CONTROLER_DEV_INFO_T;

//运行信息
//设备运行状态数据结构
/*
typedef struct 
{
	uint8_t sta;
	uint16_t runCnt;
	uint16_t stopCnt;
}EQUIPMENT_RUN_INFO_T;
*/

//故障信息

typedef	enum
{
	ERR_NULL = 0,
	ERR_UPER_LIMIT,
	ERR_LOWER_LIMIT,
	ERR_COMMU,

	EQUIPMENT_ERR = 10,
}ERR_TYPE_T;
#define	SENSOR_NUM		10
#define DEV_EQUIPMENT_NUM 10
typedef struct
{
	uint8_t devType;		//设备或者传感器类型
	uint8_t id;				//设备或者传感器序号
	ERR_TYPE_T errType;		//故障类型
	uint8_t sta;			//故障状态，.0:无故障，1：故障发生，
	
}DEV_ERR_INFO_T;
//DEV_ERR_INFO_T m_devErrInfo[SENSOR_NUM+DEV_EQUIPMENT_NUM];

//控制器支持的能力
typedef	struct
{
	uint8_t devType;	//支持的设备或传感器类型
	uint8_t num;		//支持的个数
	uint8_t index;		//设备类型。
}SUPPORT_DEV_STUCT_T;


#define	SEASON_MAX		3	//季节定义
#define	DAY_LINE_MAX	7	//日龄最大支持值
#define	TEMP_LINE_MAX	7	//通风机级别
#define	DAY_LEVEL_MAX	DAY_LINE_MAX
#define	TEMP_LEVEL_MAX	TEMP_LINE_MAX

#define	TEMP_SENSOR_NUM_MAX	3	//最大支持传感器个数
#define	HUMI_SENSOR_NUM_MAX	1	//最大支持传感器个数

#define	COLD_DEV_NUM		1//降温设备个数
#define	HEAT_DEV_NUM		1//加热设备个数
#define	SPEED_FAN_NUM		2//变频风机个数
#define	AD_INPUT_NUM		2

#define	TEMP_ALARM_CONFIRM_SHIFT_VALUE		5	//温度报警确认缓冲值0.5摄氏度
#define	ALARM_CONFIRM_TIME_MAX				200	//报警确认时间最大值200*2 = 400S
#define	HUMI_VALUE_MAX						9500	//湿度最大值。
#define	TEMP_VALUE_MAX						6000
#define	SPEED_FAN_VALUE_MAX					100		//变频风机最大速度
#define	DAY_NUM_VALUE_MAX					999		//日龄值最大值999天
#define	DAY_NUM_VALUE_MIN					1		//日龄值最小值1天
#define	TARGET_TEMP_VALUE_MAX				999	//目标温度最大值99.9
#define	TEMP_ADJUST_VALUE_MAX				50	//温度最大校准值5摄氏度
#define	HUMI_ADJUST_VALUE_MAX				200	//湿度最大校准值20%



/*
typedef enum devSort
{
    VariFreqFan1 = 0,
    Fan1,
    Fan2,
    Fan3,
    Fan4,
    Fan5,
    CurTain1,
    devMax
}devSort;
*/
/*
typedef	enum targetTempErrType
{
	errTarget_firstDayNum = 1,
	errTarget_firstTemp,
	errTarget_dayNum
}
*/
typedef enum modClassify
{
    EarlyNurseryWinner = 0,
    EarlyNurserySpring,
    EarlyNurserySummer,
    MidNurseryWinner,
    MidNurserySpring,
    MidNurserySummer,
    HigherNurseryWinner,
    HigherNurserySpring,
    HigherNurserySummer,
    FattenWinner,
    FattenSpring,
    FattenSummer,
    ModMax,
}modClassify;

typedef enum devSort1
{
    Fan1 = 0,
    Fan2,
    Fan3,
    Fan4,
    Fan5,
    Fan6,
    Fan7,
    ventWindow,//通风窗
    VariFreqFan1,
    VariFreqFan2,
    devMax,
    coldDev = devMax,
    heatDev,
    allDevMax
}devSort;


typedef	enum	seasonModeSt
{
	springMode = 0,
	summerMode,
	winterMode,
	seasonMax
}seasonMode_t;


//目标温度
typedef	struct targetTempSt
{
	uint16_t dayCount;
	uint16_t temp;
	uint16_t high_temp;
	uint16_t low_temp;
	//uint16_t heatStartTemp;
}targetTemp_t;

typedef	struct	tempAlarmShiftSt
{
	uint8_t	highShift;//高温报警便宜，最大25.5摄氏度
	uint8_t	lowShift;
}tempAlarmShift_t;

//降温设备
typedef	struct coldDev
{
	int16_t	startShift;
	int16_t	stopShift;
	int16_t	stopParam;
}coldDev_t;

//加热设备
typedef	struct heatDev
{
	int16_t startParam;
	//int16_t	stopParam;	
}heatDev_t;

//对应温度所有设备的运行情况
typedef	struct tempCtrlCfg
{
	uint16_t	tempMax;//该控制模式下的对应温度
	uint8_t	sta[devMax];
}tempCtrlCfg_t;
//每个日龄的档位配置
typedef	struct dayCtrlCfg
{
	//targetTemp_t targetTemp;
	uint16_t	dayNum;//日龄
	tempCtrlCfg_t	tempCtrlMode[TEMP_LEVEL_MAX];//该日龄下对应的温度控制模式
	heatDev_t	heatDevCfg;
}dayCtrlCfg_t;



//工作模式配置
typedef	struct devModeCfg
{
	dayCtrlCfg_t	dayCtrlMode[DAY_LEVEL_MAX];
	uint8_t			isTargetEn;//是否使用温度曲线
	uint8_t			isDefTempAlarmEn;//是否使用默认高低温模式
	//tempAlarmShift_t	tempAlarmShift;//高低温偏移
	targetTemp_t	targetTemp[DAY_LEVEL_MAX];//目标温度
	//uint8_t			seasonMode;
	coldDev_t	coldDevCfg;
	int16_t	heatStopShift;
}devModeCfg_t;



//传感器校准值
typedef	struct sensorAjust
{
	int16_t	tempAjustValue[TEMP_SENSOR_NUM_MAX];
	int16_t	humiAjustValue[HUMI_SENSOR_NUM_MAX];
}sensorAjust_t;


//传感器配置
typedef	struct sensorCfg
{
	sensorAjust_t	sensorAjust;
	uint8_t		outDoorSensorId;
	uint8_t		tempSensorNum;
	uint8_t		humiSensorNum;
}sensorCfg_t;



typedef enum alarmEvent
{
	powerOffAlarm = 0,
	highTempAlarm,
	lowTempAlarm,
	tempDiffAlarm,
	highHumiAlarm,
	sensorErrAlarm,
	alarmEventMax
}alarmEvent_s;


//报警动作
typedef	enum
{
	localBeep = 0,
	message,
	callVoice,
	acousto,
	pushMes,
	alarmActMax
}alarmAct_e;
//设备报警配置
typedef	struct
{
	uint8_t	en;//报警使能
	uint8_t	actEn[alarmActMax];
	uint16_t	param;//故障判定参数，即确认时间等
	uint16_t	condition;//报警值
}devAlarmCfg_s;

//报警配置	
typedef	struct
{
	devAlarmCfg_s devAlarmCfg[alarmEventMax];
}alarmCfg_t;

typedef	enum
{
	alarmNo = 0, 
	alarmRecover = alarmNo,
	alarmAccur
}alarmSta_e;

//故障标志,仅做故障判断时，临时使用
typedef	struct
{
	alarmSta_e	alarmFlag[alarmEventMax];
}devAlarmFlag_t;

typedef	enum
{
	alarmActPending = 0,//故障动作尚未处理
	alarmActDone		//故障动作已处理。
}alarmActSta_e;
//设备当前报警状态
typedef	struct 
{
	alarmSta_e	sta;	//故障状态。是否产生
	alarmActSta_e	actSta[alarmActMax];//报警动作处理状态。报警动作是否已执行
}devAlarmActSta_s;
//报警执行状态
typedef	struct
{
	devAlarmActSta_s devAlarmSta[alarmEventMax];
}alarmRunSta_t;

typedef	struct curRtcTime
{
	uint16_t	year;	//格式为20xxx
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t		min;
	uint8_t		sec;
	uint8_t		week;
}rtcTime_t;

//养殖阶段。保育、育肥、产房
typedef	enum
{
	baoyuLevel = 0,
	yufeiLevel,
	chanfangLevel,
	feedLevelMax
}feedLeve_e;
typedef	struct devInfo
{
	uint8_t	userName[20];//用户名称，最多支持10个汉字
	uint16_t	adminPassword;
	uint16_t	user1Password;
	uint16_t	user2Password;
	uint8_t	roomIdHead;
	uint8_t	roomIdTail;
	uint8_t	devId;
	uint8_t	feedLevel;
	uint16_t animalNum;
	uint8_t	avgWeight;
	uint16_t curDayNum;
	uint8_t	curSeason;
	uint8_t	isFlashInit;
	rtcTime_t	saveRtc;	//设置日龄时保存RTC
	uint16_t	saveDayNum;	//设置的日龄的初始值
}devInfo_t;

typedef	struct devDebugCtrl
{
	uint8_t	debugSta;
	uint8_t	devAction[devMax];
	uint8_t	coldDevAction;
	uint8_t	heatDevAction;
}devDebugCtrl_t;




/*
void initCondState(void);
uint8_t getDevState(uint8_t mode,uint16_t curDay,devSort dev,uint16_t envirPara[]);

*/

void	devRunFunc(devSort devType, uint8_t	sta);

uint8_t getDevState2(uint8_t mode,uint16_t curDay,devSort dev,uint16_t temp);

int16_t getCurDayNum(rtcTime_t	startTime, rtcTime_t curTime);


int8_t	getRunStep(uint8_t seasonMode,uint8_t	dayIndex,int16_t		temp);
uint8_t	getDayItem2(uint8_t seasonMode,uint16_t curDay);



uint8_t Ctroler_Init(void);
uint8_t Ctroler_Get_DevRunCfg(void);
uint8_t Contrler_Cfg_Save(uint8_t cfgIndex);



extern CONTROLER_DEV_CFG_T		m_Controler_Dev_Cfg_t;	//控制器外设能力
extern CONTROLER_CTRL_CFG_T		m_Controler_Ctrl_Cfg_t;	//控制配置
extern CONTROLER_ALARM_CFG_T	m_Controler_Alarm_Cfg_t;//控制器报警配置
extern ROOM_INFO_CFG_T			m_Room_Cfg_t;			//栏舍信息配置。主要是运行时间的起始点
extern EQUIPMENT_RUN_INFO_T 	m_Equipment_RunInfo_t;

extern DRVIE_BOARD_DATA_T		m_DriveBoard_Data_t;
extern CONTROLER_ALARM_PROC_T	m_Alarm_Proc_t;	
extern CONTROLER_ALARM_STA_T	m_Alarm_Sta_t;	
#endif

