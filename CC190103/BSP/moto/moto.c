#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "moto.h" 
#include "stdlib.h"
#include "math.h"
#include "..\DCMOTO\dcabmoto.h"
#include "../adc/adc.h"
#include "upload.h"


xMOTO_D moto[4]={0,};
s8 motofx[5]={1,-1,-1,1};


/*****************************************************************************
*步进电机控制程序。
*
*
*****************************************************************************
void updowninit (void)      //pwm脉冲调节    //升降步进电机初始化//time8 主模式 time4从模式。
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE); 	//使能GPIO时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;            //D8 D9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        
        //指示灯 D6 D7 D8 D9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;            //电机方向，开关
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOD, &GPIO_InitStructure);



	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;            //电机限位
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOD, &GPIO_InitStructure);

        
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource6 , GPIO_AF_TIM8);
        
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
  TIM_TimeBaseStructure.TIM_Period = (10-1 ) & 0xffff; // AKA TIMx_ARR   50hz

    // "The counter clock frequency (CK_CNT) is equal to f CK_PSC / (PSC[15:0] + 1)." - STM32F10x Reference Manual 14.4.11
    // Thus for 1Mhz: 72000000 / 1000000 = 72, 72 - 1 = 71 = TIM_Prescaler
    TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / (1000000)) - 1;

    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

    TIM_OCInitTypeDef  TIM_OCInitStructure;

    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = (5-1);
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
            TIM_OC1Init(TIM8, &TIM_OCInitStructure);
            TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
 
            
            
            	
	// 时基配置：配置脉冲计数寄存器——TIM3   9号从模式的输入为time2 time3 time10 time11
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 		     
	TIM_TimeBaseStructure.TIM_Prescaler = 0; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
	

	// 配置TIM2为主定时器 
	// Select the Master Slave Mode 
	TIM_SelectMasterSlaveMode(TIM8, TIM_MasterSlaveMode_Enable); 
	// Master Mode selection  
	TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_OC1Ref); 
	
	// 配置TIM3为从定时器 
	// Slave Mode selection: TIM3 
//	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_External1); 
//	TIM_SelectInputTrigger(TIM3, TIM_TS_ITR1); //下一条包含
	TIM_ITRxExternalClockConfig(TIM4, TIM_TS_ITR3);         //包含上两条指令。
//	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE); 		 	 
	
TIM_CtrlPWMOutputs(TIM8,ENABLE);
TIM_Cmd(TIM8, DISABLE);         //输出
TIM_Cmd(TIM4, ENABLE);          //统计
}

//===========================
//speed :速度脉冲最高32khz。pwm脉冲调节。
//===========================
s32 setspeed(s16 speed)         //设定速度值，返回位置值。()
{
  u16 spe=0;
if(((speed<0)&&(!MOTOST))||((speed>0)&&(!MOTOEND)))
 {MOTOOFF=0;TIM_Cmd(TIM4, ENABLE); 
 
  spe=0xffff-(SystemCoreClock/((u16)(abs(speed)))/10);
  spe=0xffff-spe;if(spe<0x50)spe=0x50;
  
  if(speed>0){MOTOTO=0;TIM_CounterModeConfig(TIM4,TIM_CounterMode_Up);}
        else {MOTOTO=1;TIM_CounterModeConfig(TIM4,TIM_CounterMode_Down);}
TIM_PrescalerConfig(TIM8,spe,0);        //不立即更新
TIM_Cmd(TIM8, ENABLE);}

else{MOTOOFF=1;TIM_Cmd(TIM4, DISABLE);
if(MOTOST)TIM_SetCounter(TIM4,0);
TIM_Cmd(TIM8, DISABLE);
 }
return TIM_GetCounter(TIM4);

}

stepmotowork();

{
moto[2].lastpos=*YUNTAI_Value/0x20;

if((moto[3].pos>0x3f00)||(moto[3].pos<0)||((moto[3].pos==0)&&(!MOTOST)))highreset=1;
    if(highreset==1){moto[3].pos=setspeed(-1000);if(MOTOST)highreset=0;continue;}
  

  high_con=((s16)com_data.high_pos*256- moto[3].pos);
  if(high_con>600)high_con=600;
  if(high_con<-600)high_con=-600;
 if(abs(moto[3].pos-(s16)com_data.high_pos*256)<100)high_con=0;
 high_con*=5;
 
 if(abs(high_con-hhigh_con)>500) {if((high_con-hhigh_con)>0)hhigh_con+=30;else hhigh_con-=30;}else hhigh_con=high_con;
 if ((hhigh_con!=0)&&(abs(hhigh_con)<50))if(high_con>0)hhigh_con=100;else hhigh_con=-100;
 moto[3].pos=setspeed(hhigh_con);
  
 
 
 
if((moto[2].pos+55-moto[2].lastpos)>2) moto[2].lasthz=30;
else {if((moto[2].pos+55-moto[2].lastpos)<-2) moto[2].lasthz=-30;else  moto[2].lasthz=0;
}

 if((moto[2].lasthz*motofx[2])>0)MOTO2U else MOTO2D}
}
//
*/




