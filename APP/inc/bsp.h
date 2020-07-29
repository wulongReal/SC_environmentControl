#ifndef  BSP_PRESENT
#define  BSP_PRESENT

#include "platform.h"


//#define  DEBUG_TEST
#define  V02
//#define  GEBD




/* board configuration */
// <o> SDCard Driver <1=>SDIO sdcard <0=>SPI MMC card
//   <i>Default: 1
#define STM32_USE_SDIO      0

/* whether use board external SRAM memory */
// <e>Use external SRAM memory on the board
//   <i>Enable External SRAM memory
#define STM32_EXT_SRAM          0
//  <o>Begin Address of External SRAM
//    <i>Default: 0x68000000
#define STM32_EXT_SRAM_BEGIN    0x68000000 /* the begining address of external SRAM */
//  <o>End Address of External SRAM
//    <i>Default: 0x68080000
#define STM32_EXT_SRAM_END      0x68080000 /* the end address of external SRAM */
// </e>

// <o> Internal SRAM memory size[Kbytes] <8-64>
//  <i>Default: 64
//efine STM32_SRAM_SIZE         20
#define STM32_SRAM_SIZE         48

#define STM32_SRAM_END          (0x20000000 + STM32_SRAM_SIZE * 1024)

/* RT_USING_UART */
#define RT_USING_UART1
#define RT_USING_UART2
#define RT_USING_UART3
#define RT_USING_UART4
#define RT_USING_UART5

/* USART1 */
#define UART1_GPIO_TX        GPIO_Pin_9
#define UART1_GPIO_RX        GPIO_Pin_10
#define UART1_GPIO           GPIOA
/* USART1_REMAP */
#define UART1_GPIO_REMAP_TX  GPIO_Pin_6
#define UART1_GPIO_REMAP_RX  GPIO_Pin_7
#define UART1_GPIO_REMAP     GPIOB

/* USART2 */
#define UART2_GPIO_TX        GPIO_Pin_2
#define UART2_GPIO_RX        GPIO_Pin_3
#define UART2_GPIO           GPIOA

/* USART3_REMAP[1:0] = 00 */
#define UART3_GPIO_TX        GPIO_Pin_10
#define UART3_GPIO_RX        GPIO_Pin_11
#define UART3_GPIO           GPIOB

#define UART4_GPIO_TX        GPIO_Pin_10
#define UART4_GPIO_RX        GPIO_Pin_11
#define UART4_GPIO           GPIOC

#define UART5_GPIO_TX        GPIO_Pin_12
#define UART5_GPIO_PORT_TX		GPIOC

#define UART5_GPIO_RX        GPIO_Pin_2
#define UART5_GPIO_PORT_RX		GPIOD


//485ä¼ æ„Ÿå™¨ä½¿ç”¨USsart2
#define	MODBUS_Port			2
//DEç«¯å£ä½¿ç”¨PA1
#define MASTERCON_PERIF            RCC_APB2Periph_GPIOA
#define SLAVECON_PERIF             RCC_APB2Periph_GPIOA
#define MASTERCON_PORT             GPIOA        
#define MASTERCON_GPIO             GPIO_Pin_1
#define SLAVECON_PORT              GPIOA
#define SLAVECON_GPIO              GPIO_Pin_1

#define	RS485_SENSOR_SEND			GPIO_SetBits(MASTERCON_PORT, MASTERCON_GPIO)
#define	RS485_SENSOR_RECV			GPIO_ResetBits(MASTERCON_PORT, MASTERCON_GPIO)



//å¤–è®¾GPIOå£åˆå§‹åŒ–
/*
LTE_RESET	PB12
SENSOR_EN	PB8
PWRKEY		PB13
UART2_DE	PA1
BEEP		PA8
5V_EN		PB9
4V_EN		PB3
UART4_EN	PA15
*/
#define	LTE_RESET_PORT	GPIOB
#define	LTE_RESET_PIN	GPIO_Pin_12
#define	LTE_RESET_EN	GPIO_SetBits(LTE_RESET_PORT, LTE_RESET_PIN) 
#define	LTE_RESET_DIS	GPIO_ResetBits(LTE_RESET_PORT, LTE_RESET_PIN)

