#include "CAN_ID.h"
#include "can.h"
#include "string.h"
#include "upload.h"
QueueHandle_t xCANID=NULL;

  xCANID_D CANID;

void ID_task(void *pv)
{

  u8 IDSAVE=0;          //IDSAVE ������0ʱ��Ϊд�������С�
  u8 comp=0,ID[8]={0,};
  CanRxMsg RxMess;
xCANID= xQueueCreate( 3, sizeof( CanRxMsg ) );


//CAN1_Mode_Init(1000); //�Ƿ񵥶�����can�˿�


while(1){
  if(!JudgeEncrypt())while(1);

if(xQueueReceive(xCANID,&RxMess,50/portTICK_PERIOD_MS)==errQUEUE_EMPTY) //���������50ms�������
{
if(!broadcast) CAN1_Send_Msg(65,CANID.WID,1);//������״̬ʱ�Ķ������������  �㲥��ʽ��������

  if(CANID.IDERR!=0XFF)CANID.IDERR++;

}//����������
else
{CANID.IDERR=0;
CANID.IDRSSI=RxMess.Data[2]+RxMess.Data[3];
CANID.IDWRT=RxMess.Data[1];
if(IDSAVE){if(CANID.IDWRT==0)CANID.IDCOT=0;IDSAVE=0;}
  if((RxMess.Data[1]==0)&&(RxMess.Data[2])==1){ //��ȡ����Ч��
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
 
  if((RxMess.Data[0]==2)||(comp==2))//���������������ǵڶ���������ʱ���������㣬����ʱ���Ͷ������
  {
  u8 num=1;
  comp=0;
vTaskDelay( 5/portTICK_PERIOD_MS);//�������20ms���Ͷ������
 //   xQueueGenericReset(xCANID,pdTRUE); //�����������в������㡣
  if(CANID.IDCOT){num=8;IDSAVE=1;
if( broadcast)  CAN1_Send_Msg(65,CANID.WID,num);
  }
if(!broadcast)  CAN1_Send_Msg(65,CANID.WID,num);
  } 
 }//
    
  }
}