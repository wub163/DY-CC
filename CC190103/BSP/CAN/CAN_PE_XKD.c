#include "CAN_PE_XKD.h"
#include "can.h"
#include "string.h"
QueueHandle_t xCANPE=NULL;
  xCANPE_D CANPE;
  
  u8 dcfx(void);   //电池分析

  
u8 recv[30];


void CANPE_task(void *pv)
{
u8 temp=0;
u8  DATA[4][6]={
  {0X5A,0X03,0XA1,0X01,0XF0,0X11},//基本信息。
  {0X5A,0X03,0XA1,0X02,0XF0,0X10},//实时信息。
  {0X5A,0X03,0XA1,0X03,0XF0,0X0F},//单体温度。
  {0X5A,0X03,0XA1,0X04,0XF0,0X0E},//单体电压。
  };//电池信息。
  
  CanRxMsg RxMess;
xCANPE= xQueueCreate( 3, sizeof( CanRxMsg ) );


//CAN1_Mode_Init(1000); //是否单独配置can端口


while(1){
if(xQueueReceive(xCANPE,&RxMess,200/portTICK_PERIOD_MS)==errQUEUE_EMPTY)//200ms读一次电池数据
{
  if(temp==4){if(dcfx())CANPE.POWER_err=0;}
  if(CANPE.POWER_err  <0XF0)CANPE.POWER_err++;else {CANPE.pjwd=-11;ZDCD=0;}//电池读取错误时，强制关闭继电器。
    memset(recv,0x00,30);
    CAN1_Send_Msg(CANPEID,DATA[1],6);temp=0;
}//读 取错误
else
{   temp%=4;//防越界。
memcpy(&recv[temp*8],&RxMess.Data[0],8);  
 temp++;}
}
}


u8 dcfx(void)   //电池分析
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
ZDCD=cha_state.chargeswitch;    //自动充电继电器开启。
dcerrcount=0; 
return 1;
} 
else if(dcerrcount++>50){dcerrcount=250;ZDCD=0;}return 0;
}