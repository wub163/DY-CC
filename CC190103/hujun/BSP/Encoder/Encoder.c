#include "Encoder.h"
//#include <includes.h>
#define Encoder_Val TIM8->CNT

#define ENCODER_TIMER   TIM8  // Encoder unit connected to TIM3
#define ENCODER_PPR           (u16)(400)   // number of pulses per revolution
#define SPEED_BUFFER_SIZE 8

#define COUNTER_RESET   (u16)0
#define ICx_FILTER      (u8) 6 // 6<-> 670nsec

#define TIMx_PRE_EMPTION_PRIORITY 1
#define TIMx_SUB_PRIORITY 0

#define SPEED_SAMPLING_FREQ (u16)(2000/(SPEED_SAMPLING_TIME+1))

void Encoder_Init(void)
{
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_ICInitTypeDef TIM_ICInitStructure;
	
		/*开启Encoder相关的GPIO外设时钟*/
		RCC_AHB1PeriphClockCmd ( ENCODER_A_GPIO_CLK | ENCODER_B_GPIO_CLK, ENABLE); 
		RCC_APB2PeriphClockCmd ( ENCODER_CLK, ENABLE);//TIM8时钟使能 
	
		/* 定时器复用引脚 */
		GPIO_PinAFConfig(ENCODER_A_GPIO_PORT,ENCODER_A_PINSOURCE,ENCODER_A_AF); 
		GPIO_PinAFConfig(ENCODER_B_GPIO_PORT,ENCODER_B_PINSOURCE,ENCODER_B_AF); 

	
		/* 高级控制定时器PWM输入捕获引脚 */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;   
    
    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    
    /*设置引脚为上拉模式*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		/*设置引脚速率为100MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	
	
		/*选择要控制的GPIO引脚*/															   
		GPIO_InitStructure.GPIO_Pin = ENCODER_A_PIN;	
		GPIO_Init(ENCODER_A_GPIO_PORT, &GPIO_InitStructure);	
    
    /*选择要控制的GPIO引脚*/															   
		GPIO_InitStructure.GPIO_Pin = ENCODER_B_PIN;	
    GPIO_Init(ENCODER_B_GPIO_PORT, &GPIO_InitStructure);	

//		/* Timer configuration in Encoder mode */
//		TIM_DeInit(ENCODER_TIMER);
//		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
//		
//		TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
//		TIM_TimeBaseStructure.TIM_Period = (4*ENCODER_PPR)-1;  
//		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
//		TIM_TimeBaseInit(ENCODER_TIMER, &TIM_TimeBaseStructure);
	 
		TIM_EncoderInterfaceConfig(ENCODER_TIMER, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
		TIM_ICStructInit(&TIM_ICInitStructure);
		TIM_ICInitStructure.TIM_ICFilter = ICx_FILTER;
		TIM_ICInit(ENCODER_TIMER, &TIM_ICInitStructure);

		//Reset counter
		ENCODER_TIMER->CNT = COUNTER_RESET;
		TIM_Cmd(ENCODER_TIMER, ENABLE);  

}
