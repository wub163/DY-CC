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
while(RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);  //等待随机数准备完毕
return RNG_GetRandomNumber();   //读数

}
//与驱动器之间485通信的crc校验函数
//CSM注释：未使用
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
* 名    称：void System_Init(void)
* 功    能：initiate the controller software environment,and config the inner data
* 入口参数：null
* 出口参数：null
* 说    明：
* 调用方法：null 
****************************************************************************/
void System_Init(void)
{
	#if 0
u16 count;

	//分配内存，并初始化系统参数
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
        
	//设置小车的几何参数///伺服配置错误会使回馈数据错误。转向不准
	gInnerData->Length = 425;		//340		
	gInnerData->WheelPeri = (s16)(125*3.1415926); ///轮周
	gInnerData->GearScale = 1.0;           //表示电机到轮子的齿轮比35:中大电机：10级电机，30：1  外轴900个脉冲一圈。
	gInnerData->MotorEncoder = 900;       //一周脉冲数

	gInnerData->MagDist =10;      ///10mm


	gHalData->WheelHal[0].FlagForward = -1;
	gHalData->WheelHal[1].FlagForward = 1;

	gComData->FbPosthe = 0;          ///旋转弧度值
	gComData->State = 0;            //自动状态    小车操作方式0：暂停 1.手动 2.自动
	gComData->qcd = 0;   //取菜点
	gComData->back=1;               //送餐完毕时，前进还是返回，。1，返回。 -1前进 （环形跑道，）
	gComData->magon=0;              //需要寻磁时，无磁无法直行
	gInnerData->State = 0;         ///开机回取餐点

	MotorDrivers_Init(); //电机参数初始化
  #endif
}






/****************************************************************************
* 名    称：void Status_Handle(void)
* 功    能：
* 入口参数：gHalData->Station卡号===>查出座号，和转向标志。
* 出口参数：gInnerData->ScaleObst,gInnerData->Station,gHalData->InPosition,gComData->TurnFinish
* 说    明：
* 调用方法：无 
****************************************************************************/
//卡号识别

void Status_Handle(void)
{
 
if(0x00 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) || 0x00 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1))  //完成
	{
	    gInnerData->FinishGive = 1;
	}
	else 
	{
	    gInnerData->FinishGive = 0;
	}
        
          if(ZhuoHaoData[gComData->CmdStation].ID==gHalData->Station)
	{
          gInnerData->InPosition = 1;///到位
      return;
        }///座号查询，是否到位。
       gInnerData->InPosition = 0;     ///不是座号卡，则查询是否是方向卡或限速卡。
}



