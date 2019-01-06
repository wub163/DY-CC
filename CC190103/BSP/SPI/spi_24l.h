/**
  ******************************************************************************
  * @file    SPI/SPI_FLASH/spi_flash.h
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   This file contains all the functions prototypes for the spi_flash
  *          firmware driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_24L_H
#define __SPI_24L_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "sys.h"
#include "misc.h"
  #include "common.h"


#include "delay.h"                                                                      //M3/M4
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/*SPI commands for nRF24L01*/
#define R_REG         0x00
#define W_REG         0x20
#define R_RX          0x61
#define W_TX          0xA0
#define CLR_TX           0xE1
#define CLR_RX           0xE2
#define REUSE_TX        0xE3
#define L01_NOP            0xFF
#define ACTIVATE       0X50             //激活隐藏命令。
#define R_RX_PL_WID     0X60            //数据接收缓冲区上层完整包的宽度。
#define W_ACK_PAYLOAD   0XA8    //加上0-5        //自定义应答附加包。
#define W_TX_PAYLOAD_NOACK 0xB0         //取消本包的自动应答。


/*Internal register address map and its bits definations*/
#define L01REG_CONFIG    0x00 /*Configuration Register*/
    //reserved: bit7, only '0' allowed
    #define MASK_RX_DR   6 //RW, Mask interrupt caused by RX_DR
    #define MASK_TX_DS   5 //RW, Mask interrupt caused by TX_DS
    #define MASK_MAX_RT  4 //RW, Mask interrupt caused by MAX_RT
    #define EN_CRC       3 //RW, Enable CRC.
    #define CRCO         2 //RW, CRC encoding scheme, 0: 1 byte, 1: 2 bytes
    #define PWR_UP       1 //RW, Power up, 1: power up, 0: power down
    #define PRIM_RX      0 //RW, RX/TX control, 1: PRX, 0: PTX
#define L01REG_EN_AA     0x01 /*Enable'Auto Acknowledgement function'*/
    //reserved: bit7-bit6:only '00' allowed
    #define ENAA_P5      5 //RW, Enable auto acknowledgement data pipe 5
    #define ENAA_P4      4 //RW, Enable auto acknowledgement data pipe 4
    #define ENAA_P3      3 //RW, Enable auto acknowledgement data pipe 3
    #define ENAA_P2      2 //RW, Enable auto acknowledgement data pipe 2
    #define ENAA_P1      1 //RW, Enable auto acknowledgement data pipe 1
    #define ENAA_P0      0 //RW, Enable auto acknowledgement data pipe 0
#define L01REG_EN_RXADDR 0x02 /*Enable RX Addresses*/
    //reserved: bit7-bit6:only '00' allowed
    #define ERX_P5       5 //RW, Enable data pipe 5
    #define ERX_P4       4 //RW, Enable data pipe 4
    #define ERX_P3       3 //RW, Enable data pipe 3
    #define ERX_P2       2 //RW, Enable data pipe 2
    #define ERX_P1       1 //RW, Enable data pipe 1
    #define ERX_P0       0 //RW, Enable data pipe 0
#define L01REG_SETUP_AW  0x03 /*Setup of address widths(Common for all pipes)*/
    // reserved: bit7-bit2, only '000000' allowed
    #define AW_RERSERVED 0x0 
    #define AW_3BYTES    0x1
    #define AW_4BYTES    0x2
    #define AW_5BYTES    0x3
#define L01REG_RETR      0x04 /*Setup of Automatic Retransmission*/
    //for bit7-bit4, Auto transmission delay
    #define ARD_250US    (0x00<<4)
    #define ARD_500US    (0x01<<4)
    #define ARD_750US    (0x02<<4)
    #define ARD_1000US   (0x03<<4)
    #define ARD_2000US   (0x07<<4)
    //......
    #define ARD_4000US   (0x0F<<4)
    //for bit3-bit0, Auto retransmission Count
    #define ARC_DISABLE   0x00
    //......
    #define ARC_15        0x0F
#define L01REG_RF_CH      0x05 /*Set the RF channel*/
    //reserved bit7, only '0' allowed
    //bit6-bit0, Sets the frequency channel nRF24L01+ operates on