static void MOTO_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE); 	//使能GPIO时钟

        //电机控制 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;            //0电机方向
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;            //1电机方向
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;            //2电机方向
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;            //3电机方向
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;            //3电机方向
	GPIO_Init(GPIOE, &GPIO_InitStructure);

        //电机限位 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;            //0电机方向
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;            //0电机方向
	GPIO_Init(GPIOA, &GPIO_InitStructure);

        
MOTO0B
MOTO1B
MOTO2B
MOTO3B
	
}

//直流电机驱动初始化。占空比变化。
void DCmotoinit(void)
{
  
    	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	//使能GPIO时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;            //D8 D9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);


    GPIO_PinAFConfig(GPIOC,GPIO_PinSource6 , GPIO_AF_TIM8);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource7 , GPIO_AF_TIM8);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource8 , GPIO_AF_TIM8);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource9 , GPIO_AF_TIM8);
      

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

      TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = (8000000/40000 - 1) & 0xffff; // AKA TIMx_ARR  20khz频率(0-50)最终生成20khz频率可以200间隔。

    // "The counter clock frequency (CK_CNT) is equal to f CK_PSC / (PSC[15:0] + 1)." - STM32F10x Reference Manual 14.4.11
    // Thus for 1Mhz: 72000000 / 1000000 = 72, 72 - 1 = 71 = TIM_Prescaler
    TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / ((uint32_t) 8000000)) - 1;//生成8M基频

    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

    TIM_OCInitTypeDef  TIM_OCInitStructure;

    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

            TIM_OC1Init(TIM8, &TIM_OCInitStructure);
            TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);

            TIM_OC2Init(TIM8, &TIM_OCInitStructure);
            TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);

            TIM_OC3Init(TIM8, &TIM_OCInitStructure);
            TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

            TIM_OC4Init(TIM8, &TIM_OCInitStructure);
            TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);

    TIM_CtrlPWMOutputs(TIM8, ENABLE);
    TIM_Cmd(TIM8, ENABLE);

    TIM_SetCompare1(TIM8,0);
    TIM_SetCompare2(TIM8,0);
    TIM_SetCompare3(TIM8,0);
    TIM_SetCompare4(TIM8,0);
    
}