#define	LTE_PWKEY_PORT	GPIOB
#define	LTE_PWKEY_PIN	GPIO_Pin_13
#define	LTE_PWKEY_HGIH	GPIO_SetBits(LTE_PWKEY_PORT, LTE_PWKEY_PIN) 
#define	LTE_PWKEY_LOW	GPIO_ResetBits(LTE_PWKEY_PORT, LTE_PWKEY_PIN)


#define	BEEP_PORT	GPIOA
#define	BEEP_PIN	GPIO_Pin_8
#define	BEEP_ON		GPIO_SetBits(BEEP_PORT, BEEP_PIN) 
#define	BEEP_OFF	GPIO_ResetBits(BEEP_PORT, BEEP_PIN)


//UART4_485_EN
#define	UART_485_EN_PORT	GPIOA
#define	UART_485_EN_PIN		GPIO_Pin_15
#define	UART_485_EN_SEND		GPIO_SetBits(GPIOA, GPIO_Pin_15) 
#define	UART_485_EN_RECV		GPIO_ResetBits(GPIOA, GPIO_Pin_15) 


//LCDç”µæº
#define	POWER_5V_EN_PORT	GPIOB
#define	POWER_5V_EN_PIN		GPIO_Pin_9
#define	POWER_5V_OPEN		GPIO_SetBits(POWER_5V_EN_PORT, POWER_5V_EN_PIN) 
#define	POWER_5V_OFF		GPIO_ResetBits(POWER_5V_EN_PORT, POWER_5V_EN_PIN)

//LTEç”µæº
#define	POWER_4V_EN_PORT	GPIOB	
#define	POWER_4V_EN_PIN		GPIO_Pin_3
#define	POWER_4V_OPEN		GPIO_SetBits(POWER_4V_EN_PORT, POWER_4V_EN_PIN) 
#define	POWER_4V_OFF		GPIO_ResetBits(POWER_4V_EN_PORT, POWER_4V_EN_PIN)

//SENSOR_EN 485ä¼ æ„Ÿå™¨ç”µæºæŽ§åˆ¶
#define	POWER_SENSOR_PORT	GPIOB	
#define	POWER_SENSOR_PIN		GPIO_Pin_8
#define	POWER_SENSOR_OPEN		GPIO_SetBits(POWER_4V_EN_PORT, POWER_4V_EN_PIN) 
#define	POWER_SENSOR_OFF		GPIO_ResetBits(POWER_4V_EN_PORT, POWER_4V_EN_PIN)

//ç”µæ± åœ¨ä½æ£€æµ‹ã€‚1ï¼šç”µæ± åœ¨ä½
#define	BAT_ON_CHECK_PORT	GPIOB
#define	BAT_ON_CHECK_PIN	GPIO_Pin_5
#define IS_BAT_ON			(GPIO_ReadInputDataBit(BAT_ON_CHECK_PORT, BAT_ON_CHECK_PIN) == 1)?1:0

//å……ç”µçŠ¶æ€ç›‘æµ‹.1:å……ç”µä¸­
#define	BAT_CHARGE_CHECK_PORT	GPIOB
#define	BAT_CHARGE_CHECK_PIN	GPIO_Pin_7
#define IS_BAT_CHARGING			(GPIO_ReadInputDataBit(BAT_CHARGE_CHECK_PORT, BAT_CHARGE_CHECK_PIN) == 1)?1:0

//å……ç”µå®Œæˆæ£€æµ‹.1:å……ç”µå®Œæˆ
#define	BAT_CHARGE_DONE_CHECK_PORT	GPIOB
#define	BAT_CHARGE_DONE_CHECK_PIN	GPIO_Pin_6
#define IS_BAT_CHARGE_DONE		(GPIO_ReadInputDataBit(BAT_CHARGE_DONE_CHECK_PORT, BAT_CHARGE_DONE_CHECK_PIN) == 1)?1:0

//æ–­ç”µæ£€æµ‹ 1ï¼šæ­£å¸¸ï¼Œ0ï¼šæ–­ç”µ
#define	POWER_OFF_CHECK_PORT	GPIOC
#define	POWER_OFF_CHECK_PIN	GPIO_Pin_13
#define IS_POWER_OFF	(GPIO_ReadInputDataBit(POWER_OFF_CHECK_PORT, POWER_OFF_CHECK_PIN) == 0)?1:0





