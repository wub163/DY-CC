/***********************************************************************
文件名称：main.C
功    能：
编写时间：
编 写 人：
注    意：
***********************************************************************/
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "CAN_PE_XKD.h"
#include "CAN_RMDS.h"
#include "CAN_MP3.h"

HeapRegion_t xHeapRegions[] =
{
//  	{ ( uint8_t * ) 0x10002000UL, 0xE000 }, // CCM
  	{ ( uint8_t * ) 0x20000000UL, 0x20000 }, // SRAM
// 	{ ( uint8_t * ) 0xD0000000UL, 0x800000 }, // SDRAM
  	{ NULL, 0 }                // Terminates the array.
};


void Task_StartUp(void *pdata)
{

vTaskDelay(1000/portTICK_PERIOD_MS);
xTaskCreate( cpu,      "cpu",         150,      ( void * ) NULL,        1,      NULL );

//xTaskCreate( TM1638task,        "TM1638task",           150,      ( void * ) NULL,        1,      NULL );

xTaskCreate( CAN_MP3_task,      "CAN_MP3_task",         150,      ( void * ) NULL,        1,      NULL );
xTaskCreate( RMDS_task,         "RMDS_task",            250,      ( void * ) NULL,        4,      NULL );
xTaskCreate( CANPE_task,        "CANPE_task",           150,      ( void * ) NULL,        2,      NULL );
xTaskCreate( Task_LED,          "Task_LED",             150,      ( void * ) NULL,        2,      NULL );
xTaskCreate( Task_RS232,        "Task_RS232",             250,      ( void * ) NULL,        4,      NULL );
xTaskCreate( Task_USART,        "Task_USART",             250,      ( void * ) NULL,        5,      NULL );
xTaskCreate( Task_MAIN,        "Task_MAIN",             150,      ( void * ) NULL,        2,      NULL );

xTaskCreate( Task_MOTOR,        "Task_MOTOR",             250,      ( void * ) NULL,        5,      NULL );


vTaskDelete(NULL);
while(1);
}






int main(void)
{
	//初始化板子
   GPIOInit();
   vPortDefineHeapRegions(xHeapRegions);
    BSP_Init();
        
	
	
	//创建启动任务
xTaskCreate( Task_StartUp,      "Task_StartUp",         150,      ( void * ) NULL,        2,      NULL );



 if(JudgeEncrypt())vTaskStartScheduler();
	while(1);
}