static void dcmotowork (void)
{
  static u8 m0,m1,m2,m3;
 s16 temp;
s16 sppp;//速度偏差
//static s16 aaa[10];
static u8 aa,ab=0;
 
for(int i = 0; i < 4; ++i)
//int i=2;
{
    moto[i].pos= (s32)(dcmotowz(i)*(motofx[i])); 
    moto[i].cuspe=(s16)(moto[i].pos-moto[i].lastpos);
//    moto[i].lastpos=moto[i].pos;  //实时速度求取。
//    if((moto[i].lasthz>0)&&(moto[i].cuspe>0))moto[i].cuspe=moto[i].lastcuspe;else {
//      if((moto[i].lasthz<0)&&(moto[i].cuspe<0))moto[i].cuspe=moto[i].lastcuspe;else {moto[i].lastcuspe=moto[i].cuspe;moto[i].lastpos=moto[i].pos;}}
moto[i].lastcuspe=moto[i].cuspe;
moto[i].lastpos=moto[i].pos;
}

//    aaa[aa++]=(s16)moto[3].pos;
//    if (aa>9)aa=0;
//    if(moto[3].cuspe<0)
//    {
//      ab=4;}
//if(ab==4)moto[3].comspeed=0; 


for(int i = 0; i < 4; ++i)
{
//  if(moto[i].err!=254)
  {if(i<2){                                                 //行走速度驱动
  sppp=moto[i].comspeed-moto[i].cuspe;
  if(abs(sppp)>5){if(sppp>0)moto[i].comhz-=1;else moto[i].comhz+=1;if((moto[i].err++)>250)moto[i].err=250;}else moto[i].err=0;                    //速度反馈控制
  if(abs(sppp)>100){if(sppp>0)moto[i].comhz-=5;else moto[i].comhz+=5;}                   //速度反馈控制
  if(abs(moto[i].comhz)>190){if(moto[i].comhz<0)moto[i].comhz=-190;else moto[i].comhz=190;}//最高控制量限定。

  if((abs(moto[i].lasthz)>=100)&&(abs(moto[i].cuspe)<=20))moto[i].dzstop++;else moto[i].dzstop=0;//堵转检测,计时。
    if(moto[i].dzstop>50)    {moto[i].err=254;moto[i].lasthz=0;moto[i].comhz=0;}            //堵转0.5s保护。                 
  temp=moto[i].comhz-moto[i].lasthz;
 if(abs(temp)<20){moto[i].lasthz=moto[i].comhz;} 
 else{if(temp<0)moto[i].lasthz-=10;else moto[i].lasthz+=10;}                            //控制突变限制
  }

  else                                                                          //云台混合驱动
  {
    
    
    sppp=moto[i].comspeed-moto[i].cuspe;
  if(abs(sppp)>1){if(sppp>0)moto[i].comhz-=1;else moto[i].comhz+=1;}                    //速度反馈控制
  if(abs(sppp)>10){if(sppp>0)moto[i].comhz-=3;else moto[i].comhz+=3;if((moto[i].err++)>250)moto[i].err=250;}else moto[i].err=0;                   //速度反馈控制
  if(abs(moto[i].comhz)>190){if(moto[i].comhz<0)moto[i].comhz=-190;else moto[i].comhz=190;}//最高控制量限定。

  if((abs(moto[i].lasthz)>=100)&&(abs(moto[i].cuspe)<=2))moto[i].dzstop++;else moto[i].dzstop=0;//堵转检测,计时。
    if(moto[i].dzstop>50)    {moto[i].err=254;moto[i].lasthz=0;moto[i].comhz=0;}            //堵转0.5s保护。                 

  
    temp=moto[i].comhz-moto[i].lasthz;
   if(abs(temp)<20){moto[i].lasthz=moto[i].comhz;} 
 else{if(temp<0)moto[i].lasthz-=10;else moto[i].lasthz+=10;}                            //控制突变限制

  
  }
  }}




//换向时0.1s的刹车
if(moto[0].lasthz==0) m0=0;else  m0++;
if((m0<5)||(moto[0].lasthz==0)){MOTO0B}
else{m0=200;if((moto[0].lasthz*motofx[0])>0)MOTO0U else MOTO0D}

if(moto[1].lasthz==0) m1=0;else  m1++;
if((m1<5)||(moto[1].lasthz==0)){MOTO1B}
else{m1=200;if((moto[1].lasthz*motofx[1])>0)MOTO1U else MOTO1D}

if(moto[2].lasthz==0) m2=0;else  m2++;
if((m2<5)||(moto[2].lasthz==0)){MOTO2B}
else{m2=200;if((moto[2].lasthz*motofx[2])>0)MOTO2U else MOTO2D}

if(moto[3].lasthz==0) m3=0;else  m3++;
if((m3<5)||(moto[3].lasthz==0)){MOTO3B}
else{m3=200;if((moto[3].lasthz*motofx[3])>0)MOTO3U else MOTO3D}







    TIM_SetCompare1(TIM8,abs(moto[0].lasthz));
    TIM_SetCompare2(TIM8,abs(moto[1].lasthz));
    TIM_SetCompare3(TIM8,abs(moto[2].lasthz));//控制量0-200
    TIM_SetCompare4(TIM8,abs(moto[3].lasthz));//控制量0-200

 

}


