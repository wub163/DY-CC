/*   
* ========================================================= 
*    �ļ���:  	dcabmoto.c 
*    ����: 	��ˢ�����AB�����������ļ�
*    �汾��:  	1.1   
*    ��������:  2016-12-22 15:52:03  
*    IDE����:  	IAR FOR ARM 7.30.3.8062 
*    �ַ�����:  	UTF-8   
*    ��	��:  	���   
*    ��Ȩ����:  	������������������޹�˾
*    �汾�޸���ʷ:  ��XXX:Ϊ�����ߣ�
*       2016-12-22 ���:�����ļ�   
*  	2011-4-22 XXX:���ӷ���Sub��Add   
* ===========================================================  */     

//ֱ����ˢAB��������������


#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "stm32f4xx.h"
#include "..\DCMOTO\dcabmoto.h"

/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
void moto0_GPIO_Config(void) 
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (DCMOTO0_AB1_GPIO_CLK, ENABLE); 	
	RCC_AHB1PeriphClockCmd (DCMOTO0_AB2_GPIO_CLK, ENABLE); 	

	/* ��ʱ���������� */
	GPIO_PinAFConfig(DCMOTO0_AB1_GPIO_PORT,DCMOTO0_AB1_PINSOURCE,DCMOTO0_AB_AF); 
	GPIO_PinAFConfig(DCMOTO0_AB2_GPIO_PORT,DCMOTO0_AB2_PINSOURCE,DCMOTO0_AB_AF); 
	
	
	/* �߼����ƶ�ʱ��PWM���벶������ */
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
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (DCMOTO1_AB1_GPIO_CLK, ENABLE); 	
	RCC_AHB1PeriphClockCmd (DCMOTO1_AB2_GPIO_CLK, ENABLE); 	

	/* ��ʱ���������� */
	GPIO_PinAFConfig(DCMOTO1_AB1_GPIO_PORT,DCMOTO1_AB1_PINSOURCE,DCMOTO1_AB_AF); 
	GPIO_PinAFConfig(DCMOTO1_AB2_GPIO_PORT,DCMOTO1_AB2_PINSOURCE,DCMOTO1_AB_AF); 
	
	
	/* �߼����ƶ�ʱ��PWM���벶������ */
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
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (DCMOTO2_AB1_GPIO_CLK, ENABLE); 	
	RCC_AHB1PeriphClockCmd (DCMOTO2_AB2_GPIO_CLK, ENABLE); 	

	/* ��ʱ���������� */
	GPIO_PinAFConfig(DCMOTO2_AB1_GPIO_PORT,DCMOTO2_AB1_PINSOURCE,DCMOTO2_AB_AF); 
	GPIO_PinAFConfig(DCMOTO2_AB2_GPIO_PORT,DCMOTO2_AB2_PINSOURCE,DCMOTO2_AB_AF); 
	
	
	/* �߼����ƶ�ʱ��PWM���벶������ */
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
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (DCMOTO3_AB1_GPIO_CLK, ENABLE); 	
	RCC_AHB1PeriphClockCmd (DCMOTO3_AB2_GPIO_CLK, ENABLE); 	

	/* ��ʱ���������� */
	GPIO_PinAFConfig(DCMOTO3_AB1_GPIO_PORT,DCMOTO3_AB1_PINSOURCE,DCMOTO3_AB_AF); 
	GPIO_PinAFConfig(DCMOTO3_AB2_GPIO_PORT,DCMOTO3_AB2_PINSOURCE,DCMOTO3_AB_AF); 
	
	
	/* �߼����ƶ�ʱ��PWM���벶������ */
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
  moto=2;//ָ�����2.
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
  motot=DCMOTO2_AB_TIME;//ָ�����2.
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


vTaskDelete(NULL);//ɱ������

while(1)
{
vTaskDelay(50/portTICK_PERIOD_MS);//50msˢ��
}


}
