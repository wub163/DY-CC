/*========================================================= 
*    文件名:  	        CAN_drive.c 
*    描述: 	        兴颂CAN电机驱动程序
*    版本号:  	        1.1   
*    创建日期:          2015-6-1 8:52:03  
*    IDE环境:  	        IAR Embedded Workbench for ARM version 7.30.3
*    字符编码:  	GB2312   
*    作	者:  	        吴兵  
*    版权所有:  	哈尔滨工大服务机器人有限公司
*    版本修改历史:    （XXX:为操作者）
*       2015-6-1        吴兵:创建文件   
*  	2016-3-20       吴兵:整理完善   
* ===========================================================  */     
#include "CAN_drive.h"
CanDriveSt candr;
#define GearScale 1     //传动比
#define WheelPeri 125   //轮直径
#define MotorEncoder 900 //周脉冲数
#define Len          500
#define MOTORNUM 2

#include "net.h"


extern u8 QDQERR;

//CAN初始化
//tsjw:重新同步跳跃时间单元. @ref CAN_synchronisation_jump_width   范围: ; CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   @ref CAN_time_quantum_in_bit_segment_2 范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   @refCAN_time_quantum_in_bit_segment_1  范围: ;	  CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//波特率=Fpclk1/((tsjw+tbs1+tbs2+3)*brp);
//mode: @ref CAN_operating_mode 范围：CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//返回值:0,初始化OK;
//    其他,初始化失败;
unsigned char gMag = 0;
u8 CAN1_Mode_Init(u16 BAUD)
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    //使能相关时钟
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);	
	
    //初始化GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;     //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	  //引脚复用映射配置
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1); //GPIOB8复用为CAN1
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1); //GPIOB9复用为CAN1
	
	  CAN_DeInit(CAN1);
  	//CAN单元设置
	CAN_InitStructure.CAN_TTCM = DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM = DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM = DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART = DISABLE;//ENABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM = DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP = DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //模式设置 
//  	CAN_InitStructure.CAN_Mode= CAN_Mode_LoopBack;	 //模式设置 
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;		   //BTR-SJW 重新同步跳跃宽度 2个时间单元
	 
	/* ss=1 bs1=5 bs2=3 位时间宽度为(1+5+3) 波特率即为时钟周期tq*(1+3+5)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;		   //BTR-TS1 时间段1 占用了5个时间单元
	CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;		   //BTR-TS1 时间段2 占用了3个时间单元	
	
	/* CAN Baudrate = 1 MBps (1MBps已为stm32的CAN最高速率) (CAN 时钟频率为 APB 1 = 45 MHz) */     
	CAN_InitStructure.CAN_Prescaler =5000/BAUD;		  //BTR-BRP 波特率分频器  定义了时间单元的时间长度 45/(1+5+3)/5=1 Mbps
	CAN_Init(CAN1, &CAN_InitStructure);
    
		//配置过滤器
	  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //激活过滤器1
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	  return 0;
}   



/****************************************************************************
* 名    称：void USB_LP_CAN1_RX0_IRQHandler(void)
* 功    能：驱动器中断服务程序
* 入口参数：null
* 出口参数：null
* 说    明：
* 调用方法：null 
****************************************************************************/
void CAN1_RX0_IRQHandler(void)
{
  static u8 dl[2];
    CanRxMsg RxMessage;
	RxMessage.StdId = 0;
	//CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	switch(RxMessage.StdId&0x0f)
	{
        
        
        case 0:             //CAN驱动器
                ;
        case 1:   ;          //CAN驱动器
//             QDQERR=0;
               if(candr.jczt==2)      //判断当前为什么反馈。
               switch (RxMessage.Data[0])
                {
                case 1:
                   candr.FbbPos[0]=(s32)(RxMessage.Data[4]<<24|RxMessage.Data[5]<<16|RxMessage.Data[6]<<8|RxMessage.Data[7]);break;
               case 2:
                   candr.FbbPos[0]=(s32)(RxMessage.Data[4]<<24|RxMessage.Data[5]<<16|RxMessage.Data[6]<<8|RxMessage.Data[7]);candr.jczt=1;break;
                
                }   
               else     {
               candr.dy=(RxMessage.Data[2]<<8|RxMessage.Data[3]);//当电压模块不存在时，从驱动器中读取电压值。
               switch (RxMessage.Data[0])
                {
                case 1:
                   dl[0]=RxMessage.Data[7];break;
                case 2:
                   dl[1]=RxMessage.Data[7];candr.dl=(float)(dl[0]+dl[1])/10;candr.jczt=2;break;
                
                }        }
               candr.CANERR=(RxMessage.Data[1]&0x0f);break;
               
                
              
	}
}




