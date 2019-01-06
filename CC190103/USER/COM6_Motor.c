
#include "common.h"

#define MOTORNUM (2)
#define se485 GPIO_SetBits(GPIOF,  GPIO_Pin_11)
#define re485 GPIO_ResetBits(GPIOF,  GPIO_Pin_11)
u8 moto_data[120] = {0};      
u8 moto_busy=0;
u8 num = 0;

u8 data[MOTORNUM][8];
void moto(u8 ch);
void fankui(u8 ch);
void USART6_IRQHandler(void)
{
//	OSIntEnter();
	if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)
	{
			num = USART_ReceiveData(USART6);//
	    switch(moto_busy)
			{
					case 1:
						moto(1);
					  break;
					case 2:
						fankui(0);
					  break;
					case 11:
						fankui(1);
					  break;
				  default:
						num=0;
					  moto_busy=0;
					  break;
			}
	 }
          
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
	{	
		moto_data[num++] = USART_ReceiveData(USART6);
		if(num>=118)num=0;
  }
//	OSIntExit();
  
}

char Send485 (char ch)            //485??
{                
    USART_SendData(USART2, (unsigned char) ch);
    while (!(USART2->SR & USART_FLAG_TXE));
    return (ch);
}

void MotorDrivers_Init(void)
{
	data[0][0]=0x02;
	data[0][1]=0x06;
	data[0][2]=0x00;
	data[0][3]=0x43;
	data[1][0]=0x03;
	data[1][1]=0x06;
	data[1][2]=0x00;
	data[1][3]=0x43;
}

//========================================================
static const u8 aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const u8 aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

