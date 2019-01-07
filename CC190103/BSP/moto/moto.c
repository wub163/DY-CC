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
*����������Ƴ���
*
*
*****************************************************************************
void updowninit (void)      //pwm�������    //�������������ʼ��//time8 ��ģʽ time4��ģʽ��
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE); 	//ʹ��GPIOʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;            //D8 D9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        
        //ָʾ�� D6 D7 D8 D9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;            //������򣬿���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init(GPIOD, &GPIO_InitStructure);



	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;            //�����λ
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
 
            
            
            	
	// ʱ�����ã�������������Ĵ�������TIM3   9�Ŵ�ģʽ������Ϊtime2 time3 time10 time11
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 		     
	TIM_TimeBaseStructure.TIM_Prescaler = 0; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
	

	// ����TIM2Ϊ����ʱ�� 
	// Select the Master Slave Mode 
	TIM_SelectMasterSlaveMode(TIM8, TIM_MasterSlaveMode_Enable); 
	// Master Mode selection  
	TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_OC1Ref); 
	
	// ����TIM3Ϊ�Ӷ�ʱ�� 
	// Slave Mode selection: TIM3 
//	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_External1); 
//	TIM_SelectInputTrigger(TIM3, TIM_TS_ITR1); //��һ������
	TIM_ITRxExternalClockConfig(TIM4, TIM_TS_ITR3);         //����������ָ�
//	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE); 		 	 
	
TIM_CtrlPWMOutputs(TIM8,ENABLE);
TIM_Cmd(TIM8, DISABLE);         //���
TIM_Cmd(TIM4, ENABLE);          //ͳ��
}

//===========================
//speed :�ٶ��������32khz��pwm������ڡ�
//===========================
s32 setspeed(s16 speed)         //�趨�ٶ�ֵ������λ��ֵ��()
{
  u16 spe=0;
if(((speed<0)&&(!MOTOST))||((speed>0)&&(!MOTOEND)))
 {MOTOOFF=0;TIM_Cmd(TIM4, ENABLE); 
 
  spe=0xffff-(SystemCoreClock/((u16)(abs(speed)))/10);
  spe=0xffff-spe;if(spe<0x50)spe=0x50;
  
  if(speed>0){MOTOTO=0;TIM_CounterModeConfig(TIM4,TIM_CounterMode_Up);}
        else {MOTOTO=1;TIM_CounterModeConfig(TIM4,TIM_CounterMode_Down);}
TIM_PrescalerConfig(TIM8,spe,0);        //����������
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
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE); 	//ʹ��GPIOʱ��

        //������� 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;            //0�������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;            //1�������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;            //2�������
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;            //3�������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;            //3�������
	GPIO_Init(GPIOE, &GPIO_InitStructure);

        //�����λ 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;            //0�������
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;            //0�������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

        
MOTO0B
MOTO1B
MOTO2B
MOTO3B
	
}

//ֱ�����������ʼ����ռ�ձȱ仯��
void DCmotoinit(void)
{
  
    	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	//ʹ��GPIOʱ��

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
    TIM_TimeBaseStructure.TIM_Period = (8000000/40000 - 1) & 0xffff; // AKA TIMx_ARR  20khzƵ��(0-50)��������20khzƵ�ʿ���200�����

    // "The counter clock frequency (CK_CNT) is equal to f CK_PSC / (PSC[15:0] + 1)." - STM32F10x Reference Manual 14.4.11
    // Thus for 1Mhz: 72000000 / 1000000 = 72, 72 - 1 = 71 = TIM_Prescaler
    TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / ((uint32_t) 8000000)) - 1;//����8M��Ƶ

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
s16 sppp;//�ٶ�ƫ��
//static s16 aaa[10];
static u8 aa,ab=0;
 