//
// void dcmotowork (void)
//{
//  static u8 m0,m1,m2,m3;
// s16 temp;
//s16 sppp;//速度偏差
//static s16 aaa[260],bbb[260];
//static u8 aaaa=0;
//static u8 aa,ab=0;
// 
//
//int i=2;
//{
//    moto[i].pos= (s32)(dcmotowz(i)*(motofx[i])); 
//    moto[i].cuspe=(s16)(moto[i].pos-moto[i].lastpos);
////    moto[i].lastpos=moto[i].pos;  //实时速度求取。
////    if((moto[i].lasthz>0)&&(moto[i].cuspe>0))moto[i].cuspe=moto[i].lastcuspe;else {
////      if((moto[i].lasthz<0)&&(moto[i].cuspe<0))moto[i].cuspe=moto[i].lastcuspe;else {moto[i].lastcuspe=moto[i].cuspe;moto[i].lastpos=moto[i].pos;}}
//moto[i].lastcuspe=moto[i].cuspe;
//moto[i].lastpos=moto[i].pos;
//}
//
////    aaa[aa++]=(s16)moto[3].pos;
////    if (aa>9)aa=0;
////    if(moto[3].cuspe<0)
////    {
////      ab=4;}
////if(ab==4)moto[3].comspeed=0; 
//
//
//{
//    
//    
//    sppp=moto[i].comspeed-moto[i].cuspe;
//  if(abs(sppp)>1){if(sppp>0)moto[i].comhz-=1;else moto[i].comhz+=1;}                    //速度反馈控制
//  if(abs(sppp)>10){if(sppp>0)moto[i].comhz-=5;else moto[i].comhz+=5;if((moto[i].err++)>250)moto[i].err=250;}else moto[i].err=0;                   //速度反馈控制
//  if(abs(moto[i].comhz)>200){if(moto[i].comhz<0)moto[i].comhz=-200;else moto[i].comhz=200;}//最高控制量限定。
//
//  if((abs(moto[i].lasthz)>=100)&&(abs(moto[i].cuspe)<=5))moto[i].dzstop++;else moto[i].dzstop=0;//堵转检测,计时。
//    if(moto[i].dzstop>50)    {moto[i].err=254;moto[i].lasthz=0;moto[i].comhz=0;}            //堵转0.5s保护。                 
//
//  
//    temp=moto[i].comhz-moto[i].lasthz;
//   if(abs(temp)<20){moto[i].lasthz=moto[i].comhz;} 
// else{if(temp<0)moto[i].lasthz-=10;else moto[i].lasthz+=10;}                            //控制突变限制
//
// 
// aaa[aaaa++]=moto[i].comhz;
// bbb[aaaa]=moto[i].cuspe;
//  
//  }
//
//
//
//
////换向时0.1s的刹车
//if(moto[0].lasthz==0) m0=0;else  m0++;
//if((m0<2)||(moto[0].lasthz==0)){MOTO0B}
//else{m0=200;if((moto[0].lasthz*motofx[0])>0)MOTO0U else MOTO0D}
//
//if(moto[1].lasthz==0) m1=0;else  m1++;
//if((m1<2)||(moto[1].lasthz==0)){MOTO1B}
//else{m1=200;if((moto[1].lasthz*motofx[1])>0)MOTO1U else MOTO1D}
//
//if(moto[2].lasthz==0) m2=0;else  m2++;
//if((m2<2)||(moto[2].lasthz==0)){MOTO2B}
//else{m2=200;if((moto[2].lasthz*motofx[2])>0)MOTO2U else MOTO2D}
//
//if(moto[3].lasthz==0) m3=0;else  m3++;
//if((m3<2)||(moto[3].lasthz==0)){MOTO3B}
//else{m3=200;if((moto[3].lasthz*motofx[3])>0)MOTO3U else MOTO3D}
//
//
//
//
//
//
//
//    TIM_SetCompare1(TIM8,abs(moto[0].lasthz));
//    TIM_SetCompare2(TIM8,abs(moto[1].lasthz));
//    TIM_SetCompare3(TIM8,abs(moto[2].lasthz));//控制量0-200
//    TIM_SetCompare4(TIM8,abs(moto[3].lasthz));//控制量0-200
//
// 
//
//}
//










