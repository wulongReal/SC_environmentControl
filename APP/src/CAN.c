#include "can.h"
#include "vars.h"
#include "bsp.h"


#define GPIO_Remap_CAN GPIO_Remap1_CAN1
CanTxMsg TxMessage1;



/******************CAN1�жϽ������ú���*****************************************/
//�����ʣ�SJW-1tq��BS1-8tq��BS2-7tq��Prescaler-5;
/*
void CAN1_Init(void) {
  
  CAN_InitTypeDef CAN_InitStructure;
  CAN_FilterInitTypeDef CAN_FilterInitStructure;
   NVIC_InitTypeDef	NVIC_InitStructure;

  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  // CAN cell init 
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = ENABLE;
  CAN_InitStructure.CAN_AWUM = ENABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
  CAN_InitStructure.CAN_Prescaler = 8;//2     0413
  CAN_Init(CAN1, &CAN_InitStructure);

 // CAN filter init 
  CAN_FilterInitStructure.CAN_FilterNumber=0;
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
 
  	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级�?
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级�?
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

}
*/

u8	canModeInit(u8	tsjw, u8 tbs2, u8 tbs1,u16 brp, u8	mode)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	CAN_InitTypeDef		CAN_InitStructure;
	CAN_FilterInitTypeDef	CAN_FilterInitStructure;

	//#if	CAN_RX0_INT_ENABLE
	NVIC_InitTypeDef	NVIC_InitStructure;
	//#endif

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);		//��ʼ��IO
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��IO
 	//CAN��Ԫ����
 	  CAN_InitStructure.CAN_TTCM=DISABLE;						 //��ʱ�䴥��ͨ��ģʽ  //
 	  CAN_InitStructure.CAN_ABOM=DISABLE;						 //����Զ����߹���	 //
  	CAN_InitStructure.CAN_AWUM=DISABLE;						 //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)//
  	CAN_InitStructure.CAN_NART=ENABLE;						 	//��ֹ�����Զ����� //
  	CAN_InitStructure.CAN_RFLM=DISABLE;						 //���Ĳ�����,�µĸ��Ǿɵ� // 
  	CAN_InitStructure.CAN_TXFP=DISABLE;						 //���ȼ��ɱ��ı�ʶ������ //
  	CAN_InitStructure.CAN_Mode= mode;	         //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; //
  	//���ò�����
  	CAN_InitStructure.CAN_SJW=tsjw;				//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;            //��Ƶϵ��(Fdiv)Ϊbrp+1	//
  	CAN_Init(CAN1, &CAN_InitStructure);            // ��ʼ��CAN1 

 	  CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
   	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
 	  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0

  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
//#if CAN_RX0_INT_ENABLE
	
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
//#endif
	return 0;

}


//#define CAN_RX0_INT_ENABLE 
#if	1
void	USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg rxMessage;
	u8	i = 0;

	CAN_Receive(CAN1, 0, &rxMessage);

	for(i = 0;i < 8;i++)
	{
		//printf("rxbuf[%d] = 0x%x\r\n", i, rxMessage.Data[i]);
	}
}
#endif
//can����һ�����ݣ��̶���ʽ��ID Ϊ0x12, ��׼֡������֡
u8	canSendMsg(u8 * msg, u8 len)
{
	u8 mbox;
	u16 i = 0;
	CanTxMsg	txMsg;

	txMsg.StdId = 0x41;
	txMsg.ExtId = 0X12;
	txMsg.IDE = CAN_Id_Standard;
	txMsg.RTR = CAN_RTR_DATA;
	txMsg.DLC = len;
	for(i = 0;i < len;i++)
	{
		txMsg.Data[i] = msg[i];
	}
	mbox = CAN_Transmit(CAN1, &txMsg);
	i = 0;
	while((CAN_TransmitStatus(CAN1,mbox) == CAN_TxStatus_Failed) && (i < 0xfff))
	{
		i++;
	}
	if(i >= 0xfff)
		return 1;

	return 0;
}