#define L01REG_RF_SETUP   0x06 /*RF setup register*/
    #define CONT_WAVE     7 //RW, Enables continuous carrier transmit when high
    //reserved bit6, only '0' allowed
    #define RF_DR_LOW     5 //RW, Set RF Data Rate to 250kbps
    #define PLL_LOCK      4 //RW, Force PLL lock signal, Only used in test
    #define RF_DR_HIGH    3 //RW, Low-High:00,1Mbps,01,2Mbps,10,250kbps,11,reserved
    //bit2-bit1:
        #define PWR_18DB  (0x00<<1)
        #define PWR_12DB  (0x01<<1)
        #define PWR_6DB   (0x02<<1)
        #define PWR_0DB   (0x03<<1)
    //reserved, bit0, Do not care
#define L01REG_STATUS     0x07 /*Status register for nRF24L01*/
    //reserved, bit7, only '0' allowed
    #define RX_DR         6 //RW, Data ready RX FIFO interrupt
    #define TX_DS         5 //RW, Data sent TX FIFO interrupt
    #define MAX_RT        4 //RW, Maximum number of TX retransmits interrupt
    //for bit3-bit1, R, Data pipe number for the payload available for reading from
    //RX_FIFO, 000-101, Data pipe number, 110, not used, 111, RX FIFO empty
    #define TX_FULL_0     0 //R, TX FIFO full flag
#define L01REG_OBSERVE_TX 0x08 /*R, Transmit observe register*/
    //bit7-bit4, Count lost packets
    //bit3-bit0, Count retransmitted packets
#define L01REG_RPD        0x09 /**/
    //bit7-bit1, reserved
    #define RPD           0 //Received power detector
#define L01REG_RX_ADDR_P0 0x0A /*RW, Receive address data pipe 0*/
#define L01REG_RX_ADDR_P1 0x0B /*RW, Receive address data pipe 1*/
#define L01REG_RX_ADDR_P2 0x0C /*RW, Receive address data pipe 2*/
#define L01REG_RX_ADDR_P3 0x0D /*RW, Receive address data pipe 3*/
#define L01REG_RX_ADDR_P4 0x0E /*RW, Receive address data pipe 4*/
#define L01REG_RX_ADDR_P5 0x0F /*RW, Receive address data pipe 5*/
#define L01REG_TX_ADDR    0x10 /*RW, Transmit address, used for PTX only*/

#define L01REG_RX_PW_P0   0x11 /*RW, Number of bytes in RX payloadin data pipe 0*/
    //bit7-bi6, reserved, only '00' allowed
    //bit5-bit0, number, 0-32bytes
#define L01REG_RX_PW_P1   0x12 /*RW, Number of bytes in RX payloadin data pipe 1*/
    //bit7-bi6, reserved, only '00' allowed
    //bit5-bit0, number, 0-32bytes
#define L01REG_RX_PW_P2   0x13 /*RW, Number of bytes in RX payloadin data pipe 2*/
    //bit7-bi6, reserved, only '00' allowed
    //bit5-bit0, number, 0-32bytes
#define L01REG_RX_PW_P3   0x14 /*RW, Number of bytes in RX payloadin data pipe 3*/
    //bit7-bi6, reserved, only '00' allowed
    //bit5-bit0, number, 0-32bytes
#define L01REG_RX_PW_P4   0x15 /*RW, Number of bytes in RX payloadin data pipe 4*/
    //bit7-bi6, reserved, only '00' allowed
    //bit5-bit0, number, 0-32bytes
#define L01REG_RX_PW_P5   0x16 /*RW, Number of bytes in RX payloadin data pipe 5*/
    //bit7-bi6, reserved, only '00' allowed
    //bit5-bit0, number, 0-32bytes

#define L01REG_FIFO_STATUS 0x17 /*FIFO status register*/
    //bit7, reserved, only '0' allowed
    #define TX_REUSE      6 //R,
    #define TX_FULL_1     5 //R, TX FIFO full flag
    #define TX_EMPTY      4 //R, TX FIFO empty flag
    //bit3-bit2, reserved, only '00'
    #define RX_FULL       1 //R, RX FIFO full flag
    #define RX_EMPTY      0 //R, RX FIFO empty flag
