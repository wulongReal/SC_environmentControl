#include "vars.h"
#include "TFTLED_REF.h"
#pragma diag_suppress 174

#define U3_PWM  PB7
#define U3_STR  PB4
#define U3_CLK  PA7
#define U3_DAT  PB0

void Delay_ns(void){
  uint8_t i;
  
  for (i=0;i<14;i++)
  ;
}
void LED_D12(uint16_t D24_LED){
  uint8_t i;
  uint16_t led;
  
  //U3_PWM = 0;
  //LED_EO_L();
  //U3_STR = 1;
  //LED_STR_H();
  Delay_ns();
  led = D24_LED;
  for(i=0;i<12;i++){
    //U3_CLK = 0;
    LED_CP_L();
    Delay_ns();
    // 0000 1111 1111 1111
    if (led & 0x01)
      //U3_DAT = 1;
      LED_D_H();
    else
      //U3_DAT = 0;
      LED_D_L();
    //U3_CLK = 1;
    LED_CP_H();
    Delay_ns(); 
    led >>= 1;
  }
  //U3_PWM = 1;
  //LED_EO_H();
  //U3_STR = 0;
  //LED_STR_L();
}

void TFT_DISP_Num(int32_t Num, uint16_t Color){
  uint16_t Disp;
  uint8_t Disp_bit;
  

  if (Num <0){
    Disp = -Num;
    // disp -   132 120
    Show_ICO(132,120,12,Color,27);      // -
  } else {
    Disp = Num;
    Show_ICO(132,120,11,Color,27);       // �հ�
    // disp clear -
  }
  IWDG_Feed();
  if (Disp>=10000){   // >100�� ������ʾС����  120
    Disp_bit = Disp / 10000; 
    Show_ICO(150,110,Disp_bit,Color,48);      // ����1
    Disp -= Disp_bit*10000;
    Disp_bit = Disp / 1000;
    Show_ICO(182,110,Disp_bit,Color,48);      // ����2
    Show_ICO(214,110,11,Color,27);            // 214 110 �ո�
    Show_ICO(214,129,11,Color,27);            // ���� �ո�
    Disp -= Disp_bit*1000;
    Disp_bit = Disp / 100;
    Show_ICO(230,129,Disp_bit,Color,27);      // С������    
  } else if (Disp>=1000){ // >99.9   36.5
    Disp_bit = Disp / 1000; 
    Show_ICO(150,110,Disp_bit,Color,48);      // ����1
    Disp -= Disp_bit*1000;
    Disp_bit = Disp / 100;
    Show_ICO(182,110,Disp_bit,Color,48);      // ����2
    Show_ICO(214,110,11,Color,27);            // 214 110 �ո�
    Show_ICO(214,129,10,Color,27);            // ���� С����
    Disp -= Disp_bit*100;
    Disp_bit = (Disp / 10) ;
    Show_ICO(230,129,Disp_bit,Color,27);      // С������
  } else if (Disp>=100){ // >9.9   _4.5
    Show_ICO(132,120,11,Color,27);       // �հ�
    if (Num <0){
      Show_ICO(150,110,10,Color,48);      // �հ�
      Show_ICO(155,120,12,Color,27);      // -
    } else {
      Show_ICO(150,110,10,Color,48);      // �հ�
    }                
    Disp_bit = Disp / 100;
    Show_ICO(182,110,Disp_bit,Color,48);      // ����2
    Show_ICO(214,110,11,Color,27);            // 214 110 �ո�
    Show_ICO(214,129,10,Color,27);            // ���� С����
    Disp -= Disp_bit*100;
    Disp_bit = Disp /10;
    Show_ICO(230,129,Disp_bit,Color,27);      // С������
  } else {//if (Disp>=10){ // >9.9   0.45
    Show_ICO(150,110,0,Color,48);             // 0
    Show_ICO(182,110,10,Color,48);            // �հ�
    Show_ICO(182,129,10,Color,27);            // ���� С����
    Disp_bit = Disp / 10;
    Show_ICO(196,110,Disp_bit,Color,48);      // ����2
    Disp -= Disp_bit*10;
    Show_ICO(230,129,Disp,Color,27);      // С������   160913  Disp_bit->Disp bug
  }
  //3.���ݸ�ʽ  09 B1 �¶����ݣ���ʵ���¶�����100����ֵ 2481 ʵ���¶�24.81  2481/256=9   2481%256= b1
}

#define ICO_X       64
#define ICO_Y       90
#define COD_X       110   //112->110
#define CODW_Y      114
#define COD_Y       124
#define LOG_X       116
#define LOG_Y       100
#define UNT_X       168
#define UNT_Y       158
void TFT_Clear(uint8_t Type){
  if (Type == 1){
    LCD_ClearBLK(LOG_X,LOG_Y,203,187,WHITE);//
  } else if (Type ==2){
    LCD_ClearBLK(ICO_X,ICO_Y,127,153,WHITE);     // ����
    LCD_ClearBLK(150,110,239,182,WHITE);         // ������1
    LCD_ClearBLK(240,129,253,155,WHITE);         // ������1
  }
}
#define   TIMEDLED        2
#define   TIMECLED        4
#define   TIMETFT         3
#define   FirstLine_X     5
#define   FirstLine_Y     5
#define   SecLine_X       37
#define   SecLine_Y       30   //3.3V
#define   ThiLine_Y       55   // 5V
#define   FouLine_Y       80   // 5G
#define   FifLine_Y       105  // 9v
#define   SixLine_Y       130  // 12V
#define   SevLine_Y       155  // ���1
#define   EigLine_Y       180  // ���2
#define   NinLine_Y       205  // 24V
void TFT_TST_Num(uint16_t x,uint16_t y,uint8_t Num, uint16_t Color){
  uint8_t Disp;
  uint8_t Disp_bit;
  
  Disp = Num;
  if (Disp>=100){   // >10.0
    Disp_bit = Disp / 100; 
    Show_ICO(x,y,Disp_bit,Color,24);      // ����1
    Disp -= Disp_bit*100;
    Disp_bit = Disp / 10;
    Show_ICO(x+16,y,Disp_bit,Color,24);  // ����2
    Show_ICO(x+32,y,10,Color,24);        // .
    Disp -= Disp_bit*10;
    Show_ICO(x+48,y,Disp,Color,24);  // ����3
    Show_ICO(x+64,y,11,Color,24);  // V    
  } else if(Disp>=10){   // >1.0
    Disp_bit = Disp / 10; 
    Show_ICO(x,y,Disp_bit,Color,24);      // ����1
    Show_ICO(x+16,y,10,Color,24);        // .
    Disp -= Disp_bit*10;
    Show_ICO(x+32,y,Disp,Color,24);  // ����2
    Show_ICO(x+48,y,13,Color,24);  // �ո�
    Show_ICO(x+64,y,11,Color,24);  // V
  } else {//if (Disp>=0){   // >0.0
    Show_ICO(x,y,0,Color,24);      // ����1
    Show_ICO(x+16,y,10,Color,24);        // .
    Show_ICO(x+32,y,Disp,Color,24);  // ����2
    Show_ICO(x+48,y,13,Color,24);  // �ո�
    Show_ICO(x+64,y,11,Color,24);  // V
  }  
}

