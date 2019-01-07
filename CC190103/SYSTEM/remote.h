#ifndef __RED_H
#define __RED_H 
#include "stm32f4xx.h"  
#include "FreeRTOS.h"
#include "queue.h"


//���Ŷ���
/*******************************************************/
// SWITCH_CHARG
#define SWITCH_CHARG_PIN                 GPIO_Pin_8
#define SWITCH_CHARG_GPIO_PORT           GPIOB
#define SWITCH_CHARG_GPIO_CLK            RCC_AHB1Periph_GPIOB
//  RELAY1
#define RELAY1_PIN             			     GPIO_Pin_9
#define RELAY1_GPIO_PORT          			 GPIOB
#define RELAY1_GPIO_CLK            			 RCC_AHB1Periph_GPIOB

//  REMOTE_D1
#define REMOTE_D1_PIN             			     GPIO_Pin_0
#define REMOTE_D1_GPIO_PORT          			 	 GPIOB
#define REMOTE_D1_GPIO_CLK            			 RCC_AHB1Periph_GPIOB
//  PWM1
#define PWM1_PIN             			     GPIO_Pin_1
#define PWM1_GPIO_PORT          			 	 GPIOB
#define PWM1_GPIO_CLK            			 RCC_AHB1Periph_GPIOB

#define RHEOSTAT_NOFCHANEL  4
 


// PF7 �ӵ�λ��
#define RHEOSTAT_ADC_DR_ADDR      ((u32)ADC1+0x4c)

#define RHEOSTAT_ADC_GPIO_PORT    GPIOA
#define RHEOSTAT_ADC1_CH3    			GPIO_Pin_3
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOA

#define RHEOSTAT_ADC              ADC1
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_CHANNEL3     ADC_Channel_3

// PF8 �ӵ�λ��
#define RHEOSTAT_ADC_DR_ADDR      ((u32)ADC1+0x4c)

#define RHEOSTAT_ADC_GPIO_PORT    GPIOA
#define RHEOSTAT_ADC1_CH4    			GPIO_Pin_4
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOA

#define RHEOSTAT_ADC              ADC1
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_CHANNEL4     ADC_Channel_4

// PF9 �ӵ�λ��
#define RHEOSTAT_ADC_DR_ADDR      ((u32)ADC1+0x4c)

#define RHEOSTAT_ADC_GPIO_PORT    GPIOA
#define RHEOSTAT_ADC1_CH5    			GPIO_Pin_5
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOA

#define RHEOSTAT_ADC              ADC1
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_CHANNEL5     ADC_Channel_5

// PF9 �ӵ�λ��
#define RHEOSTAT_ADC_DR_ADDR      ((u32)ADC1+0x4c)

#define RHEOSTAT_ADC_GPIO_PORT    GPIOA
#define RHEOSTAT_ADC1_CH6    			GPIO_Pin_6
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOA

#define RHEOSTAT_ADC              ADC1
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_CHANNEL6     ADC_Channel_6

// DMA2 ������0 ͨ��2
#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0

 

/************************************************************/
#define ON  0
#define OFF 1
/* ���κ꣬��������������һ��ʹ�� */
#define SWITCH_CHARG(a)	if (a)	\
					GPIO_SetBits(SWITCH_CHARG_GPIO_PORT,SWITCH_CHARG_PIN);\
					else		\
					GPIO_ResetBits(SWITCH_CHARG_GPIO_PORT,SWITCH_CHARG_PIN)

#define RELAY1(a)	if (a)	\
					GPIO_SetBits(RELAY1_GPIO_PORT,RELAY1_PIN);\
					else		\
					GPIO_ResetBits(RELAY1_GPIO_PORT,RELAY1_PIN)
					
#define RDATA1 GPIO_ReadInputDataBit(REMOTE_D1_GPIO_PORT,REMOTE_D1_PIN)	 //�������������
#define RDPWM1 GPIO_ReadInputDataBit(PWM1_GPIO_PORT,PWM1_PIN)	 //�������������
//����ң��ʶ����(ID),ÿ��ң�����ĸ�ֵ��������һ��,��Ҳ��һ����.
//����ѡ�õ�ң����ʶ����Ϊ0
#define REMOTE_ID 0      		   
extern QueueHandle_t xAUTO_CHANGE;
void change(void* pv);

#endif

