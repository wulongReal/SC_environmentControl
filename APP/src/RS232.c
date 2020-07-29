#include "app_task.h"
#include "vars.h"
#include <rtthread.h>
//#include "lcd.h"
#include "rs232.h"
#include "ver.h"
#include "string.h"

//#include "vector.h"
#include "envir.h"
#include "usart.h"
#include "rtc.h"

#pragma diag_suppress 174


uint8_t rs232DataProcSta = 0;

void LinMemCopy(uint8_t * dst, uint8_t * src, uint8_t len) {
  while (len > 0) {
    len--;
    dst[len] = src[len] ;
  }
  return;
}

void lcd_writebyte(uint8_t value)
{
	USART_SendData(UART5,value);
	while(USART_GetFlagStatus(UART5,USART_FLAG_TXE) == RESET);
}

//蜂鸣器驱动时间为time毫秒
void lcd_driveBeep(uint16_t	time)
{
	unsigned char i;
	uint8_t	buf[8] = {0};
	
	buf[0] = 0xa5;
	buf[1] = 0x5a;
	buf[2] = 0x03;
	buf[3] = 0x80;
	buf[4] = 0x02;
	buf[5] = (time/10);
    
	for(i = 0;i < 6;i++)
	{
		lcd_writebyte(buf[i]);
	}		
}
void lcd_SendByte(rt_device_t device,uint8_t *str,uint8_t len) 
{
  	uint8_t i;
  	uint8_t Data[20];   //161026  ?????? ????????
  
	for(i = 0;i < len;i++)
	{
		Data[i] = str[i];
	}
  	rt_device_write(device, 0, &Data[0], len);
}

void	lcd_show_index(unsigned int addr, unsigned int num)
{
	unsigned char i;
	uint8_t	buf[8] = {0};
	
	buf[0] = 0x5a;
	buf[1] = 0xa5;
	buf[2] = 0x05;
	buf[3] = 0x82;
	buf[4] = (addr >> 8) & 0xff;
	buf[5] = addr & 0xff;
	buf[6] = (num >> 8) & 0xff;
	buf[7] = num & 0xff;
    
	for(i = 0;i < 8;i++)
	{
		lcd_writebyte(buf[i]);
	}		
}

void	lcd_show_indexLong(unsigned int addr, uint32_t num)
{
	unsigned char i;
	uint8_t	buf[10] = {0};
	
	buf[0] = 0x5a;
	buf[1] = 0xa5;
	buf[2] = 0x07;
	buf[3] = 0x82;
	buf[4] = (addr >> 8) & 0xff;
	buf[5] = addr & 0xff;
	buf[6] = (num >> 24) & 0xff;
	buf[7] = (num >> 16) & 0xff;
	buf[8] = (num >> 8) & 0xff;
	buf[9] = num & 0xff;
    
	for(i = 0;i < 10;i++)
	{
		lcd_writebyte(buf[i]);
	}		
}


//变量显示。适用于数值显示、图标显示
//输入：变量地址
//输出：变量值
/*
void	lcd_show_index(unsigned int addr, unsigned int num)
{
	unsigned char i;
	uint8_t	buf[8] = {0};
	
	buf[0] = 0xa5;
	buf[1] = 0x5a;
	buf[2] = 0x05;
	buf[3] = 0x82;
	buf[4] = (addr >> 8) & 0xff;
	buf[5] = addr & 0xff;
	buf[6] = (num >> 8) & 0xff;
	buf[7] = num & 0xff;
    
	for(i = 0;i < 8;i++)
	{
		lcd_writebyte(buf[i]);
	}		
}
*/
//弹窗显示
//输入：弹窗对应的ID
void lcd_show_window(unsigned char id)
{
	unsigned char i;
	uint8_t	buf[6] = {0};

	buf[0] = 0xa5;
	buf[1] = 0x5a;
	buf[2] = 0x03;
	buf[3] = 0x80;
	buf[4] = 0x4f;
	buf[5] = id & 0xff;
	for(i = 0;i < 6;i++)
	{
		lcd_writebyte(buf[i]);
	}
}

//控制屏幕切换至目标ID
//输入 目标图片ID
//5A A5 07 82 0084 5A01 0003
void lcd_select_pic(unsigned int  pic_num)
{
	unsigned char i;
	uint8_t	buf[10] = {0};

	buf[0] = 0x5A;
	buf[1] = 0xa5;
	buf[2] = 0x07;
	buf[3] = 0x82;
	buf[4] = 0x00;
	buf[5] = 0x84;
	buf[6] = 0x5a;
	buf[7] = 0x01;
	
	buf[8] = (pic_num >> 8) & 0xff;
	buf[9] = pic_num & 0xff;

	for(i = 0;i < 10;i++)
		lcd_writebyte(buf[i]);
}

/*
void lcd_select_pic(unsigned int  pic_num)
{
	unsigned char i;
	uint8_t	buf[7] = {0};

	buf[0] = 0xa5;
	buf[1] = 0x5a;
	buf[2] = 0x04;
	buf[3] = 0x80;
	buf[4] = 0x03;
	buf[5] = (pic_num >> 8) & 0xff;
	buf[6] = pic_num & 0xff;

	for(i = 0;i < 7;i++)
		lcd_writebyte(buf[i]);
}
*/
//发送命令，获取液晶页面ID
void lcd_get_pic_num(void)
{
	unsigned char i;

	unsigned char send_buf[6] = {0xA5, 0x5A, 0x03, 0x81, 0x03, 0x02};
	
	for(i = 0;i < 6;i++)
		lcd_writebyte(send_buf[i]);
}
//发送命令，获取液晶RTC
void lcd_get_rtc(void)
{
	unsigned char i;

	unsigned char send_buf[6] = {0xA5, 0x5A, 0x03, 0x81, 0x20, 0x07};
	
	for(i = 0;i < 6;i++)
		lcd_writebyte(send_buf[i]);
}
void lcd_show_CodePic(uint16_t addr, uint8_t *pDate, uint8_t len)
{
	//示例5A A51C82524068 74 74 70 3A 2F 2F 77 77 77 2E 64 77 69 6E 2E 63 6F 6D 2E 63 6E 2FFFFF
	uint8_t buf[300] = {0x20};
	uint16_t i = 0;

	if(len >= 255)
	{
		len = 255;
	}
	
	buf[0] = 0x5a;
	buf[1] = 0xa5;
	buf[2] = len+5;
	buf[3] = 0x82;
	buf[4] = (addr >> 8) & 0xff;
	buf[5] = addr & 0xff;
	for(i = 0;i < len;i++)
		buf[6+i] = pDate[i];
	buf[6+i] = 0xff;
	buf[7+i] = 0xff;

	for(i = 0;i <len+8;i++)
		lcd_writebyte(buf[i]);
}
//液晶文本显示最多显15个汉字
#define	LCD_TXT_LEN_MAX	20
void lcd_show_txt(uint16_t addr, uint8_t *pBuf, uint8_t len)
{
	//示例  5A A5 09 82 5020 B1B1 3132 4142
	uint8_t buf[50] = {0x20};
	uint8_t i;
	memset(buf, 0x20, 50);
	if(len > LCD_TXT_LEN_MAX)
	{
		len = LCD_TXT_LEN_MAX;
	}

	
	buf[0] = 0x5a;
	buf[1] = 0xa5;
	buf[2] = len+3;
	buf[3] = 0x82;
	buf[4] = (addr >> 8) & 0xff;
	buf[5] = addr & 0xff;
	for(i = 0;i < len;i++)
		buf[6+i] = pBuf[i];

	for(i = 0;i <len+6;i++)
		lcd_writebyte(buf[i]);
}

void lcd_show_window_T5L(uint8_t keyCode)
{
	uint8_t buf[14] = {0x5A, 0xA5, 0x0B, 0x82, 0x00, 0xD4, 0x5A, 0xA5, 0x00, 0x04, 0xFF, 0x01, 0x00, 0x01};
	uint8_t i = 0;

	buf[11] = keyCode;
	
	for(i = 0;i <14;i++)
		lcd_writebyte(buf[i]);
}
/*
#define	TXT_SHOW_LEN_MAX	15
void	lcd_show_txt(unsigned int addr, uint8_t *str, uint8_t len)
{
	unsigned char i;
	uint8_t	buf[21] = {0};
	
	if(len > TXT_SHOW_LEN_MAX)
		return;
	memset(buf, 0x20, sizeof(buf));
	buf[0] = 0xa5;
	buf[1] = 0x5a;
	buf[2] = len+3;
	buf[3] = 0x82;
	buf[4] = (addr >> 8) & 0xff;
	buf[5] = addr & 0xff;

	//if(len <= (14-6))
	{
		for(i = 0;i <= len;i++)
		{
			buf[6 + i] = str[i];
		}
	}
	
	for(i = 0;i < sizeof(buf);i++)
	{
		lcd_writebyte(buf[i]);
	}
}
*/

void LIN_StaClr(void){
  LIN_MsgDir = LINMSG_IGN;
  LIN_StateFlags = LIN_FLAG_IGNORE;
  LIN_BufCounter = 0;
  LIN_Length =0;
 //Checksum =0;
}

