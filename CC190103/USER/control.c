#include "FreeRTOS.h"
#include "control.h"
#include "yaokong.h"
#include "task.h"
#include "queue.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "upload.h"
#include "bsp_lcd.h"
#include "gpio.h" 
#include "../SPI_DAC/spi_dac.h"
#include "../adc/adc.h"
#include "../jiami/jm.h"
#include "CAN_MAGS.h"
#include "CAN_ID.h"
#include "CCST.h"
//const s16 speed[]={-750,750,700,650,600,550,500,450,0,300,400,500,600,700,800,900,1000};
//前进和后退的速度值。-8-0-+8 偏移8 速度参数表
xSYS_D SYSST={0,};
  s8 mags[11]={0,};
  s16 mag=0;
  u8 idcount=0;//队列指针。




//void  diaodu(void)
//{
////static u8 idcount=0;//队列指针。
//static s8  maxspeed=0;//当前最大允许速度。
//static u8 zxztj=0;//转向状态机。
//static s16 zxcount=0;//转向计数
//static s8 dlcount=0;//转向计数
//static s32 zxlen=0;//转向角度计数
//      s16 speed=0,turn=0;//控制变量。
//if(  xSentData.funcount!=xComData.funcount) //队列序列反馈
//{idcount=0;xSentData.funcount=xComData.funcount;}     //队列指针清零
////队列指令获取。  
//  if(idcount>3){SYSST.dd_com=0x00;SYSST.dd_val=0;}     //如果命令序列执行完毕，则原地等待。
//  else{ if(0==xComData.id[idcount].id){                        //判断立即卡。
//            SYSST.dd_com=xComData.id[idcount].com;
//            SYSST.dd_val=xComData.id[idcount].val;
//            xComData.id[idcount].id=0xffffffff;            //卡开始执行标记。
////            idcount++;                      //立即卡执行完成标记。
//           }
//  if(xSentData.rfid){//比对是否到敏感区
//  for(int i = 0; i < 4; ++i)
//        {
//
//          if(xSentData.rfid==xComData.id[i].id){
//            SYSST.dd_com=xComData.id[i].com;
//            SYSST.dd_val=xComData.id[i].val;
//            xComData.id[i].id=0xffffffff;            //卡开始执行标记。保证每张卡，只执行一次。
//            idcount=i; if(i<3)xSentData.rfidnext=xComData.id[i+1].id;xSentData.rfidnext=0xffffffff;
//          }      
//       }
//        }
//  }
//   xSentData.funid=idcount;          //队列指针反馈
// //队列指令运行。
//    static u16 yanshi=0;
//    static u8 yanshizhong=0;
//    static u8 zhixingxunchi=0;
//    static s16 jd=0,zx=0;
//    
//   if(SYSST.dd_com!=0xff)yanshizhong=0;                
//
//   switch (SYSST.dd_com)                          //命令执行中，为oxff，执行完，gDiaoDuData->ComID_id++
//  {
//  case 0:                               //停车延时命令    秒延时  //重发指令可以推出停机延时
//    if (SYSST.dd_val==0)yanshi=0xffff;else yanshi =SYSST.dd_val*50;
//    SYSST.dd_com=0xff;yanshizhong=1;zhixingxunchi=0;maxspeed=0;
//    break;
//    
//  case 1:                               //前向寻磁
//    zx=0;zhixingxunchi=1;yanshizhong=0;maxspeed=SYSST.dd_val;
//    idcount++; SYSST.dd_com=0xff;       //过程指令，指向下一条指令执行中。
//    break;
//    
//  case 2:                               //调度方式禁止后退寻磁。
//     zx=0;zhixingxunchi=1;yanshizhong=0;maxspeed=0;
//    idcount++; SYSST.dd_com=0xff;
//    break;
//
//
//  case 3://zx表示转向开始或转向结束，jd表示需要转动的角度，0为转到磁条上。
//    zx=1;zxztj=0;jd=SYSST.dd_val;
//    if(SYSST.dd_val)  {maxspeed=0;zxztj=1;}   //停止后转向
//    else {if (SYSST.con_speed==0)
//              {maxspeed=0;zxztj=20;} //转向入磁
//              else{zxztj=40;}  //寻磁转向
//          }SYSST.dd_com=0xff;
//     break;
//   
//
//  case 4://zx表示转向开始或转向结束，jd表示需要转动的角度，0为转到磁条上。
//    zx=-1;zxztj=0;jd=SYSST.dd_val;
//    if(SYSST.dd_val)  {maxspeed=0;zxztj=1;}   //停止后转向
//    else {if (SYSST.con_speed==0)
//              {maxspeed=0;zxztj=20;} //转向入磁
//              else{zxztj=40;}  //寻磁转向
//          }SYSST.dd_com=0xff;
//    break;
//  case 5:
//     break;
//  case 6:
//    break;
//  case 8:                                       //音量输入范围是1-10
//    zhixingxunchi=0;yanshizhong=0;zxztj=0;
//    if(abs(SYSST.st_turn)>5){speed=0;turn=0;zxlen=SYSST.st_meter;break;} 
//    speed=lrspeed;turn=0;
//    if((abs(zxlen-SYSST.st_meter)>(SYSST.dd_val*10))||(CAR_MAGS.OnMag>0)){speed=0;turn=0;idcount++; SYSST.dd_com=0xff;}
//    break;
//  case 9:                                       //音量输入范围是1-10
//    zhixingxunchi=0;yanshizhong=0;zxztj=0;
//    if(abs(SYSST.st_turn)>5){speed=0;turn=0;zxlen=SYSST.st_meter;break;} 
//    speed=-lrspeed;turn=0;
//    if((abs(zxlen-SYSST.st_meter)>(SYSST.dd_val*10))||(!ZBOK)){speed=0;turn=0;if(SYSST.st_speed==0){idcount++; SYSST.dd_com=0xff;}}
//    break;
//  case 0x0a:                                       //插齿升降。
//    SYSST.con_highset=SYSST.dd_val*10;if((SYSST.con_highwork==0)&&(abs(SYSST.st_high-SYSST.con_highset)<10))
//    {idcount++; SYSST.dd_com=0xff;}
//    break;
//  default:if(SYSST.dd_com!=0xff)if(idcount++>4)idcount=4; SYSST.dd_com=0xff;//0xff表示当前指令未执行完。无效指令则跳过。
//  }
//
//    
//  if(zhixingxunchi) {speed =maxspeed; turn = xc();if(zxcount--<0){zxcount=0;CAR_MAGS.zyfx=0;}}
//  
//  //延时服务。
//  if(yanshizhong){ if(yanshi){
////            if(0 == gInnerData->FinishGive && 1 == lastfinishgive)          ///完成键按下并释放时进入。
////              idcount++; lastfinishgive = gInnerData->FinishGive;
//    yanshi--;speed = 0;turn=xc();}
//  else {idcount++;yanshizhong=0;}}else yanshi=0;
//  
////转向服务。  
////jd,zx,
//  if(zxztj) {
//    switch (zxztj)
//  {
//  case 1://转向前停车                停车转向
//    if (fabs(SYSST.st_speed) > 50){speed =maxspeed; turn = xc();}  
//    else  {zxztj=2;dlcount=10;}
//    break;
//  case 2://舵轮转过固定角
//    if(zx>0)turn=lturn; if(zx<0)turn=rturn;if(dlcount--==0){dlcount=0;
//    if(abs(SYSST.st_turn-SYSST.const_turn)<10)
//    {zxlen=SYSST.st_meter;zxztj=3;}}//如控制反馈和实际反馈接近，表示舵轮已转向到位。
//    break;
//  case 3://转过固定角位
//    if(zx>0){turn=lturn;speed=-lrspeed; 
//    if(abs(zxlen-SYSST.st_meter)>(lcount*jd/180+10)){speed=0;turn=0;zxztj=4;}}
//    if(zx<0){turn=rturn;speed=-lrspeed;
//    if(abs(zxlen-SYSST.st_meter)>(rcount*jd/180+10)){speed=0;turn=0;zxztj=4;}}
//    break;
//  case 4:                              //转向入磁
//    idcount++; SYSST.dd_com=0xff;zxztj=0;
//    break;
//  case 20:                              //转向入磁
//    if(zx>0)turn=lturn; if(zx<0)turn=rturn;
//    if(abs(SYSST.st_turn-SYSST.const_turn)<2){zxlen=SYSST.st_meter;zxztj=21;}//如控制反馈和实际反馈接近，表示舵轮已转向到位。
//    break;
//  case 21:
//    if(zx>0){turn=lturn;speed=lrspeed; }if(zx<0){turn=rturn;speed=lrspeed;}
//    if(CAR_MAGS.OnMag>0){speed=0;turn=0;zxztj=4;}//找到磁条。
//    break;
//  case 40:                              //寻磁转向
//    zhixingxunchi=1;CAR_MAGS.zyfx=zx;zxcount=200;zxztj=4; //寻磁转向延迟4秒。
//    break;
//  }}
// SYSST.con_speed=-speed;
//  SYSST.con_turn=turn;
//
//}  
//  





