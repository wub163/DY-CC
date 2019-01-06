
#include "common.h"

u8 g_RxUpBuf[3][50] = {0};  //通信接收区
u8 ddd[33] = {0};
u8 testt[260] = {0};
static u8 tmpBuf[40] = {0};

volatile vu8 AGVERR = 1; //磁条传感器故障标志
volatile vu8 IDERR = 1;  //读卡器故障标志
volatile vu8 CSBERR = 1; //

void USART2_IRQHandler(void)
{static u8 in;
	u8 data;
//	OSIntEnter();
 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		data=USART_ReceiveData(USART2);
		        ddd[in] = data;
		        in++;
		    }
		
	
	else
	{
	    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
			{
                           data = USART_ReceiveData(USART2);
                           if((in>=32)&&(pccmd[32]!=0x58)){
                                memcpy(pccmd,ddd,32);pccmd[32]=0x58;}
                          in=0;
                       }
	}
//	OSIntExit();
}
void mkjc(u8 da)
{
	tmpBuf[0] = 0xFC;
	tmpBuf[1] = 0x00;
	tmpBuf[2] = 0x99;
	tmpBuf[3] = da;
  USART2_SendData(tmpBuf,4);
}
//CSM注释：未使用
void zigbee(void)
{
	while(1)
	{
		gInnerData->mkjs=1;
		mkjc(0x11);                    //查询自定义地址
		while(gInnerData->mkjs);
		if(g_RxUpBuf[0][1]==7)
		{
			gInnerData->mkjs=2;
				mkjc(0x16);                //查询自定义地址
			while(gInnerData->mkjs);
			gInnerData->ID=g_RxUpBuf[0][1];
			return;
		}
		//mtime=0; //why...
		//while(mtime<200);
  }
}
void Status_UpLoad(void)        
{
	int i = 0;
	char crc = 0;
	static u8 num = 0;
	static char DZ = 0;
	num++;
	CSBERR++;
	//(1)磁条传感器故障检测
	if (AGVERR == 1)
  {
	  gUpData->FbErr   |=0X40; 
		
	}
	else if(AGVERR == 0)
	{
		gUpData->FbErr   &=~0x40; 
	}
	AGVERR = 1;//设置磁条传感器故障
  //(2)读卡器故障检测
	if (IDERR == 1) 
  {
	  gUpData->FbErr   |=0X20;
	}
	else 	if (IDERR == 0) 
	{
		gUpData->FbErr   &=~0x20;
	}
	IDERR = 1;//设置为读卡有故障
	
	//(3)超声波模块故障检测,检测周围不能低于200ms
	if (CSBERR == 1)
  {
	  gUpData->FbErr   |=0X80;
	}
	else
	{		
		gUpData->FbErr   &=~0x80;
	}
	CSBERR = 1;
	

	gUpData->FbErr &=0xF0;
	gUpData->FbErr |=((moto_data[52]&=0x03)|((moto_data[112]&=0x03)<<2));
	if (moto_data[0]==0xff)
	{
		gUpData->FbErr |= 0x0F;
	}
	if (moto_data[60]==0xff)
	{
		gUpData->FbErr |= 0xF0;
	}

	if((num % 20 == 0)||((DZ==0)&&(gUpData->DZ!=0))) 
	{
	gUpData->Fbposx  =(s32)gComData->FbPosx ;
	gUpData->Fbposy  =(s32)gComData->FbPosy ;

	gUpData->Fbposth =gComData->FbPosthe;
	gUpData->ID      =gHalData->Station;   

	gUpData->FbVx    =gComData->CmdVx/10;		//?????????x??;
	gUpData->FbVthe  =gComData->CmdVthe;		//?????????theta??;
	gUpData->FbStatus=(gComData->State<<4)|(gInnerData->State&0x0f);                  //????????
		
	//gUpData->DL      =(s8)(pcs.dl/0.04);  //需要实现：小车电流
	gUpData->MagIo   =gHalData->forward;                   //?????

	//gUpData->Vcc     =(u8)(pcs.dy/0.15);  //需要实现：小车电池电压
  tmpBuf[0] = 0xFC;
	tmpBuf[1] = 0x00;
	tmpBuf[2] = 0xFF;
	tmpBuf[3] = 0xFF;
	tmpBuf[4] = 0x55;
 	tmpBuf[5] = 31;
	USART2_SendData(tmpBuf,6);
	for(i = 0;i < 28;i++)
	{
		tmpBuf[5+i] = *(((char*)&(gUpData->Fbposx))+i);
	  crc += *(((char*)&(gUpData->Fbposx))+i);
	}
	gUpData->FbCRC = crc;
	tmpBuf[33] = crc;
	tmpBuf[34] = 0xaa;
  USART2_SendData(tmpBuf,35);
	gUpData->FbErr=0; 

	}
	DZ = gUpData->DZ;  

}
//
//问题原因：numfifo下溢，包不完整，取零
//void peibao(void)
//{ 
//  u8 num1 = 0;
//	u8 packet=0;
//	int i = 0;
// 	if(numfifo < 14)
//	{
//		return;
//	}
//
//
//	while((ddd[out]!=0x55)&&(ddd[out]!=0x66))
//	{
//			out++;
//			numfifo--;
//			if(numfifo<13)
//				 return;
//	} 
//
//		if((ddd[out]==0x55)&&(ddd[out+11]==0xaa))
//		{			
//				packet = 1;
//		}				
//		else 
//		{
//				if((ddd[out]==0x55)&&(ddd[out+31]==0xaa))
//				{
//						 if(numfifo<34)
//								 return;
//							packet=2;
//				} 
//				else 
//				{
//							if((ddd[out]==0x66)&&(ddd[out+44]==0xaa))
//							{
//								 if(numfifo<47)
//										 return;
//								 packet=3;
//							} 
//							else 
//									ddd[out]=0;
//				}
//		}
//
//		if(packet)
//		{
//				for(num1 = 0; num1 < 3; num1++)  //FIXME
//				{
//						if(g_RxUpBuf[num1][0]!=0x5B)
//						{ 
//								g_RxUpBuf[num1][0]=0x5B;
//								switch (packet)
//								{
//								case 1:
//									for(i = 0; i < 14; ++i)
//									{
//									 while(!numfifo)GPIO_ResetBits(GPIOF, GPIO_Pin_7); g_RxUpBuf[num1][i+1]=ddd[out++];numfifo--; 
//									}
//
//									break;
//								case 2:
//									for(i = 0; i < 34; ++i)
//									{
//										while(!numfifo)GPIO_ResetBits(GPIOF, GPIO_Pin_7); g_RxUpBuf[num1][i+1]=ddd[out++];numfifo--; 
//									}
//
//									break;
//									
//								case 3:
//									for(i = 0; i < 47; ++i)
//									{
//									 while(!numfifo)GPIO_ResetBits(GPIOF, GPIO_Pin_9);  g_RxUpBuf[num1][i+1]=ddd[out++];numfifo--; 
//									 
//									}
//
//								}GPIO_SetBits(GPIOF, GPIO_Pin_7);GPIO_SetBits(GPIOF, GPIO_Pin_9);
//							 return;
//						}
//				}
//		}//if(packet)
//}
/****************************************************************************
* 名    称：void jiema(void)
* 功    能：接收缓冲区解码
* 入口参数：g_RxUpBuf【】 
* 出口参数：自动  手动   设置
* 说    明：
* 调用方法：无 
****************************************************************************/
void jiema(void)        //所有收到的包后都有两个地址码。
{
#if 0
	static u8 mem=0,test=0;
	u8 num1,crc,bc;
	u8 i,temp=0;
	u16 aaa;
	peibao();
	for(num1 = 0; num1 < 3; num1++)
	{if(g_RxUpBuf[num1][0]==0x5B)break;
	}  
	if(num1>=3)return;                      //无完整包，退出。

if(g_RxUpBuf[num1][1]==0x66){crc=g_RxUpBuf[num1][44];bc=44;}
else {crc=(g_RxUpBuf[num1][g_RxUpBuf[num1][2]]);bc=g_RxUpBuf[num1][2];
g_RxUpBuf[num1][46]=(g_RxUpBuf[num1][g_RxUpBuf[num1][2]+2]);
g_RxUpBuf[num1][47]=(g_RxUpBuf[num1][g_RxUpBuf[num1][2]+3]);}

for(i=3;i < bc;i++)
		{
		    temp+=g_RxUpBuf[num1][i];
		}
		if(temp == crc)
                {       
      if((g_RxUpBuf[num1][47]==00)&&(g_RxUpBuf[num1][46]==00)){                //判断是否是上位机指令
       SendChar(0x55);          //透传方式，只发给协调器。
       SendChar(0x05);
       SendChar(g_RxUpBuf[num1][2]);
       SendChar(crc);
       SendChar((u8)(g_RxUpBuf[num1][2]+crc));
       SendChar(0xAA);
          if(g_RxUpBuf[num1][1]==0x66)//判断是否为拼接包，申请内存拼包。
         {
         
         COM3_SendHeadDis(0x80);  COM3_SendHeadDis(12);
         if((g_RxUpBuf[num1][2]==1)&&(mem==0)){txdata=(u8*)malloc(g_RxUpBuf[num1][3]*44);mem=1;}//如果是首包，申请内存。
          for(int i = 0; i < 40; i++)
         {
           txdata[(g_RxUpBuf[num1][2]-1)*40+i]=g_RxUpBuf[num1][4+i];
          }
            if((g_RxUpBuf[num1][2])==g_RxUpBuf[num1][3])//拼包完成，解包。
            {aaa=(*((u16*)&txdata[0])-1);temp=0;
                for(u16 i=2;i < aaa;i++)         //大包检测
		{
		    temp+=txdata[i];
		}
		if(temp == txdata[(*((u16*)&txdata[0]))-1]){ peizhi(&txdata[0]);
                for(int i = 0; i < 20; ++i)
                {
                                   SendHeadDis(0x80);  SendHeadDis(13); 
                                 mtime=0;while(mtime<250);

                }

                
                
                }
                else {SendHeadDis(0x80);  SendHeadDis(12); }
 
 
                if(mem){free(txdata);mem=0;}
            }
         
         
         
         }         
       else{  if(g_RxUpBuf[num1][3]==0x72)    ///自动包  启动后，除餐桌号都可改
        {
                  //脱离手控模式，进入自动控制阶段
              gComData->State = 2; ///小车模式选择
              gInnerData->MaxVx =(s8)g_RxUpBuf[num1][5]*100;                ///速度
              if(gInnerData->MaxVx>500)gInnerData->MaxVx=500;
              gComData->magon=1;
              gDownData->bjmp3 =(u8)g_RxUpBuf[num1][6];                ///背景音乐             
              gDownData->mp3=*((u16*)&g_RxUpBuf[num1][7]);      ///MP3
              if(gDownData->mp3==0)gDownData->mp3=250;else gDownData->mp3=gDownData->mp3+50;
              //                memcpy(&(gDownData->mp3),&g_RxUpBuf[5],4);      ///MP3

              if(gInnerData->State == 1) {
               gDownData->CmdTable=g_RxUpBuf[num1][4];               ///餐桌号
                givetable = 1;}
                //if(JudgeEncrypt()==0)while(1);//why  

             if(g_RxUpBuf[num1][9]==0x88)gInnerData->State = 8;      //撤销送餐，返回取餐点
                if(g_RxUpBuf[num1][9]==0x66)gInnerData->FinishGive=1;   //服务完成。
             
        }
      if(g_RxUpBuf[num1][3]==0x71)    ///手动包
        {
          gComData->State = 1;                   ///小车手动模式   
	  if(g_RxUpBuf[num1][4]!=0x88)gComData->CmdVx = (s8)g_RxUpBuf[num1][4]*100;       //计算x方向的速度，单位0.1米每秒  0x88 维持原速
          gComData->magon= g_RxUpBuf[num1][5];                              
	  gComData->CmdVthe = (s8)g_RxUpBuf[num1][6]*10;     //计算theta方向的速度，1-10
//        memcpy(&(gDownData->mp3),&g_RxUpBuf[5],4);      ///MP3
          gDownData->mp3=*((u16*)&g_RxUpBuf[num1][7]);      ///MP3
          gInnerData->MpNum=gDownData->mp3;               ///mp3 0xff 静音
          if(gDownData->mp3==253)gUpData->needup=1;
          if(gDownData->mp3==254)gUpData->needup=0;
          gDownData->fx=(s8)g_RxUpBuf[num1][10];
        }}
      
       }
else

      {//=================互联数据
        if(g_RxUpBuf[num1][34]<=shebei){
        if(g_RxUpBuf[num1][34]!=gInnerData->ID){                  //如果是自己发出的包则跳过。
        gInnerData->sbwz[g_RxUpBuf[num1][34]]=g_RxUpBuf[num1][30];//所有设备所处禁区。
        if((g_RxUpBuf[num1][34]-1)<shebei+1)robot[(g_RxUpBuf[num1][34]-1)]=0;                          //数据表不稳定，无法自清除。自清除计时      
                                        }}
      }//==================互联数据
                }
g_RxUpBuf[num1][0]=0x0;                 //每次心跳，只处理一次缓冲区。
                
   #endif 
}
static uint8_t tstBuf[12] = {0xFD,0x00,0xFF,0xFF,1,2,3,4,5,6,7,8};
void COM2_Task(void *pdata)
{
	pdata = pdata;
	while(1)
	{
		
		 //Status_UpLoad();//本车数据上传
		 USART2_SendData(&tstBuf[0],sizeof(tstBuf));
		 OSTimeDlyHMSM(0,0,0,200);
	}
}

