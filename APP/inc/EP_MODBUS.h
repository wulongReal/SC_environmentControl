
#include "stm32f10x.h"

typedef	enum
{
	WT_REC_ID = 1<<0,
	WT_REC_CMD = 1<<1,
	WT_REC_LEN = 1 << 2,
	WT_REC_ERR = 1 << 3,
	WT_REC_IDLE = 1 << 4
}WT_recv_event;



typedef enum 
{
	CMD_REC_START = 1 << 0,
	CMD_REC_ID = 1 << 1, 
	CMD_REC_ADDR = 1 << 2,
	CMD_REC_LEN = 1 << 3,
	CMD_REC_DATE = 1 << 4,
	CMD_REC_CRC = 1 <<5,
	
}DRIVE_BORD_CMD_RECV_EVENT;

void RS485_EP_Get(uint8_t	addr);
void RS485_EP_RecvProc(void);