//LCD接收数据格式：A5 5A LEN CMD ADDR LEN2 DATA
//A5 5A 05 81 03 02 00 01//获取到到页面ID�?。地址�?3
//A5 5A 0A 81 20 07 17 12 14 04 13 38 40 //获取到时间为2017-12-14 周四 13:38:40.地址�?x20
//A5 5A 06 83 00 00 02 00 01//按键或者输入。地址00 00 �?00 01

void LIN_RxCompleted(void)
{  
		
  	if (LIN_TmpSCIByte == LCD_CMD_STAT1)
	{
    	LIN_ProtBuf[1] =0;
    	LIN_StateFlags = LCD_RCV_STAT1;
  	}
  	if(LIN_StateFlags == LCD_FLAG_IGNORE)
 	{
 	
  	}
  	else if (LIN_StateFlags == LCD_RCV_STAT1)
	{
    	LIN_ProtBuf[0] = LCD_CMD_STAT1;
    	LIN_BufCounter=1;
    	LIN_StateFlags = LCD_RCV_STAT2;
  	}
	else if(LIN_StateFlags == LCD_RCV_STAT2)
	{
		if(LIN_TmpSCIByte == LCD_CMD_STAT2)
		{
			LIN_ProtBuf[1] = LCD_CMD_STAT2;
			LIN_BufCounter = 2;
			LIN_StateFlags = LCD_RCV_LEN;
		}
		else
		{
			LIN_StaClr();
		}
	}
	else if(LIN_StateFlags == LCD_RCV_LEN)
	{
		if(LIN_TmpSCIByte <= 0x0a)
		{
			LIN_ProtBuf[2] = LIN_TmpSCIByte;
			LIN_Length = LIN_TmpSCIByte;
			LIN_BufCounter = 3;
			LIN_StateFlags = LCD_RCV_DATA;
		}
		else
		{
			LIN_StaClr();
		}
	}
	else if(LIN_StateFlags == LCD_RCV_DATA)
	{
		LIN_ProtBuf[LIN_BufCounter] = LIN_TmpSCIByte;
		LIN_BufCounter++;
		if(LIN_BufCounter >= (LIN_Length + 3))
		{
			LIN_StaClr();
			if(LIN_ProtBuf[3] == 0X83)
			{
				FLAGS_LCD_HAS_DATE = 1;
				LinMemCopy(recvDataBuf, LIN_ProtBuf, MAX_LIN_DATANUMB);
			}
			
		}
	}
	/*

	LIN_ProtBuf[LIN_BufCounter++] = LIN_TmpSCIByte;

	if(LIN_BufCounter >= 6)
	{
		if((LIN_ProtBuf[0] == 0x5a)&&(LIN_ProtBuf[1] == 0xa5))
		{
			if(LIN_BufCounter == (LIN_ProtBuf[2] + 3))
			{
				LIN_StaClr();
				FLAGS_LCD_HAS_DATE = 1;
				LinMemCopy(recvDataBuf, LIN_ProtBuf, MAX_LIN_DATANUMB);

			}
		}
		else
		{
			LIN_StaClr();
		}
	}
	*/
}

//配置结构：日�?》温度N-》设备N

void	recvCfgProc(uint16_t	addr, uint16_t	data)
{
	int8_t	dayIndex = 0;
	int8_t	tempIndex = 0;
	int8_t	rowIndex = 0;
	//uint16_t temp = 0;
	//uint8_t	yushu = 0;

	//获得所属日龄表
	dayIndex = (addr - DAY1_TEMP1_VALUE_INPUT)/DAY_STEP_INPUT_TIMEVAL;
	//获得所属温度行
	tempIndex = (addr - DAY1_TEMP1_VALUE_INPUT)%DAY_STEP_INPUT_TIMEVAL/DAY_TEMP_VALUE_INPUT_TIMEVAL;
	//获得所属该行位�?
	rowIndex = (addr - DAY1_TEMP1_VALUE_INPUT)%DAY_STEP_INPUT_TIMEVAL%DAY_TEMP_VALUE_INPUT_TIMEVAL/DAY_TEMP_DEV_INPUT_TIMEVAL;
	//devIndex = (addr - DAY1_TEMP1_VALUE_INPUT)%DAY_STEP_INPUT_TIMEVAL%DAY_TEMP_VALUE_INPUT_TIMEVAL%DAY_TEMP_DEV_INPUT_TIMEVAL;
	if((dayIndex < 0) || (dayIndex > (DAY_LEVEL_MAX-1)))
	{
		return ;
	}
	if((tempIndex < 0) || (tempIndex > (TEMP_LEVEL_MAX-1)))
	{
		return ;
	}
	if((rowIndex < 0) || (rowIndex > allDevMax))
	{
		return ;
	}
	curTopMenumId = PAGE_DAY_LEVEL_CFG;
	curSecMenumId = dayIndex + 1;
	//FLAGS4_TOP_MENUM_PRESS = 1;
	//如果该行位置�?则代表为温度�?
	if(rowIndex == 0)
	{
		tempModeCfg.dayCtrlMode[dayIndex].tempCtrlMode[tempIndex].tempMax = data;
	}
	else
	{
		if(((rowIndex - 1) >= Fan1) && ((rowIndex - 1) <= ventWindow))//非变频设�?
		{
			tempModeCfg.dayCtrlMode[dayIndex].tempCtrlMode[tempIndex].sta[rowIndex-1] = !tempModeCfg.dayCtrlMode[dayIndex].tempCtrlMode[tempIndex].sta[rowIndex-1];
		}
		else
		{
			if(data >= SPEED_FAN_VALUE_MAX)//变频的值最�?00
			{
				data = SPEED_FAN_VALUE_MAX;
			}
			tempModeCfg.dayCtrlMode[dayIndex].tempCtrlMode[tempIndex].sta[rowIndex-1] = data;
		}
			
	}
}


uint8_t	curRtcTime[6] = {0};
int16_t passWord = 0;