void DISP_NUM(void){
  uint16_t Color;
  // 3.3V��ѹ����ֵ
  if (Val_3P3V<5){  //<0.5v
    Color = GREEN;
  } else {
    Color = BLUE;
  }
  Show_ICO(SecLine_X     ,SecLine_Y, 3,Color,24); // 3
  Show_ICO(SecLine_X + 16,SecLine_Y,10,Color,24); // .
  Show_ICO(SecLine_X + 32,SecLine_Y, 3,Color,24); // 3
  Show_ICO(SecLine_X + 48,SecLine_Y,11,Color,24); // V
  Show_ICO(SecLine_X + 64,SecLine_Y,14,Color,24); // ��
  Show_ICO(SecLine_X + 88,SecLine_Y,15,Color,24); // ѹ
  Show_ICO(SecLine_X +112,SecLine_Y,16,Color,24); // ��
  Show_ICO(SecLine_X +136,SecLine_Y,25,Color,24); // ��
  Show_ICO(SecLine_X +160,SecLine_Y,26,Color,24); // ֵ
  TFT_TST_Num(SecLine_X +190,SecLine_Y,Val_3P3V,Color);  //  3 .3V  
  // 5V��ѹ����ֵ
  if (Val_5V<8){ //0.8v
    Color = GREEN;
  } else {
    Color = BLUE;
  }
  Show_ICO(SecLine_X     ,ThiLine_Y, 5,Color,24); // 5
  Show_ICO(SecLine_X + 16,ThiLine_Y,11,Color,24); // V
  Show_ICO(SecLine_X + 32,ThiLine_Y,14,Color,24); // ��
  Show_ICO(SecLine_X + 56,ThiLine_Y,15,Color,24); // ѹ
  Show_ICO(SecLine_X + 80,ThiLine_Y,16,Color,24); // ��
  Show_ICO(SecLine_X +104,ThiLine_Y,25,Color,24); // ��
  Show_ICO(SecLine_X +128,ThiLine_Y,26,Color,24); // ֵ
  TFT_TST_Num(SecLine_X +190,ThiLine_Y,Val_5V,Color);  //  5V
  // 5VG��ѹ����ֵ
  if(Val_5VGPRS<20){
    Color = GREEN;
  } else {
    Color = BLUE;
  }
  Show_ICO(SecLine_X     ,FouLine_Y, 5,Color,24); // 5
  Show_ICO(SecLine_X + 16,FouLine_Y,12,Color,24); // G
  Show_ICO(SecLine_X + 32,FouLine_Y,11,Color,24); // V
  Show_ICO(SecLine_X + 48,FouLine_Y,14,Color,24); // ��
  Show_ICO(SecLine_X + 72,FouLine_Y,15,Color,24); // ѹ
  Show_ICO(SecLine_X + 96,FouLine_Y,16,Color,24); // ��
  Show_ICO(SecLine_X +120,FouLine_Y,25,Color,24); // ��
  Show_ICO(SecLine_X +144,FouLine_Y,26,Color,24); // ֵ
  TFT_TST_Num(SecLine_X +190,FouLine_Y,Val_5VGPRS,Color);  // 5GV
  // 9V��ѹ����ֵ
  if(Val_9V<10){
    Color = GREEN;
  } else {
    Color = BLUE;
  }
  Show_ICO(SecLine_X     ,FifLine_Y, 9,Color,24); // 9
  Show_ICO(SecLine_X + 16,FifLine_Y,11,Color,24); // V
  Show_ICO(SecLine_X + 32,FifLine_Y,14,Color,24); // ��
  Show_ICO(SecLine_X + 56,FifLine_Y,15,Color,24); // ѹ
  Show_ICO(SecLine_X + 80,FifLine_Y,16,Color,24); // ��
  Show_ICO(SecLine_X +104,FifLine_Y,25,Color,24); // ��
  Show_ICO(SecLine_X +128,FifLine_Y,26,Color,24); // ֵ
  TFT_TST_Num(SecLine_X +190,FifLine_Y,Val_9V,Color);  //  9V
  
  if(Val_12V<10){
    Color = GREEN;
  } else {
    Color = BLUE;
  }
  // 12V��ѹ����ֵ
  Show_ICO(SecLine_X     ,SixLine_Y, 1,Color,24); // 1
  Show_ICO(SecLine_X + 16,SixLine_Y, 2,Color,24); // 2
  Show_ICO(SecLine_X + 32,SixLine_Y,11,Color,24); // V
  Show_ICO(SecLine_X + 48,SixLine_Y,14,Color,24); // ��
  Show_ICO(SecLine_X + 72,SixLine_Y,15,Color,24); // ѹ
  Show_ICO(SecLine_X + 96,SixLine_Y,16,Color,24); // ��
  Show_ICO(SecLine_X +120,SixLine_Y,25,Color,24); // ��
  Show_ICO(SecLine_X +144,SixLine_Y,26,Color,24); // ֵ
  TFT_TST_Num(SecLine_X +190,SixLine_Y,Val_12V,Color);  // 12V
  // ���1����ֵ
  if(Val_BAT<10){
    Color = GREEN;
  } else {
    Color = BLUE;
  }
  Show_ICO(SecLine_X     ,SevLine_Y,14,Color,24); // ��
  Show_ICO(SecLine_X + 24,SevLine_Y,27,Color,24); // ��
  Show_ICO(SecLine_X + 48,SevLine_Y, 1,Color,24); // 1
  Show_ICO(SecLine_X + 64,SevLine_Y,14,Color,24); // ��
  Show_ICO(SecLine_X + 88,SevLine_Y,15,Color,24); // ѹ
  Show_ICO(SecLine_X +112,SevLine_Y,16,Color,24); // ��
  Show_ICO(SecLine_X +136,SevLine_Y,25,Color,24); // ��
  Show_ICO(SecLine_X +160,SevLine_Y,26,Color,24); // ֵ
  TFT_TST_Num(SecLine_X +190,SevLine_Y,Val_BAT,Color);  // BAT
  // ���2����ֵ
  if(Val_EXTBAT<10){
    Color = GREEN;
  } else {
    Color = BLUE;
  }
  Show_ICO(SecLine_X     ,EigLine_Y,14,Color,24); // ��
  Show_ICO(SecLine_X + 24,EigLine_Y,27,Color,24); // ��
  Show_ICO(SecLine_X + 48,EigLine_Y, 2,Color,24); // 2
  Show_ICO(SecLine_X + 64,EigLine_Y,14,Color,24); // ��
  Show_ICO(SecLine_X + 88,EigLine_Y,15,Color,24); // ѹ
  Show_ICO(SecLine_X +112,EigLine_Y,16,Color,24); // ��
  Show_ICO(SecLine_X +136,EigLine_Y,25,Color,24); // ��
  Show_ICO(SecLine_X +160,EigLine_Y,26,Color,24); // ֵ
  TFT_TST_Num(SecLine_X +190,EigLine_Y,Val_EXTBAT,Color);  // BATEXT
  // 24V����ֵ
  if(Val_24V<10){
    Color = GREEN;
  } else {
    Color = BLUE;
  }
  Show_ICO(SecLine_X     ,NinLine_Y, 2,Color,24); // 1
  Show_ICO(SecLine_X + 16,NinLine_Y, 4,Color,24); // 2
  Show_ICO(SecLine_X + 32,NinLine_Y,11,Color,24); // V
  Show_ICO(SecLine_X + 48,NinLine_Y,14,Color,24); // ��
  Show_ICO(SecLine_X + 72,NinLine_Y,15,Color,24); // ѹ
  Show_ICO(SecLine_X + 96,NinLine_Y,16,Color,24); // ��
  Show_ICO(SecLine_X +120,NinLine_Y,25,Color,24); // ��
  Show_ICO(SecLine_X +144,NinLine_Y,26,Color,24); // ֵ
  TFT_TST_Num(SecLine_X +190,NinLine_Y,Val_24V,Color);  // 24V
}