#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#define RT_UART_RX_BUFFER_SIZE  64

enum {
  /* modbus slave 485 receive and transmit control pin index */
  MODBUS_SLAVE_RT_CONTROL_PIN_INDEX = 0,
  /* modbus master 485 receive and transmit control pin index */
  MODBUS_MASTER_RT_CONTROL_PIN_INDEX = 1,
};

#define	 USB_TTL_BAUND_RATE			115200
#define  RS485_SENSOR_BAUD_RATE    	9600
#define  LTE_BAUD_RATE             	115200
#define  RS485_TAG_BAUD_RATE     	9600
#define  LCD_BAUD_RATE            	115200



/* USART1 */
/* USART1_REMAP */
#define UART1_GPIO_REMAP_TX  GPIO_Pin_6
#define UART1_GPIO_REMAP_RX  GPIO_Pin_7
#define UART1_GPIO_REMAP     GPIOB
/* USART2 */




// PA0 ADCIN0
// PA1
#define RS485LCN_PORT              GPIOA
#define RS485LCN_GPIO              GPIO_Pin_1
#define UART2_GPIO                 GPIOA
#define UART2_GPIO_TX              GPIO_Pin_2
#define UART2_GPIO_RX              GPIO_Pin_3

#define ADCSEC_PORT                GPIOA
#define ADCSEC_GPIO                GPIO_Pin_4
#define ADCSEB_PORT                GPIOA
#define ADCSEB_GPIO                GPIO_Pin_5
#define ADCSEA_PORT                GPIOA
#define ADCSEA_GPIO                GPIO_Pin_6
#ifdef GEBD
  #define LED_CP_PORT              GPIOD
  #define LED_CP_GPIO              GPIO_Pin_9
#else
  #define LED_CP_PORT              GPIOA
  #define LED_CP_GPIO              GPIO_Pin_7
#endif
// PA7  ADIN  KEY_IN
#ifdef GEBD
  #define TFTDC_PORT                 GPIOB
  #define TFTDC_GPIO                 GPIO_Pin_9
#else
  #define TFTDC_PORT                 GPIOA
  #define TFTDC_GPIO                 GPIO_Pin_8
#endif
#define UART1_GPIO                 GPIOA
#define UART1_GPIO_TX              GPIO_Pin_9
#define UART1_GPIO_RX              GPIO_Pin_10
#define CANRX_PORT                 GPIOA
#define CANRX_GPIO                 GPIO_Pin_11
#define CANTX_PORT                 GPIOA
#define CANTX_GPIO                 GPIO_Pin_12
// PA13 SWDIO
// PA14 SWCLK
#define TFTRST_PORT                GPIOA
#define TFTRST_GPIO                GPIO_Pin_15

#define LED_D_PORT                 GPIOB
#define LED_D_GPIO                 GPIO_Pin_0
#define TFTBL_PORT                 GPIOB
#define TFTBL_GPIO                 GPIO_Pin_1
/*
#define MASTERCON_PORT             GPIOA        // TXD3 CONROL 
#define MASTERCON_GPIO             GPIO_Pin_1
#define SLAVECON_PORT              GPIOA
#define SLAVECON_GPIO              GPIO_Pin_1
*/
#define LED1_PORT                  GPIOB
#define LED1_PIN                   GPIO_Pin_3
#ifdef GEBD
  #define LEDSTR_PORT              GPIOD
  #define LEDSTR_GPIO              GPIO_Pin_11
#else
  #define LEDSTR_PORT              GPIOB
  #define LEDSTR_GPIO              GPIO_Pin_4
#endif
#define LED2_PORT                  GPIOC
#define LED2_PIN                   GPIO_Pin_3
#define WS2812BPORT                GPIOB
#define WS2812BGPIO                GPIO_Pin_6
#ifdef GEBD
  #define LED_EO_PORT              GPIOD
  #define LED_EO_GPIO              GPIO_Pin_12
#else
  #define LED_EO_PORT              GPIOB
  #define LED_EO_GPIO              GPIO_Pin_7