//将接收到�?6进制的RTC转换�?0进制的RTC
int8_t	rtcTransFunc(uint8_t *time)
{
	//uint8_t	i = 0;
	int16_t	year = time[0];
	int8_t		mon = time[1];
	int8_t		day = time[2];
	int8_t		hour = time[3];
	int8_t		min	= time[4];
	int8_t		sec = time[5];
	int16_t	temp1 = 0,temp2 = 0,temp3 = 0;

	if(year < 1 || year > 0x99)	
	{
		return -1;
	}
	temp1 = year / 256;
	temp2 = (year % 256)/16;
	temp3 = (year%256)%16;
	year = temp1*100+temp2*10+temp3 + 2000;//格式转换�?0xx

	if(mon < 1 || mon > 0x12)
		return -1;
	temp1 = mon / 16;
	temp2 = mon %16;
	mon = temp1 * 10 + temp2;

	if(day < 1 || day > 0x31)
		return -1;
	temp1 = day / 16;
	temp2 = day %16;
	day = temp1 * 10 + temp2;

	if(hour < 0 || hour > 0x23)
		return -1;
	temp1 = hour / 16;
	temp2 = hour %16;
	hour = temp1 * 10 + temp2;

	if(min < 0 || min > 0x59)
		return -1;
	temp1 = min / 16;
	temp2 = min %16;
	min = temp1 * 10 + temp2;

	if(sec < 0 || sec > 0x59)
		return -1;
	temp1 = sec / 16;
	temp2 = sec %16;
	sec = temp1 * 10 + temp2;

	curRtc.year = year;curRtc.month = mon;curRtc.day = day;
	curRtc.hour = hour;curRtc.min = min;curRtc.sec = sec;

	return 0;
}
void SCI_RecData(void)
{
	uint8_t cmdIndex = 0;//命令类型
	uint16_t addr = 0;//寄存器地址
	uint8_t returnValueLen = 0;//按键或者输入框返回值长度�?
	int32_t dataValue = 0;
	uint8_t i;
	uint8_t index = 0;
	int8_t	tmp = 0;
	int16_t tempData = 0;
	static sensorAjust_t	sensorAjustValue;
	
	
	cmdIndex = recvDataBuf[3];
	returnValueLen = recvDataBuf[6];

	lcdRecvAddr = cmdIndex;
	if(cmdIndex == LCD_CMD_SN_1)
	{
		
		addr = recvDataBuf[4];
		if(addr == 0x03)//读取页面ID
		{
			curPicId = (recvDataBuf[6] << 8) + recvDataBuf[7];
			lcdRecvData = curPicId;
		}
		else if(addr == 0x20)//读取RTC
		{
			lcdRecvData = 0;
			curRtcTime[0] = recvDataBuf[6];curRtcTime[1] = recvDataBuf[7];curRtcTime[2] = recvDataBuf[8];
			curRtcTime[3] = recvDataBuf[10];curRtcTime[4] = recvDataBuf[11];curRtcTime[5] = recvDataBuf[12];
			
			//curRtc.year = curRtcTime[0];curRtc.month = curRtcTime[1];curRtc.day = curRtcTime[2];
			//curRtc.hour = curRtcTime[3];curRtc.min = curRtcTime[4];curRtc.sec = curRtcTime[5];
			
			tmp = rtcTransFunc(curRtcTime);
			if(tmp >= 0)
			{
				//FLAGS3_RTC_GET_SUC = 1;
			}
		}
		return;
	}
	else if(cmdIndex == LCD_CMD_SN_2)
	{
		addr = (recvDataBuf[4] << 8) + recvDataBuf[5];
		returnValueLen = recvDataBuf[6];
		for(i = 0;i < (returnValueLen*2);i++)
		{
			dataValue <<= 8;
			dataValue += recvDataBuf[7+i];
		}
		lcdRecvAddr = addr;
		lcdRecvData = dataValue;
	}

	//FLAGS3_NO_PRESS = 0;
	noPressCnt = 0;


	if((addr >=DAY1_DAYNUM_SHOW_2) && (addr <= DAY_TEMP_DEV_CFG_SAVE_BUT_INPUT))
	{
		//FLAGS3_DO_CTL_PIC = 1;
		if(addr == DAY_TEMP_DEV_CFG_SAVE_BUT_INPUT)
		{
			lcdCtrlSaveId = dataValue;
			//FLAGS3_SAVE_CTRL_CFG = 1;
			return;
		}
		else if(addr == AUTO_CFG_BUT_INPUT)
		{
				
		}
		recvCfgProc(addr, dataValue);
		return;
	}
	else if((addr >= HEAT_DEV_STOP_SHIFT) && (addr <= DEV_SAVE_BUT))
	{
		//FLAGS3_DO_DEV_PIC = 1;
		//FLAGS4_TOP_MENUM_PRESS = 1;
		onlyRefreshData = 1;
		curTopMenumId = PAGE_DEV_CTRL;
	}
	else if((addr >= ADMIN_PASSWORD_INPUT) && (addr <= ALARM_CFG_SAVE_BUT_INPUT))
	{
		//FLAGS3_DO_MANAGE_PIC = 1;
				//FLAGS4_TOP_MENUM_PRESS = 1;
		onlyRefreshData = 1;
		curTopMenumId = PAGE_MANAGEMENT;
	}
	else if((addr >= TEMP1_AJUST_INPUT) && (addr <= SENSOR_AJUST_SAVE_BUT_INPUT))
	{
		//FLAGS3_DO_AJUST_PIC = 1;
		//		FLAGS4_TOP_MENUM_PRESS = 1;
		onlyRefreshData = 1;
		curTopMenumId = PAGE_ADJUST;
	}
	else if((addr >= TEMP_SENSOR_NUM_INPUT) && (addr <= DEBUG_CTRL_INPUT))
	{
		//FLAGS3_DO_INSTALL_PIC = 1;
		//		FLAGS4_TOP_MENUM_PRESS = 1;
		onlyRefreshData = 1;
		curTopMenumId = PAGE_INSTALL;
	}
	
	switch(addr)
	{
		case	MENUM_BUT_INPUT:
		{
			//FLAGS3_IS_PRESS_MENU = 1;
			break;
		}
		case	PASSCODE_INPUT:
		{
			passWord = dataValue;
			break;
		}

		case	LOAD_BUT_INUPT:
		{
			if(dataValue == 0x01)
			{
				//FLAGS3_IS_NEED_LOAD = 1;
			}
			break;
		}
		#if	PIC_CTL
		case	CTRL_PAGE_BUT_INPUT:
		{
			if((dataValue >= 0) && (dataValue <= 7))
			{
				curSecMenumId = dataValue;//次级菜单为日龄
				curTopMenumId = PAGE_DAY_LEVEL_CFG;
			}
			else
			{
				curSecMenumId = 0;		//目标温度页面
				curTopMenumId = PAGE_TARGET_TEMP;
			}
			
			
			//FLAGS4_TOP_MENUM_PRESS = 1;
			break;
		}
		case	MENUM_TOP_SELECT_BUT_INPUT:
		{
			if((dataValue >= 1) && (dataValue <= 6))
			{
				curTopMenumId = dataValue;
			}
			else
			{
				curTopMenumId = PAGE_EH_SHOW;	
			}
			//FLAGS4_TOP_MENUM_PRESS = 1;
			break;
		}

		case	CFG_AUTO_BUT_INPUT:
		{
			if(dataValue == 1)
			{
				//FLAGS4_IS_NEED_CFG_AUTO_CREATE = TRUE;
			}
			break;
		}
		case	SEASON_INPUT:
		{
			if((dataValue <= (uint16_t)winterMode)&&(dataValue <= (uint16_t)winterMode))
			{
				if(dataValue != devInfo.curSeason)
				{
					//FLAGS3_REFRESH_LCD_2 = 1;	
				}
				devInfo.curSeason = dataValue;
				tempModeCfg = devModeCfg[devInfo.curSeason];
			}
			else
			{
				devInfo.curSeason = springMode;
				tempModeCfg = devModeCfg[devInfo.curSeason];
				//FLAGS3_REFRESH_LCD_2 = 1;	
			}
			break;
		}
		//温度曲线
		case	DAY1_NUM_INPUT:
		case	(DAY1_NUM_INPUT + DAY_NUM_INPUT_INTERVAL):
		case	(DAY1_NUM_INPUT + DAY_NUM_INPUT_INTERVAL*2):
		case	(DAY1_NUM_INPUT + DAY_NUM_INPUT_INTERVAL*3):
		case	(DAY1_NUM_INPUT + DAY_NUM_INPUT_INTERVAL*4):
		case	(DAY1_NUM_INPUT + DAY_NUM_INPUT_INTERVAL*5):
		case	(DAY1_NUM_INPUT + DAY_NUM_INPUT_INTERVAL*6):
		{
			index = (addr - DAY1_NUM_INPUT)/DAY_NUM_INPUT_INTERVAL;
			if(index < DAY_LEVEL_MAX)
			{
				if(dataValue <= DAY_NUM_VALUE_MAX)
				{
					tempModeCfg.targetTemp[index].dayCount = dataValue;
					tempModeCfg.dayCtrlMode[index].dayNum = dataValue;
				}
			}
			break;
		}
		case	TAR1_TEMP_INPUT:
		case	(TAR1_TEMP_INPUT + DAY_NUM_INPUT_INTERVAL*1):
		case	(TAR1_TEMP_INPUT + DAY_NUM_INPUT_INTERVAL*2):
		case	(TAR1_TEMP_INPUT + DAY_NUM_INPUT_INTERVAL*3):
		case	(TAR1_TEMP_INPUT + DAY_NUM_INPUT_INTERVAL*4):
		case	(TAR1_TEMP_INPUT + DAY_NUM_INPUT_INTERVAL*5):
		case	(TAR1_TEMP_INPUT + DAY_NUM_INPUT_INTERVAL*6):
		{
			if(index < DAY_LEVEL_MAX)
			{
				if(dataValue <= TARGET_TEMP_VALUE_MAX)
				{
					index = (addr - TAR1_TEMP_INPUT)/DAY_NUM_INPUT_INTERVAL;
					tempModeCfg.targetTemp[index].temp	= dataValue;
				}
			}
			break;
		}
		case	TARGET_SAVE_BUT_INPUT:
		{
			lcdCtrlSaveId = 0;
			//FLAGS3_SAVE_TARGET = 1;
			//FLAGS3_SAVE_CTRL_CFG = 1;
			break;
		}

		case	HOT1_TEMP_INPUT:
		case	HOT1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*1:	
		case	HOT1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*2:
		case	HOT1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*3:
		case	HOT1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*4:
		case	HOT1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*5:
		case	HOT1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*6:
		{
			if(index < DAY_LEVEL_MAX)
			{
				if(dataValue <= TARGET_TEMP_VALUE_MAX)
				{
					index = (addr - HOT1_TEMP_INPUT)/	DAY_NUM_INPUT_INTERVAL;
					tempModeCfg.dayCtrlMode[index].heatDevCfg.startParam  = dataValue;
				}
			}
			break;
		}
			
		case	HIGH1_TEMP_INPUT:
		case	HIGH1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*1:
		case	HIGH1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*2:
		case	HIGH1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*3:
		case	HIGH1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*4:
		case	HIGH1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*5:
		case	HIGH1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*6:
		{
			if(index < DAY_LEVEL_MAX)
			{
				if(dataValue <= TARGET_TEMP_VALUE_MAX)
				{
					index = (addr - HIGH1_TEMP_INPUT)/	DAY_NUM_INPUT_INTERVAL;
					tempModeCfg.targetTemp[index].high_temp  = dataValue;
				}
			}
			break;
		}
		case	LOW1_TEMP_INPUT:
		case	LOW1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*1:
		case	LOW1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*2:
		case	LOW1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*3:
		case	LOW1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*4:
		case	LOW1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*5:
		case	LOW1_TEMP_INPUT+DAY_NUM_INPUT_INTERVAL*6:
		{
			if(index < DAY_LEVEL_MAX)
			{
				if(dataValue <= TARGET_TEMP_VALUE_MAX)
				{
					index = (addr - LOW1_TEMP_INPUT)/	DAY_NUM_INPUT_INTERVAL;
					tempModeCfg.targetTemp[index].low_temp  = dataValue;
				}
			}
			break;
		}

		
			
		//配置参数
		#if 0
		//1日龄下温度档位�?
		case	DAY1_TEMP1_VALUE_INPUT:
		case	DAY1_TEMP1_VALUE_INPUT+DAY_TEMP_VALUE_INPUT_TIMEVAL*1:
		case	DAY1_TEMP1_VALUE_INPUT+DAY_TEMP_VALUE_INPUT_TIMEVAL*2:
		case	DAY1_TEMP1_VALUE_INPUT+DAY_TEMP_VALUE_INPUT_TIMEVAL*3:
		case	DAY1_TEMP1_VALUE_INPUT+DAY_TEMP_VALUE_INPUT_TIMEVAL*4:
		case	DAY1_TEMP1_VALUE_INPUT+DAY_TEMP_VALUE_INPUT_TIMEVAL*5:
		case	DAY1_TEMP1_VALUE_INPUT+DAY_TEMP_VALUE_INPUT_TIMEVAL*6:
		{
			index = (addr - DAY1_TEMP1_VALUE_INPUT)/DAY_TEMP_VALUE_INPUT_TIMEVAL;
			devModeCfg[seasonMode].dayCtrlMode[0].tempCtrlMode[index].tempMax = dataValue;
			FLAGS3_DO_CTL_PIC = 1;
			break;
		}
		//1日龄�?温度下所有设备的运行状态设�?
		case	DAY1_TEMP1_FAN1_INPUT:
		case	DAY1_TEMP1_FAN1_INPUT +DAY_TEMP_DEV_INPUT_TIMEVAL*1:
		case	DAY1_TEMP1_FAN1_INPUT +DAY_TEMP_DEV_INPUT_TIMEVAL*2:
		case	DAY1_TEMP1_FAN1_INPUT +DAY_TEMP_DEV_INPUT_TIMEVAL*3:
		case	DAY1_TEMP1_FAN1_INPUT +DAY_TEMP_DEV_INPUT_TIMEVAL*4:
		case	DAY1_TEMP1_FAN1_INPUT +DAY_TEMP_DEV_INPUT_TIMEVAL*5:
		case	DAY1_TEMP1_FAN1_INPUT +DAY_TEMP_DEV_INPUT_TIMEVAL*6:
		case	DAY1_TEMP1_FAN1_INPUT +DAY_TEMP_DEV_INPUT_TIMEVAL*7:
		case	DAY1_TEMP1_FAN1_INPUT +DAY_TEMP_DEV_INPUT_TIMEVAL*8:
		case	DAY1_TEMP1_FAN1_INPUT +DAY_TEMP_DEV_INPUT_TIMEVAL*9:
		{
			index = (addr - DAY1_TEMP1_FAN1_INPUT)/DAY_TEMP_DEV_INPUT_TIMEVAL;
			devModeCfg[seasonMode].dayCtrlMode[0].tempCtrlMode[0].sta[index] = dataValue;
			FLAGS3_DO_CTL_PIC = 1;
			break;	
		}
		#endif 

		case	DAY_TEMP_DEV_CFG_SAVE_BUT_INPUT:
		{
			//FLAGS3_SAVE_CTRL_CFG =1;
			if(dataValue < DAY_LEVEL_MAX)
			{
				lcdCtrlSaveId = dataValue;
			}
			break;
		}
		#endif

		#ifdef	PIC_DEV
		case	HEAT_DEV_STOP_SHIFT:
		{
			tempModeCfg.heatStopShift = dataValue;
			break;
		}
		case	COLD_DEV_START_SHIFT:
		{
			tempModeCfg.coldDevCfg.startShift = dataValue;
			break;
		}
		case	COLD_DEV_STOP_SHIFT:
		{
			tempModeCfg.coldDevCfg.stopShift = dataValue;
			break;
		}
		case	COLD_DEV_STOP_SHIFT_HUMI:
		{
			tempModeCfg.coldDevCfg.stopParam = dataValue;
			break;
		}
		case	HIGH_HUMI_VALUE_INPUT:
		{
			alarmCfg.devAlarmCfg[highHumiAlarm].condition = dataValue;
			break;
		}
		case	DEV_SAVE_BUT:
		{
			//FLAGS3_SAVE_DEV_CFG = 1;
			devSaveId = dataValue;
			break;
		}
		#endif

		#ifdef PIC_MANAGER
		case	ADMIN_PASSWORD_INPUT:
		{
			//if(FLAGS3_IS_ADMIN_USER)
			{
				devInfo.adminPassword = dataValue;
			}
			break;
		}
		case	USER1_PASSWORD_INPUT:
		{
			//if(FLAGS3_IS_ADMIN_USER)
			{
				devInfo.user1Password = dataValue;
			}
			break;
		}
		case	USER2_PASSWORD_INPUT:
		{
			//if(FLAGS3_IS_ADMIN_USER)
			{
				devInfo.user2Password = dataValue;
			}
			break;
		}
		case	ROOM_ID_HEAD_INPUT:
		{
			devInfo.roomIdHead = dataValue;
			break;
		}
		case	ROOM_ID_TAIL_INPUT:
		{
			devInfo.roomIdTail = dataValue;
			break;
		}
		case	ANIMAL_NUM_INPUT:
		{
			devInfo.animalNum = dataValue;
			break;
		}
		case	FEED_LEVEL_INPUT:
		{
			if(dataValue <= feedLevelMax)
			{
				devInfo.feedLevel = dataValue;
			}
			break;
		}
		case	AVG_WEIGHT_INPUT:
		{
			devInfo.avgWeight = dataValue;
			break;
		}

		case	ROOM_DAY_NUM_INPUT:
		{
			if(dataValue > DAY_NUM_VALUE_MAX)
			{
				dataValue = DAY_NUM_VALUE_MAX;
			}
			if(dataValue <= DAY_NUM_VALUE_MAX)
			{
				dataValue = DAY_NUM_VALUE_MIN;
			}
			devInfo.saveDayNum = dataValue;
			//FLAGS3_MODIFY_DAY_NUM = 1;
			devInfo.saveRtc = curRtc;
			break;
		}

		case	ALARM1_EN_BUT_INPUT:
		case	ALARM1_EN_BUT_INPUT + ALARM_EN_INPUT_TIMEVAL*1:
		case	ALARM1_EN_BUT_INPUT + ALARM_EN_INPUT_TIMEVAL*2:
		case	ALARM1_EN_BUT_INPUT + ALARM_EN_INPUT_TIMEVAL*3:
		case	ALARM1_EN_BUT_INPUT + ALARM_EN_INPUT_TIMEVAL*4:
		case	ALARM1_EN_BUT_INPUT + ALARM_EN_INPUT_TIMEVAL*5:
		{
			index = (addr - ALARM1_EN_BUT_INPUT)/ALARM_EN_INPUT_TIMEVAL;
			alarmCfg.devAlarmCfg[index].en = !alarmCfg.devAlarmCfg[index].en;
			break;
		}
		case	ALARM1_VALUE_BUT_INPUT:
		case	ALARM1_VALUE_BUT_INPUT + ALARM_EN_INPUT_TIMEVAL*1:
		case	ALARM1_VALUE_BUT_INPUT + ALARM_EN_INPUT_TIMEVAL*2:
		case	ALARM1_VALUE_BUT_INPUT + ALARM_EN_INPUT_TIMEVAL*3:
		case	ALARM1_VALUE_BUT_INPUT + ALARM_EN_INPUT_TIMEVAL*4:
		case	ALARM1_VALUE_BUT_INPUT + ALARM_EN_INPUT_TIMEVAL*5:
		{
			index = (addr - ALARM1_VALUE_BUT_INPUT)/ALARM_EN_INPUT_TIMEVAL;
			if(dataValue >= ALARM_CONFIRM_TIME_MAX)
			{
				dataValue = ALARM_CONFIRM_TIME_MAX;//报警确认时间为200*2 = 400S
			}
			alarmCfg.devAlarmCfg[index].param = dataValue;
			break;
		}
		case	ALARM_HIGH_HUMI_VALUE_INPUT:
		{
			alarmCfg.devAlarmCfg[highHumiAlarm].condition = dataValue;
			break;	
		}
		case	ALARM_CFG_SAVE_BUT_INPUT:
		{
			//FLAGS3_SAVE_MANAGER_CFG = 1;
			break;
		}
		#endif

		#ifdef	PIC_HISTORY
		#endif

		#ifdef	PIC_AJUST
		case	TEMP1_AJUST_INPUT:
		case	TEMP1_AJUST_INPUT+TEMP_AJUST_INPUT_TIMEVAL*1:
		case	TEMP1_AJUST_INPUT+TEMP_AJUST_INPUT_TIMEVAL*2:
		{
			//FLAGS3_AJUST_VALUE_CLR = 0;
			index = (addr - TEMP1_AJUST_INPUT)/TEMP_AJUST_INPUT_TIMEVAL;
			/*
			tempData = dataValue - (Dev_info[TEMP1 -1].Data1/10);
			tempData = abs(tempData);
			if(tempData <= TEMP_ADJUST_VALUE_MAX)//可校准范围在20%内
			{
				tempData = (Dev_info[TEMP1 -1].Data1/10);
				sensorAjustValue.tempAjustValue[index] =  dataValue - tempData;
			}
			else
			{
				sensorAjustValue.tempAjustValue[index] = sensorCfg.sensorAjust.tempAjustValue[i];
				dataValue = Dev_info[TEMP1 -1+index].Data1/10;
			}
			*/
			if(abs(dataValue - (Dev_info[TEMP1 -1].Data1/10)) <= TEMP_ADJUST_VALUE_MAX)//可校准范围在20%内
			{
				sensorAjustValue.tempAjustValue[index] =  dataValue - (Dev_info[TEMP1 -1].Data1/10);
			}
			else
			{
				sensorAjustValue.tempAjustValue[index] = sensorCfg.sensorAjust.tempAjustValue[i];
				dataValue = Dev_info[TEMP1 -1+index].Data1/10;
			}
			sensorAjustTemp.tempAjustValue[index] =  dataValue;
			break;
		}
		case	HUMI_AJUST_INPUT:
		{
			//FLAGS3_AJUST_VALUE_CLR = 0;
			index = (addr - HUMI_AJUST_INPUT)/TEMP_AJUST_INPUT_TIMEVAL;
			/*
			tempData = dataValue - (Dev_info[HUMI1 -1].Data1/10);
			tempData = abs(tempData);
			if(tempData <= HUMI_ADJUST_VALUE_MAX)//可校准范围在20%内
			{
				tempData = (Dev_info[HUMI1 -1].Data1/10);
				sensorAjustValue.humiAjustValue[index] =  dataValue - tempData;
			}
			*/
			if(abs(dataValue - (Dev_info[HUMI1 -1].Data1/10)) <= HUMI_ADJUST_VALUE_MAX)//可校准范围在20%内
			{
				sensorAjustValue.humiAjustValue[index] =  dataValue - (Dev_info[HUMI1 -1].Data1/10);
			}
			else
			{
				sensorAjustValue.humiAjustValue[index] = sensorCfg.sensorAjust.humiAjustValue[i];
				dataValue = Dev_info[HUMI1 -1].Data1/10;
			}
			sensorAjustTemp.humiAjustValue[index] =  dataValue; 
			break;
		}
		case	SENSOR_AJUST_SAVE_BUT_INPUT:
		{
			for(i = 0;i < sensorCfg.tempSensorNum;i++)
			{
				if(sensorAjustValue.tempAjustValue[i] != 0)
				{
					sensorCfg.sensorAjust.tempAjustValue[i] = sensorAjustValue.tempAjustValue[i];
				}
			}
			for(i = 0;i < sensorCfg.humiSensorNum;i++)
			{
				if(sensorAjustValue.humiAjustValue[i] != 0)
				{
					sensorCfg.sensorAjust.humiAjustValue[i] = sensorAjustValue.humiAjustValue[i];
				}
			}
			//FLAGS3_SAVE_SENSOR_AJUST = 1;
			memset(&sensorAjustTemp, 0, sizeof(sensorAjust_t));
			memset(&sensorAjustValue, 0, sizeof(sensorAjust_t));
			//FLAGS3_AJUST_VALUE_CLR = 1;
			break;
		}	
		#endif

		#ifdef	PIC_INSTALL
		case	TEMP_SENSOR_NUM_INPUT:
		{
			if(dataValue > TEMP_SENSOR_NUM_MAX)
			{
				dataValue = TEMP_SENSOR_NUM_MAX;

			}
			else if(dataValue == 0)
			{
				dataValue = 1;
			}
				
			sensorCfg.tempSensorNum = dataValue;
			if(sensorCfg.tempSensorNum == 1)
			{
				Dev_info[TEMP1 - 1].Dev_EN = 1;
				Dev_info[TEMP2 - 1].Dev_EN = 0;
				Dev_info[TEMP3 - 1].Dev_EN = 0;
			}
			if(sensorCfg.tempSensorNum == 2)
			{
				Dev_info[TEMP1 - 1].Dev_EN = 1;
				Dev_info[TEMP2 - 1].Dev_EN = 1;
				Dev_info[TEMP3 - 1].Dev_EN = 0;
			}
			if(sensorCfg.tempSensorNum == 3)
			{
				Dev_info[TEMP1 - 1].Dev_EN = 1;
				Dev_info[TEMP2 - 1].Dev_EN = 1;
				Dev_info[TEMP3 - 1].Dev_EN = 1;
			}
	
			
			break;
		}
		case	HUMI_SENSOR_NUM_INPUT:
		{
			if(dataValue > HUMI_SENSOR_NUM_MAX)
			{
				dataValue = HUMI_SENSOR_NUM_MAX;
			}
				
			sensorCfg.humiSensorNum = dataValue;
			if(sensorCfg.tempSensorNum == 1)
			{
				Dev_info[HUMI1 - 1].Dev_EN = 1;

			}
			else
			{
				Dev_info[HUMI1 - 1].Dev_EN = 0;
			}
			break;
		}
		case	OUTDOOR_SENSOR_ID_INPUT:
		{
			sensorCfg.outDoorSensorId = dataValue;
			break;
		}
		case	DEV_ID_INPUT:
		{
			devInfo.devId = dataValue;
			break;
		}
		case	INSTALL_SAVE_BUT_INPUT:
		{
			//FLAGS3_SAVE_INSTALL = 1;
			//FLAGS3_SAVE_SENSOR_AJUST = 1;
			break;
		}
		case	DEBUG_CTRL_INPUT:
		{
			//if(FLAGS3_DEBUG_ENTER == 0)
			{
				lcd_show_window(ENTER_DEBUG_WINDOW);
				//FLAGS3_DEBUG_ENTER = 1;
			}
				
			//else if(FLAGS3_DEBUG_ENTER == 1)
			{
				//FLAGS3_DEBUG_ENTER = 0;
				lcd_show_window(EXIT_DEBUG_WINDOW);
			}
				
			//if(FLAGS3_DEBUG_ENTER == 0)
			{
				for(i = 0;i < (uint8_t)devMax;i++)
				{
					devDebugCtrl.devAction[i] = 0;
				}
				devDebugCtrl.coldDevAction = 0;
				devDebugCtrl.heatDevAction = 0;
			}
			break;
		}
		case	DEBUG_FAN1_CTRL_INPUT:
		case	DEBUG_FAN1_CTRL_INPUT+DEBUG_DEV_CTRL_INPUT_INTERVAL*1:
		case	DEBUG_FAN1_CTRL_INPUT+DEBUG_DEV_CTRL_INPUT_INTERVAL*2:
		case	DEBUG_FAN1_CTRL_INPUT+DEBUG_DEV_CTRL_INPUT_INTERVAL*3:
		case	DEBUG_FAN1_CTRL_INPUT+DEBUG_DEV_CTRL_INPUT_INTERVAL*4:
		case	DEBUG_FAN1_CTRL_INPUT+DEBUG_DEV_CTRL_INPUT_INTERVAL*5:
		case	DEBUG_FAN1_CTRL_INPUT+DEBUG_DEV_CTRL_INPUT_INTERVAL*6:
		{
			index = (addr - DEBUG_FAN1_CTRL_INPUT)/	DEBUG_DEV_CTRL_INPUT_INTERVAL;
			devDebugCtrl.devAction[index] = !devDebugCtrl.devAction[index];
			break;
		}
		case	DEBUG_FAN1_CTRL_INPUT+DEBUG_DEV_CTRL_INPUT_INTERVAL*7:
		{
			if(dataValue >= SPEED_FAN_VALUE_MAX)
			{
				dataValue = SPEED_FAN_VALUE_MAX;
			}
			devDebugCtrl.devAction[VariFreqFan1] = dataValue;
			break;
		}
		case	DEBUG_FAN1_CTRL_INPUT+DEBUG_DEV_CTRL_INPUT_INTERVAL*8:
		{
			if(dataValue >= SPEED_FAN_VALUE_MAX)
			{
				dataValue = SPEED_FAN_VALUE_MAX;
			}
			devDebugCtrl.devAction[VariFreqFan2] = dataValue;
			break;
		}
		case	DEBUG_WATER_PAD_INPUT:
		{
			devDebugCtrl.coldDevAction = !devDebugCtrl.coldDevAction;
			break;
		}
		case	DEBUG_HEAT_DEV_INPUT:
		{
			devDebugCtrl.heatDevAction = !devDebugCtrl.heatDevAction;
			break;
		}

		#endif
		default :break;
	}
}

