#ifndef __LTE720_H__
#define	__LTE720_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "bsp.h"
#include "vars.h"


#pragma pack(push)
#pragma pack(1)

//大小端转换。当前系统为小端
#define BigtoLittle32(A)   ((( (uint32_t)(A) & 0xff000000) >> 24) | \
                                       (( (uint32_t)(A) & 0x00ff0000) >> 8)   | \
                                       (( (uint32_t)(A) & 0x0000ff00) << 8)   | \
                                       (( (uint32_t)(A) & 0x000000ff) << 24))
#define BigtoLittle16(A)   ((( (uint16_t)(A) & 0xff00) >> 8) | \
                                       (( (uint32_t)(A) & 0x000000ff) << 8))

typedef enum
{
	RECV_OK = 0, 
	RECV_NO_RETURN,
	RECV_JQID_ERR,
	RECV_LEN_RRR,
	RECV_CHECK_ER
}COMMU_RECV_STA_T;

struct SENSOR_DATA_S
{
	uint8_t type;
	uint8_t id;
	uint32_t value;
	uint8_t sta;
};

struct ALARM_INFO_S
{
	uint8_t type;			//报警类型
	uint8_t devType;		//报警设备类型
	uint8_t id;				//设备序号
	uint8_t sta;			//报警状态。1：产生。2：恢复
	uint8_t time[6];
};

typedef	struct
{
	uint8_t head;			//包头
	uint8_t jqid[8];		//id
	uint8_t cmd;
	uint16_t len;			//长度。整个数据帧长度
	uint8_t data[500];		
	uint16_t crc;			//crc校验。数据帧除crc外的校验
}COMMU_PACK_FORMAT_T;


void LTE_Restart(void);

uint8_t  LTE_Init(void);

uint8_t LTE_SendDataToWeb(uint8_t *pData, uint16_t len);

uint8_t Commu_Recv_Proc(uint8_t cmd );
uint8_t findByte(uint8_t destByte, uint8_t ms);
uint8_t LTE_Get_RSSI(uint8_t *rssi);
uint8_t LTE_Get_ConnectSta(void);
uint8_t LTE_Close_Net(void);


#endif
