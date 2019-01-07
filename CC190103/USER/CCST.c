#include "control.h"
#include "can.h"
#include "CCST.h"
#include "string.h"
#include "stdlib.h"
#include "..\DCMOTO\dcabmoto.h"
#include "gpio.h" 
#include "../SPI_DAC/spi_dac.h"
QueueHandle_t xCCST=NULL;

void highset(u16 high)
{
dcmotoset(0,(u16)(19.9*high));

}


void CCST_task(void *pv)        // ������������ȡ
{
  u8 count=0,temp;
  u8 data[8];
  CanRxMsg RxMess;
xCCST= xQueueCreate( 5, sizeof( CanRxMsg ) );

while(1){  
  if(!JudgeEncrypt())while(1);

  
  if(xQueueReceive(xCCST,&RxMess,20/portTICK_PERIOD_MS)==errQUEUE_EMPTY)

//xQueueReceive(xCCST,&RxMess,portMAX_DELAY);
  {
  if(SYSST.err++>200)SYSST.err=200;
  }
  else{
    
switch(RxMess.StdId & 0x793){
              case 0x181://���ٺ�״̬����
          SYSST.st_speed=*((s16*)&RxMess.Data[0]);
          SYSST.st_meterhz+=SYSST.st_speed;
          SYSST.st_meter=-SYSST.st_meterhz/177;//��������ת��
          temp=RxMess.Data[4];
          SYSST.st_stat=temp>>4&0x01;
          temp=RxMess.Data[5];
          SYSST.st_conok=temp>>6&0x01;
          SYSST.st_tbopen=temp>>4&0x01;
          SYSST.st_man=temp>>7&0x01;
          SYSST.st_ryk=temp>>3&0x01|temp>>2&0x01;
          SYSST.st_lyk=temp&0x01;
          SYSST.err=0;
          break;
              case 0x183://ת������
          SYSST.st_turn=*((s16*)&RxMess.Data[0]);
          SYSST.const_turn=*((s16*)&RxMess.Data[3]);
          SYSST.st_high=(s16)(((float) highget())/19.9);
          break;
case 0x791:
  if(RxMess.Data[02]==0x84)        
  SYSST.st_mototp=*((s16*)&RxMess.Data[3]);//����¶�
  if(RxMess.Data[02]==0x65)        
  SYSST.st_dl=*((s16*)&RxMess.Data[3]);//�������
  break;
case 0x793:
  switch (RxMess.Data[02])
      { case 0x18: SYSST.st_dy=*((s16*)&RxMess.Data[3]);break;//��ص�ѹ
        case 0x43: SYSST.st_dl=*((s16*)&RxMess.Data[3]);break;//�������
        case 0x80: SYSST.st_contp=*((s16*)&RxMess.Data[3]);break;//�������¶�
      default:;
      }
  break;
default:;
  
} 
count++;
if(count%10==0)
{
memset (data,0xff,8);
data[0]=0x02;data[1]=0x21;
data[2]=0x18;//��ص�ѹ
CAN1_Send_Msg(0x783,data,8);
//data[2]=0x43;//�������
//CAN1_Send_Msg(0x783,data,8);
data[2]=0x65;//�������
CAN1_Send_Msg(0x781,data,8);
}
if(count%31==0)
{
memset (data,0xff,8);
data[0]=0x02;data[1]=0x21;
data[2]=0x80;//�������¶�
CAN1_Send_Msg(0x783,data,8);
data[2]=0x84;//����¶�
CAN1_Send_Msg(0x781,data,8);
}}
}}


void moto_task(void *pv)//go:+-10  sturn:+-90  high:0--3000  con:0 high Ϊ�����ٶ�+-10��1��highΪ�߶�ֵ0-3000
{
  s16 go=0;s16 turn=0; 
  s16 gocount=0;
s16 lastturn=0;
  s16 highsp=0,highset=0;//�������Ƶ��ٶȺ�λ��ֵ��
  s16 highsphis=0;       //��ʷ�ٶȡ�
  u8 count=0;//����������ʱ
  s32 temp=0;//�߶�ֵ�ݴ档
u8 be=0;//��������ʶ
    TickType_t xLastWakeTime;
xLastWakeTime = xTaskGetTickCount ();
 while(1)
{ 
  vTaskDelayUntil( &xLastWakeTime, 20/portTICK_PERIOD_MS);
go=SYSST.con_speed;be=0;
if(abs(go)<4){if(abs(SYSST.st_speed)<100)gocount++;else gocount=0;
if(gocount>20)go++;if(gocount>40)go++;}//�泵����ʱ�����������������޷��𲽡�


if(go>10)go=10;if(go<-10)go=-10;
if(!CCBZ){if(go>0){go=0;GO=1;be=1;}}

if(SYSST.st_tbopen==0)
{if((STOP)||(BIZ2)){if(go<0){go=0;GO=1;be=1;CCON=1;}}
if((go<-4)&&(BIZ1)){go=-4;be=2;}}if(be)BEEP=0;else BEEP=1;
  if(go!=0){go=go*80;GO=0;if(go>0)go+=200;else go-=300;}
  
  DAC_OUT[0]=speedmid+go;// ����
  
turn=SYSST.con_turn;
  if(turn>90)turn=90;if(turn<-90)turn=-90;
//  turn+=2;//��λƫ�ơ�
turn=turn*152/10;
if((lastturn-turn)>100)lastturn-=50;
else {if((lastturn-turn)<-100)lastturn+=50;else lastturn=turn;}
DAC_OUT[1]=lrmid+lastturn;
DAC_OUT[2]=lrmid-lastturn;//ת��

if(SYSST.con_highsp>10)highsp=10;else highsp=SYSST.con_highsp;highset=SYSST.con_highset;
if((highset<highmax)&&(highset>=0)&&(SYSST.highinit==0xaa)&&(SYSST.st_mod==1))
//�߶ȺϷ����߶��ѱ궨�������Զ�״̬�£�����߶��Զ�����״̬��
{
if(SYSST.con_highset<highmin)SYSST.con_highset=highmin;
if((highset-SYSST.st_high)>0)highsp=highfast;else highsp=-highfast;


if(abs(highset-SYSST.st_high)<150){highsp/=2;}//�ӽ�����

if(abs(highset-SYSST.st_high)<10){highsp=0;SYSST.con_highwork=0;}//��λֹͣ��
    else SYSST.con_highwork=1;//��ݵ����С�
highsp*=100;  
if((highsp-highsphis)>100)highsphis+=10;
else {if((highsp-highsphis)<-100)highsphis-=10;
else highsphis=highsp;}

if(abs(highsphis)>400){//����������
    if(count++==10)temp=SYSST.st_high;
    if(count==60){if(abs(temp-SYSST.st_high)<10)SYSST.highinit=0XAF;count=0;}//һ����λ�Ƶ���10mm���򱨴�

}else count=0;

}
else {highsp*=100;highsphis=highsp;}//�ֶ����ã��ޱ�����

DAC_OUT[3]=udmid-(highsphis);
DAC_OUT[4]=udmid+(highsphis);//����


}  
}  
  
  
  