for(int i = 0; i < 4; ++i)
//int i=2;
{
    moto[i].pos= (s32)(dcmotowz(i)*(motofx[i])); 
    moto[i].cuspe=(s16)(moto[i].pos-moto[i].lastpos);
//    moto[i].lastpos=moto[i].pos;  //ʵʱ�ٶ���ȡ��
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
  {if(i<2){                                                 //�����ٶ�����
  sppp=moto[i].comspeed-moto[i].cuspe;
  if(abs(sppp)>5){if(sppp>0)moto[i].comhz-=1;else moto[i].comhz+=1;if((moto[i].err++)>250)moto[i].err=250;}else moto[i].err=0;                    //�ٶȷ�������
  if(abs(sppp)>100){if(sppp>0)moto[i].comhz-=5;else moto[i].comhz+=5;}                   //�ٶȷ�������
  if(abs(moto[i].comhz)>190){if(moto[i].comhz<0)moto[i].comhz=-190;else moto[i].comhz=190;}//��߿������޶���

  if((abs(moto[i].lasthz)>=100)&&(abs(moto[i].cuspe)<=20))moto[i].dzstop++;else moto[i].dzstop=0;//��ת���,��ʱ��
    if(moto[i].dzstop>50)    {moto[i].err=254;moto[i].lasthz=0;moto[i].comhz=0;}            //��ת0.5s������                 
  temp=moto[i].comhz-moto[i].lasthz;
 if(abs(temp)<20){moto[i].lasthz=moto[i].comhz;} 
 else{if(temp<0)moto[i].lasthz-=10;else moto[i].lasthz+=10;}                            //����ͻ������
  }

  else                                                                          //��̨�������
  {
    
    
    sppp=moto[i].comspeed-moto[i].cuspe;
  if(abs(sppp)>1){if(sppp>0)moto[i].comhz-=1;else moto[i].comhz+=1;}                    //�ٶȷ�������
  if(abs(sppp)>10){if(sppp>0)moto[i].comhz-=3;else moto[i].comhz+=3;if((moto[i].err++)>250)moto[i].err=250;}else moto[i].err=0;                   //�ٶȷ�������
  if(abs(moto[i].comhz)>190){if(moto[i].comhz<0)moto[i].comhz=-190;else moto[i].comhz=190;}//��߿������޶���

  if((abs(moto[i].lasthz)>=100)&&(abs(moto[i].cuspe)<=2))moto[i].dzstop++;else moto[i].dzstop=0;//��ת���,��ʱ��
    if(moto[i].dzstop>50)    {moto[i].err=254;moto[i].lasthz=0;moto[i].comhz=0;}            //��ת0.5s������                 

  
    temp=moto[i].comhz-moto[i].lasthz;
   if(abs(temp)<20){moto[i].lasthz=moto[i].comhz;} 
 else{if(temp<0)moto[i].lasthz-=10;else moto[i].lasthz+=10;}                            //����ͻ������

  
  }
  }}




//����ʱ0.1s��ɲ��
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
    TIM_SetCompare3(TIM8,abs(moto[2].lasthz));//������0-200
    TIM_SetCompare4(TIM8,abs(moto[3].lasthz));//������0-200

 

}


//
// void dcmotowork (void)
//{
//  static u8 m0,m1,m2,m3;
// s16 temp;
//s16 sppp;//�ٶ�ƫ��
//static s16 aaa[260],bbb[260];
//static u8 aaaa=0;
//static u8 aa,ab=0;
// 
//
//int i=2;
//{
//    moto[i].pos= (s32)(dcmotowz(i)*(motofx[i])); 
//    moto[i].cuspe=(s16)(moto[i].pos-moto[i].lastpos);
////    moto[i].lastpos=moto[i].pos;  //ʵʱ�ٶ���ȡ��
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
//  if(abs(sppp)>1){if(sppp>0)moto[i].comhz-=1;else moto[i].comhz+=1;}                    //�ٶȷ�������
//  if(abs(sppp)>10){if(sppp>0)moto[i].comhz-=5;else moto[i].comhz+=5;if((moto[i].err++)>250)moto[i].err=250;}else moto[i].err=0;                   //�ٶȷ�������
//  if(abs(moto[i].comhz)>200){if(moto[i].comhz<0)moto[i].comhz=-200;else moto[i].comhz=200;}//��߿������޶���
//
//  if((abs(moto[i].lasthz)>=100)&&(abs(moto[i].cuspe)<=5))moto[i].dzstop++;else moto[i].dzstop=0;//��ת���,��ʱ��
//    if(moto[i].dzstop>50)    {moto[i].err=254;moto[i].lasthz=0;moto[i].comhz=0;}            //��ת0.5s������                 
//
//  
//    temp=moto[i].comhz-moto[i].lasthz;
//   if(abs(temp)<20){moto[i].lasthz=moto[i].comhz;} 
// else{if(temp<0)moto[i].lasthz-=10;else moto[i].lasthz+=10;}                            //����ͻ������
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
////����ʱ0.1s��ɲ��
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
//    TIM_SetCompare3(TIM8,abs(moto[2].lasthz));//������0-200
//    TIM_SetCompare4(TIM8,abs(moto[3].lasthz));//������0-200
//
// 
//
//}
//










