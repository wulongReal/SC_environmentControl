#ifndef __RS232_H_
#define __RS232_H_
#include "app_task.h"
#include <rtthread.h>

#define LIN_ERR_COUNT      30

#define LIN_FLAG_IGNORE    0x01    //
#define LIN_RCVAA          0x02    //
#define LIN_RCVSN          0x04    // receive station number
#define LIN_RCVNUM         0x08    // receive id
#define LIN_RCVDATA        0x10    // receive data
#define LIN_RCVLEN         0x20    // send data
#define LIN_RCVCS          0x40    // return
#define LIN_RCVSP          0x80    // IDENTIFY SENSORS



#define LINMSG_IGN         0x00
#define LINMSG_SND         0x01
#define LINMSG_RCV         0x02
#define LINMSG_STA         0x04
#define LIN_INVALID_MSGLEN 0xFF6

//lcd
#define LCD_FLAG_IGNORE		0x01    //
#define LCD_RCV_STAT1       0x02    //
#define LCD_RCV_STAT2       0x04    //
#define LCD_RCV_LEN         0x08    //
#define LCD_RCV_SN         	0x10    //
#define	LCD_RCV_DATA		0X20

#define	LCD_CMD_STAT1	0X5A
#define	LCD_CMD_STAT2 	0XA5
#define	LCD_CMD_SN_1	0X81		//访问寄存器
#define	LCD_CMD_SN_2	0X83		//访问变量存储器

#define	LCD_GET_PIC_ID_LEN	0X05
#define	LCD_GET_RTC_LEN		0X0A


//一级菜单宏定义,点击获取一级菜单ID
#define	PAGE_EH_SHOW		0
#define	PAGE_TARGET_TEMP	1
#define	PAGE_DEV_CTRL		2
#define	PAGE_MANAGEMENT		3
#define	PAGE_HISTORY		4
#define	PAGE_ADJUST			5
#define	PAGE_INSTALL		6
#define	PAGE_DAY_LEVEL_CFG	7



//弹窗ID
#define	DATA_ERR_WINDOW		1		//数据不合理
#define	SAVE_OK_WINDOW		2		//保存成功
#define	PASS_WORD_ERR_WINDOW	3	//密码错误
#define	NO_ADMIN_ERR_WINDOW		4	//无权限
#define	ENTER_DEBUG_WINDOW		5	//进入调试
#define	EXIT_DEBUG_WINDOW		6	//退出调试
#define	AUTO_CREATE_OK			8	//自动生成成功
//LCD页面ID
#define	PIC_ID_SIMPLE_DATA	1
#define	PIC_ID_MORE_DATA	2
#define	PIC_ID_MENU_TRAGET		6		//菜单主页面
#define	PIC_ID_DAY_LEVEL_CTRL	8		//日龄对应档位温度
#define	PIC_ID_MENU_DEV		15
#define	PIC_ID_MENU_MANAGE		17
#define	PIC_ID_MENU_HISTORY	23
#define	PIC_ID_MENU_ADJUST		27
#define	PIC_ID_MENU_INSTALL		28

#define	PIC_DEBUG			4

#define	PIC_CTL		1
#define	PIC_DEV		1
#define	PIC_MANAGER	1
#define	PIC_HISTORY	1
#define	PIC_AJUST	1
#define	PIC_INSTALL	1
//LCD显示页面地址宏定义
//数据展示页面地址宏定义
//展示部分
#define	AVG_TEMP_SHOW			0X00
#define	AVG_HUMI_SHOW			AVG_TEMP_SHOW+2

#define	SENSOR_INC_NUM			0X06
#define	TEMP1_CUR_SHOW			0X30
#define	TEMP1_MAX_SHOW			TEMP1_CUR_SHOW + 2
#define	TEMP1_MIN_SHOW			TEMP1_MAX_SHOW + 2
#define	TEMP2_CUR_SHOW			TEMP1_CUR_SHOW + SENSOR_INC_NUM
#define	TEMP2_MAX_SHOW			TEMP2_CUR_SHOW + 2
#define	TEMP2_MIN_SHOW			TEMP2_MAX_SHOW + 2
#define	TEMP3_CUR_SHOW			TEMP2_CUR_SHOW + SENSOR_INC_NUM
#define	TEMP3_MAX_SHOW			TEMP3_CUR_SHOW + 2
#define	TEMP3_MIN_SHOW			TEMP3_MAX_SHOW + 2
#define	HUMI1_CUR_SHOW			0X42
#define	HUMI1_MAX_SHOW			HUMI1_CUR_SHOW + 2
#define	HUMI1_MIN_SHOW			HUMI1_MAX_SHOW + 2
#define	FRE_FAN1_STA_SHOW		0X48
#define	FRE_FAN2_STA_SHOW		FRE_FAN1_STA_SHOW + 2
#define	ROM_HEAD_SHOW			0X4C
#define	ROM_TAIL_SHOW			ROM_HEAD_SHOW+2
#define	DAY_NUM_SHOW			0X50
//录入部分
#define	MENUM_BUT_INPUT			0X20	//菜单按钮输入