#endif
#ifdef GEBD
  #define POWGPRS_PORT             GPIOE
  #define POWGPRS_GPIO             GPIO_Pin_10
  #define POW_PORT                 GPIOE
  #define POW_GPIO                 GPIO_Pin_11
#else
  #define POWGPRS_PORT             GPIOB
  #define POWGPRS_GPIO             GPIO_Pin_8
  #define POW_PORT                 GPIOB
  #define POW_GPIO                 GPIO_Pin_9
#endif

#define UART3_GPIO                 GPIOB
#define UART3_GPIO_TX              GPIO_Pin_10
#define UART3_GPIO_RX              GPIO_Pin_11
#ifdef GEBD
  #define TFTCS_PORT               GPIOB
  #define TFTCS_GPIO               GPIO_Pin_8
#else
  #define TFTCS_PORT               GPIOB
  #define TFTCS_GPIO               GPIO_Pin_12
#endif
#define TFTSCK_PORT                GPIOB
#define TFTSCK_GPIO                GPIO_Pin_13
#define TFTMISO_PORT               GPIOB
#define TFTMISO_GPIO               GPIO_Pin_14
#define TFTMOSI_PORT               GPIOB
#define TFTMOSI_GPIO               GPIO_Pin_15

#ifdef GEBD
  #define RS485CON1_PORT           GPIOC
  #define RS485CON1_GPIO           GPIO_Pin_0
  #define MPUINT_PORT              GPIOC
  #define MPUINT_GPIO              GPIO_Pin_1
  #define NRFINT_PORT              GPIOC
  #define NRFINT_GPIO              GPIO_Pin_2
  #define VOCINT_PORT              GPIOC
  #define VOCINT_GPIO              GPIO_Pin_3
  #define TEMP2_PORT               GPIOC
  #define TEMP2_GPIO               GPIO_Pin_4
  #define FLOW_PORT                GPIOC
  #define FLOW_GPIO                GPIO_Pin_5
  #define MTPWM_PORT               GPIOC
  #define MTPWM_GPIO               GPIO_Pin_6
  #define NRFCS_PORT               GPIOC
  #define NRFCS_GPIO               GPIO_Pin_7
  #define TEMP1_PORT               GPIOC
  #define TEMP1_GPIO               GPIO_Pin_8
  //PC9 NC
  #define RXD4_PORT                GPIOC
  #define TXD4_GPIO                GPIO_Pin_10
  

  
  
  #define RS485CON4_PORT           GPIOC
  #define RS485CON4_GPIO           GPIO_Pin_12  
#endif

#define UART4_GPIO                 GPIOC
#define UART4_GPIO_TX              GPIO_Pin_10
#define UART4_GPIO_RX              GPIO_Pin_11

#define IICSDA_PORT                GPIOC
#define IICSDA_GPIO                GPIO_Pin_13
#define IICSCL_PORT                GPIOC
#define IICSCL_GPIO                GPIO_Pin_14
#define POWBAT_PORT                GPIOC
#define POWBAT_GPIO                GPIO_Pin_15

#ifdef GEBD
  #define VOCS_PORT                GPIOD
  #define VOCS_GPIO                GPIO_Pin_0
  #define VOCO_PORT                GPIOD
  #define VOCO_GPIO                GPIO_Pin_1
  // PD2 PD3  NC
  #define MTDIS_PORT               GPIOD
  #define MTDIS_GPIO               GPIO_Pin_4
  #define NRFCE_PORT               GPIOD
  #define NRFCE_GPIO               GPIO_Pin_5
  // PD6   NC 
  #define LDRCS_PORT               GPIOD
  #define LDRCS_GPIO               GPIO_Pin_7
  // PD8   NC
  // PD9 LED_CP
  #define FLOWDET_PORT             GPIOD
  #define FLOWDET_GPIO             GPIO_Pin_10
  // PD11 LED_STR
  // PD12 LED_EO
  // PD13 PD14 PD15 NC  
  
  #define WFCH_PORT                GPIOE
  #define WFCH_GPIO                GPIO_Pin_0
  //PE2 -PE6 TEMP
  // PE7 NC  
  #define MTDIR_PORT               GPIOE
  #define MTDIR_GPIO               GPIO_Pin_8
  #define MTCS_PORT                GPIOE
  #define MTCS_GPIO                GPIO_Pin_9
  //PE10 GPRS_ON
  //PE11 POWER_ON
  //PE12 PE13 PE14 PE15 NC
