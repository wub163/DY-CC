/*   
* ========================================================= 
*    文件名:  	dcabmoto.c 
*    描述: 	有刷电机带AB编码器驱动文件
*    版本号:  	1.1   
*    创建日期:  2016-12-22 15:52:03  
*    IDE环境:  	IAR FOR ARM 7.30.3.8062 
*    字符编码:  	UTF-8   
*    作	者:  	吴兵   
*    版权所有:  	哈尔滨工大机器人有限公司
*    版本修改历史:  （XXX:为操作者）
*       2016-12-22 吴兵:创建文件   
*  	2011-4-22 XXX:增加方法Sub、Add   
* ===========================================================  */     

//直流有刷AB编码电机驱动程序


#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "stm32f4xx.h"
#include "..\DCMOTO\dcabmoto.h"

/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
void moto0_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启LED相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (DCMOTO0_AB1_GPIO_CLK, ENABLE); 	
	RCC_AHB1PeriphClockCmd (DCMOTO0_AB2_GPIO_CLK, ENABLE); 	

	/* 定时器复用引脚 */
	GPIO_PinAFConfig(DCMOTO0_AB1_GPIO_PORT,DCMOTO0_AB1_PINSOURCE,DCMOTO0_AB_AF); 
	GPIO_PinAFConfig(DCMOTO0_AB2_GPIO_PORT,DCMOTO0_AB2_PINSOURCE,DCMOTO0_AB_AF); 
	
	
	/* 高级控制定时器PWM输入捕获引脚 */
	GPIO_InitStructure.GPIO_Pin = DCMOTO0_AB1_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(DCMOTO0_AB1_GPIO_PORT, &GPIO_InitStructure);
        
	GPIO_InitStructure.GPIO_Pin = DCMOTO0_AB2_PIN;	
	GPIO_Init(DCMOTO0_AB2_GPIO_PORT, &GPIO_InitStructure);
        
}
void moto1_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启LED相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (DCMOTO1_AB1_GPIO_CLK, ENABLE); 	
	RCC_AHB1PeriphClockCmd (DCMOTO1_AB2_GPIO_CLK, ENABLE); 	

	/* 定时器复用引脚 */
	GPIO_PinAFConfig(DCMOTO1_AB1_GPIO_PORT,DCMOTO1_AB1_PINSOURCE,DCMOTO1_AB_AF); 
	GPIO_PinAFConfig(DCMOTO1_AB2_GPIO_PORT,DCMOTO1_AB2_PINSOURCE,DCMOTO1_AB_AF); 
	
	
	/* 高级控制定时器PWM输入捕获引脚 */
	GPIO_InitStructure.GPIO_Pin = DCMOTO1_AB1_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(DCMOTO1_AB1_GPIO_PORT, &GPIO_InitStructure);
        
	GPIO_InitStructure.GPIO_Pin = DCMOTO1_AB2_PIN;	
	GPIO_Init(DCMOTO1_AB2_GPIO_PORT, &GPIO_InitStructure);
        
}
void moto2_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启LED相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (DCMOTO2_AB1_GPIO_CLK, ENABLE); 	
	RCC_AHB1PeriphClockCmd (DCMOTO2_AB2_GPIO_CLK, ENABLE); 	

	/* 定时器复用引脚 */
	GPIO_PinAFConfig(DCMOTO2_AB1_GPIO_PORT,DCMOTO2_AB1_PINSOURCE,DCMOTO2_AB_AF); 
	GPIO_PinAFConfig(DCMOTO2_AB2_GPIO_PORT,DCMOTO2_AB2_PINSOURCE,DCMOTO2_AB_AF); 
	
	
	/* 高级控制定时器PWM输入捕获引脚 */
	GPIO_InitStructure.GPIO_Pin = DCMOTO2_AB1_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(DCMOTO2_AB1_GPIO_PORT, &GPIO_InitStructure);
        
	GPIO_InitStructure.GPIO_Pin = DCMOTO2_AB2_PIN;	
	GPIO_Init(DCMOTO2_AB2_GPIO_PORT, &GPIO_InitStructure);
        
}
void moto3_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启LED相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (DCMOTO3_AB1_GPIO_CLK, ENABLE); 	
	RCC_AHB1PeriphClockCmd (DCMOTO3_AB2_GPIO_CLK, ENABLE); 	

	/* 定时器复用引脚 */
	GPIO_PinAFConfig(DCMOTO3_AB1_GPIO_PORT,DCMOTO3_AB1_PINSOURCE,DCMOTO3_AB_AF); 
	GPIO_PinAFConfig(DCMOTO3_AB2_GPIO_PORT,DCMOTO3_AB2_PINSOURCE,DCMOTO3_AB_AF); 
	
	
	/* 高级控制定时器PWM输入捕获引脚 */
	GPIO_InitStructure.GPIO_Pin = DCMOTO3_AB1_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(DCMOTO3_AB1_GPIO_PORT, &GPIO_InitStructure);
        
	GPIO_InitStructure.GPIO_Pin = DCMOTO3_AB2_PIN;	
	GPIO_Init(DCMOTO3_AB2_GPIO_PORT, &GPIO_InitStructure);
        
}

 void moto_AB_INIT(void)
{       
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

//    moto0_GPIO_Config(); 
//    moto1_GPIO_Config(); 
    moto2_GPIO_Config(); 
//    moto3_GPIO_Config(); 
       
//         TIM_EncoderInterfaceConfig(DCMOTO0_AB_TIME,TIM_EncoderMode_TI1,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
//         TIM_EncoderInterfaceConfig(DCMOTO1_AB_TIME,TIM_EncoderMode_TI1,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
         TIM_EncoderInterfaceConfig(DCMOTO2_AB_TIME,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
//         TIM_EncoderInterfaceConfig(DCMOTO3_AB_TIME,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
//        TIM_Cmd(DCMOTO0_AB_TIME,ENABLE);
//        TIM_Cmd(DCMOTO1_AB_TIME,ENABLE);
        TIM_Cmd(DCMOTO2_AB_TIME,ENABLE);
//        TIM_Cmd(DCMOTO3_AB_TIME,ENABLE);

}

s32 dcmotowz(u8 moto)
{
  moto=2;//指定电机2.
switch(moto)
{
case 00:
  return  TIM_GetCounter(DCMOTO0_AB_TIME);
case 01:
  return  TIM_GetCounter(DCMOTO1_AB_TIME);
case 02:
  return  (s32)((s16)TIM_GetCounter(DCMOTO2_AB_TIME));
case 03:
  return  (s32)((s16)TIM_GetCounter(DCMOTO3_AB_TIME));
default :
  return 00;
}
}


void dcmotoset(TIM_TypeDef* motot, s16 Counter)
{
  motot=DCMOTO2_AB_TIME;//指定电机2.
TIM_SetCounter(motot,(u32)Counter);
}


void highreset(void)
{
TIM_SetCounter(DCMOTO2_AB_TIME,(u32)0);
}

u16 highget (void)
{
return TIM_GetCounter(DCMOTO2_AB_TIME);
}

void dcmoto(void* pv)
{
moto_AB_INIT();


vTaskDelete(NULL);//杀掉进程

while(1)
{
vTaskDelay(50/portTICK_PERIOD_MS);//50ms刷新
}


}
