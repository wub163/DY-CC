#ifndef __ADC_H
#define	__ADC_H


#include "stm32f4xx.h"

//     HIGH_PB0 //PB0 接电位器              ADC12-IN8          DMA2 C0 S0
#define HIGH_ADC_DR_ADDR    (ADC1->DR)
//ADC输入引脚PA0-PA7
#define HIGH_ADC_GPIO_PORT    GPIOA
#define HIGH_ADC_GPIO_PIN     GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7
#define HIGH_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOA

#define HIGH_ADC              ADC1
#define HIGH_ADC_CLK          RCC_APB2Periph_ADC1
#define HIGH_ADC_CHANNEL      ADC_Channel_0

#define HIGH_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define HIGH_ADC_DMA_CHANNEL  DMA_Channel_0
#define HIGH_ADC_DMA_STREAM   DMA2_Stream0

extern u16  *ADC_V;

void ADC8(void* PV);


#endif /* __ADC_H */