#endif


//#define ADC_AH()                   GPIO_SetBits(  ADCSEA_PORT,ADCSEA_GPIO)//PA6
//#define ADC_AL()                   GPIO_ResetBits(ADCSEA_PORT,ADCSEA_GPIO)
//#define ADC_BH()                   GPIO_SetBits(  ADCSEB_PORT,ADCSEB_GPIO)//PA5
//#define ADC_BL()                   GPIO_ResetBits(ADCSEB_PORT,ADCSEB_GPIO)
//#define ADC_CH()                   GPIO_SetBits(  ADCSEC_PORT,ADCSEC_GPIO)//PA4
//#define ADC_CL()                   GPIO_ResetBits(ADCSEC_PORT,ADCSEC_GPIO)


#define ADC_AH()                  
#define ADC_AL()                 
#define ADC_BH()                  
#define ADC_BL()               
#define ADC_CH()                   
#define ADC_CL()                 


#define SPILCD_RST_SET             GPIO_SetBits(  TFTRST_PORT, TFTRST_GPIO)//PA15
#define SPILCD_RST_RESET           GPIO_ResetBits(TFTRST_PORT, TFTRST_GPIO)//PA15  
#define SPILCD_RS_SET              GPIO_SetBits(  TFTDC_PORT,  TFTDC_GPIO)//PA8 
#define SPILCD_RS_RESET            GPIO_ResetBits(TFTDC_PORT,  TFTDC_GPIO)//PA8  
#define SPILCD_CS_H()              GPIO_SetBits(  TFTCS_PORT,  TFTCS_GPIO)//PB12 
#define SPILCD_CS_L()              GPIO_ResetBits(TFTCS_PORT,  TFTCS_GPIO)//PB12



//TFT_BL
#ifdef V02
  #define TFT_BL_ON()              GPIO_SetBits(  TFTBL_PORT,  TFTBL_GPIO)//PB1
  #define TFT_BL_OFF()             GPIO_ResetBits(TFTBL_PORT,  TFTBL_GPIO)//PB1
#else
  #define TFT_BL_ON()              GPIO_ResetBits(TFTBL_PORT,  TFTBL_GPIO)//PB1
  #define TFT_BL_OFF()             GPIO_SetBits(  TFTBL_PORT,  TFTBL_GPIO)//PB1
#endif
/*
// HEF4985 driver
//#define LED_STR_H()                GPIO_ResetBits(LEDSTR_PORT, LEDSTR_GPIO)//PB4
//#define LED_STR_L()                GPIO_SetBits(  LEDSTR_PORT, LEDSTR_GPIO)

#define LED_STR_H()             
#define LED_STR_L()               

//#define LED_D_H()                  GPIO_ResetBits(LED_D_PORT,  LED_D_GPIO)//PB0
//#define LED_D_L()                  GPIO_SetBits(  LED_D_PORT,  LED_D_GPIO)

#define LED_D_H()              
#define LED_D_L()                 


//#define LED_CP_H()                 GPIO_ResetBits(LED_CP_PORT, LED_CP_GPIO)//PA7
//#define LED_CP_L()                 GPIO_SetBits(  LED_CP_PORT, LED_CP_GPIO)

#define LED_CP_H()           
#define LED_CP_L()                 

#define LED_EO_H()                 GPIO_ResetBits(LED_EO_PORT, LED_EO_GPIO)//PB7
#define LED_EO_L()                 GPIO_SetBits(  LED_EO_PORT, LED_EO_GPIO)

#define WS2812B_HIGH()             GPIO_SetBits(  WS2812BPORT, WS2812BGPIO)
#define WS2812B_LOW()              GPIO_ResetBits(WS2812BPORT, WS2812BGPIO)
*/
#define	RUN_LED_PORT			GPIOC
#define	RUN_LED_PIN				GPIO_Pin_11
#define RUN_LED_BLINK()               ((RUN_LED_PORT->ODR & RUN_LED_PIN) ? (RUN_LED_PORT->BRR = RUN_LED_PIN): (RUN_LED_PORT->BSRR = RUN_LED_PIN))