u16 crc16(u8 * frame, u16 len )
{
    u8 ucCRCHi = 0xFF;
    u8 ucCRCLo = 0xFF;
    u8 iIndex;
    
    while( len-- )
    {
        iIndex = ucCRCLo ^ *( frame++ );
        ucCRCLo = ( u8 )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( u16 )( ucCRCHi << 8 | ucCRCLo );
}

//gHalData->WheelHal[ch].CmdVel ????? ,????????,??????
void moto(u8 ch)
{

	int i = 0;
	u16 crc;
	s16 speed;
	if(ch >= MOTORNUM)
	{
		return;
	}
	se485;  

	if(0 == gInnerData->FatalErr)  
	{
		gHalData->WheelHal[ch].CmdMotorvel = (int)(gHalData->WheelHal[ch].CmdVel * gInnerData->GearScale * 60  * gHalData->WheelHal[ch].FlagForward / gInnerData->WheelPeri);
		 
	}
	else
	{
		gHalData->WheelHal[ch].CmdMotorvel = 0;
	}
  speed=gHalData->WheelHal[ch].CmdMotorvel*150;
  data[ch][4]=(speed>>8)&0xff;
  data[ch][5]=speed&0xff;
//if(JudgeEncrypt()==0)while(1);//why

  crc=crc16(&(data[ch][0]),6);
  data[ch][7]=(crc>>8)&0xff;
  data[ch][6]=crc&0xff;
for(i = 0; i < 8; ++i)
{
  Send485(data[ch][i]);
}
Send485(0);
re485;     			                       //??????
moto_data[ch*60+1]=0xff;                                //??
    moto_busy=(ch+1);num=ch*60;  

}


void fankui(u8 ch)
{
se485;
switch (ch)
{
            case 0:
    Send485 (0x02);
    Send485 (0x03);
    Send485 (0x00);
    Send485 (0x20);
    Send485 (0x00);
    Send485 (0x15);
    Send485 (0x85);
    Send485 (0xfc);
    Send485 (0);
    break;
            case 1:
    Send485 (0x03);
    Send485 (0x03);
    Send485 (0x00);
    Send485 (0x20);
    Send485 (0x00);
    Send485 (0x15);
    Send485 (0x84);
    Send485 (0x2d);
    Send485 (0);
default:break;
} 
re485;   				                       //??????
moto_busy=(ch+11);num=ch*60+10;
}


void CalMotor(void)              /// 2015-04-22??mm/s
{
  int num = 0;
	int errcount = 0;
	int errsum = 0;
	static char firstflag[MOTORNUM] = {0,};
	static int errnum = 0;
	static char errflag[50] = {0,};
	double actpos[MOTORNUM] = {0,};
	double baseth = 0.0;
	double length = 0.0;
	static double radcount=0.0;

	for(num = 0;num < MOTORNUM; num++)
	{
		u16 bb,cc,aa;
		aa = num*60;
		bb = *((u16*)&(moto_data[55+aa]));
		cc = crc16(&moto_data[10+aa],45);
		if((bb!=cc)||(moto_data[1+aa]>0x80))            ///??08?00
		{
			errflag[errnum] = 1;moto_data[aa]=0xff;
		}
		else
		{
			errflag[errnum] = 0;
		}
		 errnum = (errnum+1)%50;     //最近50次伺服器状态记录                                               
		 for(errcount=0;errcount<50;errcount++)
		 {
		     errsum += errflag[errcount];
		 }
		 if(errsum > 45)        //最近1秒内，有45次电机错误，则保护
		 {
				gInnerData->FatalErr |= 0x01;
		 }
		 else
		 {			 
			 gInnerData->FatalErr = 0;
		 }
		 gHalData->WheelHal[num].FbbPos=(s32)((moto_data[aa+21]<<24)|(moto_data[aa+22]<<16)|(moto_data[aa+23]<<8)|(moto_data[aa+24]));
		 gHalData->WheelHal[num].FbPos=(double)(gHalData->WheelHal[num].FbbPos* gInnerData->WheelPeri*gHalData->WheelHal[num].FlagForward /(gInnerData->GearScale * gInnerData->MotorEncoder));
   //if(JudgeEncrypt()==0)while(1);//why             
                 
                 
     if(0 == firstflag[num])
		 {
       gHalData->WheelHal[num].LastFbpos	= gHalData->WheelHal[num].FbPos;
			 firstflag[num] = 1;
		 }
		 actpos[num] = (gHalData->WheelHal[num].FbPos - gHalData->WheelHal[num].LastFbpos);///??????????(mm)
		 gHalData->WheelHal[num].LastFbpos	= gHalData->WheelHal[num].FbPos;
	}
	baseth = (actpos[1] - actpos[0])/gInnerData->Length;//单位时间内两轮差距除以轮距获得弧度值(rad)
  length = (actpos[0] + actpos[1]) / 2.0 ;      
  gComData->FbPosx +=length;
	radcount+=baseth;
  gComData->FbPosthe =(s16)(radcount*100);   //弧度修正
}


/****************************************************************************
* 名    称：void moto-go(void)
* 功    能：
* 入口参数：gComData->CmdVxgo直行速度  gComData->CmdVthe转弯角度
* 出口参数：
* 说    明：根据前进速度和弧速度控制伺服动作
* 调用方法：无 
****************************************************************************/
void moto_go(void)              /// 2015-04-22改为mm/s
{
	#if 0
        s16 speed,fangx,speed1 ;
	static int num = 0,num1=0;
        static int count =0; 
        static u8 jt=0;
        
        speed=gComData->CmdVxgo;
        fangx=gComData->CmdVthe;
        if(gComData->magon&&(!gInnerData->OnMag)) ///无磁条时，根据是否寻磁，决定是否减速。
        {
          count+=6;                                     ///无磁条时的减速因子。
          speed=speed-count;
          if(speed<=0){speed=0;count=gComData->CmdVxgo-6;}
 
        }
        else count=0;
       
        speed1=	jv_js();
        if(speed1==0&&speed!=0){if(num==1)MP3CRBF(252);num++;}if((speed1==10)&&(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)==1))num=0;
        //环境监测，采样接近开关和碰撞开关
	if(speed1>8){	if(0x00 == GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0))  //避障，减速
		{
                  if(num==1){if(gComData->LastCmdVx>80){COM3_SendHeadDis(0x80);  
									SendHeadDis(12);}if(gComData->LastCmdVx>190){MP3CRBF(252);}} ///第一次遇障时提示。
                        num++;
                }
		else
		{
                  if(num) {  num=0;}             ///MP3关机。
                 SendHeadDis(0x80);  SendHeadDis(14);

		}

        if(num&&(gComData->LastCmdVx>50))speed=40;        ///遇障时的速度。
        }else {if(gComData->LastCmdVx>80){SendHeadDis(0x80);  SendHeadDis(12);}}
        speed=(speed1*speed)/10;if(speed1!=10){if(speed!=0&&speed<=50)speed=50;}        //消除磨蹭。
