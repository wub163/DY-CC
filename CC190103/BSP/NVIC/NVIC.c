#include "NVIC.h"

void Init_NVIC(void)
{
// 	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����2�������ļ���freertosϵͳ����2--4����������
		
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;   //485
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
//        NVIC_Init(&NVIC_InitStructure);
        
//        
//	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;//AGV
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
		
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  //������  
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;		
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
//	NVIC_Init(&NVIC_InitStructure);	 
  
  
//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;   //����+������
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		   
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
//        NVIC_Init(&NVIC_InitStructure);
//	
//        NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;;   //������
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;		   
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
//	NVIC_Init(&NVIC_InitStructure);	
//	
//	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;    //MP3
//       	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;		   
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
//	NVIC_Init(&NVIC_InitStructure);	
//	
//	
//	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;   //485����
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		   
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
//	NVIC_Init(&NVIC_InitStructure);	
		

//	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;   //ϵͳ������
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;		   
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
//	NVIC_Init(&NVIC_InitStructure);	
		


}

