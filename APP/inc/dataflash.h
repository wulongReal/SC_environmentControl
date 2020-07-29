//-------------------DataFlash.h----------------------------------

#ifndef   __DATAFLASH_H__
#define   __DATAFLASH_H__


#include "stm32f10x.h"
#include "stm32f10x_flash.h"




#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
  
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif




int Flash_Read(uint32_t iAddress, char *buf, int32_t iNbrToRead) ;
int Flash_Write(uint32_t iAddress, char *buf, uint32_t iNbrToWrite);


//*************88
int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum) ;
void WriteFlashOneWord(uint32_t WriteAddress,uint32_t WriteData);

#endif