void SCI_SendByte(rt_device_t device,uint8_t *str,uint8_t len) 
{
/*
  uint8_t i,j;
  uint8_t Data[20];   //161026  ̫����� ��������
  uint8_t Sen_Len=0;
 
  
  
  for(i=0;i<20;i++){
    Data[i] = 0;
  }

  rt_device_write(device, 0, &Data[0], Sen_Len);
  */
}



/*
void lcd_showEhData(void)
{
	int i = 0;
	//int16_t temp = 0;
	//uint8_t num = 0;
	//static uint8_t	sendCnt = 0;

	//平均温度，湿度显�?
	lcd_show_index(AVG_TEMP_SHOW, (avgTemper / 10));
	lcd_show_index(AVG_HUMI_SHOW, (avgHumi / 10));
	//各个传感器及最大最小值显�?
	for(i = 0;i < RS232Temp_Num;i++)
	{
		lcd_show_index((TEMP1_CUR_SHOW + SENSOR_INC_NUM*i) , (tempData[i] / 10));
		lcd_show_index((TEMP1_MAX_SHOW + SENSOR_INC_NUM*i) , (tempMax[i] / 10));
		lcd_show_index((TEMP1_MIN_SHOW + SENSOR_INC_NUM*i) , (tempMin[i] / 10));
	}
	i = 0;
	lcd_show_index(HUMI1_CUR_SHOW , (humiData[i] / 10));
	lcd_show_index(HUMI1_MAX_SHOW , (humiMax[i] / 10));
	lcd_show_index(HUMI1_MIN_SHOW , (humiMin[i] / 10));

	//变频值显�?
	lcd_show_index(FRE_FAN1_STA_SHOW , MOTOR_SPEED_REC[0]);
	lcd_show_index(FRE_FAN2_STA_SHOW , MOTOR_SPEED_REC[1]);


	lcd_show_index(ROM_HEAD_SHOW , devInfo.roomIdHead);
	lcd_show_index(ROM_TAIL_SHOW , devInfo.roomIdTail);
	lcd_show_index(DAY_NUM_SHOW , devInfo.curDayNum);


	lcd_show_index(0x870, calendar.w_year);
	lcd_show_index(0x872, calendar.w_month);
	lcd_show_index(0x874, calendar.w_date);
	lcd_show_index(0x876, calendar.hour);
	lcd_show_index(0x878, calendar.min);
	lcd_show_index(0x87a, calendar.sec);
}
*/
void lcd_showCfgData(void)
{
	uint8_t	i = 0,j = 0, k = 0;
	uint8_t	seasonTxt[8] = {0x20};
	uint16_t	addr = 0, data = 0;
	
	//显示温度曲线
	/*
	for(i = 0;i < DAY_LINE_MAX;i++)
	{
		if(targetTemp[i].dayCount != 0)
		{
			lcd_show_index((DAY1_NUM_SHOW + DAY_NUM_SHOW_INTERVAL*i), targetTemp[i].dayCount);
			lcd_show_index((TAR1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), targetTemp[i].temp);
			lcd_show_index((HIGH1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), (targetTemp[i].temp + tempAlarmShift.highShift));
			lcd_show_index((LOW1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), (targetTemp[i].temp - tempAlarmShift.lowShift));
		}
	}*/
	for(i = 0;i < DAY_LINE_MAX;i++)
	{
		//xx if(devModeCfg[devInfo.curSeason].dayCtrlMode[i].dayNum != 0)
		//if(tempModeCfg.dayCtrlMode[i].dayNum != 0)
		{
			//xx lcd_show_index((DAY1_NUM_SHOW + DAY_NUM_SHOW_INTERVAL*i), devModeCfg[devInfo.curSeason].targetTemp[i].dayCount);
			//xx lcd_show_index((TAR1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), devModeCfg[devInfo.curSeason].targetTemp[i].temp);
			//xx lcd_show_index((HIGH1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), devModeCfg[devInfo.curSeason].targetTemp[i].high_temp);
			//xx lcd_show_index((LOW1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), devModeCfg[devInfo.curSeason].targetTemp[i].low_temp);
			lcd_show_index((DAY1_NUM_SHOW + DAY_NUM_SHOW_INTERVAL*i), tempModeCfg.targetTemp[i].dayCount);
			lcd_show_index((TAR1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), tempModeCfg.targetTemp[i].temp);
			lcd_show_index((HOT1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), tempModeCfg.dayCtrlMode[i].heatDevCfg.startParam);
			lcd_show_index((HIGH1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), tempModeCfg.targetTemp[i].high_temp);
			lcd_show_index((LOW1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), tempModeCfg.targetTemp[i].low_temp);
		}
	}
	//显示季节模式
	memset(seasonTxt,0x20, sizeof(seasonTxt));
	if(devInfo.curSeason == springMode)
	{
		seasonTxt[0] = 0xB4;seasonTxt[1] = 0xBA;seasonTxt[2] = 0xC7;seasonTxt[3] = 0xEF;
		lcd_show_txt(0x68,seasonTxt,sizeof(seasonTxt));
	}
	else if(devInfo.curSeason == summerMode)
	{
		seasonTxt[0] = 0xCF;seasonTxt[1] = 0xC4;seasonTxt[2] = 0xBC;seasonTxt[3] = 0xBE;
		lcd_show_txt(0x68,seasonTxt,sizeof(seasonTxt));
	}
	else if(devInfo.curSeason == winterMode)
	{
		seasonTxt[0] = 0xB6;seasonTxt[1] = 0xAC;seasonTxt[2] = 0xBC;seasonTxt[3] = 0xBE;
		lcd_show_txt(0x68,seasonTxt,sizeof(seasonTxt));
	}
	//显示控制配置
	for(i = 0;i < DAY_LINE_MAX;i++)
	{
		//if(devModeCfg[devInfo.curSeason].targetTemp[i].dayCount != 0)
		{
			addr = DAY1_DAYNUM_SHOW_2 + DAY_NUM_SHOW_INTERVAL_2*i;
			//xx data = devModeCfg[devInfo.curSeason].targetTemp[i].dayCount;
			data = tempModeCfg.targetTemp[i].dayCount;
			lcd_show_index(addr, data);
			for(j = 0;j < TEMP_LEVEL_MAX;j++)
			{
				//if(devModeCfg[devInfo.curSeason].dayCtrlMode[i].tempCtrlMode[j].tempMax != 0)
				{
					addr = DAY1_TEMP1_VALUE_SHOW +DAY_NUM_SHOW_INTERVAL_2*i+DAY_TEMP_DEV_SHOW_TIMEVAL*j;
					//xx data = devModeCfg[devInfo.curSeason].dayCtrlMode[i].tempCtrlMode[j].tempMax;
					data = tempModeCfg.dayCtrlMode[i].tempCtrlMode[j].tempMax;
					lcd_show_index(addr, data);
					for(k = 0;k < devMax;k++)
					{
						addr = DAY1_TEMP1_FAN1_STA_SHOW + DAY_NUM_SHOW_INTERVAL_2*i + DAY_TEMP_DEV_SHOW_TIMEVAL*j + 2*k;
						//XX data = devModeCfg[devInfo.curSeason].dayCtrlMode[i].tempCtrlMode[j].sta[k];
						data = tempModeCfg.dayCtrlMode[i].tempCtrlMode[j].sta[k];
						lcd_show_index(addr, data);
					}			
				}
			}
		}	
		IWDG_Feed();
	}
}

