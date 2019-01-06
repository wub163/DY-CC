#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "stm32f4xx.h"
#include "..\TM1638\TM1638.h"
#include "sys.h"
u16 dis_data[2];
u8 DIS_BUF[8]={0,};
u8 KEY;
void diswork (void);

//=================键码表================
//      06        02      0E      0A
//      16        12      1E      1A
//      07        03      0F      0B
//      17        13      1F      1B  
//=================键码表================


const u8 disp[]={0X3F,0X06,0X5B,0X4F,0X66,0X6D,0X7D,0X27,//0-7
                  0X7F,0X6F,0X77,0X7C,0X39,0X5E,0X79,0X71,//8-F
                  0XBF,0X86,0XDB,0XCF,0XE6,0XED,0XFD,0XA7,//0.-7.
                  0XFF,0XEF,0XF7,0XFC,0XB9,0XDE,0XF9,0XF1,//8.-F.
                  0X00,0X40                               //空格，负号。
};

static void delay_us(u8 da)//300ns
{
u16 ta=da*10;  //168mhz m4 190us play函数
while(ta--);
}

void TM1638_WB(u8 da)
{

for(int i = 0; i < 8; ++i)
{
  TM_CLK(0);
 TM_DIO(da&1);da>>=1;
 delay_us(1);
  TM_CLK(1);
 delay_us(1);
  
}
  }

u8 TM1638_RB(void)
{
u8 temp=0;
  TM_DIO(1);
for(int i = 0; i < 8; ++i)
{
  TM_CLK(0); delay_us(1);
  temp<<=1;
  TM_CLK(1); delay_us(1);
  if(TM_READ)temp|=1;
}
return temp;
  }

//============显示开关（0：关 1-8：显示亮度）
void TM1638_DIS(u8 da)
{
TM_STB(0);
da&=0x0f;
if(da)
{da--;
TM1638_WB(da|0x88);}
else TM1638_WB(0x80);
TM_STB(1);
}

