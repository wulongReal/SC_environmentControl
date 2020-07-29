#include "font.h"
#include "lcd.h"
#include "bmp.h"

u16 POINT_COLOR=0x0000;	//������ɫ
u16 BACK_COLOR=0xFFFF;  //����ɫ 

//����LCD��Ҫ����
_lcd_dev lcddev;
  

uint8_t SPI_WriteByte(SPI_TypeDef* SPIx,uint8_t Byte){
  while((SPIx->SR&SPI_I2S_FLAG_TXE)==RESET);    //�ȴ���������    
  SPIx->DR=Byte;     //����һ��byte   
  while((SPIx->SR&SPI_I2S_FLAG_RXNE)==RESET);//�ȴ�������һ��byte  
  return SPIx->DR;                 //�����յ�������      
}
       
//д�Ĵ�������
//regval:�Ĵ���ֵ
void LCD_WR_REG(uint16_t regval){ 
  SPILCD_CS_L();  //LCD_CS=0
  SPILCD_RS_RESET;
  SPI_WriteByte(SPI2,regval&0x00FF);  
  SPILCD_CS_H();  //LCD_CS=1          
}
//дLCD����
//data:Ҫд���ֵ
void LCD_WR_DATA(uint16_t data){
  SPILCD_CS_L();  //LCD_CS=0
  SPILCD_RS_SET;  
  SPI_WriteByte(SPI2,data>>8);
  SPI_WriteByte(SPI2,data);
  SPILCD_CS_H();  //LCD_CS=1    
}
void LCD_WR_DATA8(uint8_t da) {  //д8λ����
  SPILCD_CS_L();  //LCD_CS=0
  SPILCD_RS_SET;                 
  SPI_WriteByte(SPI2,da);  
  SPILCD_CS_H();  //LCD_CS=1          
}             
//д�Ĵ���
//LCD_Reg:�Ĵ�����ַ
//LCD_RegValue:Ҫд�������
void LCD_WR_REG_DATA(uint8_t LCD_Reg, uint16_t LCD_RegValue){
  LCD_WR_REG(LCD_Reg);
  LCD_WR_DATA(LCD_RegValue);
}
//��ʼдGRAM
void LCD_WriteRAM_Prepare(void){
  LCD_WR_REG(lcddev.wramcmd);  
}   
//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
void opt_delay(uint8_t i){
  while(i--);
}       
//LCD������ʾ
void LCD_DisplayOn(void){             

}   
//LCD�ر���ʾ
void LCD_DisplayOff(void){     

}   

//���ù��λ��
//Xpos:������
//Ypos:������
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos){
  LCD_WR_REG(lcddev.setxcmd); 
  LCD_WR_DATA8(Xpos>>8); 
  LCD_WR_DATA8(Xpos&0xFF);   
  LCD_WR_REG(lcddev.setycmd); 
  LCD_WR_DATA8(Ypos>>8); 
  LCD_WR_DATA8(Ypos&0xFF);
}     

