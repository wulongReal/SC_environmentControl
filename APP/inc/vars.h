#ifndef  VARS_H_
#define  VARS_H_
#include <stdint.h>
#include <stdbool.h>

#include "user_mb_app.h"
#include "stm32f10x.h"
#include "envir.h"
//#include "vector.h"

typedef unsigned  char byte;

//LTE����buf����
#define	LTE_RECV_BUFFER_LEN		500
//UART4  �����壬TAGͨ�Ž���buf����
#define	UART4_RECV_BUFFER_LEN	200


//IMEI_CODE_LEN		15
#define	IMEI_LEN	15
//JQID�ַ�������
#define	JQID_LEN_MAX	8





#define MAX_LIN_DATANUMB  20

//λ������
#define PERIPH_BASE           ((uint32_t)0x40000000) /*!< Peripheral base address in the alias region */
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00)
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 


#define LED_GRN PBout(5)// PB5
#define LED_RED PBout(3)// PB3

#define CAN_ID_ERR     0x01
#define CAN_IDE_ERR    0x02
#define CAN_LEN_ERR    0x04
#define CAN_NO_ERR     0x08
#define CAN_NO_POWER   0x10
#define CAN_NO_DATA    0x20




typedef union {
  unsigned int LONG;              // Int Access
  struct {                         // Bit Access 
    unsigned char   BIT0 :1;      // bit0
    unsigned char   BIT1 :1;      // bit1
    unsigned char   BIT2 :1;      // bit2
    unsigned char   BIT3 :1;      // bit3
    unsigned char   BIT4 :1;      // bit4
    unsigned char   BIT5 :1;      // bit5
    unsigned char   BIT6 :1;      // bit6
    unsigned char   BIT7 :1;      // bit7
    unsigned char   BIT8 :1;      // bit8
    unsigned char   BIT9 :1;      // bit9
    unsigned char   BIT10:1;      // bit10
    unsigned char   BIT11:1;      // bit11
    unsigned char   BIT12:1;      // bit12
    unsigned char   BIT13:1;      // bit13
    unsigned char   BIT14:1;      // bit14
    unsigned char   BIT15:1;      // bit15 
    unsigned char   BIT16:1;      // bit16
    unsigned char   BIT17:1;      // bit17
    unsigned char   BIT18:1;      // bit18
    unsigned char   BIT19:1;      // bit19
    unsigned char   BIT20:1;      // bit20
    unsigned char   BIT21:1;      // bit21
    unsigned char   BIT22:1;      // bit22
    unsigned char   BIT23:1;      // bit23
    unsigned char   BIT24:1;      // bit24
    unsigned char   BIT25:1;      // bit25
    unsigned char   BIT26:1;      // bit26
    unsigned char   BIT27:1;      // bit27
    unsigned char   BIT28:1;      // bit28
    unsigned char   BIT29:1;      // bit29
    unsigned char   BIT30:1;      // bit30
    unsigned char   BIT31:1;      // bit31     
  }BIT;
}FLAGSINT;

#define	FLAGS_MODBUS1S				FLAGS0.BIT.BIT0			//modbus��ȡʹ��
#define	FLAGS_DRIVE_1S				FLAGS0.BIT.BIT1			//������ͨ��ʹ��
//#define	FLAGS_LCD_EH_REF_1S			FLAGS0.BIT.BIT2			//Һ������������ˢ��
//#define	FLAGS_LCD_DEV_REF_2S		FLAGS0.BIT.BIT3			//Һ�����豸״̬ˢ��
#define	FLAGS_LCD_EWM_REF_EN		FLAGS0.BIT.BIT4			//��ά��ˢ��ʹ��
#define	FLAGS_EP_RD_EN_10S			FLAGS0.BIT.BIT5			//����ȡʹ��
#define	FLAGS_EP_RD_GET_SUC			FLAGS0.BIT.BIT6			//����ȡ�ɹ�
#define	FLAGS_EP_CMD_SEND			FLAGS0.BIT.BIT7			//����ȡ�����ѷ��ͣ��л�ͨ�ſڽ���
#define	FLAGS_ADC_RD_3S				FLAGS0.BIT.BIT8			//ADC��ȡʹ��		
#define	FLAGS_PH_RD_EN_10S			FLAGS0.BIT.BIT9			//����ȡʹ��
#define	FLAGS_PH_RD_GET_SUC			FLAGS0.BIT.BIT10		//����ȡ�ɹ�
#define	FLAGS_PH_CMD_SEND			FLAGS0.BIT.BIT11		//����ȡ�����ѷ��ͣ��л�ͨ�ſڽ���

