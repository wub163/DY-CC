#include "common.h"
u8 zhuanjiao,givetable = 0;

BaseSysSt* gSysData;
BaseComSt* gComData;
BaseInnerSt* gInnerData;
BaseHalSt* gHalData;
BaseDownSt* gDownData;
BaseUpSt* gUpData;

ZhuoHaoSt*   ZhuoHaoData;
FangXiangSt* FangXiangData;
XianShuSt*   XianShuData;
u8 pccmd[33];
u8 cmd[33], stat[5][33];
void RNG_init (void)
{
RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG,ENABLE);
RNG_Cmd(ENABLE);
}

u32 SRNG (void)
{
while(RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);  //�ȴ������׼�����
return RNG_GetRandomNumber();   //����

}
//��������֮��485ͨ�ŵ�crcУ�麯��
//CSMע�ͣ�δʹ��
unsigned int RTU_CRC(unsigned char *data,unsigned char length)
{
   u8 j = 0;
	 unsigned int temp_crc = 0;
	 unsigned int reg_crc = 0xffff;
	 while(length--)
	 {
	     reg_crc ^= *data++;
		 for(j=0;j<8;j++)
		 {
		     if(reg_crc&0x01)
			 {
			     reg_crc=(reg_crc>>1)^0xa001;
			 }
			 else
			 {
			     reg_crc = reg_crc>>1;
			 }
		 }
	 }
	 temp_crc = reg_crc;
	 reg_crc = ((reg_crc<<8)&0xff00);
	 temp_crc = ((temp_crc>>8)&0x00ff);
	 reg_crc = reg_crc + temp_crc;
	 return reg_crc;
}
short jd_hd(char da)
{
s16 aa;
aa=abs(da)*628/36-abs((s8)((gComData->CmdVthe/10)*3));
if(aa<80)aa=abs(da)*628/36;
return aa;
}

/****************************************************************************
* ��    �ƣ�void System_Init(void)
* ��    �ܣ�initiate the controller software environment,and config the inner data
* ��ڲ�����null
* ���ڲ�����null
* ˵    ����
* ���÷�����null 
****************************************************************************/
void System_Init(void)
{
	#if 0
u16 count;

	//�����ڴ棬����ʼ��ϵͳ����
	gSysData = (BaseSysSt*)malloc(sizeof(BaseSysSt));
	memset(gSysData,0,sizeof(BaseSysSt));
	gComData = &(gSysData->BaseComData);
	gInnerData = &(gSysData->BaseInnerData);
	gHalData = &(gSysData->BaseHalData);
	gDownData = &(gSysData->BaseDownData);
	gUpData = &(gSysData->BaseUpData);

	ZhuoHaoData  = &(gSysData->BaseZhuoHaoData[0]);    //80*8
	FangXiangData = &(gSysData->BaseFangXiangData[0]);//40*12
	XianShuData  = &(gSysData->BaseXianShuData[0]);    //20*8     
	//        DuZhanData   = &(gSysData->BaseDuZhanData[0]);     //20*12

	count=(sizeof(ZhuoHaoSt)*zhuohao+sizeof(FangXiangSt)*fangxiang+sizeof(XianShuSt)*xianshu);
	for (u16 ai=0;ai<count;ai++)
	{            
	*((u32*)(&ZhuoHaoData[0])+ai)=*((u32*)(0x08008000+ai*4));
	}          
        
	//����С���ļ��β���///�ŷ����ô����ʹ�������ݴ���ת��׼
	gInnerData->Length = 425;		//340		
	gInnerData->WheelPeri = (s16)(125*3.1415926); ///����
	gInnerData->GearScale = 1.0;           //��ʾ��������ӵĳ��ֱ�35:�д�����10�������30��1  ����900������һȦ��
	gInnerData->MotorEncoder = 900;       //һ��������

	gInnerData->MagDist =10;      ///10mm


	gHalData->WheelHal[0].FlagForward = -1;
	gHalData->WheelHal[1].FlagForward = 1;

	gComData->FbPosthe = 0;          ///��ת����ֵ
	gComData->State = 0;            //�Զ�״̬    С��������ʽ0����ͣ 1.�ֶ� 2.�Զ�
	gComData->qcd = 0;   //ȡ�˵�
	gComData->back=1;               //�Ͳ����ʱ��ǰ�����Ƿ��أ���1�����ء� -1ǰ�� �������ܵ�����
	gComData->magon=0;              //��ҪѰ��ʱ���޴��޷�ֱ��
	gInnerData->State = 0;         ///������ȡ�͵�

	MotorDrivers_Init(); //���������ʼ��
  #endif
}