void FCXS(void)   ///gComData->CmdVx=>gComData->CmdVxgoxs=>gComData->CmdVxgo
{	
u8 i;
  static  u16  xsz,count = 200,zxxsi=200;//转向限速，转向限时。如果前方没有所要餐桌，强制回取餐点。转向限时回程使用和关闭转向开关。。
  static u16 dzjs;
  static int dzdd=0,ddkh =0;    //独占等待，独占卡号
  if(gDownData->fx){gInnerData->fx=gDownData->fx;gDownData->fx=0;zxxsi=0; if(gComData->CmdVxgoxs>200)gComData->CmdVxgo=200;}
  if((gInnerData->fx)&&(!jv_js())){zxxsi=300;dzjs=300;}                     //转向时避障，参数保持。
  if(gInnerData->sbjq[gUpData->DZ]){if(gInnerData->sbjq[gUpData->DZ]<gInnerData->ID){ddkh=gHalData->Station;dzdd=1;gComData->CmdVxgo=0;gUpData->DZ=0;zxxsi=300;return;}}//同入

  gComData->CmdVxgoxs=gComData->CmdVx;
 if((gComData->CmdVx>(xsz*100))&&(xsz!=0))gComData->CmdVxgoxs=xsz*100;
 dzjs++; if (dzjs>500){dzjs=500;gUpData->DZ=0;}   //DZ 撤销。10S计时。   
 
 
 if((dzdd)&&(ddkh)) {gHalData->Station=ddkh;ddkh=0;}else dzdd=0;      
count++;if (count>200)count=250;//防止重复。


zxxsi++;if (zxxsi>500){zxxsi=500;gInnerData->fx=0;gComData->CmdVxgo=gComData->CmdVxgoxs;}//转向开启最长5秒、
if(((gInnerData->State == 2)||(gInnerData->State == 21))&&(2 == gComData->State)){count=0;xsz=0;}
if(gHalData->Station==0)return; 

if(count>=250){
    for( i=0;i<xianshu;i++)    ///上位机区域不可为空，否则易跟取餐点混淆。
           {
                 if(XianShuData[i].ID==gHalData->Station)
                 {count=0;
                 if(xsz==0)xsz=XianShuData[i].UP;else xsz=0;
                  return;                 
                 }
           }}


if((zxxsi>=300)&&(2 == gComData->State)){                       //独占，转向作用状态。
for( i=0;i<fangxiang;i++)  
   
           {
                if(FangXiangData[i].ID==gHalData->Station)
                { 
                  if((gInnerData->sbjq[FangXiangData[i].DZ]==0)||(gInnerData->sbjq[FangXiangData[i].DZ]==gInnerData->ID)){if(FangXiangData[i].DZ<jinqu){gUpData->DZ=FangXiangData[i].DZ;dzjs=0;ddkh=0;dzdd=0;}} //如果当前禁区为空，则进行独占。
                  else {ddkh=gHalData->Station;dzdd=1;gComData->CmdVxgo=0;gUpData->DZ=0;return;}// 不同入。

                  if(((gComData->CmdStation)>=FangXiangData[i].LL)&&((gComData->CmdStation)<=FangXiangData[i].LH))
                      {gInnerData->fx=1; zxxsi=0;
                      if(gComData->CmdVxgoxs>200)gComData->CmdVxgo=200; return;}
                  if(((gComData->CmdStation)>=FangXiangData[i].RL)&&((gComData->CmdStation)<=FangXiangData[i].RH))
                      {gInnerData->fx=-1;zxxsi=0;
                      if(gComData->CmdVxgoxs>200)gComData->CmdVxgo=200; return;}
                  if(((gComData->CmdStation)>=FangXiangData[i].FL)&&((gComData->CmdStation)<=FangXiangData[i].FH))
                      {gInnerData->fx=0; return;}

//                      if(gInnerData->State!=0)gInnerData->State = 8;          ///找不到卡号，回取餐点。
                  return;}
           }}
}
/****************************************************************************
* 名    称：void Command_Handle(void)
* 功    能：calculate the wheels' speed based on cmd velocity 
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void Command_Handle(void)
{
		static s8 cdzx ;
		static u8 count;
		u8 xz;
		static unsigned char lastfinishgive = 0;
		//	static unsigned char tmpstation = 0;                    ///目标餐桌号，
		static double currentth = 0.0;
		if(gHalData->num < 4)
			xz=20;
		else 
			xz=12;
		if(1 == gComData->State)            ///小车手动模式
		{
				if(gComData->magon)
				{
					  gComData->CmdVx = abs(gComData->CmdVx);
						gComData->CmdVthe = (s16)(gInnerData->MagFIOX*xz/10);
				} ///如果寻磁，则延磁条前行。
//				if((ktime<2)&&(gUpData->needup==1))//需要实现
//					gInnerData->MpNum =253;
				count++;
				if (count>200)
					count=200;
				if (count>16)
				{
				   count=0;
				//if(0x00 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))MP3yl (05);//通知MP3任务播放音乐
				//if(0x00 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1))MP3yl (06);
				}

		}
    else if(2 == gComData->State)            ///小车自动模式
	  {
				switch (gInnerData->State)      ///回取菜点
				{
				case 0:       
						gComData->CmdStation=gComData->qcd;
						gComData->CmdVx = gInnerData->MaxVx;                        ///开始为0，原地旋转寻磁。上位机下发后前进。
						gComData->CmdVthe = (s16)(gInnerData->MagFIOX*xz/10);       ///以最大速度，磁导航方式来到取菜点。
						//如果达到起始点，则进入等待上菜阶段
						if(1 == gInnerData->InPosition)
						{
							 ///到达目的，进入等待，播放音乐
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
						//等待旋转完成90度
						gComData->CmdVx = 0.0;
						gComData->CmdVthe =40*gInnerData->TurnFlag;
						if(fabs(gComData->FbPosthe - currentth) >(jd_hd(ZhuoHaoData[gComData->qcd].ZJD)) )         ///过冲0.12
						{       
							  givetable=0;
								gInnerData->State = 1;
								gInnerData->MpNum = 251;
								cdzx=gInnerData->TurnFlag;

						}
				    break;               
                        
				case 1:  ///获得座号，启动出发

				gComData->CmdVx = 0.0;
				gComData->CmdVthe = 0.0;
				//如果give table,next stage
				if(1 == givetable )                                     ///等待，获得目标后，清除在位标志进入下一步回转流程
				{
				if(ZhuoHaoData[gComData->qcd].ZJD==0)  gInnerData->State = 2;
																					else gInnerData->State = 15;
				gComData->CmdStation = gDownData->CmdTable;
				currentth = gComData->FbPosthe;                ///因为要计算旋转角度，。所以要保存电机位置
				gInnerData->InPosition = 0;
				 givetable=0;                       
				}
				break;

				case 15:
				gComData->CmdVx = 0.0;
				gComData->CmdVthe = 40*cdzx*ZhuoHaoData[gComData->qcd].ZFH;
				if(fabs(gComData->FbPosthe - currentth) > 20 && abs(gInnerData->MagFIOX) < 25) ///大于20 让过当前磁道
				{
				gInnerData->State = 3;

				 gInnerData->MpNum = gDownData->bjmp3;          //沿途播放功能                                   
				}
				break;
				case 2:
				gComData->CmdVx = 0.0;
				if(ZhuoHaoData[gComData->qcd].ZFH==-1)gComData->CmdVthe = 0;else gComData->CmdVthe = 40;
				//原地旋转180度，以实现获取菜后，如果旋转完成，则直行去目标餐桌
				if((((gComData->FbPosthe - currentth) > 260) &&(abs(gInnerData->MagFIOX) < 25))||(ZhuoHaoData[gComData->qcd].ZFH==-1))
				{
				gInnerData->State = 3;
				gInnerData->MpNum = gDownData->bjmp3;        //沿途播放功能                                   
				}
				break;
				case 3:
				gComData->CmdVx = gInnerData->MaxVx;
				gComData->CmdVthe = (s16)(gInnerData->MagFIOX*xz/10);
				//如果到达目标餐桌，则停止
				if(1 == gInnerData->InPosition )
				{
				gInnerData->State = 4;
				}
				break;
				case 4:
				//等待完全停止下来
				gComData->CmdVx = 0.0;
				gComData->CmdVthe = 0.0;
				//如果停止下来后，则进行原地旋转90度
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
				//等待旋转完成90度
				gComData->CmdVx = 0.0;
				gComData->CmdVthe =40*gInnerData->TurnFlag;
				if((fabs(gComData->FbPosthe - currentth) >zhuanjiao )||(gInnerData->TurnFlag==0))         ///过冲0.12
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



				if(0 == gInnerData->FinishGive && 1 == lastfinishgive)          ///完成键按下并释放时进入。
				{
				gInnerData->State = 7;
				currentth = gComData->FbPosthe;
				gInnerData->MpNum = 249;                                    //转身时说谢谢

				}
				lastfinishgive = gInnerData->FinishGive;                         ///这条指令为了进入上面的判断
				break;
				case 7:
				gComData->CmdVx = 0.0;
				gComData->CmdVthe = 40*gInnerData->TurnFlag*ZhuoHaoData[gDownData->CmdTable].ZFH;if((gInnerData->TurnFlag==0)&&(ZhuoHaoData[gDownData->CmdTable].ZFH==1))gComData->CmdVthe = 40;
				if((fabs(gComData->FbPosthe - currentth) > 20 && abs(gInnerData->MagFIOX) < 25)||(gInnerData->TurnFlag==0)) ///大于20 让过当前磁道
				{
				gInnerData->State = 8;
				gInnerData->MpNum = gDownData->bjmp3;
				//                                if(abs(gDownData->Angle)>2){gInnerData->MpNum = 3;}                                           ////            5
				}
				break;
				case 8: ///回取菜点
				gInnerData->State = 0;
				gComData->CmdStation = gComData->qcd;
				gInnerData->InPosition = 0;
				break;



				}

				}
}
/****************************************************************************
* 名    称：void peizhi(void)
* 功    能：send the controller's status to upper level
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void peizhi(u8 txdata[])
{
	#if 0
  u16 count;
      if(txdata[2]==0x70)    ///设置包
        {
            switch(txdata[3])
              {
              case 11:          //座位开
                for (u8 i=0;i<txdata[4];i++ )
                {
//                  memcpy(&(ZhuoHaoData[g_RxUpBuf[8+i*8]].ID),&g_RxUpBuf[4+i*8],4);      ///ID 赋值
                  ZhuoHaoData[txdata[9+i*8]].ID=*((u32*)(&txdata[5+i*8]));
                  ZhuoHaoData[txdata[9+i*8]].ZJD=txdata[10+i*8];
                  ZhuoHaoData[txdata[9+i*8]].ZJL=txdata[11+i*8];
                  ZhuoHaoData[txdata[9+i*8]].ZFH=txdata[12+i*8];  
                  
                }
                    break;

              case 22:          //方向卡
                for (u8 i=0;i<txdata[4];i++ )
                {
//                  memcpy(&(ZhuoHaoData[g_RxUpBuf[8+i*8]].ID),&g_RxUpBuf[4+i*8],4);      ///ID 赋值
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

              case 33:          //限速卡
                for (u8 i=0;i<txdata[4];i++ )
                {
//                  memcpy(&(ZhuoHaoData[g_RxUpBuf[8+i*8]].ID),&g_RxUpBuf[4+i*8],4);      ///ID 赋值
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

u8 jv_js(void)                                          //减速因子1-10，最低不小于0.1m/s
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




void dzbwh(void)//独占表维护。
{
	#if 0
for(int i = 0; i < shebei; ++i)
{
          robot[i]++;                          //数据表不稳定，无法自清除。自清除计时      
          if(robot[i]>250){gInnerData->sbwz[i+1]=0;robot[i]=253;}
}
        memset(gInnerData->sbjq,0,sizeof(gInnerData->sbjq));
        for(int i = 1; i < shebei+1; i++)
        {
          gInnerData->sbjq[gInnerData->sbwz[i]]=i;
        }
gInnerData->sbjq[0]=0;
 //直接生成设备禁区，则无法恢复，所以先生成位置禁区，然后，生成设备禁区。
				#endif
}