#define	FLAGS_LCD_EH_REF_1S			FLAGS0.BIT.BIT12			//Һ������������ˢ��
#define	FLAGS_LCD_DEV_REF_2S		FLAGS0.BIT.BIT13			//Һ�����豸״̬ˢ��
#define FLAGS_LCD_CFG_REF_5S		FLAGS0.BIT.BIT14			//����ˢ��
#define FLAGS_LTE_GET_RSSI_10S		FLAGS0.BIT.BIT15			//LTE��ʱ��ȡrssi
#define	FLAGS_LTE_GET_CON_STA_10S	FLAGS0.BIT.BIT16		//LTE��ʱ��ȡ����״̬
#define	FLAGS_LCD_HAS_DATE			FLAGS0.BIT.BIT17			//LCD���յ���������


#define	FLAGS1_DRIVE_ERR			FLAGS1.BIT.BIT0			//���������



/*
#define FLAGS0_RS232HAS_DAT       FLAGS0.BIT.BIT0            // RS232_HAS DATA
#define FLAGS0_LINERR             FLAGS0.BIT.BIT1            // RS232 ERR
#define FLAGS0_SEND500MS          FLAGS0.BIT.BIT2            // RS232 send 500ms
#define FLAGS0_RECCONFIG          FLAGS0.BIT.BIT4            // �յ�������Ϣ
#define FLAGS0_RS485COMERR        FLAGS0.BIT.BIT9            // RS485ͨ�Ŵ��� no used
#define FLAGS0_RS232COMERR        FLAGS0.BIT.BIT10           // RS232ͨ�Ŵ���
#define FLAGS0_CAN1COMERR         FLAGS0.BIT.BIT11           // CAN1ͨ�Ź���  ��ͨ�ź�ɫLED���� ��ͨ�ź�ɫLED�� ��ͨ�� �з��������ɫ��˸
#define FLAGS0_NOEXTPOW           FLAGS0.BIT.BIT12           // û���ⲿ��Դ

#define FLAGS0_SNDCON             FLAGS0.BIT.BIT14           // ����������Ϣ
#define FLAGS0_485GRNBLK          FLAGS0.BIT.BIT15           // 485��ɫָʾ����˸
#define FLAGS0_485REDBLK          FLAGS0.BIT.BIT16           // 485��ɫ��˸
#define FLAGS0_ADC                FLAGS0.BIT.BIT17           // adc����



#define FLAGS0_DISPSEN            FLAGS0.BIT.BIT20           // ��ʾ�¶ȴ�����
#define FLAGS0_DELPOWER           FLAGS0.BIT.BIT21           // ���Ƶ�Դ
#define FLAGS0_CANDEBUG           FLAGS0.BIT.BIT22           // can����
#define FLAGS0_RECCOCH            FLAGS0.BIT.BIT23           // �յ��������ݲ��ı�������Ϣ
#define FLAGS0_ENTERTST           FLAGS0.BIT.BIT24           // �������ģʽ
#define FLAGS0_SNDVER             FLAGS0.BIT.BIT25           // ����ve
#define FLAGS0_SNDID              FLAGS0.BIT.BIT26           // ����ID
#define FLAGS0_TFTTSTF            FLAGS0.BIT.BIT28           // ��Ļ�������
#define FLAGS0_DISPIG             FLAGS0.BIT.BIT29           // DISP_PIG
#define FLAGS0_STTST              FLAGS0.BIT.BIT30           // ��ʼ����



#define FLAGS1_TSTRECTMP          FLAGS1.BIT.BIT0            // ���Գ����յ�RS232���¶ȴ�����ֵ
#define FLAGS1_TSTRECHMD          FLAGS1.BIT.BIT1            // ���Գ����յ�RS232��ʪ�ȴ�����ֵ
#define FLAGS1_TSTRECDRI          FLAGS1.BIT.BIT2            // ���Գ����յ�����������ֵ
#define FLAGS1_TSTRECDRR          FLAGS1.BIT.BIT3            // ���Գ����յ�������̵�������ֵ
#define FLAGS1_TSTRECPHV          FLAGS1.BIT.BIT4            // ���Գ����յ�RS232��PHֵ������ֵ
#define FLAGS1_TSTRECWF           FLAGS1.BIT.BIT5            // ���Գ����յ�RS232��ˮ����������ֵ
#define FLAGS1_TSTRECWP           FLAGS1.BIT.BIT6            // ���Գ����յ�RS232��ˮѹ��������ֵ
#define FLAGS1_TSTRECTEHD         FLAGS1.BIT.BIT7            // ���Գ����յ�RS232����ʪ�ȴ�����ֵ
#define FLAGS1_TSTRECNH3          FLAGS1.BIT.BIT8            // ���Գ����յ�RS232�İ���������ֵ
#define FLAGS1_TSTCHDISP          FLAGS1.BIT.BIT9            // ���Գ���TFT��ҳ��־
#define FLAGS1_TSTCANDISP         FLAGS1.BIT.BIT10           // ���Գ���TFT���з�ҳ(��ʾ��ѹ�������޷���ҳ)
#define FLAGS1_TSTTMPNUM          FLAGS1.BIT.BIT11           // ���Գ��������¶ȴ�����������
#define FLAGS1_CLRDRIV            FLAGS1.BIT.BIT12           // ���������Ϣ
//#define FLAGS1_TSTRECCD           FLAGS1.BIT.BIT13            // ���Գ����յ�RS232���¶ȴ�����ֵ   //0220

//#define FLAGS2_CANSEND            FLAGS2.BIT.BIT0            // CAN����
#define FLAGS2_FANDR              FLAGS2.BIT.BIT1            // �з������
#define FLAGS2_CORECOMM           FLAGS2.BIT.BIT2            // �յ����İ�ͨ�ŵı�־(�յ�ͨ�ź���15min��û��ͨ������һ�ε�Դ)
#define FLAGS2_RESETCORE          FLAGS2.BIT.BIT3            // �������İ��Դ
#define FLAGS2_MODBUS1S           FLAGS2.BIT.BIT4            // MODBUS����һ������1S

#define FLAGS2_FLOORHEAT          FLAGS2.BIT.BIT5            //��ů��ڱ�־
#define FLAGS2_RS232_REC          FLAGS2.BIT.BIT6            //���Ӱ���պ��İ��ů��־
#define FLAGS2_DN_2MIN            FLAGS2.BIT.BIT7            //��ů��ʱ����
#define FLAGS2_RS232_SED          FLAGS2.BIT.BIT8            //���ͱ�־
#define FLAGS2_DN_1000MS          FLAGS2.BIT.BIT9            //��ůͨ�ų�ʱ��ʱ
#define FLAGS2_DN_ENABL           FLAGS2.BIT.BIT10            //��ů����ʹ��
#define FLAGS2_DN_RANGE           FLAGS2.BIT.BIT11            //��ů���������־
#define FLAGS2_TMP_ORHM           FLAGS2.BIT.BIT12            // 170106
#define FLAGS2_CDSENOR1_ERR       FLAGS2.BIT.BIT13            //����������1�����־
#define FLAGS2_MOTOR_REC          FLAGS2.BIT.BIT14            //���ܷ��ת����Ϣ��־
#define FLAGS2_MOTOR_SET          FLAGS2.BIT.BIT15            //���Ƶ�����ñ�־
#define FLAGS2_CDSENOR2_ERR       FLAGS2.BIT.BIT16            //����������2�����־
#define FLAGS0_CAN2COMERR         FLAGS2.BIT.BIT17           // CAN2ͨ�Ź���  ��ͨ�ź�ɫLED���� ��ͨ�ź�ɫLED�� ��ͨ�� �з��������ɫ��˸  0516yao


#define	FLAGS3_PASSCODE_RIGHT	FLAGS3.BIT.BIT0		//������֤�ɹ�
#define	FLAGS3_GET_LCD_INPUT	FLAGS3.BIT.BIT1	//��ȡLCD�������
#define	FLAGS3_GET_LCD_ID_SEND	FLAGS3.BIT.BIT2	//���������ȡLCD��ǰҳ��
#define	FLAGS3_GET_LCD_ID_RECV	FLAGS3.BIT.BIT3	//�յ���LCD��ǰҳ��
#define	FLAGS3_GET_RTC_SEND		FLAGS3.BIT.BIT4	//���������ȡLCD��ǰRTC
#define	FLAGS3_GET_RTC_RECV		FLAGS3.BIT.BIT5	//�յ���LCD��ǰRTC
#define	FLAGS3_REFRESH_LCD		FLAGS3.BIT.BIT6	//����LCDҳ��
#define	FLAGS3_DO_CTL_PIC		FLAGS3.BIT.BIT7	//��ǰ��������ҳ��
#define	FLAGS3_DO_DEV_PIC		FLAGS3.BIT.BIT8	//��ǰ�����豸ҳ��
#define	FLAGS3_DO_MANAGE_PIC	FLAGS3.BIT.BIT9	//��ǰ��������ҳ��
#define	FLAGS3_DO_HISTORY_PIC	FLAGS3.BIT.BIT10//��ǰ������ʷҳ��
#define	FLAGS3_DO_AJUST_PIC		FLAGS3.BIT.BIT11//��ǰ������װҳ��
#define	FLAGS3_DO_INSTALL_PIC	FLAGS3.BIT.BIT12//��ǰ������װҳ��
#define	FLAGS3_SAVE_TARGET		FLAGS3.BIT.BIT13//�����¶�����
#define	FLAGS3_SAVE_CTRL_CFG	FLAGS3.BIT.BIT14//���浵λ����
#define	FLAGS3_SAVE_DEV_CFG		FLAGS3.BIT.BIT15
#define	FLAGS3_SAVE_MANAGER_CFG	FLAGS3.BIT.BIT16
#define	FLAGS3_SAVE_SENSOR_AJUST	FLAGS3.BIT.BIT17
#define	FLAGS3_SAVE_INSTALL		FLAGS3.BIT.BIT18
#define	FLAGS3_DEBUG_ENTER		FLAGS3.BIT.BIT19
#define	FLAGS3_DEBUG_EXIT		FLAGS3.BIT.BIT20
#define	FLAGS3_REFRESH_LCD_2	FLAGS3.BIT.BIT21	//��ʱˢ��Һ����
#define	FLAGS3_GET_CUR_DAY_NUM	FLAGS3.BIT.BIT22	//��ʱ���㵱ǰ����
#define	FLAGS3_MODIFY_DAY_NUM	FLAGS3.BIT.BIT23	//�޸������־
#define	FLAGS3_RTC_GET_SUC		FLAGS3.BIT.BIT24	//RTC��ȡ�ɹ���־���ϵ���Ȼ�ȡRTC
#define	FLAGS3_IS_ADMIN_USER	FLAGS3.BIT.BIT25	//�Ƿ�Ϊ����Ա�˻�����admin�˻�
#define	FLAGS3_IS_NEED_LOAD		FLAGS3.BIT.BIT26	//�Ƿ���Ҫ��¼
#define	FLAGS3_IS_PRESS_MENU	FLAGS3.BIT.BIT27	//�Ƿ����˵���ť��
#define	FLAGS3_NO_PRESS			FLAGS3.BIT.BIT28	//��ʱ���޲�����־��
#define	FLAGS3_AJUST_VALUE_CLR	FLAGS3.BIT.BIT29	//У׼ֵ����ɹ����ɹ��������ʾֵ��
#define	FLAGS3_CLR_DAY_PEAK_VALUE	FLAGS3.BIT.BIT30	//ÿ��ҹ��12�����ÿ��ļ�ֵ��
	

#define	FLAGS4_POWER_OFF		FLAGS4.BIT.BIT0		//�����ϵ�
#define	FLAGS4_TEMPER_HIGH		FLAGS4.BIT.BIT1		//��������
#define	FLAGS4_TEMPER_LOW		FLAGS4.BIT.BIT2		//��������
#define	FLAGS4_TEMPER_DIFF		FLAGS4.BIT.BIT3		//�²����
#define	FLAGS4_HUMI_HIGH		FLAGS4.BIT.BIT4		//ʪ�ȹ���
#define	FLAGS4_SENSOR_ERR		FLAGS4.BIT.BIT5		//����������
#define	FLAGS4_JQID_GET_SUC		FLAGS4.BIT.BIT10	//�ɹ���ȡJQID����Ҫ����
#define	FLAGS4_ALARM_ACT_GET_SUC	FLAGS4.BIT.BIT11	//�ɹ���ȡ�����������ã���Ҫ����
#define	FLAGS4_IS_NEED_CFG_AUTO_CREATE	FLAGS4.BIT.BIT14	//�Ƿ���Ҫ�Զ��������á�0������Ҫ��1����Ҫ
#define	FLAGS4_EH_DATA_REFRESH	FLAGS4.BIT.BIT15	//ˢ��EH����
#define	FLAGS4_TOP_MENUM_PRESS	FLAGS4.BIT.BIT16	//�����TOP MENUM
#define	FLAGS4_DAC_5V_10V_SELECT	FLAGS4.BIT.BIT17	//DAC�������ѡ��0:5V			1:10V
*/