void delay_ms(u16 nms){  
  uint16_t i,j;
  for(i=0;i<nms;i++){
    for(j=0;j<800;j++)
    ;
  }
}
//��ʼ��lcd
void LCD_Init(void){      
  SPILCD_RST_RESET ;  //LCD_RST=0   //SPI�ӿڸ�λ
  delay_ms(200); // delay 20 ms 
  SPILCD_RST_SET ;  //LCD_RST=1    
  delay_ms(200);
  ////DeviceIdCode = LCD_ReadReg(R00H);//LCD_ReadID();

  lcddev.width  =320;
  lcddev.height =240;
  lcddev.wramcmd=0x2C;
  lcddev.setxcmd=0x2A;
  lcddev.setycmd=0x2B;

  #if 1  

  LCD_WR_REG(0xCF);    // Power control B
  LCD_WR_DATA8(0x00); 
  LCD_WR_DATA8(0xD9); 
  LCD_WR_DATA8(0x30); 
   
  LCD_WR_REG(0xED);    // Power on sequence control
  LCD_WR_DATA8(0x64); 
  LCD_WR_DATA8(0x03); 
  LCD_WR_DATA8(0x12); 
  LCD_WR_DATA8(0x81); 
   
  LCD_WR_REG(0xE8);      // Driver timing control A
  LCD_WR_DATA8(0x85); 
  LCD_WR_DATA8(0x10); 
  LCD_WR_DATA8(0x78); 
   
  LCD_WR_REG(0xCB);      // Power control A
  LCD_WR_DATA8(0x39); 
  LCD_WR_DATA8(0x2C); 
  LCD_WR_DATA8(0x00); 
  LCD_WR_DATA8(0x34); 
  LCD_WR_DATA8(0x02); 
   
  LCD_WR_REG(0xF7);      // Pump ratio control
  LCD_WR_DATA8(0x20); 
   
  LCD_WR_REG(0xEA);      // Driver timing control B
  LCD_WR_DATA8(0x00); 
  LCD_WR_DATA8(0x00); 
   
  LCD_WR_REG(0xC0);     //Power control 
  LCD_WR_DATA8(0x21);   //VRH[5:0] 
   
  LCD_WR_REG(0xC1);     //Power control 
  LCD_WR_DATA8(0x12);   //SAP[2:0];BT[3:0] 
   
  LCD_WR_REG(0xC5);    //VCM control 
  LCD_WR_DATA8(0x32); 
  LCD_WR_DATA8(0x3C); 
   
  LCD_WR_REG(0xC7);     //VCM control2 
  LCD_WR_DATA8(0xC1); 
   
  LCD_WR_REG(0x36);    // Memory Access Control 
  //LCD_WR_DATA8(0x08);  //����� 
  LCD_WR_DATA8((1<<5)|(0<<6)|(1<<7)|(1<<3));
   
  LCD_WR_REG(0x3A);     // COLMOD: Pixel Format Set
  LCD_WR_DATA8(0x55); 
  
  LCD_WR_REG(0xB1);     // Frame Rate Control (In Normal Mode/Full Colors) (
  LCD_WR_DATA8(0x00);    
  LCD_WR_DATA8(0x18); 
   
  LCD_WR_REG(0xB6);    // Display Function Control 
  LCD_WR_DATA8(0x0A); 
  LCD_WR_DATA8(0xA2);
     
  LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
  LCD_WR_DATA8(0x00); 
   
  LCD_WR_REG(0x26);    //Gamma curve selected 
  LCD_WR_DATA8(0x01);
  
  
  LCD_WR_REG(0x2B);     //set the page address ����Ļ����
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0xEF);
  
  LCD_WR_REG(0x2A);    //set the column address
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x01);
  LCD_WR_DATA8(0x3F);


  LCD_WR_REG(0xE0);    //Set Gamma 
  LCD_WR_DATA8(0x0F); 
  LCD_WR_DATA8(0x3A); 
  LCD_WR_DATA8(0x36); 
  LCD_WR_DATA8(0x0B); 
  LCD_WR_DATA8(0x0D); 
  LCD_WR_DATA8(0x06); 
  LCD_WR_DATA8(0x4C); 
  LCD_WR_DATA8(0x91); 
  LCD_WR_DATA8(0x31); 
  LCD_WR_DATA8(0x08); 
  LCD_WR_DATA8(0x10); 
  LCD_WR_DATA8(0x04); 
  LCD_WR_DATA8(0x11); 
  LCD_WR_DATA8(0x0C); 
  LCD_WR_DATA8(0x00); 
   
  LCD_WR_REG(0xE1);    //Set Gamma 
  LCD_WR_DATA8(0x00); 
  LCD_WR_DATA8(0x06); 
  LCD_WR_DATA8(0x0A); 
  LCD_WR_DATA8(0x05); 
  LCD_WR_DATA8(0x12); 
  LCD_WR_DATA8(0x09); 
  LCD_WR_DATA8(0x2C); 
  LCD_WR_DATA8(0x92); 
  LCD_WR_DATA8(0x3F); 
  LCD_WR_DATA8(0x08); 
  LCD_WR_DATA8(0x0E); 
  LCD_WR_DATA8(0x0B); 
  LCD_WR_DATA8(0x2E); 
  LCD_WR_DATA8(0x33); 
  LCD_WR_DATA8(0x0F);
   
  LCD_WR_REG(0x11);    //Exit Sleep 
  delay_ms(120); 
  LCD_WR_REG(0x29);    //Display on 
  delay_ms(120);    
  #else 
  #endif
  //LCD_Clear(WHITE); 
  Show_Pig(BLACK,WHITE);
  //Show_Ver();
}  
//��������
//color:Ҫ���������ɫ
void LCD_Clear(uint16_t color){
  uint32_t index=0;      
  uint32_t totalpoint=lcddev.width;
  totalpoint *= lcddev.height;   //�õ��ܵ���
  LCD_SetCursor(0x00,0x0000);  //���ù��λ�� 
  LCD_WriteRAM_Prepare();      //��ʼд��GRAM       
  for(index=0;index<totalpoint;index++) {
    LCD_WR_DATA(color);
  }
}

