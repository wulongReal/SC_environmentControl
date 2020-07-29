//-------------------DataFlash.c----------------------------------
#include "DataFlash.h"

uint16_t Flash_Write_Without_check(uint32_t iAddress, uint8_t *buf, uint16_t iNumByteToWrite) {
    uint16_t i;
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
    i = 0;
    
//    FLASH_UnlockBank1();
    while((i < iNumByteToWrite) && (FLASHStatus == FLASH_COMPLETE))
    {
      FLASHStatus = FLASH_ProgramHalfWord(iAddress, *(uint16_t*)buf);
      i = i+2;
      iAddress = iAddress + 2;
      buf = buf + 2;
    }
    
    return iNumByteToWrite;
}
/**
  * @brief  Programs a half word at a specified Option Byte Data address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  buf: specifies the data to be programmed.
  * @param  iNbrToWrite: the number to write into flash
  * @retval if success return the number to write, -1 if error
  *  
  */
int Flash_Write(uint32_t iAddress, char *buf, uint32_t iNbrToWrite) { //ע���������д
                /* Unlock the Flash Bank1 Program Erase controller */
        uint32_t secpos;
        uint32_t iNumByteToWrite = iNbrToWrite;
				uint16_t secoff;
				uint16_t secremain;  
				//uint16_t i = 0;    
        //uint8_t tmp[FLASH_PAGE_SIZE];
        volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

        FLASH_UnlockBank1();
				FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
				secpos=iAddress & (~(FLASH_PAGE_SIZE -1 )) ;//������ַ 
				secoff=iAddress & (FLASH_PAGE_SIZE -1);     //�������ڵ�ƫ��
				secremain=FLASH_PAGE_SIZE-secoff;           //����ʣ��ռ��С
        
        if(iNumByteToWrite<=secremain) secremain = iNumByteToWrite;//������4096���ֽ�
        
        while( 1 ) {
            /*Flash_Read(secpos, tmp, FLASH_PAGE_SIZE);   //������������     �ڶ��Ĺ����г����˴���  
            for(i=0;i<secremain;i++) {       //У������
                if(tmp[secoff+i]!=0XFF)break;       //��Ҫ����   ����һ���ֽڲ�Ϊ0xff ����ѭ��
            }
            if(i<secremain) {  //��Ҫ����
                FLASHStatus = FLASH_ErasePage(secpos); //�����������
                if(FLASHStatus != FLASH_COMPLETE)
                  return -1;
                for(i=0;i<secremain;i++) {   //����
                        tmp[i+secoff]=buf[i];   
                }
                Flash_Write_Without_check(secpos ,tmp ,FLASH_PAGE_SIZE);//д����������
            } else {
                Flash_Write_Without_check(iAddress,buf,secremain);//д�Ѿ������˵ģ�ֱ��д������ʣ�����䡣
            }*/
						FLASHStatus = FLASH_ErasePage(secpos); //�����������
            if(FLASHStatus != FLASH_COMPLETE)
               return -1;
						Flash_Write_Without_check(iAddress,buf,secremain);
            
            if(iNumByteToWrite==secremain) //д�������
                break;
            else {
                secpos += FLASH_PAGE_SIZE;
                secoff = 0;//ƫ��λ��Ϊ0
                buf += secremain;  //ָ��ƫ��
                iAddress += secremain;//д��ַƫ��  
                iNumByteToWrite -= secremain;  //�ֽ����ݼ�
                if(iNumByteToWrite>FLASH_PAGE_SIZE) secremain=FLASH_PAGE_SIZE;//??????????
                else secremain = iNumByteToWrite;  //��һ����������д����
            }
            
         }
        
        FLASH_LockBank1();
        return iNbrToWrite; 
}


/**
  * @brief  Programs a half word at a specified Option Byte Data address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  buf: specifies the data to be programmed.
  * @param  iNbrToWrite: the number to read from flash
  * @retval if success return the number to write, without error
  *  
  */
int Flash_Read(uint32_t iAddress, char *buf, int32_t iNbrToRead) {
        int i = 0;
        while(i < iNbrToRead ) {
           *(buf + i) = *(__IO uint8_t*) iAddress++;
           i++;
        }
        return i;
}

//*****************************

#define  STARTADDR  0x08008000                   	 //STM32F103RB ????????,??? 
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;      //Flash??????


/****************************************************************
*Name:		ReadFlashNBtye
*Function:	???Flash??N????
*Input:		ReadAddress:????(????)ReadBuf:????	ReadNum:?????
*Output:	       ??????  
*Author:        ValerianFan
*Date:		2014/04/09
*E-Mail:	fanwenjingnihao@163.com
*Other:		
****************************************************************/
int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum) 
{
        int DataNum = 0;
		ReadAddress = (uint32_t)STARTADDR + ReadAddress; 
        while(DataNum < ReadNum) 
		{
           *(ReadBuf + DataNum) = *(__IO uint8_t*) ReadAddress++;
           DataNum++;
        }
        return DataNum;
}

/****************************************************************
*Name:		WriteFlashOneWord
*Function:	???Flash??32???
*Input:		WriteAddress:????(????)WriteData:????
*Output:	NULL 
*Author:    ValerianFan
*Date:		2014/04/09
*E-Mail:	fanwenjingnihao@163.com
*Other:		
****************************************************************/

void WriteFlashOneWord(uint32_t WriteAddress,uint32_t WriteData)
{
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
    FLASHStatus = FLASH_ErasePage(STARTADDR);

	if(FLASHStatus == FLASH_COMPLETE)
	{
		FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress, WriteData);    //flash.c ?API??
		//FLASHStatus = FLASH_ProgramWord(StartAddress+4, 0x56780000);                      //???????????
		//FLASHStatus = FLASH_ProgramWord(StartAddress+8, 0x87650000);                      //???????????
	}
	FLASH_LockBank1();
} 



