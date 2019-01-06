#include "NVIC.h"

void Init_NVIC(void)
{
// 	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组2，两个四级（freertos系统可用2--4级）？？？
		
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
	
		
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  //串口屏  
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;		
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
//	NVIC_Init(&NVIC_InitStructure);	 
  
  
//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;   //表情+超声波
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		   
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
//        NVIC_Init(&NVIC_InitStructure);
//	
//        NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;;   //读卡器
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
//	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;   //485驱动
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		   
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
//	NVIC_Init(&NVIC_InitStructure);	
		

//	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;   //系统心跳。
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;		   
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
//	NVIC_Init(&NVIC_InitStructure);	
		


}

