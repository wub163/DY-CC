#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "yaokong.h"
#include "../jiami/jm.h"
#include "bianmaqi.h"
#include "485.h"
#include "gpio.h" 
#include "spi_flash.h"
#include "NVIC.h"
#include "usart.h"
#include "upload.h"
#include "bsp_lcd.h"
#include "netconf.h" 
#include "cpuinfo.h"
#include "LED2811.h"
#include "control.h"
#include "..\SPI_DAC\spi_dac.h"
#include "..\remote.h"
#include "../adc/adc.h"
#include "..\DCMOTO\dcabmoto.h"
#include "can.h"
#include "CAN_ID.h"
#include "CAN_MAGS.h"
#include "CCST.h"
#include "../mscom/ms_com.h"



void Device_Init(void);
void led1 (void* pa);
void led2 (void* pa);
void speed (void* pa);
//void turnto (void* pa);
 TaskHandle_t xAUTOCHANGEHandle = NULL;
/****************************************************************************
* 名称:void Device_Init( void )
* 功能:initiate stm32's peripheral devices
* 入口参数:无
* 出口参数:无
* 说明:
* 调用方法:无 
****************************************************************************/
void Device_Init(void)
{
WriteEncrypt();
  GPIOInit();             //初始化GPIO 
  Init_NVIC();            //初始化中断优先级
// sFLASH_Init();
 LCD_Init();
 LCD_LayerInit();
  InitCOM();
//  CAN1_Mode_Init(500);
//  CAN1_Mode_Init(500);
}





HeapRegion_t xHeapRegions[] =
{
//  	{ ( uint8_t * ) 0x10002000UL, 0xE000 }, // CCM
  	{ ( uint8_t * ) 0x20000000UL, 0x30000 }, // SRAM
 	{ ( uint8_t * ) 0xD0000000UL, 0x800000 }, // SDRAM
  	{ NULL, 0 }                // Terminates the array.
};



void led1 (void* pa)
{
 // char dispBuff[100];
u8 a=0;


while(1){
 vTaskDelay(200/portTICK_PERIOD_MS);
 switch((a++)%3)
 {
 case 0:LEDR=1; LEDY=1; LEDB=1;BEEP=0;
   break;
 case 1:LEDR=1; LEDY=0; LEDB=1;BEEP=0;
//        OUT0=1;OUT1=1;OUT2=1;OUT3=1;OUT4=1;OUT5=1;OUT6=1;OUT7=1;
 break;
 case 2:LEDR=1; LEDY=0;LEDB=1;BEEP=0;
 //       OUT0=0;OUT1=0;OUT2=0;OUT3=0;OUT4=0;OUT5=0;OUT6=0;OUT7=0;
break;
 }
//  sprintf(dispBuff,"MEM-FREE:%8X ",xPortGetFreeHeapSize());
// LCD_DisplayStringLine(LINE(2),420,(uint8_t* )dispBuff);
        }

}


void mprog(void* pa)
{


  
//  lwip_comm_init();                   //网络支持
xTaskCreate( led1,      "LED1",         150,      ( void * ) NULL,        2,      NULL );//86
xTaskCreate( cpu,       "cpu",          100,     ( void * ) NULL,        1,      NULL );//86
//xTaskCreate( yk_ppm,  "yk_ppm",       100,      ( void * ) NULL,        3,      NULL );
//xTaskCreate( DAC8,      "DAC8",         100,     ( void * ) NULL,        4,      NULL );//
//xTaskCreate( ADC8,      "ADC8",         120,     ( void * ) NULL,        4,      NULL );//
//xTaskCreate( dcmoto,    "dcmoto",       120,     ( void * ) NULL,        4,      NULL );//
//xTaskCreate( control,  "control",       500,     ( void * ) NULL,        4,      NULL );//
//xTaskCreate( MAGS_task,  "MAGS_task",   120,     ( void * ) NULL,        5,      NULL );//
//xTaskCreate( ID_task,   "ID_task",      120,     ( void * ) NULL,        5,      NULL );//
//xTaskCreate( CCST_task,  "CCST_task",   120,     ( void * ) NULL,        3,      NULL );//
//xTaskCreate( moto_task,  "moto_task",   120,     ( void * ) NULL,        3,      NULL );//
xTaskCreate( mscomtask,  "mscomtask",   120,     ( void * ) NULL,        2,      NULL );//


//uxTaskGetStackHighWaterMark( NULL );  //剩余92。
vTaskDelete(NULL);
while(1);
}



int main(void)
{
//for(int i = 0; i < 65555; ++i)
//{
//  ;
//}

   Device_Init();                          //硬件初始化
   vPortDefineHeapRegions(xHeapRegions);
   LCD_DisplayStringLine(LINE(1),100,(uint8_t* )"Laser Car");
//   LEDR=1; LEDY=1; LEDB=1;
//   upload_init();//上传初始化
  
 xTaskCreate( mprog,      "mprog",        200,    ( void * ) NULL,        1,      NULL );
if(JudgeEncrypt())
 vTaskStartScheduler();        
        while(1);

}






