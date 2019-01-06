/****************************************************************************
* Copyright (C), 2013
***************************************************************************/
#include "math.h"
#include "485.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "upload.h"


typedef struct ComDriveStruct
{          //电机编号1,FL 2，FR 3,BL 4,BR 5，云台升降
        float CmdVel[MOTORNUM485];              //控制速度
        s16 moto[MOTORNUM485];                  //控制用脉冲数
        int FbbPos[MOTORNUM485];                  //脉冲数
        double  FbPos[MOTORNUM485];               //mm数
        double  length;                 //设备里程
        double LastFbPos[MOTORNUM485];
        s8  FatalErr[MOTORNUM485];                   //通讯代码故障
//        u8 COMERR;                      //无通讯故障。
        s8 FlagForward[MOTORNUM485];
        u8 data[MOTORNUM485][9];          //速度控制时的命令数据
        u8 wdata[MOTORNUM485][18];         //位置控制时的命令数据
        
}ComDriveSt;

  int fmid=0,bmid=0,hh=0,hl=0; 

 ComDriveSt comdr;


 QueueHandle_t xTURNTO, xMOVE,xHIGH;
 SemaphoreHandle_t sMOTOok;


void fankui(u8 ch);

void CalMotor(void) ;

void moto (u8 ch);
u8 moto_data[MOTORNUM485*60]={0,};    //电机接收区。     
u8 moto_busy=0;
u16 num = 0;
ComDriveSt comdr;


void USART2_IRQHandler(void)
{
 	
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
        {
         USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);		                       //接收中断使能
        USART_ReceiveData(USART2);
         xSemaphoreGiveFromISR(sMOTOok,0);
        }
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{	
		moto_data[num++] = USART_ReceiveData(USART2);
		if(num>=(MOTORNUM485*60))num=0;
                 USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);	
        }
        
}

static void moto485Init(u32 BaudRate)                                    //串口2初始化  485
{  	 
        GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); 
	
      	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;   //485
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
        NVIC_Init(&NVIC_InitStructure);

        
	//485使能
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //必须为上拉，否则。485发送期会接收到乱码
	GPIO_Init(GPIOD,&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure); 
	
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断
//	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//开启相关中断
  USART_Cmd(USART2, ENABLE);  //使能串口2 

}

/**************************************************************
* 串口2 发送函数
* data:发送数据
* length:发送的字节长度
**************************************************************/
void moto_SendData(uint8_t *data,uint8_t length)
{
		int i = 0;
		for(;i<length;i++)
		{
			 USART_SendData(USART2,*(data+i));
			 while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET); 
		}
}


/****************************************************************************
* 名    称：void Drivers_Init(void)
* 功    能：initiate the drivers
* 入口参数：null
* 出口参数：null
* 说    明：
* 调用方法：null 
****************************************************************************/
void Drivers_Init(void)
{

  for(u8 num = 0;num < MOTORNUM485; num++)
{
comdr.data[num][0]=(num+2);
comdr.data[num][1]=0x06;
comdr.data[num][2]=0x00;
comdr.data[num][3]=0x43;

comdr.wdata[num][ 0]=(num+2);
comdr.wdata[num][ 1]=0x10;
comdr.wdata[num][ 2]=0x00;
comdr.wdata[num][ 3]=0x44;
comdr.wdata[num][ 4]=0x00;
comdr.wdata[num][ 5]=0x04;
comdr.wdata[num][ 6]=0x08;
comdr.wdata[num][ 9]=0x00;
comdr.wdata[num][10]=0x00;//绝对位置方式：0绝对 ，1相对

}
comdr.FlagForward[0]=MotoFlagForward0;
comdr.FlagForward[1]=MotoFlagForward1;
//comdr.FlagForward[2]=1;
//comdr.FlagForward[3]=-1;
//comdr.FlagForward[4]=1;
//comdr.FlagForward[5]=-1;
//comdr.FlagForward[6]=1;

}


//与驱动器之间485通信的crc校验函数
u16 RTU_CRC(unsigned char *data,unsigned char length)
{
     u8 j = 0;
	 u16 reg_crc = 0xffff;
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
	 return reg_crc;
}