void lcd_showDevCfg(void)
{
	//显示加热器关闭偏�?
	lcd_show_index(HEAT_DEV_STOP_SHIFT, tempModeCfg.heatStopShift);
	//显示湿帘配置
	lcd_show_index(COLD_DEV_START_SHIFT, tempModeCfg.coldDevCfg.startShift);
	lcd_show_index(COLD_DEV_STOP_SHIFT, tempModeCfg.coldDevCfg.stopShift);
	lcd_show_index(COLD_DEV_STOP_SHIFT_HUMI, tempModeCfg.coldDevCfg.stopParam);
	//lcd_show_index(HIGH_HUMI_VALUE_INPUT, alarmCfg.devAlarmCfg[highTempAlarm].condition);
}


void  lcd_showManagerInfo(void)
{
	uint8_t	i = 0;
	uint8_t	feedLevelStr[4] = {0x20};
	uint8_t	noteStr[10] = {0xCE,0xDE,0xD0,0xDE,0xB8,0xC4,0xC8,0xA8,0xCF,0xDE};
	//显示管理数据
		//舍参�?
//	if(FLAGS3_IS_ADMIN_USER)
	{
		lcd_show_index(ADMIN_PASSWORD_INPUT, devInfo.adminPassword);
		lcd_show_index(USER1_PASSWORD_INPUT, devInfo.user1Password);
		lcd_show_index(USER2_PASSWORD_INPUT, devInfo.user2Password);
		memset(noteStr,0x20,sizeof(noteStr));
		lcd_show_txt(NOTE_TXT_SHOW,noteStr,sizeof(noteStr));
	}
	//else
	{
		lcd_show_index(ADMIN_PASSWORD_INPUT, 0);
		lcd_show_index(USER1_PASSWORD_INPUT, 0);
		lcd_show_index(USER2_PASSWORD_INPUT, 0);
		lcd_show_txt(NOTE_TXT_SHOW,noteStr,sizeof(noteStr));
	}
	
	lcd_show_index(ROOM_ID_HEAD_SHOW, devInfo.roomIdHead);
	lcd_show_index(ROOM_ID_TAIL_SHOW, devInfo.roomIdTail);
	lcd_show_index(ANIMAL_NUM_SHOW, devInfo.animalNum);
	lcd_show_index(DEV_ID_SHOW, devInfo.devId);
	if(devInfo.feedLevel == baoyuLevel)
	{
		feedLevelStr[0] = 0xb1;feedLevelStr[1] = 0xa3;feedLevelStr[2] = 0xd3;feedLevelStr[3] = 0xfd;
	}
	else if(devInfo.feedLevel == yufeiLevel)
	{
		feedLevelStr[0] = 0xd3;feedLevelStr[1] = 0xfd;feedLevelStr[2] = 0xb7;feedLevelStr[3] = 0xca;
	}
	else if(devInfo.feedLevel == chanfangLevel)
	{
		feedLevelStr[0] = 0xb2;feedLevelStr[1] = 0xfa;feedLevelStr[2] = 0xb7;feedLevelStr[3] = 0xbf;
	}
	lcd_show_txt(FEED_LEVEL_SHOW, feedLevelStr,sizeof(feedLevelStr));
	lcd_show_index(AVG_WEIGHT_SHOW, devInfo.avgWeight);
	lcd_show_index(ROOM_DAY_NUM_SHOW, devInfo.curDayNum);
	//报警参数
	for(i = 0;i < alarmEventMax;i++)
	{
		lcd_show_index(ALARM1_EN_SHOW + ALARM_EN_SHOW_TIMEVAL*i, alarmCfg.devAlarmCfg[i].en);
		lcd_show_index(ALARM1_VALUE_SHOW + ALARM_EN_SHOW_TIMEVAL*i, alarmCfg.devAlarmCfg[i].param);
	}
	lcd_show_index(ALARM_HIGH_HUMI_VALUE_INPUT, alarmCfg.devAlarmCfg[highHumiAlarm].condition);
		//版本显示
	lcd_show_index(HARD_VERSION_SHOW, hardVersion);
	lcd_show_index(SOFT_VERSION_SHOW, softVersion);
	lcd_show_index(LCD_SOFT_VERSION_SHOW, lcdSoftVersion);
	

	lcd_show_index(PASSCODE_INPUT, 0);
}