extern volatile FLAGSINT  FLAGS0;
extern volatile FLAGSINT  FLAGS1;
extern volatile FLAGSINT  FLAGS2;
extern volatile FLAGSINT  FLAGS3;
extern volatile FLAGSINT  FLAGS4;



#define BIT_SET(x, bitn) (x |= (1 << bitn))
#define BIT_CLR(x, bitn) (x &= ~(1 << bitn))
#define IS_BIT_SET(x, bitn) ((x & (1 << bitn)) != 0)
#define IS_BIT_CLR(x, bitn) ((x & (1 << bitn)) == 0)

#define LINMSG_SND               0x01
#define LINMSG_RCV               0x02


//��������������
#define TEMP_NUM   2			//�¶ȸ���
#define TEMPG_NUM  1			//�����¶ȸ���
#define HMD_NUM    0			//ʪ�ȸ���
#define DR_NUM     1
#define PHV_NUM    0
#define WF_NUM     0
#define WP_NUM     0
#define TH_NUM     0
#define NH_NUM     0


//Ӧ�ó����̺߳궨��
/*
#define	THREAD_ADC_EN	1
#define	THREAD_CAN_EN	1
#define	THREAD_DAC_EN	1
*/

//485���������
#define DEV_NUM 		25   	//485������������
#define	SHUIYA_4M		1		//ˮѹ����

#define TEMP1           1		//�¶�
#define TEMP2           2
#define TEMP3           3
#define TEMP4           4
#define TEMP5           5
#define TEMP6           6
#define HUMI1           7		//ʪ��
#define HUMI2           8
#define HUMI3           9
#define	TEMP_OUTDOOR	10		//�����¶�
#define NH3_1           11		//����
#define NH3_2           12
#define WEIGH_1         13		//����
#define WEIGH_2         14
#define WEIGH_3         15
#define WEIGH_4         16
#define WATER_PRESU1    17		//ˮѹ
#define WATER_PRESU2    18
#define PH              19
#define	CO2				20
#define	AIR_PRESSURE	21		///��ѹ��
#define	LIGHT_LUX		22		//����
#define	WATER_FLOW		23		//����
#define	EP_METER		24		//���
#define	WATER_METER		25		//ˮ��




struct Device485_Info{  //����485�������ṹ��
  uint8_t  ID;        // sn
  uint16_t Addr;
  uint8_t  Lenth;     // 
  int16_t  Data1;     // 
  int16_t  Data2;     //
  uint8_t  Dev_sta;   //
  uint8_t  Dev_EN;    //  ��������Ч
};


typedef enum 
{
	COMMU_CMD_UPLOAD_VER = 1, 				//1 �ϴ��汾
	COMMU_CMD_UPLOAD_ALARM,					//2�ϴ�������Ϣ
	COMMU_CMD_UPLOAD_SENSOR_DATE,			//3 �ϴ�����������
	COMMU_CMD_UPLOAD_RUN_INFO,				//4 �ϴ�������Ϣ
	COMMU_CMD_SYS_TARGET_TEMPER,			//5 ͬ��Ŀ���¶�
	COMMU_CMD_SYS_AIR_CTRL,					//6 ͬ��ͨ����Ϣ
	COMMU_CMD_SYS_HEATER_CTRL,				//7 ͬ������
	COMMU_CMD_SYS_COLDPAD_CTRL,				//8 ͬ��ʪ��
	COMMU_CMD_SYS_LIGHT_CTRL,				//9 ͬ���ƹ�
	COMMU_CMD_SYS_ALARM_CFG,				//10 ��������ͬ��
	COMMU_CMD_SYS_DEV_EN_CFG				//11 ����豸
}COMMU_CMD_T;
typedef	struct lcd_ctl_info
{
	uint8_t	curLcdId;
	uint8_t	curMenumId;
}lcd_ctl_info_t;