#define	EX_WDT_FEED_PORT			GPIOC
#define	EX_WDT_FEED_PIN				GPIO_Pin_9
#define EX_WDT_FEED_TOG()               ((EX_WDT_FEED_PORT->ODR & EX_WDT_FEED_PIN) ? (EX_WDT_FEED_PORT->BRR = EX_WDT_FEED_PIN): (EX_WDT_FEED_PORT->BSRR = EX_WDT_FEED_PIN))


//#define LED1_ON()                  GPIO_ResetBits(LED1_PORT, LED1_PIN)//PB3
//#define LED1_OFF()                 GPIO_SetBits(  LED1_PORT, LED1_PIN)
/*


#define LED1_ON()                 
#define LED1_OFF()                 

#define LED2_ON()                  GPIO_ResetBits(LED2_PORT, LED2_PIN)//PB5
#define LED2_OFF()                 GPIO_SetBits(  LED2_PORT, LED2_PIN)
//#define LED1_BLINK()               ((LED1_PORT->ODR & LED1_PIN) ? (LED1_PORT->BRR = LED1_PIN): (LED1_PORT->BSRR = LED1_PIN))  

         

#define LED2_BLINK()               ((LED2_PORT->ODR & LED2_PIN) ? (LED2_PORT->BRR = LED2_PIN): (LED2_PORT->BSRR = LED2_PIN))
#define RED_BLINK()                LED1_BLINK()
#define GRN_BLINK()                LED2_BLINK()
#define RED_ON()                   LED1_ON()
#define RED_OFF()                  LED1_OFF()
#define GRN_ON()                   LED2_ON()
#define GRN_OFF()                  LED2_OFF()
*/
//#define POW_OFF()                  GPIO_ResetBits(POW_PORT,     POW_GPIO) //PB9
//#define POW_ON()                   GPIO_SetBits(  POW_PORT,     POW_GPIO)
    


//#define POWGPRS_OFF()              GPIO_ResetBits(POWGPRS_PORT, POWGPRS_GPIO)
//#define POWGPRS_ON()               GPIO_SetBits(  POWGPRS_PORT, POWGPRS_GPIO)

           




#if 1
//#define POWBAT_BLINK()             ((POWBAT_PORT->ODR & POWBAT_GPIO) ? (POWBAT_PORT->BRR = POWBAT_GPIO): (POWBAT_PORT->BSRR = POWBAT_GPIO))
#endif

#define IIC_SDA_H()                GPIO_SetBits(  IICSDA_PORT, IICSDA_GPIO)
#define IIC_SDA_L()                GPIO_ResetBits(IICSDA_PORT, IICSDA_GPIO)
#define IIC_SCL_H()                GPIO_SetBits(  IICSCL_PORT, IICSCL_GPIO)
#define IIC_SCL_L()                GPIO_ResetBits(IICSCL_PORT, IICSCL_GPIO)

#define DIR_PORT                   GPIOB
#define DIR_GPIO                   GPIO_Pin_2
#define DIR_IN()                   GPIO_ResetBits(DIR_PORT,DIR_GPIO)  //1018  PB2
#define DIR_OUT()                  GPIO_SetBits(DIR_PORT,DIR_GPIO)    //1018
// TXD1 RXD1 CONRROL PA9  PA10 PC0  RS232 AND RS485   RS232  CORE USE
// TXD2 RXD2 CONTROL PA2  PA3  PA1  RS232 AND RS485   RS485  BOOTLOADER
// TXD3 RXD3 CONTROL PB10 PB11 PB2  RS232 AND RS485   RS485  SENSOR
// TXD4 RXD4 CONTROL PC11 PC10 PC12 RS485 AND NC7SZ126 AND EPS8266     
//

#ifdef GEBD
  #define RADIO_EN_CS()              GPIO_ResetBits(NRFCS_PORT, NRFCS_GPIO)
  #define RADIO_DI_CS()              GPIO_SetBits(  NRFCS_PORT, NRFCS_GPIO)
  #define RADIO_DI_CE()              GPIO_ResetBits(NRFCE_PORT, NRFCE_GPIO)
  #define RADIO_EN_CE()              GPIO_SetBits(  NRFCE_PORT, NRFCE_GPIO)
