#ifndef __SPI_DAC_H
#define __SPI_DAC_H

#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
/*�����-��ͷ*******************************/
//�������á�
//WTM: Writing data to a channel's register causes the DAC output to change.//д�ڴ棬����ͨ����
#define	DAC_128S_MODE_WTM					0x9000		//1001 xxxx xxxx xxxx WTM

//The registers of each DAC Channel can be written to without causing their outputs to change.��д�ڴ档
#define	DAC_128S_MODE_WRM					0x8000		//1000 xxxx xxxx xxxx WRM
//дͨ�����ݡ�0x0000---0x7000
//Update Select: The DAC outputs of the channels selected with a 1 in DB[7:0] are updated simultaneously to the values in their respective control registers.
#define	DAC_128S_UPDATA_SELECT		0xaf00		//1010 xxxx HGFE DCBA  ͨ�����¡�

//дAͨ�������Ҹ�������ͨ����
//Channel A Write: The control register and DAC output of channel A are updated to the data in DB[11:0]. 
//The outputs of the other seven channels are also updated according to their respective control register values.
#define	DAC_128S_A_WRITE					0xb000		//1011 xxxx xxxx xxxx

//ͨ���㲥��д�����мĴ��������Ҹ���ͨ����
//Broadcast: The data in DB[11:0] is written to all channel control registers and DAC output simultaneously.
#define	DAC_128S_BROADCAST				0xc000		//1100 xxxx xxxx xxxx
//�������          0xd000  xxxx HGFE DCBA
//���100k          0xe000  xxxx HGFE DCBA
//���2.5k          0xf000  xxxx HGFE DCBA

/*�����-��β*******************************/


/*SPI�ӿڶ���-��ͷ****************************/
//SPI��
#define DAC_SPI                           SPI5
#define DAC_SPI_CLK                       RCC_APB2Periph_SPI5
#define DAC_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd
//SCK����
#define DAC_SPI_SCK_PIN                   GPIO_Pin_7                  
#define DAC_SPI_SCK_GPIO_PORT             GPIOF                    
#define DAC_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOF
#define DAC_SPI_SCK_PINSOURCE             GPIO_PinSource7
#define DAC_SPI_SCK_AF                    GPIO_AF_SPI5
//MISO����
#define DAC_SPI_MISO_PIN                  GPIO_Pin_8                
#define DAC_SPI_MISO_GPIO_PORT            GPIOF                  
#define DAC_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define DAC_SPI_MISO_PINSOURCE            GPIO_PinSource8
#define DAC_SPI_MISO_AF                   GPIO_AF_SPI5
//MOSI����
#define DAC_SPI_MOSI_PIN                  GPIO_Pin_9               
#define DAC_SPI_MOSI_GPIO_PORT            GPIOF                      
#define DAC_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define DAC_SPI_MOSI_PINSOURCE            GPIO_PinSource9
#define DAC_SPI_MOSI_AF                   GPIO_AF_SPI5

//CS(NSS)����
#define DAC_SPI_NSS_PIN                   GPIO_Pin_6                
#define DAC_SPI_NSS_GPIO_PORT             GPIOF                     
#define DAC_SPI_NSS_GPIO_CLK              RCC_AHB1Periph_GPIOF
#define DAC_SPI_NSS_PINSOURCE             GPIO_PinSource6
#define DAC_SPI_NSS_AF                    GPIO_AF_SPI5

//CS(NSS)����
#define DAC_CS_PIN                        GPIO_Pin_6                
#define DAC_CS_GPIO_PORT                  GPIOF                     
#define DAC_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOF

//DMA  ����
#define spi_dac                          SPI5
#define spi_dac_DMA_PERIPHERAL           RCC_AHB1Periph_DMA2
#define spi_dac_DMA_RCC_PERIPHERAL       RCC_AHB1PeriphClockCmd
#define spi_dac_DMA_SOURCE               
#define spi_dac_DMA_CHANNEL              DMA_Channel_7
#define spi_dac_DMA_Stream               DMA2_Stream6
#define spi_dac_IRQ                      DMA2_Stream6_IRQn
#define spi_dac_DMA_TC_FLAG              DMA_FLAG_TCIF6
#define spi_dac_DMAHandler               DMA2_Stream6_IRQHandler 


//����CS(NSS)��������͵�ƽ
#define SPI_DAC_CS_LOW()      {DAC_CS_GPIO_PORT->BSRRH=DAC_CS_PIN;}	
//����CS(NSS)��������ߵ�ƽ			
#define SPI_DAC_CS_HIGH()     {DAC_CS_GPIO_PORT->BSRRL=DAC_CS_PIN;}

/*SPI�ӿڶ���-��β****************************/

//void SPI5_DAC_Config(void);
//u8 DAC_Channel(u8 Channel,u16 Voltage);
//u8 DAC_SPI_SendData(u8 Ctrl,u16 Voltage);
//


extern u16* DAC_OUT;
extern void DAC8 (void* pv);

#endif /* __SPI_DAC_H */

