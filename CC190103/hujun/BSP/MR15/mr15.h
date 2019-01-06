#ifndef __MR15_H
#define	__MR15_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#define MR15_ENABLE                  		0
#define MR15_DISENABLE                  1


//硬件使能
#define MR15_EN_PIN                  GPIO_Pin_9
#define MR15_EN_GPIO_PORT            GPIOG
#define MR15_EN_GPIO_CLK             RCC_AHB1Periph_GPIOG

/* 带参宏，可以像内联函数一样使用 */
#define MR15_EN(a)	if (a)	\
					GPIO_SetBits(MR15_EN_GPIO_PORT,MR15_EN_PIN);\
					else		\
					GPIO_ResetBits(MR15_EN_GPIO_PORT,MR15_EN_PIN)
void MR15_EN_GPIO_Config(void);
void MR15AceCtrl(int16_t speed);
void MR15EpsCtrl(int16_t angle);
void MR15AceIoCtrl(uint8_t actiontype,int16_t lift,uint8_t pwm);
#endif
