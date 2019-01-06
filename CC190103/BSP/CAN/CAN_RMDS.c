/*****************************************8888
单驱动器格式

**********************************************/
#include "CAN_RMDS.h"
#include "can.h"
#include "string.h"
#include "gpio.h"
#include "stdlib.h"
#include "main.h"
void CANBus_Messagehandling(CanRxMsg* RxMessage);//求取反馈速度平均值

static u8 data[8];
vu8 drvinit;
u8 splock=0;//异常停车标志
QueueHandle_t xCANRMDS=NULL;



xCANRMDS_D CANDD;    //驱动器参数



//===================
//驱动器初始化配置为电流速度模式：控制数据为：C1C2V3V4
//===================
 void RMDS_init(void)
 {
//CAN1_Mode_Init(1000); //是否单独配置can端口
   LEDY=0;//黄灯亮，驱动器复位。
drvinit=1;
   memset(data,0x55,8);
    CAN1_Send_Msg(drvid0|resetmod,data,8); //发送can信息。复位
    vTaskDelay(500/portTICK_PERIOD_MS);

    data[0]=pvmod-1;    
    CAN1_Send_Msg(drvid0|modmod,data,8); //发送can信息。模式配置
    vTaskDelay(500/portTICK_PERIOD_MS);

    data[0]=set1;
    data[1]=set2;
    CAN1_Send_Msg(drvid1|setmod,data,8); //发送can信息。反馈参数配置，不可广播
//    CAN1_Send_Msg(drvid2|setmod,data,8); //发送can信息。反馈参数配置，不可广播
    memset(&CANDD,0x00,sizeof(CANDD));
    CANDD.DRVERR=0;                            //12/28 行走驱动器经常性报警，
//    CANDD[1].DRVERR=0;

CANDD.MotoFlagForward=MotoFlagForward0;
drvinit=0;
LEDY=1;
 }

//=======================
//驱动器参数读取
//=======================
 void RMDS_int(void *pv)
 {
     CanRxMsg RxMess;
     while(1){     
 if(xQueueReceive(xCANRMDS,&RxMess,100/portTICK_PERIOD_MS)==errQUEUE_EMPTY)
{
if(CANDD.DRVERR++>250)CANDD.DRVERR=250;
  if(CANDD.DRVERR>=40)
  { RMDS_init();//3秒断线，驱动器复位 
//  CANDD.DRVERR=20;
  }
}//读卡器错误
else
{
CANDD.DRVERR=0;

  
  if(RxMess.StdId ==0x11b)
{
  CANDD.scurrent=((RxMess.Data[0]<<8)|RxMess.Data[1]);
  CANDD.sspeed=((RxMess.Data[2]<<8)|RxMess.Data[3]);
  CANDD.position=((RxMess.Data[4]<<24)|(RxMess.Data[5]<<16)|(RxMess.Data[6]<<8)|RxMess.Data[7]);
  CANDD.posmm=CANDD.position * WheelPeri /(GearScale * MotorEncoder)*3;
CANBus_Messagehandling(&RxMess);
}
    }
     }}


 
 
 
 
void moto_go(void)
{
  memset(data,0x55,8);

data[0]=(u8)((CANDD.cpwm>>8)&0xff);
data[1]=(u8)((CANDD.cpwm)&0xff);
data[2]=(u8)(((CANDD.cspeed*CANDD.MotoFlagForward)>>8)&0xff);
data[3]=(u8)((CANDD.cspeed*CANDD.MotoFlagForward)&0xff);
  CAN1_Send_Msg(drvid1|pvmod,data,8); 

}