void control(void* pa)
{  
  TickType_t xLastWakeTime;
 xLastWakeTime = xTaskGetTickCount ();
 while(1)
{
 vTaskDelayUntil( &xLastWakeTime, 20/portTICK_PERIOD_MS);


//uxTaskGetStackHighWaterMark( NULL );

}

}

//highinit:0xA0  进入设置流程。0xA1，下降找跳变点，0xA2，上升找跳变点。0xAA，初始化完成。0xAF 初始化失败。
void high_init(void)
{
  static u8 count=0;
  static s32 temp=0;//高度值暂存。
  if((SYSST.highinit&0xf8)==0xa0){//高度自动找零位。
    SYSST.con_speed=0;SYSST.con_turn=0;
    if((count++)==10)
      temp=SYSST.st_high;
    if(count==60){if(abs(temp-SYSST.st_high)<10)
      SYSST.highinit=0XAF;count=0;}//一秒内位移低于10mm，则报错。
   switch(SYSST.highinit)
  {
  case 0xA0:
    if(ZBAQ){SYSST.con_highsp=-5;SYSST.highinit=0XA1;}
    else  {SYSST.con_highsp=5;SYSST.highinit=0XA2;}
    break;
  case 0xa1:
    if(!ZBAQ){
      SYSST.highinit=0XA0;count=0;}
    break;
  case 0xa2:
    if(ZBAQ)
    {highset(highbase);SYSST.highinit=0XAA;SYSST.con_highsp=0;SYSST.st_mod=1;}
    break;
  case 0xaa:
    
    break;
  default:
    break;
     
  
  
  }
  }else count=0;

}