if((num==0)&&(speed1<5)&&(speed>110)&&(num1==1)){num1++;MP3CRBF(252);}else num1=0;
//if ((0x00 == GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) )&&(speed!=0)) {jt=1;}	        //紧急停车，暂时不用。PC1用于电源模块通讯。
if(num>140||jt==1)       //防碰撞  或者长时间遇阻停车
	{
	    gInnerData->ScaleEStop = 0; COM3_SendHeadDis(0x80);  COM3_SendHeadDis(12);
	}
	else
	{
	    gInnerData->ScaleEStop = 1;
	} 
  if(0x00 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) && 0x00 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1))  //完成
  {jt=0;}
   if (jt)gUpData->FbErr   |=0X10;else gUpData->FbErr   &=~0X10;
   
   //if(JudgeEncrypt()==0)while(1);//why
   
	//对th方向速度进行加减速控制
	if(fangx > gComData->LastCmdVthe + 4)
	{
	    fangx = gComData->LastCmdVthe + 4;
	}
	else if(fangx < gComData->LastCmdVthe - 4)
	{
	    fangx = gComData->LastCmdVthe - 4;
	}
	if(fangx > 100)
	{
	    fangx = 100;
	}
	else if(fangx < -100)
	{
	    fangx = -100;
	}

	gComData->LastCmdVthe	= fangx;    ///当前弧度保存


	//对x方向速度进行加减速控制                         提速限速
	if(speed > gComData->LastCmdVx +  3)            ///6
	{
	    speed = gComData->LastCmdVx + 3;            ///6
	}
	else if(speed < gComData->LastCmdVx - 9)            ///减速限速
	{
	    speed = gComData->LastCmdVx - 9;
	}

	if(speed>12000)speed=1000;
	
        
         
	gComData->LastCmdVx	= speed;      ///速度保存
	speed	*= gInnerData->ScaleEStop;      ///紧急停车开关
        /// gComData->CmdVthe *= gInnerData->ScaleEStop;  ///障碍不影响转向

	//计算四个轮子的速度,单位是mm/s
	gHalData->WheelHal[0].CmdVel = (speed - fangx * (gInnerData->Length)/200);
	gHalData->WheelHal[1].CmdVel = (speed + fangx * (gInnerData->Length)/200);  
        moto(0);

 


/*	for(char num = 0; num < MOTORNUM; num++)
	{
	    if(0 == gInnerData->FatalErr)       ///根據車速，速比，輪周長，和方向計算 電機轉速。
		{
	        gHalData->WheelHal[num].CmdMotorvel = (int)(gHalData->WheelHal[num].CmdVel * gInnerData->GearScale * 60  * gHalData->WheelHal[num].FlagForward / gInnerData->WheelPeri);
			 
		}
		else
		{
			gHalData->WheelHal[num].CmdMotorvel = 0;
			GPIO_SetBits(GPIOE, GPIO_Pin_2);
		}
	}
 */
#endif
}


void COM6_Task(void *pdata)
{
	pdata = pdata;
	while(1)
	{
			OSTimeDlyHMSM(0,0,0,5);
	}
}

