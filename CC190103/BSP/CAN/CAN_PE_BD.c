#include "CAN_PE.h"
#include "can.h"
#include "string.h"
QueueHandle_t xCANPE=NULL;
  xCANPE_D CANPE;

void CANPE_task(void *pv)
{
u8 temp=0;
u8  DATA[5][8]={
  {0X16,0XBB,0,0,0,0,0,0X7E},//电池信息。
  {0X16,0XC1,0,0,0,0,0,0X7E},//1-4单体信息
  {0X16,0XC2,0,0,0,0,0,0X7E},//5-6单体信息
  {0X16,0XC3,0,0,0,0,0,0X7E},//7-11单体信息
  {0X16,0XC4,0,0,0,0,0,0X7E}};//12-15单体信息
  
  CanRxMsg RxMess;
xCANPE= xQueueCreate( 3, sizeof( CanRxMsg ) );


//CAN1_Mode_Init(1000); //是否单独配置can端口


while(1){
if(xQueueReceive(xCANPE,&RxMess,100/portTICK_PERIOD_MS)==errQUEUE_EMPTY)
{
  if(CANPE.DRVERR<0XF0)CANPE.DRVERR++;
}//读卡器错误
else
{
  CANPE.DRVERR=0;
  switch(temp)
  {
  case 0:
  CANPE.DY[0]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DL   =RxMess.Data[2]<<8|RxMess.Data[3];
  CANPE.cap  =RxMess.Data[4];
  CANPE.temp =RxMess.Data[5];
  CANPE.stat =RxMess.Data[6];
  CANPE.num  =RxMess.Data[7];
  if(CANPE.stat)CANPE.DL=-CANPE.DL;
    break;
  case 1:
  CANPE.DY[1]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[2]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[3]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[4]=RxMess.Data[0]<<8|RxMess.Data[1];
    break;
  case 2:
  CANPE.DY[5]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[6]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[7]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[8]=RxMess.Data[0]<<8|RxMess.Data[1];
    break;
  case 3:
  CANPE.DY[9]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[10]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[11]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[12]=RxMess.Data[0]<<8|RxMess.Data[1];
    break;
  case 4:
  CANPE.DY[13]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[14]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[15]=RxMess.Data[0]<<8|RxMess.Data[1];
  CANPE.DY[16]=RxMess.Data[0]<<8|RxMess.Data[1];
    break;
    
  }

  vTaskDelay( 100/portTICK_PERIOD_MS);
  
  } 
temp++;
temp=temp%5;
  CAN2_Send_Msg(CANPEID,DATA[temp],8);
 }

}