void lcd_showAdjust(void)
{
	uint8_t	i =0 ;
	
	//显示校准设置。校准处输入当前校准值。旁边显示当前SENSOR的值作为对�?
	for(i = 0;i <TEMP_SENSOR_NUM_MAX;i++)
	{
		//lcd_show_index(TEMP1_AJUST_SHOW + 2*i, ajustShowData[i]);
		//lcd_show_index(TEMP1_AJUST_SHOW + 2*i, sensorCfg.sensorAjust.tempAjustValue[i]);
		//lcd_show_index(TEMP1_AJUST_SHOW + 2*i, 0);
//		if(!FLAGS3_AJUST_VALUE_CLR)
		{
			lcd_show_index(TEMP1_AJUST_SHOW + 2*i, sensorAjustTemp.tempAjustValue[i]);
		}
		//else
		{
			lcd_show_index(TEMP1_AJUST_SHOW + 2*i, 0);
		}
	}
	//lcd_show_index(HUMI_AJUST_SHOW + 4*i, 0);
	//lcd_show_index(TEMP1_AJUST_SHOW + 2*4, sensorCfg.sensorAjust.humiAjustValue[0]);
	//lcd_show_index(TEMP1_AJUST_SHOW + 2*3, 0);
//	if(!FLAGS3_AJUST_VALUE_CLR)
	{
		lcd_show_index(TEMP1_AJUST_SHOW + 2*3, sensorAjustTemp.humiAjustValue[0]);
	}
	//else
	{
		lcd_show_index(TEMP1_AJUST_SHOW + 2*3, 0);
	}
	
}

