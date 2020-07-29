#include  "iic.h"
#include  "bsp.h"
#include  "vars.h"
#pragma diag_suppress 174




void IIC_NOP(void){
  __NOP;
  __NOP;
  __NOP;
  __NOP;
}

void IIC_Delay(void){
  uint32_t i=100;
  while(i--); 
}
void delay_ms1(uint8_t n){
  int i;
  while(n--)
  for(i=0;i<12000;i++);
}

uint8_t IIC_Start(void){
  #if 1
  IIC_SDA_H(); 
  IIC_Delay();
   
  IIC_SCL_H(); 
  IIC_Delay();    
  if(!SDAread) {
    ////DebugPrint("IIC_START:BUSY\n");
    return IIC_BUS_BUSY;
  }
  IIC_SDA_L();
  IIC_Delay();
  
  IIC_SCL_L();  
  IIC_Delay(); 
  if(SDAread) {
    ////DebugPrint("IIC_START:BUS ERROR\n");
    return IIC_BUS_ERROR;
  }
  return IIC_READY;
  #else
  IIC_SCL_L();
  IIC_NOP();
  IIC_SDA_H();
  IIC_SCL_L();
  IIC_Delay();
  IIC_SDA_L();
  IIC_Delay();
  IIC_SCL_L();
  return IIC_READY;
  #endif
}

void IIC_Stop(void){
  #if 1
  IIC_SCL_H();
  IIC_SDA_L();
  IIC_Delay();
  IIC_SDA_H();
  #else
  #endif
}

void IIC_Ack(void){  
  IIC_SCL_L();
  IIC_SDA_L();
  IIC_Delay();
  IIC_SCL_H();
  IIC_Delay();
  IIC_SCL_L();
}

void IIC_NoAck(void){  
  IIC_SCL_L();
  IIC_SDA_H();
  IIC_Delay();
  IIC_SCL_H();
  IIC_Delay();
  IIC_SCL_L();
}

bool IIC_WaitAck(void) {   //����ֵ=1��ʾ��ACK; //����ֵ=0��ʾ��ACK.
  IIC_SCL_L();    
  IIC_Delay();
  IIC_SCL_H();
  IIC_Delay();
  if(SDAread){
    IIC_SCL_L();
    return FALSE;
  }
  IIC_SCL_L();
  return TRUE;
}

void IIC_SendByte(uint8_t SendByte) { //���ݴӸ�λMSB����λLSB����
  uint8_t i=8;
  
  while(i--){
    IIC_SCL_L();//SCLΪ�͵�ƽSDA�ſ��Ա䶯
    IIC_Delay();
    if(SendByte&0x80)
      IIC_SDA_H();  
    else 
      IIC_SDA_L();   
    SendByte<<=1;
    IIC_Delay();
    IIC_SCL_H();
    IIC_Delay();
  }
  IIC_SCL_L();
}

uint8_t IIC_ReceiveByte(void) {  //���ݴӸ�λ����λ
  uint8_t i=8;
  uint8_t ReceiveByte=0;

  IIC_SDA_H();        
  while(i--){
    ReceiveByte<<=1;      
    IIC_SCL_L();
    IIC_Delay();
    IIC_SCL_H();
    IIC_Delay();  
    if(SDAread) {
      ReceiveByte|=0x01;
    }
  }
  IIC_SCL_L();
  return ReceiveByte;
}

//д��һ���ֽ�  ��д���ֽ�    ��д���ַ    ��������(M24C02)
bool IIC_WriteByte(uint8_t SendByte, uint8_t DeviceAddress){    
  if(IIC_Start())
    return FALSE;
  IIC_SendByte(DeviceAddress|0x0);
  if(!IIC_WaitAck()) {
    IIC_Stop(); 
    return FALSE;
  }  
  IIC_SendByte(SendByte);
  IIC_WaitAck();   
  IIC_Stop(); 
  return TRUE;
}



//ע�ⲻ�ܿ�ҳд
//д����һҳ������   ��д�������ַ ��д�볤��    ��������(M24C02)
bool IIC_BufferWrite(uint8_t* pBuffer, uint8_t length, uint8_t DeviceAddress){
  if(IIC_Start())
     return FALSE;
  IIC_SendByte(DeviceAddress |0x0); 
  if(!IIC_WaitAck()) {
    IIC_Stop();
    return FALSE;
  }
  while(length--) {
    IIC_SendByte(*pBuffer);
    IIC_WaitAck();
    pBuffer++;
  }
   IIC_Stop();
   return TRUE;
}

//����1������     ��Ŷ�������  ����������    ��������ַ    ��������(M24C02)  
bool IIC_ReadByte(uint8_t* pBuffer,   uint8_t length,    uint8_t DeviceAddress){    
  if(IIC_Start())
    return FALSE;
  IIC_SendByte(DeviceAddress |0x1);
  if(!IIC_WaitAck()){
    IIC_Stop();
    return FALSE;
  }
  // IIC_Start();
  //IIC_SendByte(DeviceAddress|0x1);
  //IIC_WaitAck();
  ///while(!SCLread);
  delay_ms1(35);        // �ȴ�ת��
  while(length) {
    *pBuffer = IIC_ReceiveByte();
    if(length == 1)
      IIC_NoAck();
    else 
      IIC_Ack(); 
    pBuffer++;
    length--;
  }
  IIC_Stop();
  return TRUE;
}