void TFT_Refresh(void){
  if (FLAGS0_DISPIG){//��ʾ����
    Show_Pig(BLACK,WHITE);
    Show_Ver();//��ʾ�汾
    FLAGS0_DISPIG = FALSE;
  }
  
  if (FLAGS0_ENTERTST){  // test mode
    IWDG_Feed();
    // 1.��ѹ����
    //  ȫ���򿪲��Ե�ѹ        5
    //  �ر�GPRS������Դ        5
    //  �ر�POWER_ON������Դ    5
    //  �ر��ⲿ���            5
    // 2.�¶ȴ�������
    //  ��չ��485�����¶�
    //  ��չ��232�����¶ȴ���������
    // 3.���������(ʵ�ʼ̵��������)
    //  ��չ����������ź� 
    //  ��չ����Լ̵�������
    //  ��������������źŲ���
    // 4.˫ɫLED����
    // 5.��ɫLED����
    // 6.���
    if (Test_TFT<=(3*TIMETFT)){
      LCD_Clear(WHITE);
      TFT_BL_ON();
    } else if (Test_TFT<(4*TIMETFT)){
      TFT_BL_OFF();
    } else if (Test_TFT<(5*TIMETFT)){
      LCD_Clear(RED);
      TFT_BL_ON();
    } else if (Test_TFT<(6*TIMETFT)){
      LCD_Clear(GREEN);
    } else if (Test_TFT<(7*TIMETFT)){
      LCD_Clear(BLUE);
    } else if (Test_TFT<(8*TIMETFT)){
      LCD_Clear(BLACK);
    }  else if (Test_TFT<(9*TIMETFT)){
      LCD_Clear(WHITE);
    } else if (Test_TFT<(14*TIMETFT)){
      POW_ON();
     // POWGPRS_ON();
      //POWBAT_ON();        1129 test 
      Show_ICO(FirstLine_X     ,FirstLine_Y, 1,BLUE,24); // 1
      Show_ICO(FirstLine_X + 16,FirstLine_Y,10,BLUE,24); // .
      Show_ICO(FirstLine_X + 32,FirstLine_Y,14,BLUE,24); // ��
      Show_ICO(FirstLine_X + 56,FirstLine_Y,15,BLUE,24); // ѹ
      Show_ICO(FirstLine_X + 80,FirstLine_Y,16,BLUE,24); // ��
      Show_ICO(FirstLine_X +104,FirstLine_Y,17,BLUE,24); // ��
      Show_ICO(FirstLine_X +128,FirstLine_Y,18,BLUE,24); // -
      Show_ICO(FirstLine_X +152,FirstLine_Y,19,BLUE,24); // ȫ
      Show_ICO(FirstLine_X +176,FirstLine_Y,20,BLUE,24); // ��
      Show_ICO(FirstLine_X +200,FirstLine_Y,21,BLUE,24); // ��
      Show_ICO(FirstLine_X +224,FirstLine_Y,22,BLUE,24); // ʱ
      DISP_NUM();
    } else if (Test_TFT<(19*TIMETFT)){
      POWGPRS_OFF();
      Show_ICO(FirstLine_X     ,FirstLine_Y, 2,BLUE,24); // 2
      Show_ICO(FirstLine_X + 16,FirstLine_Y,10,BLUE,24); // .
      Show_ICO(FirstLine_X + 32,FirstLine_Y,14,BLUE,24); // ��
      Show_ICO(FirstLine_X + 56,FirstLine_Y,15,BLUE,24); // ѹ
      Show_ICO(FirstLine_X + 80,FirstLine_Y,16,BLUE,24); // ��
      Show_ICO(FirstLine_X +104,FirstLine_Y,17,BLUE,24); // ��
      Show_ICO(FirstLine_X +128,FirstLine_Y,18,BLUE,24); // -
      Show_ICO(FirstLine_X +152,FirstLine_Y,28,BLUE,24); // ��
      Show_ICO(FirstLine_X +176,FirstLine_Y,29,BLUE,24); // ��
      Show_ICO(FirstLine_X +200,FirstLine_Y, 1,BLUE,24); // 1
      Show_ICO(FirstLine_X +216,FirstLine_Y,22,BLUE,24); // ʱ
      Show_ICO(FirstLine_X +240,FirstLine_Y,13,BLUE,24); // �ո�
      DISP_NUM();
    } else if (Test_TFT<(24*TIMETFT)){
     // POWGPRS_ON();
     // POW_OFF();
      Show_ICO(FirstLine_X     ,FirstLine_Y, 3,BLUE,24); // 3
      Show_ICO(FirstLine_X + 16,FirstLine_Y,10,BLUE,24); // .
      Show_ICO(FirstLine_X + 32,FirstLine_Y,14,BLUE,24); // ��
      Show_ICO(FirstLine_X + 56,FirstLine_Y,15,BLUE,24); // ѹ
      Show_ICO(FirstLine_X + 80,FirstLine_Y,16,BLUE,24); // ��
      Show_ICO(FirstLine_X +104,FirstLine_Y,17,BLUE,24); // ��
      Show_ICO(FirstLine_X +128,FirstLine_Y,18,BLUE,24); // -
      Show_ICO(FirstLine_X +152,FirstLine_Y,28,BLUE,24); // ��
      Show_ICO(FirstLine_X +176,FirstLine_Y,29,BLUE,24); // ��
      Show_ICO(FirstLine_X +200,FirstLine_Y, 2,BLUE,24); // 2
      Show_ICO(FirstLine_X +216,FirstLine_Y,22,BLUE,24); // ʱ
      Show_ICO(FirstLine_X +240,FirstLine_Y,13,BLUE,24); // �ո�
      DISP_NUM();      
    } else if (Test_TFT<(29*TIMETFT)){
      POW_ON();
     // POWBAT_OFF();
      Show_ICO(FirstLine_X     ,FirstLine_Y, 4,BLUE,24); // 4
      Show_ICO(FirstLine_X + 16,FirstLine_Y,10,BLUE,24); // .
      Show_ICO(FirstLine_X + 32,FirstLine_Y,14,BLUE,24); // ��
      Show_ICO(FirstLine_X + 56,FirstLine_Y,15,BLUE,24); // ѹ
      Show_ICO(FirstLine_X + 80,FirstLine_Y,16,BLUE,24); // ��
      Show_ICO(FirstLine_X +104,FirstLine_Y,17,BLUE,24); // ��
      Show_ICO(FirstLine_X +128,FirstLine_Y,18,BLUE,24); // -
      Show_ICO(FirstLine_X +152,FirstLine_Y,28,BLUE,24); // ��
      Show_ICO(FirstLine_X +176,FirstLine_Y,29,BLUE,24); // ��
      Show_ICO(FirstLine_X +200,FirstLine_Y, 3,BLUE,24); // 3
      Show_ICO(FirstLine_X +216,FirstLine_Y,22,BLUE,24); // ʱ
      Show_ICO(FirstLine_X +240,FirstLine_Y,13,BLUE,24); // �ո�
      DISP_NUM();
    } else if (Test_TFT<(31*TIMETFT)){
    //  POWBAT_ON();  1129  test
      LCD_Clear(WHITE);
      Show_ICO(FirstLine_X     ,FirstLine_Y, 5,BLUE,24); // 5
      Show_ICO(FirstLine_X + 16,FirstLine_Y,10,BLUE,24); // .
      Show_ICO(FirstLine_X + 40,FirstLine_Y,16,BLUE,24); // ��
      Show_ICO(FirstLine_X + 64,FirstLine_Y,17,BLUE,24); // ��
      Show_ICO(FirstLine_X + 88,FirstLine_Y,30,BLUE,24); // ��
      Show_ICO(FirstLine_X +112,FirstLine_Y,31,BLUE,24); // ��
      Show_ICO(FirstLine_X +136,FirstLine_Y,32,BLUE,24); // ��
    } else {
      TstOV_Count =0;
      FLAGS0_DISPIG = TRUE;//��ʾ����
      FLAGS0_TFTTSTF = TRUE;
      Test_TFT =0;
    }
    Test_TFT++;
  } else {   // no test
				//�����ʾ0414�޸�			
				Show_ICO(200,35,0,BLUE,16);      // ��ط���
		    if(Val_BAT>28){//���С��9.7V�ϵ�
	       if (Val_24V > 180){//�ж������ⲿ��Դ
         LCD_ClearBLK(202,39,232,47,GREEN);     // ���ⲿ��Դʱ  232 ��ɫ
        //LCD_ClearBLK((Len+203),39,232,47,BLUE);     // ZNHK-174
          }else{ //���ⲿ��Դ
         if(Val_BAT>=120){
				  LCD_ClearBLK(203,39,232,47,YELLOW); //����
			    }
			   else if(Val_BAT>115&&Val_BAT<120){ //�ĸ�
				  LCD_ClearBLK(203,39,(232-6),47,YELLOW);
          //LCD_ClearBLK(226,39,232,47,WHITE);					 
			    }
			   else if(Val_BAT>110&&Val_BAT<115){ //����
				  LCD_ClearBLK(203,39,(232-12),47,YELLOW);
          //LCD_ClearBLK(220,39,232,47,WHITE);					 
			    }
			   else if(Val_BAT>105&&Val_BAT<110){ //����
				  LCD_ClearBLK(203,39,(232-18),47,RED);
          //LCD_ClearBLK(214,39,232,47,WHITE);					 
			    }
			   else if(Val_BAT>100&&Val_BAT<105){ //һ��
				  LCD_ClearBLK(203,39,(232-24),47,RED); 
					//LCD_ClearBLK(208,39,232,47,WHITE);
			    }
			   else if(Val_BAT>95&&Val_BAT<100){ //��
				  LCD_ClearBLK(203,39,205,47,RED);
          //LCD_ClearBLK(205,39,232,47,WHITE);					 
			    }
			}				
	}
/***********************************************************************************************
    //uint8_t Len;
    //Show_ICO(200,35,0,BLACK,16);      // ��ط���
    #ifdef SORT  //����վ
    if (FLAGS0_BTBLINK){
      Show_ICO(213,12,0,BLUE, 22);      // ��������
    } else {
      LCD_ClearBLK(213,12,228,33,WHITE);
    }
    #endif
    Len = Val_BAT/4;
    if (Len>30)//Val_BAT>12V
      Len = 30;
    if (Len>7){//Val_BAT>2.8V
      if (Val_24V > 180){//�ж������ⲿ��Դ
        LCD_ClearBLK(202,39,(Len+202),47,GREEN);     // ���ⲿ��Դʱ  232 ��ɫ
        LCD_ClearBLK((Len+203),39,232,47,BLUE);     // ZNHK-174
      } else {
        //LCD_ClearBLK(203,39,(Len+202),47,YELLOW);    // ���ⲿ��Դ  ��ɫ
        //LCD_ClearBLK((Len+203),39,232,47,BLUE);     // ZNHK-174
				LCD_ClearBLK(202,39,232,47,YELLOW);    // ���ⲿ��Դ  ��ɫ
        LCD_ClearBLK((Len+203),39,232,47,BLUE);     // ZNHK-174
      }
    } else {
      LCD_ClearBLK(203,39,(Len+202),47,RED);
      LCD_ClearBLK((Len+203),39,232,47,WHITE);       // ZNHK-174
    }
*///**********************************************************************************************
    #if defined(DEBUG_TEST)//����ģʽ
      if (FLAGS0_DISPLOGO){
        FLAGS0_DISPLOGO =FALSE;
        TFT_Clear(1);//logo�������
      }
      Show_ICO(ICO_X,ICO_Y,0,Temp_COLOR,64);       // ����
      Show_ICO(COD_X,COD_Y,(Temp_NumDG+1),Temp_COLOR,27);// 39 34       
      Show_ICO(UNT_X,UNT_Y,0,Temp_COLOR,26);       // ��λ
      TFT_DISP_Num(Temp_RS485[Temp_NumDG],Temp_COLOR);
      Temp_NumDG++;
      if (Temp_NumDG>=RS232Temp_Num){      
        Temp_NumDG =0;    
      }
    #else //�ǲ���ģʽ
    if ((TFT_Inf_FRCR[0]>=0x11) && (TFT_Inf_FRCR[0]<=0x18))
	{
      	FLAGS0_DISPSEN = TRUE;// ��ʾ�¶ȴ�����
      	if (FLAGS0_DISPLOGO)
		{// ��ʾlogo��������
        	FLAGS0_DISPLOGO =FALSE;
        	TFT_Clear(1);//logo�������
      	}
    }    
    if (TFT_Inf_FRCR[0] == 0)
	{    // start ��������
      	if ((FLAGS0_DISPSEN) ||(FLAGS0_DISPWARN))
		{
        	FLAGS0_DISPWARN =FALSE;
        	FLAGS0_DISPSEN  =FALSE;
        	TFT_Clear(2);
      	}
      	Show_Logo(LOG_X,LOG_Y);   //��ʾLOGO 
      	FLAGS0_DISPLOGO =TRUE;    
    } 
	else if (TFT_Inf_FRCR[0] <= 9)
	{   // warn ����0~9
      	IWDG_Feed();
      	FLAGS0_DISPWARN =TRUE;
      	if (FLAGS0_DISPLOGO)
	  	{
        	FLAGS0_DISPLOGO =0;
        	TFT_Clear(1);
      	}
      	if (FLAGS0_DISPSEN)
	  	{//��ʾ�¶ȴ�����
        	FLAGS0_DISPSEN =0;
        	TFT_Clear(2);
    	}
      	Show_ICO(ICO_X,ICO_Y,7,WARN_COLOR,64);       // ���Ϸ���    
      	Show_ICO(COD_X,CODW_Y,TFT_Inf_FRCR[0],WARN_COLOR,27);// // ������ 
    } 
	else if (TFT_Inf_FRCR[0] == 0x11)
	{   // temp        0x11 ��ʾ�¶ȴ�����ֵ 
      	Show_ICO(ICO_X,ICO_Y,0,Temp_COLOR,64);       // ����
      	if (Num_Sensor<=TEMPER_SENSOR_NUM)
		{// �¶ȴ��������
        	Show_ICO(COD_X,COD_Y,(Num_Sensor+1),Temp_COLOR,27);// 39 34
      	}
      	Show_ICO(UNT_X,UNT_Y,0,Temp_COLOR,26);      // ��λ
      	//TFT_DISP_Num(Temp_RS485[Num_Sensor],Temp_COLOR);  Dev_info[TEMP1-1+i]
      	TFT_DISP_Num(Dev_info[TEMP1-1+ Num_Sensor].Data1,Temp_COLOR);  //160928
    } 
	else if (TFT_Inf_FRCR[0] == 0x12)
	{   // 0x12 ʪ�ȴ�����ֵ
      	Show_ICO(ICO_X,ICO_Y,2,HMD_COLOR,64);      //����
      	Show_ICO(UNT_X,UNT_Y,4,HMD_COLOR,26);      // ��λ
		if (Num_Sensor<=3)
		{// ʪ�ȴ��������
        	Show_ICO(COD_X,COD_Y,(Num_Sensor+1),HMD_COLOR,27);// 39 34
      	}
      	//TFT_DISP_Num(Hmd_RS485[Num_Sensor],HMD_COLOR);  //160928
      	TFT_DISP_Num(Dev_info[HUMI1-1+Num_Sensor].Data1,HMD_COLOR);
    } else if (TFT_Inf_FRCR[0] == 0x13){   // 0x14 phֵ
      Show_ICO(ICO_X,ICO_Y,3,PHV_COLOR,64);
      Show_ICO(UNT_X,UNT_Y,5,PHV_COLOR,26);      // ��λ
      //TFT_DISP_Num(PHV_RS485[Num_Sensor],PHV_COLOR);  //160928
      TFT_DISP_Num(Dev_info[PH-1].Data1,PHV_COLOR);       //160928
    } else if (TFT_Inf_FRCR[0] == 0x14){   // 0X15 ˮ����ֵ
      Show_ICO(ICO_X,ICO_Y,6,WF_COLOR,64);
      Show_ICO(UNT_X,UNT_Y,2,WF_COLOR,26);      // ��λ
      //TFT_DISP_Num(WF_RS485[Num_Sensor],WF_COLOR);  //160928
      TFT_DISP_Num(Dev_info[WATER_FLOW-1].Data1,WF_COLOR);  //160928
    } else if (TFT_Inf_FRCR[0] == 0x15){   // 0X16 ˮѹ��ֵ
      Show_ICO(ICO_X,ICO_Y,4,WP_COLOR,64);
      Show_ICO(UNT_X,UNT_Y,3,WP_COLOR,26);      // ��λ
      //TFT_DISP_Num(WP_RS485[Num_Sensor],WP_COLOR);
      Show_ICO(100,127,(Num_Sensor+1),WP_COLOR,27);// 39 34
      if (Num_Sensor==0)
		   //TFT_DISP_Num((Dev_info[WATER_PRESU1-1].Data1*30/100),WP_COLOR);	//Data*6/2000  160928	161025 170912
		   TFT_DISP_Num((Dev_info[WATER_PRESU1-1].Data1/10),WP_COLOR);	//Data*6/2000  160928	161025 170912
      else if (Num_Sensor==1)
        //TFT_DISP_Num((Dev_info[WATER_PRESU2-1].Data1*30/100),WP_COLOR);  //Data*6/2000  160928   161025 170912  
        TFT_DISP_Num((Dev_info[WATER_PRESU2-1].Data1/10),WP_COLOR);  //Data*6/2000  160928   161025 170912  
    } else if (TFT_Inf_FRCR[0] == 0x16){   // 0x17 �����¶�
      Show_ICO(ICO_X,ICO_Y,1,Temp_COLOR,64);       // ����
      if (Num_Sensor<=3){// �¶ȴ��������
        Show_ICO(COD_X,COD_Y,(Num_Sensor+1),Temp_COLOR,27);// 39 34  num
      }     
      Show_ICO(UNT_X,UNT_Y,0,Temp_COLOR,26);      // ��λ
      //TFT_DISP_Num(Temp_RS485[Num_Sensor],Temp_COLOR);  // 160923
      //////TFT_DISP_Num(Dev_info[TEMP3-1+Num_Sensor].Data1,Temp_COLOR);  // 160923
    } else if (TFT_Inf_FRCR[0] == 0x17){   // 0x18 ����Ũ��ֵ
      Show_ICO(ICO_X,ICO_Y,5,NH3_COLOR,64);//  ����  0315yao
      Show_ICO(UNT_X,UNT_Y,1,NH3_COLOR,26);      // ��λ
      //TFT_DISP_Num(NH3_RS485[Num_Sensor],NH3_COLOR);//160928
			if (Num_Sensor<=2){  // �������������
        Show_ICO(COD_X,COD_Y,(Num_Sensor+1),NH3_COLOR,27);// 39 34
      }
      TFT_DISP_Num(Dev_info[NH3_1-1+Num_Sensor].Data1*100,NH3_COLOR);    //0-1000     160928
    } 
/****************************************************************************************************	
		//             ����վ
		  else if (TFT_Inf_FRCR[0] == 0x21){   //     0x21 ������Ϣ
      Show_ICO(ICO_X,ICO_Y,13,KG_COLOR,64);
      Show_ICO(91,124,Num_Sensor,KG_COLOR,13);//// num
      Show_ICO(UNT_X,UNT_Y, 6,KG_COLOR,26);      // ��λ
      TFT_DISP_Num(Weight_Sort[Num_Sensor],KG_COLOR);
    }  else if (TFT_Inf_FRCR[0] == 0x22){   //     0x22 ������Ϣ
      Show_ICO(ICO_X,ICO_Y,12,Temp1_COLOR,64);
      Show_ICO(UNT_X,UNT_Y, 0,Temp1_COLOR,26);      // ��λ
      TFT_DISP_Num(Temp1_Sort,Temp1_COLOR);
    } else if (TFT_Inf_FRCR[0] == 0x23){   //     0x23  ������Ϣ
      Show_ICO(ICO_X,ICO_Y,9,RFID_COLOR,64);
      Show_ICO(UNT_X,UNT_Y,5,RFID_COLOR,26);      // ��λ
      TFT_DISP_Num(RFID,RFID_COLOR);
    } else if (TFT_Inf_FRCR[0] == 0x24){   //     0x24 ����ģʽ״̬
      Show_ICO(ICO_X,ICO_Y,10,RUNMD_COLOR,64);
      Show_ICO(83,125,RunMode,RUNMD_COLOR,13); //num
      Show_ICO(UNT_X,UNT_Y, 5,RUNMD_COLOR,26);      // ��λ 
    } else if (TFT_Inf_FRCR[0] == 0x25){   // 0x25  �豸����״̬
      Show_ICO(ICO_X,ICO_Y,11,DEVMOD_COLOR,64);
      Show_ICO(90,117,DevMode,DEVMOD_COLOR,13);// num
      Show_ICO(UNT_X,UNT_Y, 5,DEVMOD_COLOR,26);      // ��λ
    }
*///***************************************************************************************************
    #endif
  }
  //LED1_ON();
}

