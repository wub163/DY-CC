#ifndef __ENCODER_H
#define __ENCODER_H 
#include "stm32f4xx.h"  

//Òý½Å¶¨Òå
/*******************************************************/

#ifdef DEVICE_TYPE_AGV41

#endif

#ifdef DEVICE_TYPE_AGV40

#endif


#define ENCODER_CLK										RCC_APB2Periph_TIM8

// ENCODER  A  B
#define ENCODER_A_PIN                 GPIO_Pin_6
#define ENCODER_A_GPIO_PORT           GPIOC
#define ENCODER_A_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define ENCODER_A_PINSOURCE        		GPIO_PinSource6
#define ENCODER_A_AF			          	GPIO_AF_TIM8

#define ENCODER_B_PIN                 GPIO_Pin_7
#define ENCODER_B_GPIO_PORT           GPIOC
#define ENCODER_B_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define ENCODER_B_PINSOURCE        		GPIO_PinSource7
#define ENCODER_B_AF			          	GPIO_AF_TIM8
void Encoder_Init(void);

#endif