#define L01REG_DYNPD      0x1C /**/
    //reserved, bit7-bit6, only '00' allowed
    #define DPL_P5        5 //RW, Enable dynamic payload length data pipe 5
    #define DPL_P4        4 //RW, Enable dynamic payload length data pipe 4
    #define DPL_P3        3 //RW, Enable dynamic payload length data pipe 3
    #define DPL_P2        2 //RW, Enable dynamic payload length data pipe 2
    #define DPL_P1        1 //RW, Enable dynamic payload length data pipe 1
    #define DPL_P0        0 //RW, Enable dynamic payload length data pipe 0
#define L01REG_FEATRUE    0x1D
    //reserved, bit7-bit3, only '0000' allowed
    #define EN_DPL        2 //RW, Enables Dynamic payloard length
    #define EN_ACK_PAY    1 //RW, Enable Payload with ACK
    #define EN_DYN_ACK    0 //RW, Enables the W_TX_PAYLOAD_NOACK command
   
   
   
   
   
  extern u8 addr[];
     extern   u8 resend;
  extern   u8 s24L_Send[32],s24L_Receive[32];
   /* M4 新主板板设置 （小智2.6 v1.0主板） */  
//#define s24L_SPI                           SPI2
//#define s24L_SPI_CLK                       RCC_APB1Periph_SPI2
//#define s24L_SPI_CLK_INIT                  RCC_APB1PeriphClockCmd
//
//#define s24L_SPI_SCK_PIN                   GPIO_Pin_13
//#define s24L_SPI_SCK_GPIO_PORT             GPIOB
//#define s24L_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
//#define s24L_SPI_SCK_SOURCE                GPIO_PinSource13
//#define s24L_SPI_SCK_AF                    GPIO_AF_SPI2
//
//#define s24L_SPI_MISO_PIN                  GPIO_Pin_2
//#define s24L_SPI_MISO_GPIO_PORT            GPIOC
//#define s24L_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
//#define s24L_SPI_MISO_SOURCE               GPIO_PinSource2
//#define s24L_SPI_MISO_AF                   GPIO_AF_SPI2
//
//#define s24L_SPI_MOSI_PIN                  GPIO_Pin_3
//#define s24L_SPI_MOSI_GPIO_PORT            GPIOC
//#define s24L_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
//#define s24L_SPI_MOSI_SOURCE               GPIO_PinSource3
//#define s24L_SPI_MOSI_AF                   GPIO_AF_SPI2
//
//#define s24L_CS_PIN                        GPIO_Pin_7
//#define s24L_CS_GPIO_PORT                  GPIOG
//#define s24L_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOG
//
//#define s24L_MOD_PIN                       GPIO_Pin_6
//#define s24L_MOD_GPIO_PORT                 GPIOG
//#define s24L_MOD_GPIO_CLK                  RCC_AHB1Periph_GPIOG
//
//#define s24L_INT_PIN                       GPIO_Pin_8
//#define s24L_INT_GPIO_PORT                 GPIOG
//#define s24L_INT_GPIO_CLK                  RCC_AHB1Periph_GPIOG

 #define s24L_CS     PGout(7)
#define s24L_MOD     PGout(6)
#define s24L_INT     PGin(8)

   
   /* M4 开发板设置  */  
#define s24L_SPI                           SPI1
#define s24L_SPI_CLK                       RCC_APB2Periph_SPI1
#define s24L_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define s24L_SPI_SCK_PIN                   GPIO_Pin_3
#define s24L_SPI_SCK_GPIO_PORT             GPIOB
#define s24L_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define s24L_SPI_SCK_SOURCE                GPIO_PinSource3
#define s24L_SPI_SCK_AF                    GPIO_AF_SPI1

#define s24L_SPI_MISO_PIN                  GPIO_Pin_4
#define s24L_SPI_MISO_GPIO_PORT            GPIOB
#define s24L_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define s24L_SPI_MISO_SOURCE               GPIO_PinSource4
#define s24L_SPI_MISO_AF                   GPIO_AF_SPI1