//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void LCD_ClearBLK(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color){          
  uint16_t i,j;
  uint16_t xlen=0;
  xlen=ex-sx+1;     
  for(i=sy;i<=ey;i++) {                     
    LCD_SetCursor(sx,i);              //���ù��λ�� 
    LCD_WriteRAM_Prepare();           //��ʼд��GRAM    
    for(j=0;j<xlen;j++) LCD_WR_DATA(color);  //���ù��λ��       
  }
}  


void Show_Pig(uint16_t Color1,uint16_t Color2) { 
  uint32_t index=0;      
  uint32_t totalpoint,i;
  
  totalpoint = sizeof(Pig_RAR);
  LCD_SetCursor(0x00,0x00);  //���ù��λ�� 
  LCD_WriteRAM_Prepare();      //��ʼд��GRAM       
  //for(index=0;index<totalpoint;index++) {
  while(index < totalpoint){
    if (Pig_RAR[index] == 0xff){
      for(i=0;i<8*Pig_RAR[index+1];i++)
        LCD_WR_DATA(Color1);
      index +=2;
    } else if (Pig_RAR[index] == 0x00){
      for(i=0;i<8*Pig_RAR[index+1];i++)
        LCD_WR_DATA(Color2);
      index+=2;
    } else {
      for(i=0;i<8;i++){          
        if((Pig_RAR[index]&(0x80>>i))==0) {   // f7
          LCD_WR_DATA(Color2);
        } else {
          LCD_WR_DATA(Color1);
        }   
      }
      index++;
    }
  }
}

void Show_ICO(unsigned int x,unsigned int y,unsigned char index,uint16_t color,unsigned char Type) { 
  unsigned char i,j,k,len;  
  const unsigned char *temp;//*temp=ICO;
  
  if(Type ==64 ){
    temp = &ICO[0];
    len =8;
  } else if (Type == 48) {
    temp = &DIG_3248[0];
    len =4;
  } else if (Type == 27) {
    temp = &DIG_2427[0];
    len =3;
  } else if (Type == 26) {
    temp = &UNIT_7226[0];
    len = 9;
  } else if (Type == 24) {
    temp = &CHN_2424[0];
    len = 3;
  } else if (Type == 16) {
    temp = &BAT_4016[0];
    len = 5;
  } else if (Type == 22) {
    temp = &BLUETOO_1622[0];
    len = 2;
  } else if (Type == 13) {
    temp = &DIG_1613[0];
    len = 2;
  }  
  temp+=index*len*Type;
  for(j=0;j<Type;j++){
    LCD_SetCursor(x,y+j);
    LCD_WriteRAM_Prepare();  //��ʼд��GRAM
    for(k=0;k<len;k++) {
      for(i=0;i<8;i++){
        if((*temp&(1<<i))!=0) {
          LCD_WR_DATA(color);//������ɫ
        } else {
          LCD_WR_DATA(WHITE);//���屳����ɫ
        }   
      }
      temp++;
    }
  }
}

void Show_Logo(uint16_t x,uint16_t y){
  uint32_t i,j,k;
  uint16_t da;
  k=0;
  for(i=0;i<IMAGE_WIDTH2 ;i++){  
    LCD_SetCursor(x,y+i);
    LCD_WriteRAM_Prepare();           //��ʼд��GRAM  
    for(j=0;j<IMAGE_HEIGHT2;j++) {         
      da=HV_LOGO[k*2+1];
      da<<=8;
      da|=HV_LOGO[k*2]; 
      LCD_WR_DATA(da);
      k++;
    }
  }
}
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//���ù��λ�� 
	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
	LCD_WR_DATA(POINT_COLOR); 
} 
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
  u8 temp,t1,t;
	u16 y0=y;
	u16 colortemp=POINT_COLOR;      			     
	//���ô���		   
	num=num-' ';//�õ�ƫ�ƺ��ֵ
	if(!mode) //�ǵ��ӷ�ʽ
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //����1206����
			else temp=asc2_1608[num][t];		 //����1608���� 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				LCD_DrawPoint(x,y);	
				temp<<=1;
				y++;
				if(y>=lcddev.height){POINT_COLOR=colortemp;return;}//��������
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=lcddev.width){POINT_COLOR=colortemp;return;}//��������
					break;
				}
			}  	 
	    }    
	}else//���ӷ�ʽ
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //����1206����
			else temp=asc2_1608[num][t];		 //����1608���� 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)LCD_DrawPoint(x,y); 
				temp<<=1;
				y++;
				if(y>=lcddev.height){POINT_COLOR=colortemp;return;}//��������
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=lcddev.width){POINT_COLOR=colortemp;return;}//��������
					break;
				}
			}  	 
	    }     
	}
	POINT_COLOR=colortemp;	    	   	 	  
}


void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        LCD_ShowChar(x,y,*p,size,1);
        x+=size/2;
        p++;
    }  
}



