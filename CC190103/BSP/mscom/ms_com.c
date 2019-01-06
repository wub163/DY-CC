
//DMA 设置。

#include "../mscom/ms_com.h"
#include "semphr.h"
#include "string.h"
#include "usart.h"
#include "gpio.h" 
#include "upload.h"
#include "control.h"
  u8* DMATX_BUF=NULL,  * DMARX_BUF=NULL;
SemaphoreHandle_t sDMARXTC;



//串口号：USART3        USART2
//DMATX:  DMA1-C4-S3    DMA1-C4-S6
//DMARX:  DMA1-C4-S1    DMA1-C4-S5


void MSCOM_NVIC(void)
{
 	NVIC_InitTypeDef NVIC_InitStructure;
		
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;   //     地址接收
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
        NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel =  DMA1_Stream6_IRQn;   //发送DMA
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
        NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel =  DMA1_Stream5_IRQn;   //接收DMA
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
        NVIC_Init(&NVIC_InitStructure);
}






void DMA_INIT(void)  //DMA通讯
{
  
/* DMA1 Channel Config */
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);			   //复用功能使能

   
    DMA_DeInit(DMA1_Stream6);          //_TX
    DMA_DeInit(DMA1_Stream5);          //_RX

    DMA_StructInit(&DMA_InitStructure);//接收通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr  = (uint32_t)DMARX_BUF;
    DMA_InitStructure.DMA_DIR =  DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = DMARX_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;

    DMA_Init(DMA1_Stream5 , &DMA_InitStructure);



    DMA_StructInit(&DMA_InitStructure);//发送通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr  = (uint32_t)DMATX_BUF;
    DMA_InitStructure.DMA_DIR =  DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = DMATX_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;

    DMA_Init(DMA1_Stream6 , &DMA_InitStructure);


    USART_DMACmd(USART2 ,USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
//    USART_DMACmd(USART2 ,USART_DMAReq_Tx, ENABLE);
    DMA_ITConfig(DMA1_Stream6 , DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA1_Stream5 , DMA_IT_TC, ENABLE);
//DMA_Cmd(DMA1_Stream3 ,ENABLE); 
DMA_Cmd(DMA1_Stream5 ,ENABLE); //时刻开启，循环接收。
}

void MSCOM_INIT(void)
{
DMATX_BUF=pvPortMalloc(DMATX_SIZE);
DMARX_BUF=pvPortMalloc(DMARX_SIZE);
MSCOM_NVIC();
DMA_INIT();

}


void USART2_IRQHandler(void)
{
static u8 num=0;
  if(USART_GetITStatus(USART2,USART_IT_IDLE)){
  USART_ReceiveData(USART2);num=0;
//          DMA_Cmd(DMA1_Stream5, ENABLE);

//xSemaphoreGiveFromISR(sDMARXTC,0);
  }
  if(USART_GetITStatus(USART2,USART_IT_RXNE)){
  *(DMARX_BUF+(num++))=USART_ReceiveData(USART2);
  if(num>DMARX_SIZE)num=0;//防越界。
  }

}


void DMA1_Stream5_IRQHandler (void)             //接收通道
{
    if (DMA_GetFlagStatus(DMA1_Stream5,DMA_FLAG_TCIF5)) {
//        DMA_Cmd(DMA1_Stream5, DISABLE);
        DMA_ClearFlag(DMA1_Stream1,DMA_FLAG_TCIF5);
//
//        
//        DMA_SetCurrDataCounter(DMA1_Stream5, DMARX_SIZE);  // load number of bytes to be transferred
//          DMA_Cmd(DMA1_Stream5, ENABLE);
        xSemaphoreGiveFromISR(sDMARXTC,0);

}
}
void DMA1_Stream6_IRQHandler (void)             //发送通道
{
    if (DMA_GetFlagStatus(DMA1_Stream6,DMA_FLAG_TCIF6)) {
        DMA_Cmd(DMA1_Stream6, DISABLE);
        DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);

       DMA_SetCurrDataCounter(DMA1_Stream6, DMATX_SIZE);  // load number of bytes to be transferred
        
 
//        xSemaphoreGiveFromISR(sYK_DATAok,0);
        
}
}

        
        



void mscomtask (void* pv)
{
u8 comerr=0;
  TickType_t xLastWakeTime;
  sDMARXTC=xSemaphoreCreateBinary();

 
  
MSCOM_INIT();
	 xLastWakeTime = xTaskGetTickCount ();

  while(1)
  {
//===================状态反馈。
    vTaskDelayUntil( &xLastWakeTime, 50/portTICK_PERIOD_MS );
upload();
memcpy(DMATX_BUF,&xSentData, DMATX_SIZE);
DMA_Cmd(DMA1_Stream6 ,ENABLE);
  
  
//===================指令获取。  
if(!xSemaphoreTake( sDMARXTC,0/portTICK_PERIOD_MS)) {if((comerr++)>250)comerr=250;}
else {
  if(jiema(DMARX_SIZE,(RECV_D*)DMARX_BUF)){comerr =0;SYSST.dd_ok=1;}
else {vTaskDelay(10/portTICK_PERIOD_MS);DMA_INIT();}//同步dma
}//接收到包时的判误。
if(comerr>200)//通讯中断。
{
SYSST.dd_ok=0;//超过10秒未收到调度包。
}//2S内正常通讯。

//  uxTaskGetStackHighWaterMark( NULL ); 
  }

}