#endif

/*
fdef GEBD
  #define RS485CON1_PORT           GPIOC
  #define RS485CON1_GPIO           GPIO_Pin_0
  
  #define MPUINT_PORT              GPIOC
  #define MPUINT_GPIO              GPIO_Pin_1
  #define NRFINT_PORT              GPIOC
  #define NRFINT_GPIO              GPIO_Pin_2
  #define VOCINT_PORT              GPIOC
  #define VOCINT_GPIO              GPIO_Pin_3
  
  #define TEMP2_PORT               GPIOC
  #define TEMP2_GPIO               GPIO_Pin_4
  #define FLOW_PORT                GPIOC
  #define FLOW_GPIO                GPIO_Pin_5
  #define MTPWM_PORT               GPIOC
  #define MTPWM_GPIO               GPIO_Pin_6
  #define NRFCS_PORT               GPIOC
  #define NRFCS_GPIO               GPIO_Pin_7
  #define TEMP1_PORT               GPIOC
  #define TEMP1_GPIO               GPIO_Pin_8
  //PC9 NC
  #define RXD4_PORT                GPIOC
  #define TXD4_GPIO                GPIO_Pin_10
  #define RS485CON4_PORT           GPIOC
  #define RS485CON4_GPIO           GPIO_Pin_12  
  */
  /*
  #define VOCS_PORT                GPIOD
  #define VOCS_GPIO                GPIO_Pin_0
  #define VOCO_PORT                GPIOD
  #define VOCO_GPIO                GPIO_Pin_1
  // PD2 PD3  NC
  #define MTDIS_PORT               GPIOD
  #define MTDIS_GPIO               GPIO_Pin_4
  #define NRFCE_PORT               GPIOD
  #define NRFCE_GPIO               GPIO_Pin_5
  // PD6   NC 
  #define LDRCS_PORT               GPIOD
  #define LDRCS_GPIO               GPIO_Pin_7
  // PD8   NC
  // PD9 LED_CP
  #define FLOWDET_PORT             GPIOD
  #define FLOWDET_GPIO             GPIO_Pin_10
  // PD11 TFTBL
  // PD12 LED_EO
  // PD13 PD14 PD15 NC  
  
  #define WFCH_PORT                GPIOE
  #define WFCH_GPIO                GPIO_Pin_0
  //PE2 -PE6 TEMP
  // PE7 NC  
  #define MTDIR_PORT               GPIOE
  #define MTDIR_GPIO               GPIO_Pin_8
  #define MTCS_PORT                GPIOE
  #define MTCS_GPIO                GPIO_Pin_9
  //PE10 GPRS_ON
  //PE11 POWER_ON
  //PE12 PE13 PE14 PE15 NC
  */

/*************************³ÆÄ£¿é¼Ä´æÆ÷ÆðÊ¼µØÖ·**********************************/
#define AD_ADDR_2                 0         //adÖµ
#define MZ_ADDR_2                 2         //Ã«ÖØ
#define JZ_ADDR_2                 4         //¾»ÖØ
#define FLAG_ADDR                 0x06      //±êÖ¾Î»
#define VER_ADDR                  0x07      //¹Ì¼þ°æ±¾
#define ADSPEED_ADDR              0X08      //ADËÙÂÊ   5£¬10£¬20£¬40
#define FILTER_DEEP               0X09      //  ÂË²¨Éî¶È 1£¬2£¬3
#define DEV_ADDR                  0x0a      //Ä£¿éµØÖ·£¬1~63
#define BAUTE_ADDR                0x0b      //²¨ÌØÂÊ  2400£¬4800£¬9600£¬19200£¬38400£¬57600
#define XSD_ADDR                  0x0c      //Ð¡Êýµã   0£¬1£¬2£¬3
#define DIV_ADDR                  0x0d      //·Ö¶ÈÖµ   1£¬2£¬5£¬10£¬50£¬100
#define BD_ADDR_2                 0x11      //±ê¶¨    
#define FULL_ADDR_2               0x13      //ÂúÁ¿³Ì
#define QUPI_ADDR_2               0x15      //È¥Æ¤
#define SET0_ADDR                 0x17      //ÊÖ¶¯ÖÃÁã
#define REST_ADDR                 0x18      //¸´Î»
#define INIT_ADDR                 0x19      //³õÊ¼»¯
/**********************************************************************************/