extern uint8_t CoreP_Ver[];

void Show_Ver(void){  // CoreP_Ver[6] [7] [8]
  // 240 215 12
	uint8_t Ver[8]={0};
	Ver[0]='V';
	//Ver[1]=' ';
	Ver[3]='.';
	Ver[5]='.';
	POINT_COLOR=BLACK;
	if(CoreP_Ver[6]<=9){
		Ver[1]=' ';
		Ver[2]=CoreP_Ver[6]+'0';
	}else{
		Ver[1]=(CoreP_Ver[6]/10)+'0';
		Ver[2]=(CoreP_Ver[6]%10)+'0';
	}
	if(CoreP_Ver[8]<=9){
		Ver[4]=CoreP_Ver[8]+'0';
		Ver[6]=0+'0';
		//Ver[5]=0+'0';
		//Ver[6]=CoreP_Ver[8]+'0';
	}else{		
		Ver[4]=(CoreP_Ver[8]/10)+'0';
		Ver[6]=(CoreP_Ver[8]%10)+'0';
	}
	//LCD_ClearBLK(260,220,320,240,WHITE);
	LCD_ClearBLK(258,220,320,240,WHITE);
	LCD_ShowString(262,222,320,20,16,Ver);	//�汾����
}

void Double_Ref(void){
  
//           ����
  // 0x010 D36_RED BLUETOOTH ERR
  // 0x020 D36_GRN BT    OK
//           ���
  // 0x004 D35_RED MOT   ERR
  // 0x008 D35_GRN MOT   OK
//           485ͨѶ
  // 0x001 D31_RED RS485 ERR
  // 0x002 D31_RED RS485 OK
//	         232ͨѶ
  // 0x400 D29_RED RS232 ERR
  // 0x800 D29_GRN RS232 OK
//	         canͨѶ
  // 0x100 D27_RED CAN   ERR
  // 0x200 D27_GRN CAN   OK
//	         ���
  // 0x040 D26_RED BAT   ERR
  // 0x080 D26_GRN BAT   OK
  #if 0
  #else 
  if (FLAGS0_ENTERTST){  // test mode
    if (Test_DLED< (3*TIMEDLED)){
      DouLED = 0xFFF;
    } else if (Test_DLED<(4*TIMEDLED)){
      DouLED = 0x000;//ȫ��
    } else if (Test_DLED<(5*TIMEDLED)){
      DouLED = 0x020;//������
    } else if (Test_DLED<(6*TIMEDLED)){
      DouLED = 0x010;//������
    } else if (Test_DLED<(7*TIMEDLED)){
      DouLED = 0x008;//�����
    } else if (Test_DLED<(8*TIMEDLED)){
      DouLED = 0x004;//�����
    } else if (Test_DLED<(9*TIMEDLED)){
      DouLED = 0x002;//485��
    } else if (Test_DLED<(10*TIMEDLED)){
      DouLED = 0x001;//485��
    } else if (Test_DLED<(11*TIMEDLED)){
      DouLED = 0x800;//232��
    } else if (Test_DLED<(12*TIMEDLED)){
      DouLED = 0x400;//232��
    } else if (Test_DLED<(13*TIMEDLED)){
      DouLED = 0x200;//can��
    } else if (Test_DLED<(14*TIMEDLED)){
      DouLED = 0x100;//can��
    } else if (Test_DLED<(15*TIMEDLED)){
      DouLED = 0x080;//�����
    } else if (Test_DLED<(16*TIMEDLED)){
      DouLED = 0x040;//��غ�
    } else if (Test_DLED<(17*TIMEDLED)){
      DouLED = 0x000;//ȫ��
    } else {
      Test_DLED =0;
    }
  } else {        // control mode �ǲ���
    // 0x010 D36_RED BLUETOOTH   ERR
    // 0x020 D36_GRN BLUETOOTH   OK
    #if 0
    if ((DouLED_FRCR[0] & 0x03) == 0){   //0��1λ
      BIT_CLR(DouLED,4);//r off
      BIT_CLR(DouLED,5);//g off
    } else if ((DouLED_FRCR[0] & 0x03) == 0x01){
      BIT_SET(DouLED,4);//err
      BIT_CLR(DouLED,5);
    } else if ((DouLED_FRCR[0] & 0x03) == 0x02){
      BIT_CLR(DouLED,4);//ok
      BIT_SET(DouLED,5);
    } else {
      BIT_CLR(DouLED,4);//r
      DouLED ^= 0x020;
    }
    #endif
    // 0x004 D35_RED MOT   ERR
    // 0x008 D35_GRN MOT   OK
		/*
		if((FLAGS2_CDSENOR1_ERR==FALSE)&&(FLAGS2_CDSENOR2_ERR==FALSE)){//������⴫�����޴���
    if ((DouLED_FRCR[0] & 0x0C) == 0){   //2��3λ
      BIT_CLR(DouLED,2);//��
      BIT_CLR(DouLED,3);
    } else if ((DouLED_FRCR[0] & 0x0C) == 0x04){
      BIT_SET(DouLED,2);// err
      BIT_CLR(DouLED,3);
    } else if ((DouLED_FRCR[0] & 0x0C) == 0x08){
      BIT_CLR(DouLED,2);// ok
      BIT_SET(DouLED,3);
    } //else {
      //BIT_CLR(DouLED,2);// r
      //DouLED ^= 0x008;//green blink
    //}
	}else{//������⴫�����д���
		  //FLAGS2_CDSENOR_ERR=FALSE;//���������������־  0417yao
		  if((DouLED_FRCR[0] & 0x0C) == 0x04){ //�������
		  BIT_CLR(DouLED,3);// r
	    DouLED ^= 0x004;//red blink
			}else{
			BIT_CLR(DouLED,2);// r
	    DouLED ^= 0x008;//green blink	
			}
	}
	*/
    // 0x001 D31_RED RS485 ERR
    // 0x002 D31_RED RS485 OK
    //FLAGS0_485REDBLK =0;
    //FLAGS0_485GRNBLK =1;
    // 161013 ������Э���޸Ŀ��Ʒ�ʽ
    if ((DouLED_FRCR[0] & 0x30) == 0)
	{   //4��5λ                   g r g r g r g r g r g r
      	if (FLAGS2_TMP_ORHM)
		{  //����ʪ�ȴ������¶�ֵ
        	DouLED ^= 0x002;     // G   ��ɫ��˸
        	BIT_CLR(DouLED,0);   // R
      	} 
		else 
		{//δ����ʪ�ȴ������¶�ֵ
        	if (FLAGS0_485GRNBLK)
			{ //�޴���������
          		BIT_CLR(DouLED,0);   // R  OK  ��ɫ����
          		BIT_SET(DouLED,1);   // G  160928
        	} 
			else if (FLAGS0_485REDBLK)
			{ //�д���������
          		DouLED ^= 0x001;     // R   //��ɫ��˸
          		BIT_CLR(DouLED,1);   // G
        	} 
			else 
			{   //�޴�����
          		BIT_SET(DouLED,0);   // r err  red on
          		BIT_CLR(DouLED,1);   // g
        	}
      	}
	}else if ((DouLED_FRCR[0] & 0x30) == 0x10){ //err
      BIT_SET(DouLED,0);     // R
      BIT_CLR(DouLED,1);   // G
    } else if ((DouLED_FRCR[0] & 0x30) == 0x20){ //OK
      BIT_CLR(DouLED,0);     // R
      BIT_SET(DouLED,1);   // G
    } else if ((DouLED_FRCR[0] & 0x30) == 0x30){ //ȫ��
      BIT_CLR(DouLED,0);     // R
      BIT_CLR(DouLED,1);   // G
    }
    // 0x400 D29_RED RS232 ERR
    // 0x800 D29_GRN RS232 OK   
    if ((DouLED_FRCR[0] & 0xC0) == 0){   //6��7λ
      if (FLAGS0_RS232COMERR==1){//RS232ͨ�Ŵ���
        BIT_SET(DouLED,10);// r
        BIT_CLR(DouLED,11);// g
      } else {
        BIT_CLR(DouLED,10);// r
        //DouLED ^= 0x800;
        BIT_SET(DouLED,11);// g    // 160928
      }
    } else if ((DouLED_FRCR[0] & 0xC0) == 0x40){
      BIT_SET(DouLED,10);// r
      BIT_CLR(DouLED,11);// g
    } else if ((DouLED_FRCR[0] & 0xC0) == 0x80){
      BIT_CLR(DouLED,10);// r
      BIT_SET(DouLED,11);// g
    } else if ((DouLED_FRCR[0] & 0xC0) == 0xC0){
      BIT_CLR(DouLED,10);// r
      BIT_CLR(DouLED,11);// g
    }
    // 0x100 D27_RED CAN   ERR
    // 0x200 D27_GRN CAN   OK		
    if ((DouLED_FRCR[1] & 0x03) == 0){   //8��9λ
      if(FLAGS0_CAN1COMERR==1||FLAGS0_CAN2COMERR==1){  // CANͨ�Ź���  ��ͨ�ź�ɫLED���� ��ͨ�ź�ɫLED�� ��ͨ�� �з��������ɫ��˸
        BIT_SET(DouLED,8);// r
        BIT_CLR(DouLED,9);// g
      } else {
        BIT_CLR(DouLED,8);// r
        //DouLED^= 0x200;
        BIT_SET(DouLED,9);// g 160928
      }
    } else if ((DouLED_FRCR[1] & 0x03) == 0x01){
      BIT_SET(DouLED,8);// r
      BIT_CLR(DouLED,9);// g
    } else if ((DouLED_FRCR[1] & 0x03) == 0x02){
      BIT_CLR(DouLED,8);// r
      BIT_SET(DouLED,9);// g
    } else if ((DouLED_FRCR[1] & 0x03) == 0x03){
      BIT_CLR(DouLED,8);// r
      BIT_CLR(DouLED,9);// g
    }
    // 0x040 D26_RED BAT   ERR
    // 0x080 D26_GRN BAT   OK  
    if ((DouLED_FRCR[1] & 0x0C) == 0x00){   //10��11λ
      if (FLAGS0_NOEXTPOW){// û���ⲿ��Դ
        BIT_SET(DouLED,6);// r
        BIT_CLR(DouLED,7);// g
      } else {
        BIT_CLR(DouLED,6);// r
        //DouLED ^= 0x080;
        BIT_SET(DouLED,7);// g 160928
      }
    } else if ((DouLED_FRCR[1] & 0x0C) == 0x04){
      BIT_SET(DouLED,6); // r
      BIT_CLR(DouLED,7); // g
    } else if ((DouLED_FRCR[1] & 0x0C) == 0x08){
      BIT_CLR(DouLED,6); // r
      BIT_SET(DouLED,7); // g
    } else if ((DouLED_FRCR[1] & 0x0C) == 0x0C){
      BIT_CLR(DouLED,6); // r
      BIT_CLR(DouLED,7); // g
    } 
  }
  #endif
  LED_D12(DouLED & 0xFFF); 
}