bool IIC_ReadByte1(uint8_t* pBuffer,   uint8_t length,    uint8_t DeviceAddress){    
  if(IIC_Start())return FALSE;
  IIC_SendByte(DeviceAddress |0x1);
  if(!IIC_WaitAck()) {
    IIC_Stop();
    return FALSE;
  }
  // IIC_Start();
  //IIC_SendByte(DeviceAddress|0x1);
  //IIC_WaitAck();
  ///while(!SCLread);
  //delay_ms1(35);        // �ȴ�ת��
  while(length) {
    *pBuffer = IIC_ReceiveByte();
    if(length == 1)
      IIC_NoAck();
    else 
      IIC_Ack(); 
    pBuffer++;
    length--;
  }
  IIC_Stop();
  return TRUE;
}


/****************************************************************************/
/* ��������write_iic_data                                                   */
/* ��  �ܣ���iic�豸д��N���ֽ�                                             */
/* ��  �ߣ�spot                                                             */
/* ��  �룺                                                                 */
/*         write_data              ���д���ֽڵ�����                       */
/*         address                 ָ����address����ʼд��                  */
/*         num                     д����ֽ���                             */
/*                                                                          */
/* ��  ������                                                               */
/*                                                                          */
/* ��ʷ��¼��                                                               */
/*  spot          2005-06-20     Creat Inital version. (Version 1.0)        */
/****************************************************************************/
void write_iic_data(unsigned char write_data[], unsigned char address, unsigned char cmd,unsigned char num){
  unsigned char n;
  IIC_Start();
  IIC_SendByte(address+1);
  (void)IIC_WaitAck();  
  IIC_SendByte(cmd);
  (void)IIC_WaitAck();
  for(n=0; n<num; n++){
    IIC_SendByte(write_data[n]);
    (void)IIC_WaitAck();
  }
  IIC_Stop();
}
/****************************************************************************/
/* ��������read_iic_data                                                    */
/* ��  �ܣ���iic�豸��ȡN���ֽ�                                             */
/* ��  �ߣ�spot                                                             */
/* ��  �룺                                                                 */
/*         read_data               ��Ŷ�ȡ�ֽڵ�����                       */
/*         address                 ָ����address����ʼ��ȡ                  */
/*         num                     ��ȡ���ֽ���                             */
/*                                                                          */
/* ��  ������                                                               */
/*                                                                          */
/* ��ʷ��¼��                                                               */
/*  spot          2005-06-20     Creat Inital version. (Version 1.0)        */
/****************************************************************************/
// S | Slave Address | W  | A | Read Reg Cmd | A | Sr | Slave Address | R | A | Read Data | NA | P |
void Read_IIC_Data(unsigned char read_data[], unsigned char address,unsigned char cmd, unsigned char num,unsigned char type){
  unsigned char n;
  IIC_Start();
  IIC_SendByte(address+1);  
  (void)IIC_WaitAck();  
  IIC_SendByte(cmd);
  (void)IIC_WaitAck();
  IIC_Start();
  IIC_SendByte(address);
  if (type == 0){
    (void)IIC_NoAck();
    IIC_Start();
    IIC_SendByte(address);
  }
  (void)IIC_WaitAck();
  for (n=0; n<num; n++) {
    read_data[n] = IIC_ReceiveByte();
    if (n != (num - 1)){     // ���һ�����ݲ�Ӧ��     
      IIC_Ack();
    }
  }
  
  
  IIC_NoAck();
  IIC_Stop();
}



#define TEHM_ADD    0x80
void TEHM_Hot(void){
  
  // S | Slave Address | W  | A | 0xE0         | A | Sr | Slave Address | R | A | MS Byte   | A  | LS Byte | NA | P|
   //IIC_ReadByte(Recdata[1],1,0x80)
}

void si7020_function(void){
  uint8_t Recdata[2]={0};
  Read_IIC_Data(Recdata,TEHM_ADD,READ0HUMI_NOHOLD,2,0);     // Measure Relative Humidity, No Hold Master Mode
  Debug_dat[0] = Recdata[0];
  Debug_dat[1] = Recdata[1];
  Read_IIC_Data(Recdata,TEHM_ADD,READ0TEMP_NOHOLD,2,1); 
  Debug_dat[0] = Recdata[0];
  Debug_dat[1] = Recdata[1];

  #if 0
  uint8_t cmd=0xe5,i;
  uint8_t Recdata[2][5]={0};
  uint8_t sendata[10]={WR_USR_REG,0x3A};
  
  if(IIC_WriteByte(cmd,0x80))
    ;//debug_uart1_sendbyte(0x11);
  if(IIC_ReadByte(Recdata[0],2,0x80)){
    Debug_dat[0] = Recdata[0][0];//debug_uart1_sendbyte(0x22);
    Debug_dat[1] = Recdata[0][1];
  }
  
  if(IIC_WriteByte(0xe3,0x80))
    ;//debug_uart1_sendbyte(0x11);
  if(IIC_ReadByte(Recdata[0],2,0x80)){
    Debug_dat[2] = Recdata[0][0];//debug_uart1_sendbyte(0x22);
    Debug_dat[3] = Recdata[0][1];
  }
  //for(i=0;i<2;i++)
  //  debug_uart1_sendbyte(Recdata[0][i]);
  
  if(IIC_WriteByte(RE_HEAT_REG,0x80))  
    ;//debug_uart1_sendbyte(0x33);
  if(IIC_ReadByte(Recdata[1],1,0x80))
    Debug_dat[4] = Recdata[1][0];//debug_uart1_sendbyte(0x44);
  //for(i=0;i<1;i++)
    //debug_uart1_sendbyte(Recdata[1][i]);
    
  if(IIC_BufferWrite(sendata,2,0x80))
    ;//debug_uart1_sendbyte(0x55);
  #endif
}