static void CalMotor(void)              //只计算一，二号电机的坐标运算。
{
// float dy;
//	static char firstflag[2] = {0,};

       	static double basex = 0.0;//x坐标
	static double basey = 0.0;//y坐标
	static double radcount=0.0;//旋转弧度。
	double baseth = 0.0;//旋转角度
	double length = 0.0;//行驶里程

        
	double actpos[2] = {0,};

          actpos[0]=moto[0].cuspe* WheelPeri*3.14 /(GearScale * MotorEncoder);           //1号电机移动量。
          actpos[1]=moto[1].cuspe* WheelPeri*3.14 /(GearScale * MotorEncoder);           //2号电机移动量。
                

    
//小车行驶里程数
        length=(actpos[0]+actpos[1])/2;

        
       	baseth = (actpos[0] - actpos[1])/MotoLength;                  ///单位时间的两轮差距除以轮距获得弧度值(rad)
        radcount-=baseth;                       //小车旋转弧度
        xSentData.car_rad=(s16)radcount;                 //小车旋转弧度
        xSentData.car_rad=(s16)(radcount*360/6.28);//小车旋转角度

        basex += length*cos(radcount+1.57);
	basey += length*sin(radcount+1.57);

	xSentData.car_pos[0] = (s32)basex;      ///X偏移量 累加
	xSentData.car_pos[1] = (s32)basey;      ///Y偏移量  累加
        xSentData.drv_pos[0] = moto[0].pos;        //车轮脉冲数       
        xSentData.drv_pos[1] = moto[1].pos;
        
      
// if(xDRV)xQueueSend(xDRV,&moto,0); 

       
}



void mototask(void* pv)
{
//  u8 i;
  s16 high_con=0;               //当前滑台控制值。
  s16 hhigh_con=0;
  u8 highreset=0;               //开机复位 升降复位
  u8 hhighreset=0;               //开机复位 云台复位
  const s32 highcount=28500; //满刻度
  const s32 highoversheet=500; //超调量
  const s32 hhighcount=1900;
  const s32 hhighoversheet=100;
  
  s32 highcon,hhighcon;
  
  TickType_t xLastWakeTime;
  MOTO_GPIOInit();
  DCmotoinit();  //PWM脉宽调整
   moto_AB_INIT();
//   updowninit ();//PWM频率调整
//  YUNTAI_Init();
vTaskDelay(1000/portTICK_PERIOD_MS);
  
	 xLastWakeTime = xTaskGetTickCount ();
  
  
while (1)
{
//===========开机回零
  if(highreset==0)moto[2].comspeed=-40;
 if(hhighreset==0)moto[3].comspeed=-5;
 if(!HTLOW&&highreset!=0x8)  {moto[2].comspeed=0;if(moto[2].lastcuspe==0){dcmotoset(DCMOTO2_AB_TIME,0); highreset=0x8;}}
 if(!YTLOW&&hhighreset!=0x8) {moto[3].comspeed=0;if(moto[3].lastcuspe==0){dcmotoset(DCMOTO3_AB_TIME,0);hhighreset=0x8;}}
//===========异常回零，清零。
 if(!HTLOW&&moto[2].lastpos>0){dcmotoset(DCMOTO2_AB_TIME,0);}
 if(!YTLOW&&moto[3].lastpos>0){dcmotoset(DCMOTO3_AB_TIME,0);}

 if(highreset==0x8)  {
   highcon=highcount*moto[2].conpos/60;//计算停车位。
   if(abs(moto[2].pos-highcon)>highoversheet)
   {
     if(moto[2].pos>highcon){moto[2].comspeed=-60;}
     else {moto[2].comspeed=60;}
     if(abs(moto[2].pos-highcon)<(highoversheet*5))moto[2].comspeed>>=1;
   }
 else moto[2].comspeed=0;
 }
moto[2].highpos=moto[2].lastpos*60/highcount;
 
 
 if(hhighreset==0x8) {
   hhighcon=hhighcount*moto[3].conpos/15;
   if(abs(moto[3].pos-hhighcon)>hhighoversheet)
   {
     if(moto[3].pos>hhighcon){moto[3].comspeed=-10;}
     else {moto[3].comspeed=10;}
     if(abs(moto[3].pos-hhighcon)<(hhighoversheet*2))moto[3].comspeed>>=1;
   }
 else moto[3].comspeed=0;
 }
moto[3].highpos=moto[3].lastpos*15/hhighcount;

 
 
 vTaskDelayUntil( &xLastWakeTime, 10/portTICK_PERIOD_MS );
dcmotowork();  
CalMotor();
}
 
}