void Driver_ColorLED(void){                  //  b r g
  uint32_t RGB ;
  uint8_t i,k,j;
  #ifdef CODE_DLED
  __asm {
    //PUSH     {r4-r8,lr}
    //LDR      R8,CLED_Color
    //LDR      R6,0x40010c00  
    MOVS     R7,#0
  LL12:
    LDR      R8,[R8,#0]
    MOVS     R4,#0
    NOP
  LL20:
    AND      R0,R5,#1
    NOP

    MOVS     r1,#0x40
    MOV      R0,R6
    //BL       GPIO_SetBits
    MOVS     R1,#0x40
    MOV      R0,R6
    //BL       GPIO_ResetBits
    ADDS     R0,R4,#1
    NOP 
    LSRS     R5,R5,#1
    UXTB     R4,R0
    CMP      R4,#0x18
    BCC      LL20
    ADDS     R0,R7,#1
    UXTB     R7,R0
    CMP      R7,#0x0C
    BCC      LL12
    MOV      R0,R6
//    POP      {r4-r8,lr}
    MOVS     r1,#0x40
    //B.W      GPIO_ResetBits
  }
  #else
  ////CLED_Color = 0X05;
  for(k=0;k<12;k++){
    RGB = CLED_Color;
    for(i=0;i<24;i++){
      if ((RGB &0x01)==1) {
        //WS2812B_HIGH();
        #if defined(PLATFORM_F103)
       // WS2812BPORT->BSRR = WS2812BGPIO;
        #elif defined (PLATFORM_F405)
        #endif
        for(j=0;j<4;j++){
        }
        //WS2812B_LOW();
        #if defined(PLATFORM_F103)
       // WS2812BPORT->BRR = WS2812BGPIO;
        #elif defined (PLATFORM_F405)
        #endif
        __NOP;
        __NOP;
        __NOP;
      } else {
        //WS2812B_HIGH();
        #if defined(PLATFORM_F103)
       // WS2812BPORT->BSRR = WS2812BGPIO;
        #elif defined (PLATFORM_F405)
        #endif
        __NOP;
        __NOP;
        __NOP;
        //WS2812B_LOW();
        #if defined(PLATFORM_F103)
       // WS2812BPORT->BRR = WS2812BGPIO;
        #elif defined (PLATFORM_F405)
        #endif
        for(j=0;j<4;j++){
        }
      }
      RGB >>=1;
    }
  }
  //WS2812B_LOW();
  #if defined(PLATFORM_F103)
  //WS2812BPORT->BRR = WS2812BGPIO;
  #elif defined (PLATFORM_F405)
  #endif
  #endif
}

void ColorLED_Refresh(uint8_t Type){
  uint32_t Color;               //  b r g
  //uint32_t RGB ;
  //uint8_t i,j,k;
  
  #if 1

  #else
  if (FLAGS0_ENTERTST){
    if (Test_DLED< (1*TIMECLED)){
      CLED_Color = 0x000020;
    } else if (Test_DLED<(2*TIMECLED)){
      CLED_Color = 0x002000;
    } else if (Test_DLED<(3*TIMECLED)){
      CLED_Color = 0x200000;
    } else if (Test_DLED<(4*TIMECLED)){
      CLED_Color = 0x002020;
    } else if (Test_DLED<(5*TIMECLED)){
      CLED_Color = 0x202020;
    } else if (Test_DLED<(6*TIMECLED)){
      CLED_Color = 0x000000;
    } else if (Test_DLED<(7*TIMECLED)){
      CLED_Color = 0x000040;
    } else if (Test_DLED<(8*TIMECLED)){
      CLED_Color = 0x004000;
    } else if (Test_DLED<(9*TIMECLED)){
      CLED_Color = 0x400000;
    } else if (Test_DLED<(10*TIMECLED)){
      CLED_Color = 0x404040;
    } else if (Test_DLED<(11*TIMECLED)){
      CLED_Color = 0x606060;
    } else if (Test_DLED<(13*TIMECLED)){
      CLED_Color = 0x101010;
    } else {
      CLED_Color =0;
    }
  } else {
    if (Type == 0){  //��                   //0x00 00 00
      CLED_Color =0;              //         g  r  b
    } else if (Type == 1){ //ȫ��
      CLED_Color = 0x00303030;
    } else {
      Color =  ((uint32_t)ColorLED_FRCR[2] <<16)+ ((uint32_t)ColorLED_FRCR[1] <<8) + (uint32_t)ColorLED_FRCR[0];
			if(Color==0x000001||Color==0x000010||Color==0x000100||Color==0x001000||Color==0x010000||Color==0x100000){
			CLED_Color =	Color;
			}else{
			CLED_Color = 0x100000;	
			}
    }
  }
  #endif

}

