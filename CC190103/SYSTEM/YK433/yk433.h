#ifndef __YK433_TIM_H
#define	__YK433_TIM_H
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "stm32f4xx.h"


/* 高级控制定时器PWM输入捕获 */
/* PWM输入捕获引脚 */
#define YK433_ICPWM_PIN             GPIO_Pin_6            
#define YK433_ICPWM_GPIO_PORT       GPIOC                      
#define YK433_ICPWM_GPIO_CLK        RCC_AHB1Periph_GPIOC
#define YK433_ICPWM_PINSOURCE	    GPIO_PinSource6
#define YK433_ICPWM_AF		    GPIO_AF_TIM8
#define YK433_IC1PWM_CHANNEL        TIM_Channel_1
#define YK433_IC2PWM_CHANNEL        TIM_Channel_2

/* 高级控制定时器 */
#define YK433_TIM           	    TIM8
#define YK433_TIMER_CH              CCR2                //(DMA源地址)CCR1为周期数据，CCR2为脉宽数据
#define YK433_TIM_CLK       	    RCC_APB2Periph_TIM8
#define YK433_TIMER_RCC_PERIPHERAL  RCC_APB2PeriphClockCmd

/* 捕获/比较中断 */
#define YK433_TIM_IRQn		    TIM8_CC_IRQn
#define YK433_TIM_IRQHandler        TIM8_CC_IRQHandler

/*DMA配置参数*/             //CH1触发，CH2读取。
#define YK433_DMA_PERIPHERAL           RCC_AHB1Periph_DMA2
#define YK433_DMA_RCC_PERIPHERAL       RCC_AHB1PeriphClockCmd
#define YK433_DMA_SOURCE               TIM_DMA_CC1
#define YK433_DMA_CHANNEL              DMA_Channel_7
#define YK433_DMA_Stream               DMA2_Stream2
#define YK433_IRQ                      DMA2_Stream2_IRQn
#define YK433_DMA_TC_FLAG              DMA_FLAG_TCIF2
#define YK433_DMAHandler               DMA2_Stream2_IRQHandler 

#define YK433_DMA_BUFFER_SIZE 110



void YK433_task(void *pv);
extern QueueHandle_t xYKKEY;   //u32*2
#endif /* __GENERAL_TIM_H */

