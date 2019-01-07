/*========================================================= 
*    �ļ���:  	        yaokong.c 
*    ����: 	        ��ģң����������
*    �汾��:  	        1.1   
*    ��������:          2016-6-18 8:52:03  
*    IDE����:  	        IAR Embedded Workbench for ARM version 7.30.3
*    �ַ�����:  	GB2312   
*    ��	��:  	        ���  
*    ��Ȩ����:  	���������������������޹�˾
*    �汾�޸���ʷ:    ��XXX:Ϊ�����ߣ�
*       2016-4-1        ���:�����ļ�   
*  	2016-6-18       ���:��ֲstm32f429ƽ̨��freertos   
* ===========================================================  */     
#include "stm32f4xx.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bsp_lcd.h"

#define yk_dio PDin(6)

u8 mtime_count;
 QueueHandle_t systickfifo,ykfifo;
void yk_init(void)
{
EXTI_InitTypeDef intt;
NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);  //ʹ��GPIOEʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  //gpio��ʼ����
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode =   GPIO_Mode_IN ;  //��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

/*�ⲿ�ж�*/
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD,GPIO_PinSource6);
    intt.EXTI_Line=EXTI_Line6;
    intt.EXTI_LineCmd=ENABLE;
    intt.EXTI_Mode= EXTI_Mode_Interrupt;
    intt.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
  EXTI_Init(&intt);

 /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  systickfifo   = xQueueCreate( 20, sizeof(s32 ) );
  ykfifo        = xQueueCreate( 1, 8*sizeof( s8 ) );
  
  
}

void EXTI9_5_IRQHandler(void)
{
static s32 ddtime,systick;
u32 ulDummy;
s32 systime,mtime,systt;
ulDummy=taskENTER_CRITICAL_FROM_ISR();
if (yk_dio){systick=SysTick->VAL;ddtime=xTaskGetTickCount();}
  else {
systime=systick-SysTick->VAL;
mtime=(s32)(xTaskGetTickCount())-ddtime;
systt=mtime*(configCPU_CLOCK_HZ/configTICK_RATE_HZ)+systime;
xQueueSendFromISR(systickfifo,&systt,0);
  }
  EXTI_ClearITPendingBit(EXTI_Line6);
taskEXIT_CRITICAL_FROM_ISR(ulDummy);
}


void yk_ppm(void*  pa)
{
s32 tickdd ;
s8 a[10]={0};
u8 count;
yk_init();
while(1){
xQueueReceive(systickfifo,&tickdd,portMAX_DELAY);

if((count>19)||(tickdd>300000))//���������27��
{count=0;
xQueueSend(ykfifo,&a,0);vTaskDelay(15/portTICK_PERIOD_MS);//20��������20ms���ң�
}
else 
{
  tickdd/=920;
  tickdd-=215;
  a[count++]=(u16)tickdd;
}
}

}

////Ӧ���г̡�
//void yk_count(void* pa)
//{
// char dispBuff[100];
//s8 a[10];
//while(1)
//{
//
//xQueueReceive(ykfifo,&a,1000);
// 
//  sprintf(dispBuff,"1: %4d  2: %4d  3: %4d  4: %4d   5: %4d",a[0],a[1],a[2],a[3],a[4]);
// LCD_DisplayStringLine(LINE(14),0,(uint8_t* )dispBuff);
// sprintf(dispBuff,"6: %4d  7: %4d  8: %4d  9: %4d  10: %4d",a[5],a[6],a[7],a[8],a[9]);
// LCD_DisplayStringLine(LINE(15),0,(uint8_t* )dispBuff);
//vTaskDelay(100/portTICK_PERIOD_MS);
////uxTaskGetStackHighWaterMark( NULL );
//}
//
//}