//gHalData->WheelHal[ch].CmdVel 每秒线速度 ，转换成每分角速度，再换成脉冲。
void moto (u8 ch)
{
  u16 crc;
  s16 speed;
  

  
  if(ch>=MOTORNUM485)return;
se485;  
//gInnerData->FatalErr=0;
if(20 >= comdr.FatalErr[ch])       ///根據車速，速比，輪周長，和方向計算 電機轉速。
		{
 //                 comdr.moto[ch] = (int)(comdr.CmdVel[ch] * GearScale * 60  * comdr.FlagForward[ch] / WheelPeri);
                speed=comdr.moto[ch]*comdr.FlagForward[ch] ;                       
                  
		}
		else
		{
		speed = 0;//电机故障时，仅仅保护行走。
		}
  
  comdr.data[ch][4]=(speed>>8)&0xff;
  comdr.data[ch][5]=speed&0xff;


  crc=RTU_CRC(&(comdr.data[ch][0]),6);
  comdr.data[ch][7]=(crc>>8)&0xff;
  comdr.data[ch][6]=crc&0xff;
  comdr.data[ch][8]=0;
moto_SendData((u8*)&(comdr.data[ch]),9);
  re485;     			                       //接收中断使能
moto_data[ch*60+1]=0xff;                                //破包
    moto_busy=(ch+1);num=ch*60;  
}

//电机位置控制
void wmoto (u8 ch)
{
  u16 crc;
//  int jd_weizhi;
  

  
  if(ch>=MOTORNUM485)return;
se485;  
//gInnerData->FatalErr=0;
if(20 >= comdr.FatalErr[ch])       ///根據車速，速比，輪周長，和方向計算 電機轉速。
		{
//                  jd_weizhi = comdr.CmdVel[ch];//输入+-100求出绝对编码值。
                                         
                  comdr.wdata[ch][ 7]=(MotorTurn>>8)&0xff;
                  comdr.wdata[ch][ 8]=(MotorTurn)&0xff;
		}
		else
		{
                  comdr.wdata[ch][ 7]=0;
                  comdr.wdata[ch][ 8]=0;//电机故障时运行停止。
		}


  crc=RTU_CRC(&(comdr.wdata[ch][0]),15);
  comdr.wdata[ch][16]=(crc>>8)&0xff;
  comdr.wdata[ch][15]=crc&0xff;
  comdr.wdata[ch][17]=0;
moto_SendData((u8*)&(comdr.wdata[ch]),18);
  re485;     			                       //接收中断使能
moto_data[ch*60+1]=0xff;                                //破包
    moto_busy=(ch+1);num=ch*60;  

}


void motosp (s16 speed )
{
comdr.CmdVel[0]=(float)speed;
comdr.CmdVel[1]=(float)speed;
//comdr.CmdVel[2]=(float)speed;
//comdr.CmdVel[3]=(float)speed;
}

int htonl(int n)
{
  return ((n & 0xff) << 24) |
    ((n & 0xff00) << 8) |
    ((n & 0xff0000UL) >> 8) |
    ((n & 0xff000000UL) >> 24);
}




void fankui(u8 ch)
{
  u8 da[][9]={
    {0x02,0x03,0x00,0x20,0x00,0x16,0xC5,0xFD,0},
    {0x03,0x03,0x00,0x20,0x00,0x16,0xC4,0x2C,0},
//    {0x04,0x03,0x00,0x20,0x00,0x16,0xC5,0x9B,0},
//    {0x05,0x03,0x00,0x20,0x00,0x16,0xC4,0x4A,0},
//    {0x06,0x03,0x00,0x20,0x00,0x16,0xC4,0x79,0},
//    {0x07,0x03,0x00,0x20,0x00,0x16,0xC5,0xA8,0},
//    {0x08,0x03,0x00,0x20,0x00,0x16,0xC5,0x57,0},
 
  };
  se485;
  if(ch>=MOTORNUM485)return;          
moto_SendData((u8*)&(da[ch]),9);
re485;   				                       //接收中断使能
moto_busy=(ch+11);num=ch*60+10;

if(ch==1)CalMotor();

}


