#ifndef __TFTLED_REF_H_
#define __TFTLED_REF_H_
#include "lcd.h"
#include "vars.h"
#include <rtthread.h>
#include "can.h"
#include "rs232.h"
void TFT_DISP_Num(int32_t Num, uint16_t Color);
void TFT_Refresh(void);
void ColorLED_Refresh(uint8_t Type);
void Double_Ref(void);
void LED_D12(uint16_t D24_LED);
void Show_Ver(void);
void Driver_ColorLED(void);


#endif