//密码验证页面
#define	PASSCODE_INPUT			0X54
#define	LOAD_BUT_INUPT			0X56

//控制页面
	//温度曲线
	//输入
#define	SEASON_INPUT			0X60
#define	SEASON_TXT_SHOW			0X68

#define	DAY_NUM_INPUT_INTERVAL	0X10
#define	DAY1_NUM_INPUT			0X70
#define	TAR1_TEMP_INPUT			0X72
#define	HOT1_TEMP_INPUT			0X74
#define	HIGH1_TEMP_INPUT		0X76
#define	LOW1_TEMP_INPUT			0X78
	//输出显示
#define	DAY_NUM_SHOW_INTERVAL	0X10
#define	DAY1_NUM_SHOW			0X70
#define	TAR1_TEMP_SHOW			0X72
#define	HOT1_TEMP_SHOW			0X74
#define	HIGH1_TEMP_SHOW			0X76
#define	LOW1_TEMP_SHOW			0X78

#define	TARGET_SAVE_BUT_INPUT		0XE0
#define	CTRL_PAGE_BUT_INPUT			0XE6
#define	MENUM_TOP_SELECT_BUT_INPUT				0XE8	//一级菜单按钮
#define	CFG_AUTO_BUT_INPUT			0XEA		//配置自动生成按钮。

	//配置显示及输入
	//输出
#define	DAY_NUM_SHOW_INTERVAL_2		0xe0
#define	DAY_TEMP_DEV_SHOW_TIMEVAL	0X20
#define	DAY1_DAYNUM_SHOW_2			0xf0
#define	DAY1_TEMP1_VALUE_SHOW				0XF2
#define	DAY1_TEMP1_FAN1_STA_SHOW			0xF4
#define	DAY1_TEMP1_FAN2_STA_SHOW			0XF6
#define	DAY1_TEMP1_FAN3_STA_SHOW			0XF8
#define	DAY1_TEMP1_FAN4_STA_SHOW			0xFA
#define	DAY1_TEMP1_FAN5_STA_SHOW			0XFC
#define	DAY1_TEMP1_FAN6_STA_SHOW			0XFE
#define	DAY1_TEMP1_FAN7_STA_SHOW			0X100
#define	DAY1_TEMP1_WTPAD_STA_SHOW			0X102
#define	DAY1_TEMP1_FREFAN1_STA_SHOW			0X104
#define	DAY1_TEMP1_FREFAN2_STA_SHOW			0X106

#define	AUTO_CFG_BUT_INPUT			0X456
	//配置输入
#define	DAY_STEP_INPUT_TIMEVAL			0XE0
#define	DAY_TEMP_VALUE_INPUT_TIMEVAL	0X20
#define	DAY_TEMP_DEV_INPUT_TIMEVAL		0X02
#define	DAY1_TEMP1_VALUE_INPUT			0xf2
#define	DAY1_TEMP1_FAN1_INPUT			0xF4
#define	DAY1_TEMP1_FAN2_INPUT			0XF6
#define	DAY1_TEMP1_FAN3_INPUT			0XF8
#define	DAY1_TEMP1_FAN4_INPUT			0xFA
#define	DAY1_TEMP1_FAN5_INPUT			0XFC
#define	DAY1_TEMP1_FAN6_INPUT			0XFE
#define	DAY1_TEMP1_FAN7_INPUT			0X100
#define	DAY1_TEMP1_WTPAD_INPUT			0X102
#define	DAY1_TEMP1_FREFAN1_INPUT			0X104
#define	DAY1_TEMP1_FREFAN2_INPUT			0X106

#define	DAY_TEMP_DEV_CFG_SAVE_BUT_INPUT		0X720

//设备页面
#define	HEAT_DEV_STOP_SHIFT		0x730
#define	COLD_DEV_START_SHIFT	0X738	//开启温度偏移
#define	COLD_DEV_STOP_SHIFT		0X73a	//关闭温度偏移
#define	COLD_DEV_STOP_SHIFT_HUMI		0X732	//关闭湿度
#define	HIGH_HUMI_VALUE_INPUT	0X734		//高湿报警值
#define	DEV_SAVE_BUT			0X736


//管理页面
	//用户参数
	//舍参数
		//输出
