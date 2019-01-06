#include "CAN_MP3.h"
#include "can.h"
#include "string.h"


void mp3play (u8 mp3num, u8 MP3vol)
{

static u8 oldnum=0,oldvol=0;
  u8 data[8];
  
  if(cha_state.mp3mute==0)MP3vol=0;//音量上层控制。
  if (MP3vol>28) MP3vol=28;
  if((mp3num!=oldnum)||(oldvol!=MP3vol)){
  data[0]=0X01;//驱动器力矩控制
data[1]=0x51;
data[2]=mp3num;
data[3]=MP3vol;
data[4]=0x00;
data[5]=0x00;
data[6]=data[0]^data[1]^data[2]^data[3]^data[4]^data[5];
data[7]=0x02;
 if(CAN1_Send_Msg(0xf1,data,8)>3)return;//发送失败
// if(SYSST.mp3num<80)SYSST.mp3num=0;
 oldnum=mp3num;oldvol=MP3vol;
  }

}


void CAN_MP3_task(void *pv)
{
  


//CAN1_Mode_Init(1000); //是否单独配置can端口


while(1){
 
  if (cha_state.MP3_Vol>31)cha_state.MP3_Vol=initmp3vol;//如果音量值异常，则恢复音量初始。
 mp3play(cha_state.Mp3_num,cha_state.MP3_Vol);

  vTaskDelay( 100/portTICK_PERIOD_MS);
  
}
 

}