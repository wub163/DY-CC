/***********************************************************************
文件名称：NVIC.C
功    能：
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
#include "main.h"

void NVIC_Configuration(void)
{
	
	
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	
	/* 2 bit for pre-emption priority, 2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;   //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
		
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;   //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
	
 	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;  
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;  
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
 	NVIC_Init(&NVIC_InitStructure); 
	
	 	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;  
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;  
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
 	NVIC_Init(&NVIC_InitStructure);
}