static void CalMotor(void)              //ֻ����һ�����ŵ�����������㡣
{
// float dy;
//	static char firstflag[2] = {0,};

       	static double basex = 0.0;//x����
	static double basey = 0.0;//y����
	static double radcount=0.0;//��ת���ȡ�
	double baseth = 0.0;//��ת�Ƕ�
	double length = 0.0;//��ʻ���

        
	double actpos[2] = {0,};

          actpos[0]=moto[0].cuspe* WheelPeri*3.14 /(GearScale * MotorEncoder);           //1�ŵ���ƶ�����
          actpos[1]=moto[1].cuspe* WheelPeri*3.14 /(GearScale * MotorEncoder);           //2�ŵ���ƶ�����
                

    
//С����ʻ�����
        length=(actpos[0]+actpos[1])/2;

        
       	baseth = (actpos[0] - actpos[1])/MotoLength;                  ///��λʱ������ֲ������־��û���ֵ(rad)
        radcount-=baseth;                       //С����ת����
        xSentData.car_rad=(s16)radcount;                 //С����ת����
        xSentData.car_rad=(s16)(radcount*360/6.28);//С����ת�Ƕ�

        basex += length*cos(radcount+1.57);
	basey += length*sin(radcount+1.57);

	xSentData.car_pos[0] = (s32)basex;      ///Xƫ���� �ۼ�
	xSentData.car_pos[1] = (s32)basey;      ///Yƫ����  �ۼ�
        xSentData.drv_pos[0] = moto[0].pos;        //����������       
        xSentData.drv_pos[1] = moto[1].pos;
        
      
// if(xDRV)xQueueSend(xDRV,&moto,0); 

       
}



void mototask(void* pv)
{
//  u8 i;
  s16 high_con=0;               //��ǰ��̨����ֵ��
  s16 hhigh_con=0;
  u8 highreset=0;               //������λ ������λ
  u8 hhighreset=0;               //������λ ��̨��λ
  const s32 highcount=28500; //���̶�
  const s32 highoversheet=500; //������
  const s32 hhighcount=1900;
  const s32 hhighoversheet=100;
  
  s32 highcon,hhighcon;
  
  TickType_t xLastWakeTime;
  MOTO_GPIOInit();
  DCmotoinit();  //PWM�������
   moto_AB_INIT();
//   updowninit ();//PWMƵ�ʵ���
//  YUNTAI_Init();
vTaskDelay(1000/portTICK_PERIOD_MS);
  
	 xLastWakeTime = xTaskGetTickCount ();
  
  
while (1)
{
//===========��������
  if(highreset==0)moto[2].comspeed=-40;
 if(hhighreset==0)moto[3].comspeed=-5;
 if(!HTLOW&&highreset!=0x8)  {moto[2].comspeed=0;if(moto[2].lastcuspe==0){dcmotoset(DCMOTO2_AB_TIME,0); highreset=0x8;}}
 if(!YTLOW&&hhighreset!=0x8) {moto[3].comspeed=0;if(moto[3].lastcuspe==0){dcmotoset(DCMOTO3_AB_TIME,0);hhighreset=0x8;}}
//===========�쳣���㣬���㡣
 if(!HTLOW&&moto[2].lastpos>0){dcmotoset(DCMOTO2_AB_TIME,0);}
 if(!YTLOW&&moto[3].lastpos>0){dcmotoset(DCMOTO3_AB_TIME,0);}

 if(highreset==0x8)  {
   highcon=highcount*moto[2].conpos/60;//����ͣ��λ��
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