void rt_hw_board_init(void);

void IWDG_Configuration(void);
void IWDG_Feed(void);




/*********************************************************************************************************/
/**                                                 MACRO'S                         */
/***********************************************************************************************************/


//Èí¡¢Ó²¼þ°æ±¾ºÅ¶¨Òå
#define VERSION_SOFTWARE_MAJOR     1
#define VERSION_SOFTWARE_MINOR     0
#define VERSION_HARDWARE_MAJOR     1
#define VERSION_HARDWARE_MINOR     0   

/***********************************************************************************************************/
/*                                               DATA TYPES                         */
/***********************************************************************************************************/


/**********************************************************************************************************
*                                            GLOBAL VARIABLES
**********************************************************************************************************/




/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void BSP_Init(void);

void rt_hw_board_init(void);
void IWDG_Feed(void);
void rt_hw_timer_handler(void);
uint8_t AvoidTimeout(uint32_t TimeOfTimeout,uint32_t Period,uint8_t (*DetectCondition)(),uint8_t ConditionValue);
void Delay(vu32 nCount);
/*
*********************************************************************************************************
*                                           INTERRUPT SERVICES
*********************************************************************************************************
*/
#define FAN1_OUT               GPIOA,GPIO_Pin_0
#define FAN2_OUT               GPIOA,GPIO_Pin_4    //
#define FAN3_OUT               GPIOA,GPIO_Pin_5   //
#define FAN4_OUT               GPIOA,GPIO_Pin_6   //
#define FAN5_OUT               GPIOA,GPIO_Pin_7    //
#define FAN6_OUT               GPIOC,GPIO_Pin_4    //
#define FAN7_OUT               GPIOC,GPIO_Pin_5    //
//#define FAN8_OUT               GPIOB,GPIO_Pin_0    //
#define FAN8_OUT               GPIOB,GPIO_Pin_9    //
#define FAN9_OUT               GPIOB,GPIO_Pin_8   //
#define FAN10_OUT               GPIOB,GPIO_Pin_7  //
#define FAN11_OUT               GPIOB,GPIO_Pin_6  //
#define FAN12_OUT               GPIOB,GPIO_Pin_5   //
#define FAN13_OUT               GPIOB,GPIO_Pin_4  //
#define FAN14_OUT               GPIOB,GPIO_Pin_3  //
#define FAN15_OUT               GPIOD,GPIO_Pin_2  //


#define	DA_OUT_MAX_5V			0
#define	DA_OUT_MAX_10V			1
#define	DA_OUT_MAX_SEL_INPUT	GPIOC,GPIO_Pin_1
#define	GET_DA_OUT_MAX()			GPIO_ReadInputDataBit(DA_OUT_MAX_SEL_INPUT)

#define	ALARM_SHOW_PIN			FAN15_OUT
#define	ALARM_BEEP_RUN			GPIO_ResetBits(ALARM_SHOW_PIN)
#define	ALARM_BEEP_OFF			GPIO_SetBits(ALARM_SHOW_PIN)



/*
#define	ESIM_RESET_OUT			GPIOB,GPIO_Pin_0
#define	ESIM_RESET_MODE			GPIO_SetBits(ESIM_RESET_OUT)
#define	ESIM_WORK_MODE			GPIO_ResetBits(ESIM_RESET_OUT)

#define	INPUT_PORT_1			GPIOB,GPIO_Pin_1
#define	POWER_OFF_CHECK_INPUT	GPIO_ReadInputDataBit(INPUT_PORT_1)
#define	IS_POWER_OFF()			POWER_OFF_CHECK_INPUT>0?0:1

#define	BEEP_OUT_PIN			GPIOC,GPIO_Pin_12
#define	BEEP_OPEN			GPIO_SetBits(BEEP_OUT_PIN)
#define	BEEP_CLOSE			GPIO_ResetBits(BEEP_OUT_PIN)
*/

#endif                                                          /* End of module include.                               */