void Init_TM1638(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(TM1638_STB_PORT_PER|TM1638_CLK_PORT_PER|TM1638_DIO_PORT_PER, ENABLE); 	//使能GPIO时钟

	GPIO_InitStructure.GPIO_Pin = TM1638_STB_PIN;  //S1 S2 S3 S4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =   GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(TM1638_STB_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = TM1638_CLK_PIN;  //S1 S2 S3 S4
	GPIO_Init(TM1638_CLK_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = TM1638_DIO_PIN;  //S1 S2 S3 S4
	GPIO_InitStructure.GPIO_OType =   GPIO_OType_OD;
	GPIO_Init(TM1638_DIO_PORT, &GPIO_InitStructure);

  
  
  
//  GPIO_Init(TM1638_STB_PORT, TM1638_STB_PIN,GPIO_Mode_Out_PP_High_Fast);
//    GPIO_Init(TM1638_CLK_PORT, TM1638_CLK_PIN,GPIO_Mode_Out_PP_High_Fast);
//    GPIO_Init(TM1638_DIO_PORT, TM1638_DIO_PIN,GPIO_Mode_Out_OD_HiZ_Fast);
    
//    GPIO_Init(TM1638_DIO_PORT, TM1638_DIO_PIN,GPIO_Mode_In_FL_No_IT);
TM1638_DIS(8);
}




u8 DIS_BUF_com[8]={0,};
void change (void)              //共阴共阳转换
{
for(int i = 0; i < 8; ++i)
{
 for(int ii = 0; ii < 8; ++ii)
 {
   DIS_BUF_com[i]<<=1;
   DIS_BUF_com[i]|=((disp[DIS_BUF[ii]]>>(7-i))&0x01);
 }
 }
}


//====显示缓冲区送显示
void TM1638_PLAY(void)
{
static u32 KEY_TMP=0;

  change ();
TM_STB(0);
TM1638_WB(0x40);//普通，地址曾一，写显示
TM_STB(1);
 delay_us(3);
TM_STB(0);
TM1638_WB(0xc0);

for(int i = 0; i < 8; ++i)
{
TM1638_WB(DIS_BUF_com[7-i]);
TM1638_WB(0X0);
}
TM_STB(1);
 delay_us(3);
TM_STB(0);
TM1638_WB(0x42);//普通，地址曾一，读键值。
KEY_TMP=TM1638_RB();
KEY_TMP|=((u32)TM1638_RB()<<8);
KEY_TMP|=((u32)TM1638_RB()<<16);
KEY_TMP|=((u32)TM1638_RB()<<24);
TM_STB(1);
KEY=0;
for(int i = 0; i < 32; ++i)
{
  if((KEY_TMP>>i)&0x01)KEY=i+1;
}
TM1638_DIS(5);
return;



}

void TM1638task (void* pa)              //获取实时键值，所以启用单独任务。
{
 vTaskDelay(5000/portTICK_PERIOD_MS);

Init_TM1638();

while(1)
{
TM1638_PLAY();
 vTaskDelay(100/portTICK_PERIOD_MS);
diswork();
}

}


//**********************************************
u16 htod(s16 ac)
{
  
  u16 dac;
if(ac<0)dac=0x8000;else dac=0x000;ac=abs(ac);dac+=((ac/100)<<8)+((ac%100)/10<<4)+(ac%10);

return dac;
}





void diswork (void)
{
  s16 ac,ao,vc,vo;
  u16 dac,dao,dvc,dvo;    //角度的输入输出，速度的输入输出。10进制值。
static  u8 dis=0;
u16 dis1=0,dis2=0;
switch(KEY)
{
//case 0x06:handcon[0]=1;break;
//case 0x02:handcon[0]=-1;break;
//case 0x0e:handcon[0]=0;break;
//case 0x16:handcon[1]=2000;break;
//case 0x12:handcon[1]=-2000;break;
//case 0x1e:handcon[1]=0;break;  //遥控码显示
//case 0x07:handcon[2]=1000;break;
//case 0x03:handcon[2]=-1000;break;
//case 0x0f:handcon[2]=0;break;
//case 0x17:handcon[3]=1000;break;
//case 0x13:handcon[3]=-1000;break;
//case 0x1f:handcon[3]=0;break;



case 0x0a:dis=1;break;  //转向控制值，驱动值
case 0x1a:dis=2;break;  //行走控制值，驱动值
case 0x0b:dis=3;break;  //转向控制值，行走控制值
case 0x1b:dis=4;break;  //当前传感器值。 
}


ac=cha_state.wheelst.CmdMotorAngle;
ao=(s16)cha_state.turn_moto_getpro.RealPosition;
vc=cha_state.wheelst.CmdMotorvel;
//vo=CANDD.cspeed ;                     //显示实时控制速度
vo=cha_state.wheelst.FdbMotorVel;       //显示实时反馈速度
dac=htod(ac);dao=htod(ao);dvc=htod(vc);dvo=htod(vo);
    switch(dis)
{
  case 0:       //速度，位置显示   （升降功能）
    dis1=0x1234;
    dis2=0x5678;
    break; 
  case 1:
    dis1=dac;
    dis2=dao;
    break; //自动默认显示
  case 2:
    dis1=dvc;
    dis2=dvo ;
    break; //左右称重显示     （检球功能）  
  case 3:       //键码，编码器显示 （摆角功能）
    dis1=dac;
    dis2=dvc;
    break;
  case 4:
    dis1=STOP<<12|LSCB<<8|ZBJC<<4|ZXLW|0;
    dis2=CCCX<<12|AUTO1<<8|AUTO2<<4|UPOK|0;
    break; //前后位置显示     （推球功能）
  
 //=功能描述： 
  
}
    DIS_BUF[0]=(dis1>>12)&0x0f;
    DIS_BUF[1]=(dis1>>8)&0x0f;
    DIS_BUF[2]=(dis1>>4)&0x0f;
    DIS_BUF[3]=(dis1)&0x0f;
    DIS_BUF[4]=(dis2>>12)&0x0f;
    DIS_BUF[5]=(dis2>>8)&0x0f;
    DIS_BUF[6]=(dis2>>4)&0x0f;
    DIS_BUF[7]=(dis2)&0x0f;
    
    if(dis&0x03){
    if(DIS_BUF[0]==0x08)DIS_BUF[0]=0x21;else DIS_BUF[0]=0x20;
    if(DIS_BUF[4]==0x08)DIS_BUF[4]=0x21;else DIS_BUF[4]=0x20;
    }
    if(DIS_BUF[1]==0x0){DIS_BUF[1]=0x20;if(DIS_BUF[2]==0x0){DIS_BUF[2]=0x20;}}
    if(DIS_BUF[5]==0x0){DIS_BUF[5]=0x20;if(DIS_BUF[6]==0x0){DIS_BUF[6]=0x20;}}
    
}
//***************************************/
