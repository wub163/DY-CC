#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stm32f4xx.h"
#include "bsp_lcd.h"
u32 sti;
u8 cpu_max,cpu_avg;
void vApplicationIdleHook(void)
{
sti++;

}

void vApplicationTickHook(void)
{
  static u8 a[101];
  u8 max=0,avg=0;
  int count=0;
a[100]=(18700-sti)/187;
if(a[100]>0x64)
a[100]=100;
  for(int i = 0; i <100; ++i)
  {
    a[i]=a[i+1];
    count+=a[i];
    if(a[i]!=0x64)max=max>a[i]?max:a[i];
  }
  avg=count/100;
  cpu_max=max;cpu_avg=avg;
sti=0;
}


void cpu (void* pa)
{
   	char dispBuff[100];
while(1)
{
vTaskDelay(500/portTICK_PERIOD_MS);
  
//  sprintf(dispBuff,"CPU_max: %d  CPU_avg: %d ",cpu_max,cpu_avg);
//  LCD_DisplayStringLine(LINE(1),400,(uint8_t* )dispBuff);
//  uxTaskGetStackHighWaterMark( NULL );
}
}