void lcd_showInstall(void)
{
	uint8_t i = 0;
	uint8_t	debugStaStr[11] = {0xBD,0xF8,0xC8,0xEB,0xB5,0xF7,0xCA,0xD4,0x2E,0x2E,0x2E};
	//uint8_t	imeiCode[IMEI_LEN] = {0};
	//安装显示
		//设备设置显示
	lcd_show_index(TEMP_SENSOR_NUM_SHOW, sensorCfg.tempSensorNum);
	lcd_show_index(HUMI_SENSOR_NUM_SHOW, sensorCfg.humiSensorNum);
	lcd_show_index(OUTDOOR_SENSOR_ID_SHOW, sensorCfg.outDoorSensorId);
	lcd_show_index(DEV_ID_SHOW, devInfo.devId);
		//调试显示
	for(i = 0;i < devMax;i++)
	{
		lcd_show_index(DEBUG_FAN1_STA_SHOW + DEBUG_FAN1_SHOW_TIMEVAL*i, devDebugCtrl.devAction[i]);
	}
	//if(!FLAGS3_DEBUG_ENTER)
	{
		memset(debugStaStr, 0x20, sizeof(debugStaStr));
	}
	lcd_show_txt(DEBUG_MODE_TXT_SHOW,debugStaStr,sizeof(debugStaStr));
	lcd_show_index(DEBUG_WATER_PAD_INPUT, devDebugCtrl.coldDevAction);
	lcd_show_index(DEBUG_HEAT_DEV_INPUT, devDebugCtrl.heatDevAction);

	//lcd_show_index(DEBUG_JQID_SHOW, devIdInfo.jqid);
	//lcd_show_txt(DEBUG_IMEI_CODE_SHOW, (uint8_t *)&devIdInfo.imeiCode, IMEI_LEN);
	//lcd_show_index(DEBUG_REBOOT_CNT_SHOW, devIdInfo.rebootCnt);
	//lcd_show_index(DEBUG_ESIM_STA_SHOW, isEsimConnect);
}

void lcd_showTargetPage(void)
{
	uint8_t	i = 0;
	uint8_t	seasonTxt[8] = {0x20};
	//uint16_t	addr = 0, data = 0;
	
	for(i = 0;i < DAY_LINE_MAX;i++)
	{
		//xx if(devModeCfg[devInfo.curSeason].dayCtrlMode[i].dayNum != 0)
		//if(tempModeCfg.dayCtrlMode[i].dayNum != 0)
		{
			//xx lcd_show_index((DAY1_NUM_SHOW + DAY_NUM_SHOW_INTERVAL*i), devModeCfg[devInfo.curSeason].targetTemp[i].dayCount);
			//xx lcd_show_index((TAR1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), devModeCfg[devInfo.curSeason].targetTemp[i].temp);
			//xx lcd_show_index((HIGH1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), devModeCfg[devInfo.curSeason].targetTemp[i].high_temp);
			//xx lcd_show_index((LOW1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), devModeCfg[devInfo.curSeason].targetTemp[i].low_temp);
			lcd_show_index((DAY1_NUM_SHOW + DAY_NUM_SHOW_INTERVAL*i), tempModeCfg.targetTemp[i].dayCount);
			lcd_show_index((TAR1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), tempModeCfg.targetTemp[i].temp);
			lcd_show_index((HOT1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), tempModeCfg.dayCtrlMode[i].heatDevCfg.startParam);
			lcd_show_index((HIGH1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), tempModeCfg.targetTemp[i].high_temp);
			lcd_show_index((LOW1_TEMP_SHOW + DAY_NUM_SHOW_INTERVAL*i), tempModeCfg.targetTemp[i].low_temp);
		}
	}
	//显示季节模式
	memset(seasonTxt,0x20, sizeof(seasonTxt));
	if(devInfo.curSeason == springMode)
	{
		seasonTxt[0] = 0xB4;seasonTxt[1] = 0xBA;seasonTxt[2] = 0xC7;seasonTxt[3] = 0xEF;
		lcd_show_txt(SEASON_TXT_SHOW,seasonTxt,sizeof(seasonTxt));
	}
	else if(devInfo.curSeason == summerMode)
	{
		seasonTxt[0] = 0xCF;seasonTxt[1] = 0xC4;seasonTxt[2] = 0xBC;seasonTxt[3] = 0xBE;
		lcd_show_txt(SEASON_TXT_SHOW,seasonTxt,sizeof(seasonTxt));
	}
	else if(devInfo.curSeason == winterMode)
	{
		seasonTxt[0] = 0xB6;seasonTxt[1] = 0xAC;seasonTxt[2] = 0xBC;seasonTxt[3] = 0xBE;
		lcd_show_txt(SEASON_TXT_SHOW,seasonTxt,sizeof(seasonTxt));
	}
}