#define s24L_SPI_MOSI_PIN                  GPIO_Pin_5
#define s24L_SPI_MOSI_GPIO_PORT            GPIOB
#define s24L_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define s24L_SPI_MOSI_SOURCE               GPIO_PinSource5
#define s24L_SPI_MOSI_AF                   GPIO_AF_SPI1

#define s24L_CS_PIN                        GPIO_Pin_7
#define s24L_CS_GPIO_PORT                  GPIOG
#define s24L_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOG

#define s24L_MOD_PIN                       GPIO_Pin_6
#define s24L_MOD_GPIO_PORT                 GPIOG
#define s24L_MOD_GPIO_CLK                  RCC_AHB1Periph_GPIOG

#define s24L_INT_PIN                       GPIO_Pin_8
#define s24L_INT_GPIO_PORT                 GPIOG
#define s24L_INT_GPIO_CLK                  RCC_AHB1Periph_GPIOG
   

   
      /* M3 开发板设置  */  
//#define s24L_SPI                           SPI2
//#define s24L_SPI_CLK                       RCC_APB1Periph_SPI2
//#define s24L_SPI_CLK_INIT                  RCC_APB1PeriphClockCmd
//
//#define s24L_SPI_SCK_PIN                   GPIO_Pin_13
//#define s24L_SPI_SCK_GPIO_PORT             GPIOB
//#define s24L_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOB
//#define s24L_SPI_SCK_SOURCE                GPIO_PinSource14
////#define s24L_SPI_SCK_AF                    GPIO_AF_SPI1
//
//#define s24L_SPI_MISO_PIN                  GPIO_Pin_14
//#define s24L_SPI_MISO_GPIO_PORT            GPIOB
//#define s24L_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOB
//#define s24L_SPI_MISO_SOURCE               GPIO_PinSource13
////#define s24L_SPI_MISO_AF                   GPIO_AF_SPI1
//
//#define s24L_SPI_MOSI_PIN                  GPIO_Pin_15
//#define s24L_SPI_MOSI_GPIO_PORT            GPIOB
//#define s24L_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOB
//#define s24L_SPI_MOSI_SOURCE               GPIO_PinSource15
////#define s24L_SPI_MOSI_AF                   GPIO_AF_SPI1
//
//#define s24L_CS_PIN                        GPIO_Pin_10
//#define s24L_CS_GPIO_PORT                  GPIOF
//#define s24L_CS_GPIO_CLK                   RCC_APB2Periph_GPIOF
//
//#define s24L_MOD_PIN                       GPIO_Pin_5
//#define s24L_MOD_GPIO_PORT                 GPIOC
//#define s24L_MOD_GPIO_CLK                  RCC_APB2Periph_GPIOC
//
//#define s24L_INT_PIN                       GPIO_Pin_15
//#define s24L_INT_GPIO_PORT                 GPIOG
//#define s24L_INT_GPIO_CLK                  RCC_APB2Periph_GPIOG
//
//#define s24L_CS     PFout(10)
//#define s24L_MOD     PCout(5)
//#define s24L_INT     PGin(15)

/* Exported functions ------------------------------------------------------- */
/* High layer functions  */
void s24L_Init(void);
void s24L_WriteBuffer(u8 cmd,u8* pBuffer,  u8 NumByteToWrite);
void s24L_ReadBuffer(u8 cmd,u8* pBuffer,  u8 NumByteToWrite);
u8 s24L_ready(void);
u8 s24L_OK(void);
u8 s25L_SETCH(u8 ch);
u8 s24L_SETADDR(u8* da ,u8 addr);
u8 s24L_CLRTX(void);
u8 s24L_CLRRX(void);
u8 s24L_status(void);
u8 s24L_CLRINT(void);
u8 s24L_CNTBUG(void);
u8 s24L_CDTEST(void);
u8 s24L_DASEND(u8* addr,u8* data, u8 len, u8 wack);
u8 s24L_ACK_CH(u8 CH, u8 CMD);
u8 s24L_set_CH(u8 CH, u8* addr,u8 onoff,u8 len,u8 ACK);
#ifdef __cplusplus
}
#endif

#endif /* __SPI_FLASH_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