void moto_control(void)
{
  static u8 lscbcount=0;//拉绳触边累计滤波。
  static s16 oldspeed=0;
s16 speed=0,cpwm=0,mp3vol=0;
static u8 count,stoplock=0;//锁抱闸延时器,急停锁。
static s8 baoz=0;

  
cpwm=cha_state.wheelst.CmdMotorPwm;   //温度过低时电机无动力输出电机动力
speed=cha_state.wheelst.CmdMotorvel;//电机速度

if(cha_state.mp3mute){cha_state.Mp3_num=initmp3num;}else cha_state.Mp3_num=0;//判定是否静音，播放静音文件
if(cha_state.mp3mute==0xff)cha_state.MP3_Vol=initmp3vol;  else cha_state.MP3_Vol=cha_state.mp3mute;//判定的默认音量，还是给定音量

if(cha_state.Battery_Temp<powerruntemp){cpwm=0;cha_state.Mp3_num=mp3_powertemplow;mp3vol=1;} //极限运行低温保护
if(cha_state.Battery_Voltage<powervol){cpwm=0;cha_state.Mp3_num=mp3_powerlow;mp3vol=1;}      //极限运行低压保护

if(stoplock==0x88){cpwm=0;cha_state.Mp3_num=mp3_stop;mp3vol=1;}           //急停保护提示
if(stoplock==0x55){cpwm=0;cha_state.Mp3_num=mp3_stop_lock;mp3vol=1;}      //防撞锁定提示

if((cha_state.Battery_Current<-10)&&(cha_state.Battery_Temp<powerchtemp))
{cha_state.Mp3_num=mp3_powertemplow;   mp3vol=1;}

if(mp3vol)cha_state.MP3_Vol=25;
//else {cha_state.MP3_Vol = initmp3vol;cha_state.Mp3_num=0;}//警示条件解除。



//oldspeed=speed;

if(abs(oldspeed-speed)>30){if(oldspeed<speed){if(oldspeed>0)oldspeed+=speedinc;else oldspeed+=speeddec; }
                                        else  {if(oldspeed<0)oldspeed-=speedinc;else oldspeed-=speeddec;}}
      else oldspeed=speed;
     

  if(STOP)
  {cpwm=0;baoz=0;stoplock=0x88;}else {if(stoplock==0x88)stoplock=0x11;}
  //急停按钮；开锁   0x55:上锁  0x88:半开   0x11:解除中   0x00：自由
if(!LSCB)lscbcount++;else lscbcount=0;
  if((lscbcount>3)&&(stoplock==0)&&(oldspeed))stoplock=0x55;                              //上锁
  if (stoplock==0x11){if(!LSCB){if(oldspeed){cpwm=5000;oldspeed=-30;}}else{stoplock=0;}}
  
  if(stoplock){if(stoplock!=0x11){cpwm=0;baoz=0;oldspeed=0;}}//有锁
    
    
  if(abs(oldspeed)>0){baoz=1;count=0;}//松抱闸
  if(oldspeed==0){if(count>50)baoz=0;else count++;}    //命令速度和当前速度都为零时，延时锁抱闸。

  if(!CCCX)oldspeed=0;//触须防撞
  splock=stoplock;
 BAOZ=baoz;

 if(AUTO1&&AUTO2){if(!((cha_state.manctrl>>manspeed)&0x01)){cpwm=0;if((cha_state.manctrl>>manbz)&0x01)BAOZ=1;}}
  
  //oldspeed=0;

if(oldspeed>800)oldspeed=800;//速度限速
if(oldspeed<-800)oldspeed=-800;//速度限速

CANDD.cpwm=cpwm;
CANDD.cspeed=oldspeed;


if(drvinit){
CANDD.cpwm=0;
CANDD.cspeed=0;
return;}//复位期间禁止操作。初始复位。  

moto_go();  
}

void RMDS_task(void *pv)
{
  
    vTaskDelay(2000/portTICK_PERIOD_MS);
  
RMDS_init();
  
xCANRMDS= xQueueCreate( 3, sizeof( CanRxMsg ) );

xTaskCreate( RMDS_int,    "RMDS_int",       200,      ( void * ) NULL,        3,      NULL );//启动中断服务进程。

while(1){
//  if(!JudgeEncrypt())while(1);

//RMDS_cal();
moto_control();
  
      vTaskDelay(20/portTICK_PERIOD_MS);
        BEEP=drvinit;//标记复位期间鸣响。
 }
}


//}  
//
//
//
void CANBus_Messagehandling(CanRxMsg* RxMessage)//求取反馈速度平均值
{
  int8_t i,s,n;
static u8 CAN1_data[10];
static s16 Sum_data[20];



		for(i = 0;i < 8;i ++)
		{
			CAN1_data[i] = 	RxMessage->Data[i];
		}
		for(i = 18; i >= 0; i--)
		{
				Sum_data[i+1] = Sum_data[i];
		}
		Sum_data[0] = (int16_t)((uint16_t)(CAN1_data[2]<<8)|(uint16_t)(CAN1_data[3]));
		if((Sum_data[0] == 0)||(splock))//收到驱动器为零或停车异常时，反馈清零。
		{
				cha_state.wheelst.FdbMotorVel = 0;
				memset(Sum_data,0,20);
		}
		else
		{
		s32	SumVel = 0;
				n = 0;
				for(s = 0; s < 20; s++)
				{
					if(abs(Sum_data[s]) > 5)
					{
						n++;
						SumVel += Sum_data[s];
					}
				}
				cha_state.wheelst.FdbMotorVel = (int16_t)(SumVel/n);
		}
}
