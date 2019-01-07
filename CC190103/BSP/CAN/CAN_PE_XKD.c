#include "CAN_PE_XKD.h"
#include "can.h"
#include "string.h"
QueueHandle_t xCANPE=NULL;
  xCANPE_D CANPE;
  
  u8 dcfx(void);   //��ط���

  
u8 recv[30];


void CANPE_task(void *pv)
{
u8 temp=0;
u8  DATA[4][6]={
  {0X5A,0X03,0XA1,0X01,0XF0,0X11},//������Ϣ��
  {0X5A,0X03,0XA1,0X02,0XF0,0X10},//ʵʱ��Ϣ��
  {0X5A,0X03,0XA1,0X03,0XF0,0X0F},//�����¶ȡ�
  {0X5A,0X03,0XA1,0X04,0XF0,0X0E},//�����ѹ��
  };//�����Ϣ��
  
  CanRxMsg RxMess;
xCANPE= xQueueCreate( 3, sizeof( CanRxMsg ) );


//CAN1_Mode_Init(1000); //�Ƿ񵥶�����can�˿�


while(1){
if(xQueueReceive(xCANPE,&RxMess,200/portTICK_PERIOD_MS)==errQUEUE_EMPTY)//200ms��һ�ε������
{
  if(temp==4){if(dcfx())CANPE.POWER_err=0;}
  if(CANPE.POWER_err  <0XF0)CANPE.POWER_err++;else {CANPE.pjwd=-11;ZDCD=0;}//��ض�ȡ����ʱ��ǿ�ƹرռ̵�����
    memset(recv,0x00,30);
    CAN1_Send_Msg(CANPEID,DATA[1],6);temp=0;
}//�� ȡ����
else
{   temp%=4;//��Խ�硣
memcpy(&recv[temp*8],&RxMess.Data[0],8);  
 temp++;}
}
}


u8 dcfx(void)   //��ط���
{
u8 temp=0;
static u8 dcerrcount=0;
for(int i = 0; i < 26; ++i)
{
  temp+=recv[i];
}
if((recv[1]==0x17)&(temp==0))
{
  
 memcpy((u8*)&CANPE,&(recv[2]),23) ;
 cha_state.Battery_Temp=CANPE.pjwd;
 cha_state.Battery_Voltage=CANPE.dqdy;
 cha_state.Battery_Current=-CANPE.dqdl;
ZDCD=cha_state.chargeswitch;    //�Զ����̵���������
dcerrcount=0; 
return 1;
} 
else if(dcerrcount++>50){dcerrcount=250;ZDCD=0;}return 0;
}