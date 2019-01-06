#include "delay.h"
#include "sys.h"
////////////////////////////////////////////////////////////////////////////////// 	 
extern u32 mtime;
extern u32 lwip_localtime;

//修改说明
//无
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
	    
	reload=1000*SYSCLK/8;		//每秒钟的计数次数 单位为K	   
							//reload为24位寄存器,最大值:16777216,在168M下,约合0.7989s左右	
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 	//每1/OS_TICKS_PER_SEC秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK

}								    