#define	ROOM_NAME_SHOW		0X750
#define	ROOM_ID_HEAD_SHOW	0X760
#define	ROOM_ID_TAIL_SHOW	0X762
#define	ANIMAL_NUM_SHOW		0X764
#define	FEED_LEVEL_SHOW		0X758
#define	AVG_WEIGHT_SHOW		0X768
#define	ROOM_DAY_NUM_SHOW	0X76A
		//输入
#define	ADMIN_PASSWORD_INPUT	0X740
#define	USER1_PASSWORD_INPUT	0X742
#define	USER2_PASSWORD_INPUT	0X744
#define	NOTE_TXT_SHOW			0X746



#define	ROOM_NAME_INPUT		ROOM_NAME_SHOW
#define	ROOM_ID_HEAD_INPUT	ROOM_ID_HEAD_SHOW
#define	ROOM_ID_TAIL_INPUT	ROOM_ID_TAIL_SHOW
#define	ANIMAL_NUM_INPUT		ANIMAL_NUM_SHOW
#define	FEED_LEVEL_INPUT		FEED_LEVEL_SHOW
#define	AVG_WEIGHT_INPUT		AVG_WEIGHT_SHOW
#define	ROOM_DAY_NUM_INPUT	ROOM_DAY_NUM_SHOW

	//报警参数
		//输出
#define	ALARM_EN_SHOW_TIMEVAL	0X6
#define	ALARM1_EN_SHOW		0X772
#define	ALARM1_VALUE_SHOW	0X774

		//输入
#define	ALARM_EN_INPUT_TIMEVAL	ALARM_EN_SHOW_TIMEVAL	
#define	ALARM1_EN_BUT_INPUT	0x770
#define	ALARM1_VALUE_BUT_INPUT	0x774

#define	ALARM_CFG_SAVE_BUT_INPUT	0X7A0

#define	ALARM_HIGH_HUMI_VALUE_INPUT	0X7A2	//高湿度报警值


	//版本
		//输出
#define	HARD_VERSION_SHOW	0X794
#define	SOFT_VERSION_SHOW	0X79A
#define	LCD_SOFT_VERSION_SHOW	0X79C



//校准页面
//输入
#define	TEMP_AJUST_INPUT_TIMEVAL	2

#define	TEMP1_AJUST_INPUT	0X7B0
#define	TEMP1_DATA_SHOW		0x7b0
#define	HUMI_AJUST_INPUT	0X7B6

#define	SENSOR_AJUST_SAVE_BUT_INPUT	0X7b8

#define	TEMP1_AJUST_SHOW	0X7b0


//安装页面
	//设备设置页面
//输出
#define	TEMP_SENSOR_NUM_SHOW	0x7c0
#define	HUMI_SENSOR_NUM_SHOW		0X7C2
#define	OUTDOOR_SENSOR_ID_SHOW	0X7C4
#define	DEV_ID_SHOW				0X7C6
//输入
#define	TEMP_SENSOR_NUM_INPUT	0X7C0
#define	HUMI_SENSOR_NUM_INPUT	0X7C2
#define	OUTDOOR_SENSOR_ID_INPUT	0X7C4
#define	DEV_ID_INPUT			0X7C6
	//设备参数页面

#define	INSTALL_SAVE_BUT_INPUT	0X7EA


//调试页面
//输出
#define	DEBUG_FAN1_SHOW_TIMEVAL	0X04
#define	DEBUG_FAN1_STA_SHOW		0X7F0
#define	DEBUG_MODE_TXT_SHOW		0X820
#define	DEBUG_JQID_SHOW			0X840
#define	DEBUG_IMEI_CODE_SHOW	0X844
#define	DEBUG_REBOOT_CNT_SHOW		0X860
#define	DEBUG_ESIM_STA_SHOW			0X862

//输入
#define	DEBUG_DEV_CTRL_INPUT_INTERVAL	0X04
#define	DEBUG_FAN1_CTRL_INPUT	0X7F0
#define	DEBUG_CTRL_INPUT		0X81C

#define	DEBUG_WATER_PAD_INPUT	0X814
#define	DEBUG_HEAT_DEV_INPUT	0X818







void LinMemCopy(uint8_t * dst, uint8_t * src, uint8_t len);
uint8_t LIN_GetMsgLength(byte msgId,byte send_rec);
void SCI_RecData(void);
void SCI_SendByte(rt_device_t device,uint8_t *str,uint8_t len);
void SCI_SendData(rt_device_t write_device);
void LIN_RxCompleted(void);

void lcd_select_pic(unsigned int  pic_num);
void lcd_driveBeep(uint16_t	time);
void lcd_show_window(unsigned char id);
void lcd_show_CodePic(uint16_t addr, uint8_t *pDate, uint8_t len);

void	lcd_show_index(unsigned int addr, unsigned int num);
void	lcd_show_indexLong(unsigned int addr, uint32_t num);
void lcd_show_window_T5L(uint8_t keyCode);



#endif
