
#include "can.h"
#include "CCST.h"
#include "main.h"
void NVIC_CAN (void);

//
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
u8 CAN1_Mode_Init(u16 BAUD)   //CAN1 PB8，9  CAN2 PB12,13
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    //使能相关时钟
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);	
	
    //初始化GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;     //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	  //引脚复用映射配置
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOB8复用为CAN1
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOB9复用为CAN1
	  CAN_DeInit(CAN1);
  	//CAN单元设置
	CAN_InitStructure.CAN_TTCM = DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM = DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM = DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART = DISABLE;//ENABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM = DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP = DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //模式设置 
//        CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		   //BTR-SJW 重新同步跳跃宽度 1个时间单元
//        CAN_InitStructure.CAN_BS1=CAN_BS1_6tq;		   //BTR-TS1 时间段1 占用了2个时间单元
//        CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;		   //BTR-TS1 时间段2 占用了3个时间单元
//  	CAN_InitStructure.CAN_Prescaler = 6000/BAUD;  //分频系数(Fdiv)为brp+1	
//  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 

        
        //  	CAN_InitStructure.CAN_Mode= CAN_Mode_LoopBack;	 //模式设置 
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;		   //BTR-SJW 重新同步跳跃宽度 2个时间单元
	 
	/* ss=1 bs1=5 bs2=3 位时间宽度为(1+5+3) 波特率即为时钟周期tq*(1+3+5)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;		   //BTR-TS1 时间段1 占用了5个时间单元
	CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;		   //BTR-TS1 时间段2 占用了3个时间单元	
	
	/* CAN Baudrate = 1 MBps (1MBps已为stm32的CAN最高速率) (CAN 时钟频率为 APB 1 = 45 MHz) */     
	CAN_InitStructure.CAN_Prescaler =5000/BAUD;		  //BTR-BRP 波特率分频器  定义了时间单元的时间长度 42/(1+4+2)/6=1 Mbps
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
        
        NVIC_CAN();
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	  return 0;
}   



u8 CAN2_Mode_Init(u16 BAUD)   //CAN1 PB8，9  CAN2 PB12,13
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    //使能相关时钟
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1|RCC_APB1Periph_CAN2, ENABLE);//单独开CAN2时钟无效，发送错误，CAN1也必须打开。	
	
    //初始化GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;     //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	  //引脚复用映射配置
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2); //GPIOB8复用为CAN1
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2); //GPIOB9复用为CAN1
	  CAN_DeInit(CAN2);
  	//CAN单元设置
	CAN_InitStructure.CAN_TTCM = DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM = DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM = DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART = DISABLE;//ENABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM = DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP = DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //模式设置 
//        CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		   //BTR-SJW 重新同步跳跃宽度 1个时间单元
//        CAN_InitStructure.CAN_BS1=CAN_BS1_6tq;		   //BTR-TS1 时间段1 占用了2个时间单元
//        CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;		   //BTR-TS1 时间段2 占用了3个时间单元
//  	CAN_InitStructure.CAN_Prescaler = 6000/BAUD;  //分频系数(Fdiv)为brp+1	
//  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 

        
        //  	CAN_InitStructure.CAN_Mode= CAN_Mode_LoopBack;	 //模式设置 
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		   //BTR-SJW 重新同步跳跃宽度 2个时间单元
	 
	/* ss=1 bs1=5 bs2=3 位时间宽度为(1+5+3) 波特率即为时钟周期tq*(1+3+5)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_4tq;		   //BTR-TS1 时间段1 占用了5个时间单元
	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		   //BTR-TS1 时间段2 占用了3个时间单元	
	
	/* CAN Baudrate = 1 MBps (1MBps已为stm32的CAN最高速率) (CAN 时钟频率为 APB 1 = 45 MHz) */     
	CAN_InitStructure.CAN_Prescaler =6000/BAUD;		  //BTR-BRP 波特率分频器  定义了时间单元的时间长度 42/(1+4+2)/6=1 Mbps
	CAN_Init(CAN2, &CAN_InitStructure);

        
		//配置过滤器         CAN1
//	CAN_FilterInitStructure.CAN_FilterNumber = 0;
//  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
//  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
//  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
//  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
//  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
//  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
//   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
//  	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //激活过滤器1
//  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化

		//配置过滤器         CAN2  
        CAN_FilterInitStructure.CAN_FilterNumber=14;            //CAN2的起始过滤器。
        CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
        CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
        CAN_FilterInitStructure.CAN_FilterIdHigh=0;
        CAN_FilterInitStructure.CAN_FilterIdLow=0;
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0;
        CAN_FilterInitStructure.CAN_FilterMaskIdLow=0;
        CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
        CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
        CAN_FilterInit(&CAN_FilterInitStructure);
        
        NVIC_CAN();
	  CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);
	  return 0;
}   

void CAN1_RX0_IRQHandler(void)                               
{

  CanRxMsg RxMessage;
  RxMessage.StdId = 0;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

   
    switch(RxMessage.StdId & 0x11f)  //读卡器和磁条传感器的地址空间只能（1-127）
    {
              case 0x11B://电机驱动器。
          if(xCANRMDS)xQueueSendFromISR(xCANRMDS,&RxMessage,0);break;
              case 0x017://鑫科德电池
          if(xCANPE)xQueueSendFromISR(xCANPE,&RxMessage,0);break;
              
              default:;     

    }
}

void CAN2_RX0_IRQHandler(void)                               
{

  
  CanRxMsg RxMessage;
    CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);

   
    switch(RxMessage.StdId & 0x0F0)
    {
    case 0x10: //电池
//      if(xCANID)xQueueSendFromISR(xCANPE,&RxMessage,0);break;
    default:      break;

    }
}


//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CAN1_Send_Msg(u16 id,u8* msg,u8 len)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=id;	 // 标准标识符为0
  TxMessage.ExtId=0;	 // 设置扩展标示符（29位）
  TxMessage.IDE=0;		  // 使用扩展标识符
  TxMessage.RTR=0;		  // 消息类型为数据帧，一帧8位
  TxMessage.DLC=len;							 // 发送两帧信息
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				 // 第一帧信息          

  mbox=CAN_Transmit(CAN1, &TxMessage);
  if( mbox>3)
  {vTaskDelay(5/portTICK_PERIOD_MS);//如果邮箱堵塞，延时重发。
 CAN_Transmit(CAN1, &TxMessage);}
return mbox;

}


u8 CAN2_Send_Msg(u16 id,u8* msg,u8 len)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=id;	 // 标准标识符为0
  TxMessage.ExtId=0;	 // 设置扩展标示符（29位）
  TxMessage.IDE=0;		  // 使用扩展标识符
  TxMessage.RTR=0;		  // 消息类型为数据帧，一帧8位
  TxMessage.DLC=len;							 // 发送两帧信息
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				 // 第一帧信息          

  mbox=CAN_Transmit(CAN2, &TxMessage);
  if( mbox>3)
  {vTaskDelay(5/portTICK_PERIOD_MS);//如果邮箱堵塞，延时重发。
 CAN_Transmit(CAN2, &TxMessage);}
return mbox;

}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
    for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}

u8 CAN2_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN2,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
    CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);//读取数据	
    for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}



void NVIC_CAN (void)
{
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	
      	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;   //485
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
        NVIC_Init(&NVIC_InitStructure);
	
      	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;   //485
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
        NVIC_Init(&NVIC_InitStructure);

}