typedef union
{
	uint8_t byte[4];
	float 	f;
}FLOAT_TYPE_T;
          




//extern const uint32_t baud_table[];
//extern uint8_t Rs485_byte;
//extern uint8_t Rs485_Count;
//extern uint8_t Rs485_Databuf[];
extern uint8_t Rs232_Comand;
extern uint8_t Rs232_Rec_buf[];
extern uint8_t DN_Count;
extern uint8_t DN_RS485_COUNT;


extern uint8_t LIN_ProtBuf[];     //�����������
extern uint8_t recvDataBuf[];

extern uint8_t LIN_Length;
extern uint8_t LIN_TmpSCIStatus;
extern uint8_t LIN_TmpSCIByte;
extern uint8_t LIN_BufCounter;
extern uint8_t LIN_StateFlags;
extern uint8_t LIN_MsgDir;
extern uint8_t LIN_Err;
extern uint8_t Checksum;
extern int16_t  Temp_RS485[];
extern uint8_t  Temp_Sta[];
extern uint8_t  Err_Count[];
extern uint16_t Hmd_RS485[];
extern uint8_t  Hmd_Sta[];
extern uint16_t PHV_RS485[];
extern uint8_t  PHV_Sta[];
extern uint16_t WF_RS485[];
extern uint8_t  WF_Sta[];
extern uint16_t WP_RS485[];
extern uint8_t  WP_Sta[];
extern uint32_t TEHD_RS485[];
extern uint8_t  TEHD_Sta[];
extern uint16_t NH3_RS485[];
extern uint8_t  NH3_Sta[]; 