/******************CAN1���ݴ��ͺ���*******************************************/
//����:DataLength:�����ֽڳ���
//     Data_buf  :���ͻ������׵�ַ
//     addr:      �豸��ַ
//     Cmd:       ������
/***************************************************************************/
void CAN_TransmitData(uint8_t DataLength, uint8_t *Data_Buf, uint8_t Addr, uint8_t Cmd) {
  /* transmit 1 message */
  CanTxMsg TxMessage;
  u32 i;
  uint8_t TransmitMailbox = 0;
  uint8_t CAN_Transmitindex = 0;
  
  //if (FLAGS0_NOEXTPOW==0){   // 160926
  
    TxMessage.StdId = (((uint16_t)(Cmd))<<6)|(Addr);//   can id
    //TxMessage.ExtId = ID;
    TxMessage.IDE = CAN_ID_STD;//��׼֡��ʽ
    TxMessage.RTR = CAN_RTR_DATA;
    //_______________Set Data length _________________
    TxMessage.DLC = DataLength;
    for (CAN_Transmitindex = 0; CAN_Transmitindex < DataLength;
        CAN_Transmitindex++) {
      TxMessage.Data[CAN_Transmitindex] = Data_Buf[CAN_Transmitindex];
    }
    //_____________________________________________
    TransmitMailbox = CAN_Transmit(CAN1,&TxMessage);

    i=0;
    while((CAN_TransmitStatus(CAN1,TransmitMailbox) != CANTXOK) && (i != 0x80))
    i++;
    
    i=0;
    while((CAN_MessagePending(CAN1,CAN_FIFO0) < 1) && (i != 0x80))
    i++;
  //}
}

uint8_t  CAN_Recei_Data(uint8_t DataLength, uint8_t *Data_buf, uint8_t Addr, uint8_t Cmd ){
  #if 1
  uint8_t  i;
  uint32_t ID;
  CanRxMsg RxMessage1;
  
  
  //if( CAN_GetFlagStatus( CAN1, CAN_FLAG_FF0 ) == SET )  {
  //}
  //if (FLAGS0_NOEXTPOW==1)    // 160926
  //  return CAN_NO_POWER;
  ID = Cmd;
  ID <<= 6;
  ID += Addr;
  //#define CAN_ID_ERR     0x01
  //#define CAN_IDE_ERR    0x02
  //#define CAN_LEN_ERR    0x04
  //#define CAN_NO_ERR     0x08
  //#define CAN_NO_POWER   0x10
  
  if(0 == CAN_GetFlagStatus(CAN1,CAN_FLAG_FF0)) {
    CAN_ClearFlag(CAN1,CAN_FLAG_FF0);
    return CAN_NO_DATA;
  //} else if(1 == CAN_GetFlagStatus(CAN1,CAN_FLAG_FOV0)) {
  //  CAN_ClearFlag(CAN1,CAN_FLAG_FOV0);
  //  return CAN_ID_ERR;
  } else {     
    CAN_Receive(CAN1,CAN_FIFO0, &RxMessage1);    
    if (RxMessage1.DLC!=DataLength) { //���ͺͽ��ղ�һ�£�����FAILED
      return CAN_LEN_ERR;
    }
    if (RxMessage1.StdId == ID){  //���ͺͽ��ղ�һ�£�����FAILED
      for(i=0;i<DataLength;i++)
        Data_buf[i] = RxMessage1.Data[i];
    } else {
      return CAN_ID_ERR;
      //return (ID >> 6);
    }
    if (RxMessage1.IDE != CAN_ID_STD) { //���ͺͽ��ղ�һ�£�����FAILED    CAN_Id_Standard
      return CAN_IDE_ERR;
    }
  }
  #endif
  return CAN_NO_ERR; /* Test Passed */ //���򷵻�PASSED
}

//void CAN_Init_Config(void) {
//  CAN1_Init();          //�жϽ�������
//}




