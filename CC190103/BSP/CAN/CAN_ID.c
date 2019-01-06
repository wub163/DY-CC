#include "CAN_ID.h"
#include "can.h"
#include "string.h"
#include "upload.h"
QueueHandle_t xCANID=NULL;

  xCANID_D CANID;

void ID_task(void *pv)
{

  u8 IDSAVE=0;          //IDSAVE 不等于0时，为写卡过程中。
  u8 comp=0,ID[8]={0,};
  CanRxMsg RxMess;
xCANID= xQueueCreate( 3, sizeof( CanRxMsg ) );


//CAN1_Mode_Init(1000); //是否单独配置can端口


while(1){
  if(!JudgeEncrypt())while(1);

if(xQueueReceive(xCANID,&RxMess,50/portTICK_PERIOD_MS)==errQUEUE_EMPTY) //非正常情况50ms补发命令。
{
if(!broadcast) CAN1_Send_Msg(65,CANID.WID,1);//非正常状态时的读卡器命令补发。  广播方式，不发送

  if(CANID.IDERR!=0XFF)CANID.IDERR++;

}//读卡器错误
else
{CANID.IDERR=0;
CANID.IDRSSI=RxMess.Data[2]+RxMess.Data[3];
CANID.IDWRT=RxMess.Data[1];
if(IDSAVE){if(CANID.IDWRT==0)CANID.IDCOT=0;IDSAVE=0;}
  if((RxMess.Data[1]==0)&&(RxMess.Data[2])==1){ //读取到有效卡
  if(RxMess.Data[0]==1)
  {
    
    ID[0]=RxMess.Data[4];
    ID[1]=RxMess.Data[5];
    ID[2]=RxMess.Data[6];
    ID[3]=RxMess.Data[7];
    comp=1;
  }
 if(RxMess.Data[0]==2)
  {
    if(comp==1){
    ID[4]=RxMess.Data[4];
    ID[5]=RxMess.Data[5];
    ID[6]=RxMess.Data[6];
    ID[7]=RxMess.Data[7];
   memcpy(CANID.ID,ID,8);
   xSentData.rfid=*(u32*)ID;
    comp=2;}
  }
 }else comp =0;
 
  if((RxMess.Data[0]==2)||(comp==2))//读到完整卡，或是第二个反馈包时，队列清零，并延时发送读卡命令。
  {
  u8 num=1;
  comp=0;
vTaskDelay( 5/portTICK_PERIOD_MS);//正常情况20ms发送读卡命令。
 //   xQueueGenericReset(xCANID,pdTRUE); //防丢卡，队列不可清零。
  if(CANID.IDCOT){num=8;IDSAVE=1;
if( broadcast)  CAN1_Send_Msg(65,CANID.WID,num);
  }
if(!broadcast)  CAN1_Send_Msg(65,CANID.WID,num);
  } 
 }//
    
  }
}