extern int16_t  Temp_RS232[];
extern uint16_t Hmd_RS232[];
extern uint16_t PHV_RS232[];
extern uint16_t WF_RS232[];
extern uint16_t WP_RS232[];
extern uint32_t TEHD_RS232[];
extern uint16_t NH3_RS232[];
//extern uint16_t  CD_RS232[];   //0220yao
extern uint8_t  Temp232_Sta[];
extern uint8_t  HMD232_Sta[];
extern uint8_t  PHV232_Sta[];
extern uint8_t  WF232_Sta[];
extern uint8_t  WP232_Sta[];
extern uint8_t  TEHD232_Sta[];
extern uint8_t  NH3232_Sta[];
extern uint8_t  DrIn_Sta[];
extern uint8_t  DrRe_Sta[];

extern uint8_t  RS232ERR_Cout;
//extern uint8_t  FANDR_Cout;
extern uint8_t  TIM_Count;
//extern uint8_t  CAN1ERR_Cout;
//extern uint8_t  CAN2ERR_Cout;
extern uint8_t  RS232Send_Cout;
//extern uint8_t  Num_Sensor;

extern uint8_t  RS232Temp_Num;
extern uint8_t  RS232Hmd_Num;
extern uint8_t  RS232DR_Num;
extern uint8_t  RS232PHV_Num;
extern uint8_t  RS232WF_Num;
extern uint8_t  RS232WP_Num;
extern uint8_t  RS232TH_Num;
extern uint8_t  RS232NH_Num;
extern uint8_t  RS232WG_Num;
extern uint8_t  RS232RF_Num;
extern uint8_t  RS232GRT_Num;
extern uint8_t  SendCon_Count;
extern uint8_t  RS232CD_Num;   //0220yao


extern uint8_t	driveBoardNum;




extern uint8_t  Val_3P3V;
extern uint8_t  Val_5V;
extern uint8_t  Val_5VGPRS;
extern uint8_t  Val_9V;
extern uint8_t  Val_12V;
extern uint8_t  Val_BAT;
extern uint8_t  Val_EXTBAT;
extern uint8_t  Val_24V;
extern uint16_t INT_Temp;
extern uint16_t INT_HMD;

extern uint8_t  Val_3P3V;
extern uint8_t  Val_5V;
extern uint8_t  Val_5VGPRS;
extern uint8_t  Val_9V;
extern uint8_t  Val_12V;
extern uint8_t  Val_BAT;
extern uint8_t  Val_EXTBAT;
extern uint8_t  Val_24V;
extern uint16_t INT_Temp;
extern uint16_t INT_HMD;
//extern uint8_t  Adc_Count;
extern uint8_t  Debug_dat[];
extern uint8_t  Temp_NumDG;
extern uint8_t  LeaTST_Count; 
extern uint16_t DouLED;
extern uint8_t  Ref_Count;
extern uint8_t  CoLED_Count; 
extern uint8_t  Test_DLED;
extern uint16_t Test_TFT;
extern uint8_t  Test_CAN;
extern uint8_t  TFT_Count;
extern uint8_t  TstOV_Count;
extern uint8_t  CoreP_ICODIP[];
extern uint8_t  TstOV_Count;
extern unsigned short RS485_TEMP;
extern uint32_t CLED_Color;
extern uint8_t  TEMPNum_Count;