/****************************************************************************
* 名    称：void CalMotor_can(void)
* 功    能：电机参数计算程序
* 入口参数：null
* 出口参数：null
* 说    明：null
* 调用方法：null 
****************************************************************************/
void CalMotor_can(void)              /// 2015-04-22改为mm/s
{
    int num = 0;
	int errcount = 0;
	int errsum = 0;
	static char firstflag[MOTORNUM] = {0,};
	static int errnum = 0;
	static char errflag[50] = {0,};
	double actpos[MOTORNUM] = {0,};
//	double basex = 0.0;
//	double basey = 0.0;
	double baseth = 0.0;
	double length = 0.0;
	static double radcount=0.0;


		 errnum = (errnum+1)%50;                                                         ///最近50次伺服状态记录
          if((candr.CANERR+candr.CANERR)>0)            
		 {
                      errflag[errnum] = 1;
		 }
		 else
		 {
                      errflag[errnum] = 0;
		 }
		 for(errcount=0;errcount<50;errcount++)
		 {
		     errsum += errflag[errcount];
		 }
		 if(errsum > 45)        ///最近1s内有45次电机错误，则保护
		 
		     candr.FatalErr |= 0x01;
		 else candr.FatalErr = 0;
                 candr.FatalErr = candr.FatalErr;
                 
                 
	for(num = 0;num < MOTORNUM; num++)
	{
                 candr.FbPos[num%2]=(double)( candr.FbbPos[num%2]* WheelPeri*candr.FlagForward[num%2] /(GearScale * MotorEncoder));
                 

                 
                 if(0 == firstflag[num])
		 {
                         candr.LastFbPos[num%2]	= candr.FbPos[num%2];
			 firstflag[num] = 1;
		 }
		 actpos[num] = (candr.FbPos[num%2] - candr.LastFbPos[num%2]);///单位时间单轮移动距离（mm）
		  candr.LastFbPos[num%2]	= candr.FbPos[num%2];
	}

	//gComData->FbVx = (gHalData->WheelHal[0].FbVel + gHalData->WheelHal[1].FbVel + gHalData->WheelHal[2].FbVel + gHalData->WheelHal[3].FbVel)/4.0;
	//gComData->FbVy = (-gHalData->WheelHal[0].FbVel + gHalData->WheelHal[1].FbVel + gHalData->WheelHal[2].FbVel - gHalData->WheelHal[3].FbVel)/4.0;
	//gComData->FbVthe = (-gHalData->WheelHal[0].FbVel + gHalData->WheelHal[1].FbVel - gHalData->WheelHal[2].FbVel + gHalData->WheelHal[3].FbVel)/(2*(gInnerData->Length + gInnerData->Width));
    
	baseth = (actpos[1] - actpos[0])/Len;                  ///单位时间的两轮差距除以轮距获得弧度值(rad)


        
  length = (actpos[0] + actpos[1]) / 2.0 ;      
/*	length = (actpos[0] + actpos[1]) / (2.0 * baseth);                      ///单位时间转弯半径
	if(fabs(baseth) <= 0.001)
	{
	    basex = (actpos[0] + actpos[1])/2.0;
	}
	else
	{
	    basex = sin(baseth)*length;
	}
	basey = length*(1 - cos(baseth));

	gComData->FbPosx += basex*cos(gComData->FbPosthe) - basey*sin(gComData->FbPosthe);      ///X偏移量 累加
	gComData->FbPosy += basex*sin(gComData->FbPosthe) + basey*cos(gComData->FbPosthe);      ///Y偏移量  累加
*/
       candr.length +=length;
	radcount+=baseth;
        candr.rad =(s16)(radcount*100);   //弧度修正                                                        ///弧度偏移量  累加
	
}


void Candr_init(void)
{
candr.FlagForward[0]=-1;
candr.FlagForward[1]=1;
candr.jczt=1;
}


/****************************************************************************
* 名    称：void moto_can (void)
* 功    能：电机参数下发程序
* 入口参数：ch：电机编号
* 出口参数：null
* 说    明：根据gHalData->WheelHal[ch].Cmdvel线速度控制电机
* 调用方法：null 
****************************************************************************/

void moto_can (void)
{
  static u8 moto_err_delay;
   s16 speed;
 CanTxMsg motot;
CalMotor_can();
Candr_init();
 //         motot.StdId=1;                //驱动器编号
          motot.IDE=CAN_Id_Standard;
          motot.RTR=CAN_RTR_Data;
          motot.DLC=8;
          motot.Data[2]=0;//运行模式。
          motot.Data[3]=0;//故障清除。
          motot.Data[4]=1;//电机控制。
char i=0;
while(i<MOTORNUM){
//while(i<2){

if(0 == candr.FatalErr)       ///根據車速，速比，輪周長，和方向計算 電機轉速。
		{
	        moto_err_delay=0;
                  candr.moto[i%2]= (int)(candr.CmdVel[i%2] * GearScale * 60  * candr.FlagForward[i%2] / WheelPeri);
			 
		}
		else
		{
                  moto_err_delay++;
                  candr.moto[i%2] = 0;
                  if (moto_err_delay>250){moto_err_delay=0;motot.Data[3]=1;}             //驱动器报警5秒钟后，自动清除故障。

		}
          speed=(u16)(candr.moto[i%2]*500/15);//每分钟角速度转换为脉冲。0.1hz(100*5倍极对数*6.5r/min)
 //speed=2000;
          motot.StdId=(i/2+1);
          motot.Data[0]=i%2+1;                      //电机选择。
          motot.Data[1]=1;//数据返回、
          motot.Data[5]=speed<0?1:0;                      //电机方向。
           speed=abs(speed);
         motot.Data[6]=(speed>>8)&0xff;        //电机速度H。
          motot.Data[7]=speed&0xff;             //电机速度L。
          CAN_Transmit(CAN1,&motot);
        i++;
//         if (i==1){for(u32 a=0;a<2000;a++);}// can有三个邮箱，无需等待。
}

}

void moto_turn(s16 forward,s16 back)
{candr.CmdVel[0]=(float)forward;
  candr.CmdVel[1]=(float)back;
  *(u16*)&writebuf[1]=candr.dy;
  moto_can();
}