/****************************************************************************
* ��    �ƣ�void Status_Handle(void)
* ��    �ܣ�
* ��ڲ�����gHalData->Station����===>������ţ���ת���־��
* ���ڲ�����gInnerData->ScaleObst,gInnerData->Station,gHalData->InPosition,gComData->TurnFinish
* ˵    ����
* ���÷������� 
****************************************************************************/
//����ʶ��

void Status_Handle(void)
{
 
if(0x00 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) || 0x00 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1))  //���
	{
	    gInnerData->FinishGive = 1;
	}
	else 
	{
	    gInnerData->FinishGive = 0;
	}
        
          if(ZhuoHaoData[gComData->CmdStation].ID==gHalData->Station)
	{
          gInnerData->InPosition = 1;///��λ
      return;
        }///���Ų�ѯ���Ƿ�λ��
       gInnerData->InPosition = 0;     ///�������ſ������ѯ�Ƿ��Ƿ��򿨻����ٿ���
}



void FCXS(void)   ///gComData->CmdVx=>gComData->CmdVxgoxs=>gComData->CmdVxgo
{	
u8 i;
  static  u16  xsz,count = 200,zxxsi=200;//ת�����٣�ת����ʱ�����ǰ��û����Ҫ������ǿ�ƻ�ȡ�͵㡣ת����ʱ�س�ʹ�ú͹ر�ת�򿪹ء���
  static u16 dzjs;
  static int dzdd=0,ddkh =0;    //��ռ�ȴ�����ռ����
  if(gDownData->fx){gInnerData->fx=gDownData->fx;gDownData->fx=0;zxxsi=0; if(gComData->CmdVxgoxs>200)gComData->CmdVxgo=200;}
  if((gInnerData->fx)&&(!jv_js())){zxxsi=300;dzjs=300;}                     //ת��ʱ���ϣ��������֡�
  if(gInnerData->sbjq[gUpData->DZ]){if(gInnerData->sbjq[gUpData->DZ]<gInnerData->ID){ddkh=gHalData->Station;dzdd=1;gComData->CmdVxgo=0;gUpData->DZ=0;zxxsi=300;return;}}//ͬ��

  gComData->CmdVxgoxs=gComData->CmdVx;
 if((gComData->CmdVx>(xsz*100))&&(xsz!=0))gComData->CmdVxgoxs=xsz*100;
 dzjs++; if (dzjs>500){dzjs=500;gUpData->DZ=0;}   //DZ ������10S��ʱ��   
 
 
 if((dzdd)&&(ddkh)) {gHalData->Station=ddkh;ddkh=0;}else dzdd=0;      
count++;if (count>200)count=250;//��ֹ�ظ���


zxxsi++;if (zxxsi>500){zxxsi=500;gInnerData->fx=0;gComData->CmdVxgo=gComData->CmdVxgoxs;}//ת�����5�롢
if(((gInnerData->State == 2)||(gInnerData->State == 21))&&(2 == gComData->State)){count=0;xsz=0;}
if(gHalData->Station==0)return; 

if(count>=250){
    for( i=0;i<xianshu;i++)    ///��λ�����򲻿�Ϊ�գ������׸�ȡ�͵������
           {
                 if(XianShuData[i].ID==gHalData->Station)
                 {count=0;
                 if(xsz==0)xsz=XianShuData[i].UP;else xsz=0;
                  return;                 
                 }
           }}


if((zxxsi>=300)&&(2 == gComData->State)){                       //��ռ��ת������״̬��
for( i=0;i<fangxiang;i++)  
   
           {
                if(FangXiangData[i].ID==gHalData->Station)
                { 
                  if((gInnerData->sbjq[FangXiangData[i].DZ]==0)||(gInnerData->sbjq[FangXiangData[i].DZ]==gInnerData->ID)){if(FangXiangData[i].DZ<jinqu){gUpData->DZ=FangXiangData[i].DZ;dzjs=0;ddkh=0;dzdd=0;}} //�����ǰ����Ϊ�գ�����ж�ռ��
                  else {ddkh=gHalData->Station;dzdd=1;gComData->CmdVxgo=0;gUpData->DZ=0;return;}// ��ͬ�롣

                  if(((gComData->CmdStation)>=FangXiangData[i].LL)&&((gComData->CmdStation)<=FangXiangData[i].LH))
                      {gInnerData->fx=1; zxxsi=0;
                      if(gComData->CmdVxgoxs>200)gComData->CmdVxgo=200; return;}
                  if(((gComData->CmdStation)>=FangXiangData[i].RL)&&((gComData->CmdStation)<=FangXiangData[i].RH))
                      {gInnerData->fx=-1;zxxsi=0;
                      if(gComData->CmdVxgoxs>200)gComData->CmdVxgo=200; return;}
                  if(((gComData->CmdStation)>=FangXiangData[i].FL)&&((gComData->CmdStation)<=FangXiangData[i].FH))
                      {gInnerData->fx=0; return;}

//                      if(gInnerData->State!=0)gInnerData->State = 8;          ///�Ҳ������ţ���ȡ�͵㡣
                  return;}
           }}
}
/****************************************************************************
* ��    �ƣ�void Command_Handle(void)
* ��    �ܣ�calculate the wheels' speed based on cmd velocity 
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void Command_Handle(void)
{
		static s8 cdzx ;
		static u8 count;
		u8 xz;
		static unsigned char lastfinishgive = 0;
		//	static unsigned char tmpstation = 0;                    ///Ŀ������ţ�
		static double currentth = 0.0;
		if(gHalData->num < 4)
			xz=20;
		else 
			xz=12;
		if(1 == gComData->State)            ///С���ֶ�ģʽ
		{
				if(gComData->magon)
				{
					  gComData->CmdVx = abs(gComData->CmdVx);
						gComData->CmdVthe = (s16)(gInnerData->MagFIOX*xz/10);
				} ///���Ѱ�ţ����Ӵ���ǰ�С�
//				if((ktime<2)&&(gUpData->needup==1))//��Ҫʵ��
//					gInnerData->MpNum =253;
				count++;
				if (count>200)
					count=200;
				if (count>16)
				{
				   count=0;
				//if(0x00 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))MP3yl (05);//֪ͨMP3���񲥷�����
				//if(0x00 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1))MP3yl (06);
				}

		}
    else if(2 == gComData->State)            ///С���Զ�ģʽ
	  {
				switch (gInnerData->State)      ///��ȡ�˵�
				{
				case 0:       
						gComData->CmdStation=gComData->qcd;
						gComData->CmdVx = gInnerData->MaxVx;                        ///��ʼΪ0��ԭ����תѰ�š���λ���·���ǰ����
						gComData->CmdVthe = (s16)(gInnerData->MagFIOX*xz/10);       ///������ٶȣ��ŵ�����ʽ����ȡ�˵㡣
						//����ﵽ��ʼ�㣬�����ȴ��ϲ˽׶�
						if(1 == gInnerData->InPosition)
						{
							 ///����Ŀ�ģ�����ȴ�����������
								gComData->CmdVx = 0.0;
								gComData->CmdVthe = 0.0;
								if(ZhuoHaoData[gComData->qcd].ZJD==0)  
								{
									 gInnerData->MpNum = 251;
									 gInnerData->State = 1;
									 givetable=0;
								}
								else 
									gInnerData->State = 11;
						}
				    break;
				case 11:
						gComData->CmdVx = 0.0;
						gComData->CmdVthe = 0.0;
				    if(abs(gComData->LastCmdVx) < 5)   //CSM:MOD  20151108
						{
							gInnerData->State = 12;
						  currentth = gComData->FbPosthe;
						}
						if(ZhuoHaoData[gComData->qcd].ZJD<0)
            {
						   gInnerData->TurnFlag=-1;
						}
						else 
							gInnerData->TurnFlag=1;
    				break;

				case 12:
						//�ȴ���ת���90��
						gComData->CmdVx = 0.0;
						gComData->CmdVthe =40*gInnerData->TurnFlag;
						if(fabs(gComData->FbPosthe - currentth) >(jd_hd(ZhuoHaoData[gComData->qcd].ZJD)) )         ///����0.12
						{       
							  givetable=0;
								gInnerData->State = 1;
								gInnerData->MpNum = 251;
								cdzx=gInnerData->TurnFlag;

						}
				    break;               
                        
				case 1:  ///������ţ���������

				gComData->CmdVx = 0.0;
				gComData->CmdVthe = 0.0;
				//���give table,next stage
				if(1 == givetable )                                     ///�ȴ������Ŀ��������λ��־������һ����ת����
				{
				if(ZhuoHaoData[gComData->qcd].ZJD==0)  gInnerData->State = 2;
																					else gInnerData->State = 15;
				gComData->CmdStation = gDownData->CmdTable;
				currentth = gComData->FbPosthe;                ///��ΪҪ������ת�Ƕȣ�������Ҫ������λ��
				gInnerData->InPosition = 0;
				 givetable=0;                       
				}
				break;

				case 15:
				gComData->CmdVx = 0.0;
				gComData->CmdVthe = 40*cdzx*ZhuoHaoData[gComData->qcd].ZFH;
				if(fabs(gComData->FbPosthe - currentth) > 20 && abs(gInnerData->MagFIOX) < 25) ///����20 �ù���ǰ�ŵ�
				{
				gInnerData->State = 3;

				 gInnerData->MpNum = gDownData->bjmp3;          //��;���Ź���                                   
				}
				break;
				case 2:
				gComData->CmdVx = 0.0;
				if(ZhuoHaoData[gComData->qcd].ZFH==-1)gComData->CmdVthe = 0;else gComData->CmdVthe = 40;
				//ԭ����ת180�ȣ���ʵ�ֻ�ȡ�˺������ת��ɣ���ֱ��ȥĿ�����
				if((((gComData->FbPosthe - currentth) > 260) &&(abs(gInnerData->MagFIOX) < 25))||(ZhuoHaoData[gComData->qcd].ZFH==-1))
				{
				gInnerData->State = 3;
				gInnerData->MpNum = gDownData->bjmp3;        //��;���Ź���                                   
				}
				break;
				case 3:
				gComData->CmdVx = gInnerData->MaxVx;
				gComData->CmdVthe = (s16)(gInnerData->MagFIOX*xz/10);
				//�������Ŀ���������ֹͣ
				if(1 == gInnerData->InPosition )
				{
				gInnerData->State = 4;
				}
				break;
				case 4:
				//�ȴ���ȫֹͣ����
				gComData->CmdVx = 0.0;
				gComData->CmdVthe = 0.0;
				//���ֹͣ�����������ԭ����ת90��
				if(abs(gComData->LastCmdVx) < 5)
				{
				gInnerData->State = 5;
				currentth = gComData->FbPosthe;
				if(ZhuoHaoData[gDownData->CmdTable].ZJD<0){gInnerData->TurnFlag=-1;}
				else gInnerData->TurnFlag=1;
				if(ZhuoHaoData[gDownData->CmdTable].ZJD==0){gInnerData->TurnFlag=0;}
				zhuanjiao=(jd_hd(ZhuoHaoData[gDownData->CmdTable].ZJD));
				}
				break;
				case 5:
				//�ȴ���ת���90��
				gComData->CmdVx = 0.0;
				gComData->CmdVthe =40*gInnerData->TurnFlag;
				if((fabs(gComData->FbPosthe - currentth) >zhuanjiao )||(gInnerData->TurnFlag==0))         ///����0.12
				{
				gInnerData->State = 6;
				gInnerData->MpNum = (u8)gDownData->mp3;


				}
				break;
				case 6:
				gComData->CmdVx = 0.0;
				gComData->CmdVthe = 0.0;
				SendHeadDis(0x80);  
				SendHeadDis(13);



				if(0 == gInnerData->FinishGive && 1 == lastfinishgive)          ///��ɼ����²��ͷ�ʱ���롣
				{
				gInnerData->State = 7;
				currentth = gComData->FbPosthe;
				gInnerData->MpNum = 249;                                    //ת��ʱ˵лл

				}
				lastfinishgive = gInnerData->FinishGive;                         ///����ָ��Ϊ�˽���������ж�
				break;
				case 7:
				gComData->CmdVx = 0.0;
				gComData->CmdVthe = 40*gInnerData->TurnFlag*ZhuoHaoData[gDownData->CmdTable].ZFH;if((gInnerData->TurnFlag==0)&&(ZhuoHaoData[gDownData->CmdTable].ZFH==1))gComData->CmdVthe = 40;
				if((fabs(gComData->FbPosthe - currentth) > 20 && abs(gInnerData->MagFIOX) < 25)||(gInnerData->TurnFlag==0)) ///����20 �ù���ǰ�ŵ�
				{
				gInnerData->State = 8;
				gInnerData->MpNum = gDownData->bjmp3;
				//                                if(abs(gDownData->Angle)>2){gInnerData->MpNum = 3;}                                           ////            5
				}
				break;
				case 8: ///��ȡ�˵�
				gInnerData->State = 0;
				gComData->CmdStation = gComData->qcd;
				gInnerData->InPosition = 0;
				break;



				}

				}
}
/****************************************************************************
* ��    �ƣ�void peizhi(void)
* ��    �ܣ�send the controller's status to upper level
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void peizhi(u8 txdata[])
{
	#if 0
  u16 count;
      if(txdata[2]==0x70)    ///���ð�
        {
            switch(txdata[3])
              {
              case 11:          //��λ��
                for (u8 i=0;i<txdata[4];i++ )
                {
//                  memcpy(&(ZhuoHaoData[g_RxUpBuf[8+i*8]].ID),&g_RxUpBuf[4+i*8],4);      ///ID ��ֵ
                  ZhuoHaoData[txdata[9+i*8]].ID=*((u32*)(&txdata[5+i*8]));
                  ZhuoHaoData[txdata[9+i*8]].ZJD=txdata[10+i*8];
                  ZhuoHaoData[txdata[9+i*8]].ZJL=txdata[11+i*8];
                  ZhuoHaoData[txdata[9+i*8]].ZFH=txdata[12+i*8];  
                  
                }
                    break;

              case 22:          //����
                for (u8 i=0;i<txdata[4];i++ )
                {
//                  memcpy(&(ZhuoHaoData[g_RxUpBuf[8+i*8]].ID),&g_RxUpBuf[4+i*8],4);      ///ID ��ֵ
                  FangXiangData[i].ID=*((u32*)(&txdata[5+i*11]));
                  FangXiangData[i].LL=txdata[9+i*11];
                  FangXiangData[i].LH=txdata[10+i*11];
                  FangXiangData[i].RL=txdata[11+i*11];
                  FangXiangData[i].RH=txdata[12+i*11];
                  FangXiangData[i].FL=txdata[13+i*11];
                  FangXiangData[i].FH=txdata[14+i*11];
                  FangXiangData[i].DZ=txdata[15+i*11];
//                  FangXiangData[i].BH=g_RxUpBuf[15+i*12];
                }
                break;

              case 33:          //���ٿ�
                for (u8 i=0;i<txdata[4];i++ )
                {
//                  memcpy(&(ZhuoHaoData[g_RxUpBuf[8+i*8]].ID),&g_RxUpBuf[4+i*8],4);      ///ID ��ֵ
                  XianShuData[i].ID=*((u32*)(&txdata[5+i*6]));
                  XianShuData[i].UP=txdata[9+i*6];
                  XianShuData[i].DN=txdata[10+i*6];
                  
                }
                break;

               

             }
         FLASH_Unlock();
          FLASH_ErasePage(0x08008000);
          count=sizeof(ZhuoHaoSt)*zhuohao+sizeof(FangXiangSt)*fangxiang+sizeof(XianShuSt)*xianshu;
          for ( u16 i=0;i<count;i=i+4)
          {            
            FLASH_ProgramWord(0x08008000+i,*((u32*)(&ZhuoHaoData[0])+i/4));
          }

          FLASH_Lock();
        }
				#endif
}

u8 jv_js(void)                                          //��������1-10����Ͳ�С��0.1m/s
{

u8 speed,speed1=10,speed2=10,speed3=10;
u16 bj=600,bstop=300,mj=1000,stop=400;
if(jl[6]>bj&&jr[6]>bj&&jm[6]>mj)speed=10;else
{
if((jl[6]<bj)&&(jl[6]>bstop))speed1=((jl[6]-bstop)*10/(bj-bstop));
if((jr[6]<bj)&&(jr[6]>bstop))speed2=((jr[6]-bstop)*10/(bj-bstop));
if((jm[6]<mj)&&(jm[6]>stop))speed3=((jm[6]-stop)*10/(mj-stop));
if(speed1<speed2)speed=speed1; else speed=speed2;
if(speed>speed3)speed=speed3;
if(jl[6]<=stop) speed=0;
if(jm[6]<=stop) speed=0;
if(jr[6]<=stop) speed=0;
}
return speed;
}




void dzbwh(void)//��ռ��ά����
{
	#if 0
for(int i = 0; i < shebei; ++i)
{
          robot[i]++;                          //���ݱ��ȶ����޷���������������ʱ      
          if(robot[i]>250){gInnerData->sbwz[i+1]=0;robot[i]=253;}
}
        memset(gInnerData->sbjq,0,sizeof(gInnerData->sbjq));
        for(int i = 1; i < shebei+1; i++)
        {
          gInnerData->sbjq[gInnerData->sbwz[i]]=i;
        }
gInnerData->sbjq[0]=0;
 //ֱ�������豸���������޷��ָ�������������λ�ý�����Ȼ�������豸������
				#endif
}