extern uint16_t Temp1_COLOR;
extern uint16_t KG_COLOR;
extern uint16_t RFID_COLOR;
extern uint16_t RUNMD_COLOR;
extern uint16_t DEVMOD_COLOR;
extern uint16_t Temp1_Sort;
extern uint16_t Weight_Sort[4];
extern uint16_t RFID;
extern uint8_t  RunMode;
extern uint8_t  DevMode;
extern uint16_t TFT_REFCount;
extern uint8_t  Tmpcount;
extern uint16_t CoreRstCount;
//extern uint8_t  CoreStCount;
extern uint8_t  SEN_CurrentIdx;
extern struct  Device485_Info Dev_info[DEV_NUM];   //0222
extern uint8_t  SEN_Count;
extern uint16_t Rs485_threadCount;
extern uint16_t Rs485REC_threadCount;
extern uint8_t  NoCore_Count;
//extern uint16_t datebuf[CD_NUMB][MAX_CDNUMB][MAX_CDCOUNT];            //0221yao
//extern uint8_t	currentBoardData[MAX_CURRENT_CHANNEL];//0814wl
extern uint8_t CD_channel_num;
//extern uint16_t MOTOR_Count;//0421yao
extern uint16_t MOTOR_SPEED_SEND;
extern uint16_t MOTOR_SPEED_REC[SPEED_FAN_NUM];





//extern Vector gDevCondState[seasonMax][DAY_LINE_MAX];
//extern targetTemp_t	targetTemp[DAY_LINE_MAX];
//extern tempAlarmShift_t	tempAlarmShift;
//extern uint8_t	seasonMode;
extern devModeCfg_t	devModeCfg[seasonMax];
extern alarmCfg_t		alarmCfg;
extern alarmCfg_t	alarmCfgTemp;

extern sensorCfg_t	sensorCfg;
extern sensorAjust_t	sensorAjustTemp;
extern  uint16_t	noPressCnt;
//extern uint8_t		debugDevCtrl[devMax];
extern devInfo_t	devInfo;

/////temp
extern devModeCfg_t	tempModeCfg;

extern devDebugCtrl_t	devDebugCtrl;
extern rtcTime_t		curRtc;
//extern uint16_t	ajustShowData[TEMP_SENSOR_NUM_MAX+1];
extern uint8_t	curPicId;
extern uint8_t	curTopMenumId;
extern uint8_t	curSecMenumId;
extern uint8_t	onlyRefreshData;

extern int8_t	dayNumLevel, tempStep;
extern int16_t	curAvgTemp;
extern int16_t	curAvgHumi;
extern int16_t tempData[TEMP_SENSOR_NUM_MAX];
extern int16_t	humiData[HUMI_SENSOR_NUM_MAX];
extern int16_t tempMax[TEMP_SENSOR_NUM_MAX];
extern int16_t	tempMin[TEMP_SENSOR_NUM_MAX];
extern int16_t	humiMax[HUMI_SENSOR_NUM_MAX], humiMin[HUMI_SENSOR_NUM_MAX];

extern uint8_t	lcdCtrlSaveId;
extern uint8_t	devSaveId;

extern uint8_t	alarmHappenFlag[alarmEventMax];

extern uint16_t softVersion;
extern uint16_t hardVersion;
extern uint16_t lcdSoftVersion ;
extern uint16_t		lcdRecvAddr;
extern uint16_t		lcdRecvData;

extern alarmRunSta_t	alarmRunSta;
extern devAlarmFlag_t	devAlarmFlag;

extern uint8_t	devRunSta[allDevMax];




//extern char	imeiCode[IMEI_LEN+1];

extern uint8_t	jqidGetEn;//
extern uint8_t	jqidEmpty;//j
extern uint8_t	jqidGetForce;
extern char isEsimConnect;
extern int esimLostCnt;
extern char esimLostFlag;
extern uint8_t esimConnectErrCnt;
extern bool esimErr;


typedef struct  
{
	uint8_t syncTime;  
	uint8_t alarmEnable;	
	uint8_t alarmPara;
	uint8_t temp;	
	uint8_t humidity;
	uint8_t nh3;
	uint8_t floorHeat;
	uint8_t getRemoteJqid;
	uint8_t getRssi;
}_uploadMes;


typedef struct 
{
	uint8_t Commu_Sysc_Time;
	uint8_t Commu_Upload_Sensor;
	uint8_t Commu_Upload_DevSta;
	uint8_t Commu_Upload_Cfg;
	uint8_t Commu_Upload_DevEn;
	uint8_t Commu_Sys_TargetCfg;
	uint8_t Commu_Sys_AirCtrlCfg;
	uint8_t Commu_Sys_HeaterCfg;
	uint8_t Commu_Sys_WaterPadCfg;
	uint8_t Commu_Sys_Light_Cfg;
	uint8_t Commu_Sys_AlarmCfg;
}COMMU_EN_T;


