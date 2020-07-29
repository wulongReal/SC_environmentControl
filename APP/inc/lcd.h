#ifndef __LCD_H
#define __LCD_H  
#include "bsp.h"



  
//LCD��Ҫ������
typedef struct{               
  uint16_t width;      //LCD ���
  uint16_t height;     //LCD �߶�
  uint16_t id;         //LCD ID
  uint8_t  wramcmd;    //��ʼдgramָ��
  uint8_t  setxcmd;    //����x����ָ��
  uint8_t  setycmd;    //����y����ָ��   
}_lcd_dev;     

//LCD����
extern _lcd_dev lcddev;  //����LCD��Ҫ����
//LCD�Ļ�����ɫ�ͱ���ɫ     
extern uint16_t  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern uint16_t  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ


//////////////////////////////////////////////////////////////////////////////////   
  
//LCD��ַ�ṹ��
typedef struct{
  uint16_t LCD_REG;
  uint16_t LCD_RAM;
} LCD_TypeDef;
//ʹ��NOR/SRAM�� Bank1.sector4,��ַλHADDR[27,26]=11 A10��Ϊ�������������� 
//ע������ʱSTM32�ڲ�������һλ����! 111110=0x3E          
#define LCD_BASE        ((uint32_t)(0x60000000 | 0x0007FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
//////////////////////////////////////////////////////////////////////////////////

//������ɫ
#define WHITE            0xFFFF  // RGB 565
#define BLACK            0x0000    
#define BLUE             0x001F  
#define BRED             0xF81F
#define GRED             0xFFE0
#define GBLUE            0x07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0xBC40 //��ɫ
#define BRRED            0xFC07 //�غ�ɫ
#define GRAY             0x8430 //��ɫ
#define ORANGE           0xFBE0
//GUI��ɫ

#define DARKBLUE         0x01CF  //����ɫ
#define LIGHTBLUE        0x7D7C  //ǳ��ɫ  
#define GRAYBLUE         0x5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN       0x841F //ǳ��ɫ
//#define LIGHTGRAY      0xEF5B //ǳ��ɫ(PANNEL)
#define LGRAY            0xC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0xA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0x2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
                                      
void LCD_Init(void);                               //��ʼ��
void LCD_DisplayOn(void);                          //����ʾ
void LCD_DisplayOff(void);                          //����ʾ
void LCD_Clear(uint16_t Color);                           //����
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);                    //���ù��
void LCD_DrawPoint(uint16_t x,uint16_t y);                      //����
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color);                //���ٻ���
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r);                    //��Բ
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);              //����
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);               //������
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);               //��䵥ɫ
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);        //���ָ����ɫ
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);            //��ʾһ���ַ�
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size);              //��ʾһ������
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);        //��ʾ ����
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);    //��ʾһ���ַ���,12/16����
    
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
void LCD_WriteRAM_Prepare(void);
void LCD_WR_DATA8(uint8_t da);   //д8λ����  
 
//void showhanzi16(unsigned int x,unsigned int y,unsigned char index);//16*16����
//void showhanzi32(unsigned int x,unsigned int y,unsigned char index);//32*32����


void Show_Logo(uint16_t x,uint16_t y);
void Show_Pig(uint16_t Color1,uint16_t Color2);
void Show_ICO(unsigned int x,unsigned int y,unsigned char index,uint16_t color,unsigned char Type);
void LCD_ClearBLK(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);

#endif  
   
   



