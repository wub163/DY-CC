#include "delay.h"
#include "sys.h"
////////////////////////////////////////////////////////////////////////////////// 	 
extern u32 mtime;
extern u32 lwip_localtime;

//�޸�˵��
//��
//void SysTick_Handler(void)
//{				   
//mtime++;
//// lwip_localtime++;
//
//}
			   
void delay_init(u8 SYSCLK)
{
	u32 reload;
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	    
	reload=1000*SYSCLK/8;		//ÿ���ӵļ������� ��λΪK	   
							//reloadΪ24λ�Ĵ���,���ֵ:16777216,��168M��,Լ��0.7989s����	
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD=reload; 	//ÿ1/OS_TICKS_PER_SEC���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK

}								    