#define	FLASH_ID_CFG_SAVE_ADDR	0X0801DC00

#define	FLASH_SAVE_PAGE_NUM	6
//devInfo 0X0801E000
//sensorCfg	0X0801E400
//alarmCfg 0X0801E800
//devModeCfg 0,1,2   0X0801Ec00  0X0801f000  0X0801f400
#define	PAGE_SIZE					0X400
#define	FLASH_DEV_INFO_SAVE_ADRR	0X0801E000
#define	FLASH_SENSOR_CFG_SAVE_ADDR	FLASH_DEV_INFO_SAVE_ADRR+PAGE_SIZE
#define	FLASH_ALARM_CFG_SAVE_ADDR	FLASH_SENSOR_CFG_SAVE_ADDR+PAGE_SIZE
#define	FLASH_MODE_CFG_SAVE_ADDR_1	FLASH_ALARM_CFG_SAVE_ADDR+PAGE_SIZE
#define	FLASH_MODE_CFG_SAVE_ADDR_2	FLASH_MODE_CFG_SAVE_ADDR_1+PAGE_SIZE
#define	FLASH_MODE_CFG_SAVE_ADDR_3	FLASH_MODE_CFG_SAVE_ADDR_2+PAGE_SIZE


extern void Init_Vars(void);
extern void Change_Sen(unsigned char flag);
uint8_t	getDevInfoData(void);
uint8_t	saveDevInfoData(void);

extern uint8_t	getSensorCfgData(void);
extern uint8_t	saveSensorCfgData(void);
extern uint8_t	getAlarmCfgData(void);
extern uint8_t saveAlarmCfgData(void);
extern uint8_t getModeCfgData(void);
extern uint8_t saveModeCfgData(uint8_t season);


extern uint8_t	saveDevIdCfg(void);


extern uint8_t LTE_RecvBuffer[LTE_RECV_BUFFER_LEN];
extern uint16_t LTE_RecvLen;
extern uint8_t	LTE_RecvByte;
extern uint8_t LTE_RecvBuf_Busy;
extern uint8_t esimSumCheck;
extern uint32_t jqid;

extern uint8_t jqidString[];
extern COMMU_EN_T	m_CommuEn_T;
extern uint8_t UART4_RecvBuf[];
extern uint8_t UART4_RecvByte;
extern uint8_t UART4_RecvLen;

extern uint8_t UART2_RecvByte;

extern uint32_t inputBits;
extern uint32_t outputBits;
extern uint16_t dacValueOut[4] ;
extern uint16_t adcValueIn[4];
extern int16_t	curAvgTemp;						//ƽ���¶�
extern int16_t curAvgTemp_DayMax;				//��ƽ���¶����ֵ
extern int16_t curAvgTemp_DayMin;				//��ƽ���¶���Сֵ
extern int16_t	curAvgHumi;						//ƽ��ʪ��
extern int16_t curAvgHumi_DayMax;				//��ƽ��ʪ�����ֵ
extern int16_t curAvgHumi_DayMin;				//��ƽ��ʪ����Сֵ
extern uint16_t curElecPower_DayUseStart;				//�պĵ���ʼֵ
extern uint16_t curElecPower_DayUse;					//�պĵ���
extern uint16_t curWaterPower_DayUseStart;				//�պ�ˮ����ʼֵ
extern uint16_t curWaterPower_DayUse;					//�պ�ˮ��


extern uint8_t curDayNum;						//��ǰ����
extern uint8_t curDayLevel;					//��ǰ���伶��
extern uint8_t curAirLevel;					//��ǰͨ�缶��
extern uint16_t curTargetTemp;					//��ǰĿ���¶�
extern uint8_t m_Is_PowerOff;					//�Ƿ�ϵ�
extern uint8_t m_Is_LTE_Connect ;
extern uint8_t m_BatVol_Percent;
extern uint8_t m_Is_Charging;
extern uint8_t m_LTE_Rssi;
extern uint8_t m_Is_Tcp_ConnectOk;

extern uint8_t m_LightCtrl_En;
extern uint8_t m_Is_AlarmOccur;
extern uint8_t m_Is_NeedRefresh_LCD_Cfg;

#endif




