void lcd_showCtrlCfg(uint8_t	index)
{
	uint8_t	j = 0, k = 0;
	uint8_t	seasonTxt[8] = {0x20};
	uint16_t	addr = 0, data = 0;

	//显示季节模式
	memset(seasonTxt,0x20, sizeof(seasonTxt));
	if(devInfo.curSeason == springMode)
	{
		seasonTxt[0] = 0xB4;seasonTxt[1] = 0xBA;seasonTxt[2] = 0xC7;seasonTxt[3] = 0xEF;
		lcd_show_txt(0x68,seasonTxt,sizeof(seasonTxt));
	}
	else if(devInfo.curSeason == summerMode)
	{
		seasonTxt[0] = 0xCF;seasonTxt[1] = 0xC4;seasonTxt[2] = 0xBC;seasonTxt[3] = 0xBE;
		lcd_show_txt(0x68,seasonTxt,sizeof(seasonTxt));
	}
	else if(devInfo.curSeason == winterMode)
	{
		seasonTxt[0] = 0xB6;seasonTxt[1] = 0xAC;seasonTxt[2] = 0xBC;seasonTxt[3] = 0xBE;
		lcd_show_txt(0x68,seasonTxt,sizeof(seasonTxt));
	}
	//显示控制配置
	//for(i = 0;i < DAY_LINE_MAX;i++)
	{
		//if(devModeCfg[devInfo.curSeason].targetTemp[i].dayCount != 0)
		{
			addr = DAY1_DAYNUM_SHOW_2 + DAY_NUM_SHOW_INTERVAL_2*index;
			//xx data = devModeCfg[devInfo.curSeason].targetTemp[i].dayCount;
			data = tempModeCfg.targetTemp[index].dayCount;
			lcd_show_index(addr, data);
			for(j = 0;j < TEMP_LEVEL_MAX;j++)
			{
				//if(devModeCfg[devInfo.curSeason].dayCtrlMode[i].tempCtrlMode[j].tempMax != 0)
				{
					addr = DAY1_TEMP1_VALUE_SHOW +DAY_NUM_SHOW_INTERVAL_2*index+DAY_TEMP_DEV_SHOW_TIMEVAL*j;
					//xx data = devModeCfg[devInfo.curSeason].dayCtrlMode[i].tempCtrlMode[j].tempMax;
					data = tempModeCfg.dayCtrlMode[index].tempCtrlMode[j].tempMax;
					lcd_show_index(addr, data);
					for(k = 0;k < devMax;k++)
					{
						addr = DAY1_TEMP1_FAN1_STA_SHOW + DAY_NUM_SHOW_INTERVAL_2*index + DAY_TEMP_DEV_SHOW_TIMEVAL*j + 2*k;
						//XX data = devModeCfg[devInfo.curSeason].dayCtrlMode[i].tempCtrlMode[j].sta[k];
						data = tempModeCfg.dayCtrlMode[index].tempCtrlMode[j].sta[k];
						lcd_show_index(addr, data);
					}			
				}
			}
		}	
		IWDG_Feed();
	}
}
void lcdPageShow(uint8_t	topMenum, uint8_t index)
{
	switch(topMenum)
	{
		case	PAGE_EH_SHOW:
		{
			//lcd_showEhData();
			if(!onlyRefreshData)
				lcd_select_pic(PIC_ID_SIMPLE_DATA);
			break;
		}
		case	PAGE_TARGET_TEMP:	
		{
			lcd_showTargetPage();
			if(!onlyRefreshData)	lcd_select_pic(PIC_ID_MENU_TRAGET);
			break;
		}
		case	PAGE_DAY_LEVEL_CFG:
		{
			if(index == 0)
			{
				lcd_showTargetPage();
				if(!onlyRefreshData)	lcd_select_pic(PIC_ID_MENU_TRAGET);
			}
			else
			{
				lcd_showCtrlCfg(index - 1);
				if(!onlyRefreshData)	lcd_select_pic(PIC_ID_DAY_LEVEL_CTRL + (index - 1));
			}
			break;
		}
		case	PAGE_DEV_CTRL:
		{
			lcd_showDevCfg();
			if(!onlyRefreshData)	lcd_select_pic(PIC_ID_MENU_DEV);
			break;
		}
		case	PAGE_MANAGEMENT:
		{
			lcd_showManagerInfo();
			if(!onlyRefreshData)	lcd_select_pic(PIC_ID_MENU_MANAGE);
			break;
		}
		case	PAGE_HISTORY:
		{
			lcd_select_pic(PIC_ID_MENU_HISTORY);
			break;
		}
		case	PAGE_ADJUST:
		{
			lcd_showAdjust();
			if(!onlyRefreshData)	lcd_select_pic(PIC_ID_MENU_ADJUST);
			break;
		}
		case	PAGE_INSTALL:
		{
			lcd_showInstall();
			if(!onlyRefreshData) lcd_select_pic(PIC_ID_MENU_INSTALL);
			break;
		}
		default:break;
	}
}
#define	LCD_FRESH_CNT_MAX	3
void SCI_SendData(rt_device_t write_device)
{
	//static uint8_t cnt0,cnt1,cnt2,cnt3,cnt4,cnt5;
	//static uint8_t	lcdReturnFlag =0;

	//如果点击登录，则进行密码验证。并进行页面切换及弹窗提示
	//if(FLAGS3_NO_PRESS)
	{
		passWord = -1;
		//FLAGS3_PASSCODE_RIGHT = 0;
		if(curPicId > PIC_ID_MORE_DATA)
		{
			lcd_select_pic(PIC_ID_SIMPLE_DATA);
		}	
	}
	//if(FLAGS3_IS_NEED_LOAD)
	{
		
		if((passWord != 0)&&((passWord == 8888) || (passWord == devInfo.adminPassword)|| (passWord == devInfo.user1Password)|| (passWord == devInfo.user2Password)))
		{
			//FLAGS3_PASSCODE_RIGHT = 1;
			if((passWord == 8888) || (passWord == devInfo.adminPassword))
			{
				//FLAGS3_IS_ADMIN_USER = 1;
			}
			else
			{
				//FLAGS3_IS_ADMIN_USER = 0;
			}
			lcd_select_pic(PIC_ID_MENU_TRAGET);
		}
		else
		{
		//	FLAGS3_PASSCODE_RIGHT = 0;
			lcd_show_window(PASS_WORD_ERR_WINDOW);	
		}	
		//FLAGS3_IS_NEED_LOAD = 0;
	}


//	if(FLAGS4_EH_DATA_REFRESH)
	{
		//lcd_showEhData();
		//FLAGS4_EH_DATA_REFRESH = 0;
	}

//	if(FLAGS4_TOP_MENUM_PRESS)
	{
		//FLAGS4_TOP_MENUM_PRESS = 0;
		lcdPageShow(curTopMenumId, curSecMenumId);
		onlyRefreshData = 0;
	}
	
	/*
	if(FLAGS3_DO_CTL_PIC)
	{
		while(cnt0++ <LCD_FRESH_CNT_MAX) lcd_showCfgData();
		if(cnt0++ >= LCD_FRESH_CNT_MAX)
		{
			cnt0 = 0;
			FLAGS3_DO_CTL_PIC = 0;
		}
	}
	if(FLAGS3_DO_DEV_PIC)
	{
		while(cnt1++ <LCD_FRESH_CNT_MAX)  lcd_showDevCfg();
		if(cnt1++ >= LCD_FRESH_CNT_MAX)
		{
			cnt1 = 0;
			FLAGS3_DO_DEV_PIC = 0;
		}
	}
	if(FLAGS3_DO_MANAGE_PIC)
	{
		while(cnt2++ <LCD_FRESH_CNT_MAX)  lcd_showManagerInfo();
		if(cnt2++ >= LCD_FRESH_CNT_MAX)
		{
			cnt2 = 0;
			FLAGS3_DO_MANAGE_PIC = 0;
		}
	}
	if(FLAGS3_DO_AJUST_PIC)
	{
		while(cnt4++ <LCD_FRESH_CNT_MAX)  lcd_showAdjust();
		if(cnt4++ >= LCD_FRESH_CNT_MAX)
		{
			cnt4 = 0;
			FLAGS3_DO_AJUST_PIC = 0;
		}
	}
	if(FLAGS3_DO_INSTALL_PIC)
	{
		while(cnt5++ <LCD_FRESH_CNT_MAX)  lcd_showInstall();
		if(cnt5++ >= LCD_FRESH_CNT_MAX)
		{
			cnt5 = 0;
			FLAGS3_DO_INSTALL_PIC = 0;
		}
	}
	
	*/
	//if(FLAGS3_REFRESH_LCD_2)
	{
		lcd_showCfgData();
		lcd_showDevCfg();
		lcd_showManagerInfo();
		lcd_showAdjust();
		lcd_showInstall();
		//FLAGS3_REFRESH_LCD_2 = 0;
		lcd_show_index(0x22, dayNumLevel+1);
		lcd_show_index(0x24, tempStep+1);
	}

	//定时获取LCD页面ID
	//if(FLAGS3_GET_LCD_ID_SEND)
	{
		lcd_get_pic_num();
		//FLAGS3_GET_LCD_ID_SEND = 0;
	}
	//定时获取RTC
	//if(FLAGS3_GET_RTC_SEND)
	{
		lcd_get_rtc();
		//FLAGS3_GET_RTC_SEND = 0;
	}
}