void CalMotor(void)              /// 2015-04-22改为mm/s 2016-07-26 改取值寄存器范围为20h-35h
{
xDRV_D moto;
// float dy;
	static char firstflag[MOTORNUM485] = {0,};
        	double baseth = 0.0;

	double actpos[MOTORNUM485] = {0,};
	for(u8 num = 0;num < MOTORNUM485; num++)
	{
          u16 bb,cc,aa;aa=num*60;
          bb=*((u16*)&(moto_data[57+aa]));cc=RTU_CRC(&moto_data[10+aa],47);
          if((bb!=cc)||(moto_data[1+aa]>0x80))            ///等于08或00
		 {
                      if((comdr.FatalErr[num] ++)>120)comdr.FatalErr[num]=120;moto_data[aa]=0xFF;
		 }
		 else
		 {
                      if(( --comdr.FatalErr[num])<0)comdr.FatalErr[num]=0;
		 }

          comdr.FbbPos[num]=(s32)((moto_data[aa+21]<<24)|(moto_data[aa+22]<<16)|(moto_data[aa+23]<<8)|(moto_data[aa+24])) ;
          comdr.FbPos[num]=comdr.FbbPos[num]* WheelPeri*comdr.FlagForward[num] /(GearScale * MotorEncoder);
                
//==============状态提取
    moto.drv_pos[num]=comdr.FbbPos[num]*comdr.FlagForward[num];
    moto.drv_spe[num]=(s16)((moto_data[aa+17]<<8)|(moto_data[aa+18]))*comdr.FlagForward[num];
    moto.moto_dl[num]=(u8)((s16)((moto_data[aa+15]<<8)|(moto_data[aa+16]))/5);
    moto.moto_zt[num]=moto_data[aa+52];
    
    moto.moto_dy[num]=(s16)((moto_data[aa+33]<<8)|(moto_data[aa+34]))*72/100;
    if((moto_data[aa+38]==0)||(moto_data[aa+40]==0))moto.moto_zt[num]=5;
//======================                 
                 
                 if(0 == firstflag[num])
		 {
                         comdr.LastFbPos[num]	= comdr.FbPos[num];
			 firstflag[num] = 1;
		 }
		 actpos[num] = (comdr.FbPos[num] - comdr.LastFbPos[num]);///单位时间单轮移动距离（mm）
		 comdr.LastFbPos[num]	= comdr.FbPos[num];
	}

	//gComData->FbVx = (gHalData->WheelHal[0].FbVel + gHalData->WheelHal[1].FbVel + gHalData->WheelHal[2].FbVel + gHalData->WheelHal[3].FbVel)/4.0;
	//gComData->FbVy = (-gHalData->WheelHal[0].FbVel + gHalData->WheelHal[1].FbVel + gHalData->WheelHal[2].FbVel - gHalData->WheelHal[3].FbVel)/4.0;
	//gComData->FbVthe = (-gHalData->WheelHal[0].FbVel + gHalData->WheelHal[1].FbVel - gHalData->WheelHal[2].FbVel + gHalData->WheelHal[3].FbVel)/(2*(gInnerData->Length + gInnerData->Width));
    
        baseth=(actpos[0]+actpos[1])/2;
        comdr.length+=baseth;
  
      
 if(xDRV)xQueueSend(xDRV,&moto,0); 

       
}


void MOTOtask(void* pv)          //电机驱动任务
{
  
  s16 sp[2]={0,};
 u8  stopcount=0;
  TickType_t xLastWakeTime;
  xMOVE  = xQueueCreate( 1, sizeof( int16_t )*2 );       //小车移动速度值

  vSemaphoreCreateBinary(sMOTOok);
  moto485Init(115200);
 xLastWakeTime = xTaskGetTickCount ();
  moto_busy=0;
  
  while(1){
    Drivers_Init();
    xSemaphoreTake(sMOTOok,10/portTICK_PERIOD_MS);//超过10ms则为信号丢失

   
    switch(moto_busy)
              {
                case 00:moto(0);break;
                case 01:moto(1);break;
                case 02:fankui(0);break;
                case 11:fankui(1);break;
               default:num=0;moto_busy=0;xSemaphoreGive(sMOTOok);
              
              
                  if(xQueueReceive(xMOVE,&sp,20/portTICK_PERIOD_MS)==errQUEUE_EMPTY)
                    {if((stopcount++)>20)stopcount=252;}    //500ms内无速度更新则停车保护。
                  else stopcount=0;
 //////////////////// motosp (sp);
                  if(stopcount>250){sp[0]=0;sp[1]=0;}
                 comdr.moto[0]=sp[0];
                 comdr.moto[1]=sp[1];
              
//                    vTaskDelayUntil( &xLastWakeTime, 100/portTICK_PERIOD_MS);//电机的固定刷新频率。
              }
//      uxTaskGetStackHighWaterMark( NULL );
    
  }
  
  
}




