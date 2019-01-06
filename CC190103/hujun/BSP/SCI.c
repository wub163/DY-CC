
#include "main.h"

/***********************************************************************
文件名称：SCI.C
功    能：完成对usart1和usart2的操作
编写时间：2012.11.22
编 写 人：
注    意：
***********************************************************************/

volatile unsigned char RS485_REC_Flag = 0;
u8* RS485_buff=NULL;//[RS485_REC_BUFF_SIZE];//用于接收数据
volatile unsigned int RS485_rec_counter = 0;//用于RS485接收计数
unsigned short RS485_send_data_length = 0;

volatile unsigned char RS232_REC_Flag = 0;
u8* RS232_buff=NULL;//[RS232_REC_BUFF_SIZE];//用于接收数据
volatile unsigned int RS232_rec_counter = 0;//用于RS232接收计数
unsigned short RS232_send_data_length = 0;

uint8_t Uart2Buf[20] ;  

//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;  

	//while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
	//USART2->DR = (u8) ch;  	
	return ch;
}
/*
函数功能：获取wifi控制信息
参数：   nByte---接收到的字节
*/

//void Getwifi(uint8_t nByte)
//{
//    static uint8_t recvLen = 0;
//    static uint8_t  nCnt = 0;	
//
////	uint16_t nCRC;//CRC校验值
//
//	Uart2Buf[nCnt] = nByte;
//	
//    if(nCnt == 0)
//    {
//        if(0x55 == nByte)
//        {
//            nCnt++;
//        }
//    }
//    else if(nCnt == 1)
//    {
//        nCnt++;
//        switch(nByte)
//        {
//						case 0x08://读保持寄存器
//							recvLen = 10; //正常情况下返回7个字节
//							break;
//						default:
//							nCnt = 0;
//							break;
//         }
//    }
//    else if(nCnt >= 2)
//    {
//        nCnt++;
//        if(nCnt == recvLen)
//        {
//            nCnt = 0;
//						if(nByte == 0xAA)
//						{
//								if(Uart2Buf[3] == 11)
//									{
////											cha_state.wheelst.CmdWheelAngle = (int16_t)Uart2Buf[5];
////											cha_state.wheelst.CmdWheelVel = (int16_t)Uart2Buf[6];
//									}
//						}
//        }
//	  }
//}
/***********************************************************************
函数名称：void USART_232_Configuration(void) 
功    能：
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：RS232用的是USART1
***********************************************************************/
void USART_232_Configuration(void)
{ 
  
	GPIO_InitTypeDef GPIO_InitStructure;//定义GPIO_InitTypeDef类型的结构体成员GPIO_InitStructure

	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	//使能需要用到的GPIO管脚时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD, ENABLE);
	//使能USART1 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	///复位串口1
	USART_DeInit(USART1);
	
	USART_StructInit(&USART_InitStructure);//载入默认USART参数
	USART_ClockStructInit(&USART_ClockInitStruct);//载入默认USART参数        
	//配置串口1的管脚 PA8 USART1_EN PA9 USART1_TX PA10 USART1_RX    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);        //管脚PA9复用为USART1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;        
	GPIO_Init(GPIOA, &GPIO_InitStructure);                                                                                                                 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	USART_ClockInit(USART1,&USART_ClockInitStruct);


	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure); 
	
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);  
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
// 	USART_ITConfig(USART1, USART_IT_TC, ENABLE);// 
	
// 	//采用DMA方式发送  
 	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  
	//采用DMA方式接收  
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); 
	
	
	USART_Cmd(USART1, ENABLE); 
	USART_ClearITPendingBit(USART1, USART_IT_TC);//清除中断TC位	
RS232_buff=pvPortMalloc(RS232_REC_BUFF_SIZE);
}
/***********************************************************************
函数名称：void USART_485_Configuration(void) 
功    能：
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：RS485用的是USART2
***********************************************************************/
void USART_485_Configuration(void)
{ 	
//	GPIO_InitTypeDef GPIO_InitStructure;//定义GPIO_InitTypeDef类型的结构体成员GPIO_InitStructure
//	USART_InitTypeDef USART_InitStructure;

//	//使能需要用到的GPIO管脚时钟
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
//      
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //复用
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);        

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);                                                                                                                 
//	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);

//	USART_InitStructure.USART_BaudRate = 115200;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//	USART_Init(USART2,&USART_InitStructure); 

//	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);  
//	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
//// 	USART_ITConfig(USART3, USART_IT_TC, ENABLE);// 
//	
//// 	//采用DMA方式发送  
//// 	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);  
//	//采用DMA方式接收  
//	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); 
//	
//	
//	USART_Cmd(USART2, ENABLE); 
//	USART_ClearITPendingBit(USART2, USART_IT_TC);//清除中断TC位	
//    
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);    
	
	GPIO_InitTypeDef GPIO_InitStructure;//定义GPIO_InitTypeDef类型的结构体成员GPIO_InitStructure
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;

	//使能需要用到的GPIO管脚时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
	USART_DeInit(USART2);
	USART_StructInit(&USART_InitStructure);//载入默认USART参数
	USART_ClockStructInit(&USART_ClockInitStruct);//载入默认USART参数 
      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);        

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOD, &GPIO_InitStructure);                                                                                                                 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure); 

	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);  
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
// 	USART_ITConfig(USART3, USART_IT_TC, ENABLE);// 
	
// 	//采用DMA方式发送  
// 	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  
	//采用DMA方式接收  
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); 
	
	
	USART_Cmd(USART2, ENABLE); 
	USART_ClearITPendingBit(USART2, USART_IT_TC);//清除中断TC位	
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);    
  
        RS485_buff=pvPortMalloc(RS485_REC_BUFF_SIZE);
}
/***********************************************************************
函数名称：void USART1_IRQHandler(void) 
功    能：完成SCI的数据的接收，并做标识
输入参数：
输出参数：
编写时间：2012.11.22
编 写 人：
注    意：RS232用的是USART1
***********************************************************************/
void USART1_IRQHandler(void)  
{

	unsigned char temp = 0;
//	s8 err;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{	
		temp = USART1->SR; temp = USART1->DR; temp=temp;												//清USART_IT_IDLE标志 
		DMA_Cmd(DMA2_Stream5, DISABLE); 							  	//读取数据长度先关闭DMA 

		RS232_rec_counter = RS232_REC_BUFF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream5);//获取DMA接收的数据长度，
                  if(!RS232_REC_Flag)
                  {if(RS232DATA)memcpy(RS232DATA,RS232_buff,RS232_rec_counter);}

		DMA_SetCurrDataCounter(DMA2_Stream5,RS232_REC_BUFF_SIZE);		//设置传输数据长度    
		DMA_Cmd(DMA2_Stream5, ENABLE);
		RS232_REC_Flag = 1;	   											//DMA接收标志
		if(sem_RS232_rec_flag)xSemaphoreGiveFromISR(sem_RS232_rec_flag,0);  //抛出一个信号量表示RS232已经接收完成一帧数据
	}
	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)					//串口发送中断
	{
		USART_ClearITPendingBit(USART1, USART_IT_TC);
		USART_ITConfig(USART1, USART_IT_TC, DISABLE);// 
		RS232_dma_send_flag = 0;										//允许再次发送
	}	
}
/***********************************************************************
函数名称：void USART1_IRQHandler(void) 
功    能：完成SCI的数据的接收，并做标识
输入参数：
输出参数：
编写时间：2012.11.22
编 写 人：
注    意：RS232用的是USART1
***********************************************************************/
void USART2_IRQHandler(void)                	//串口2中断服务程序
{
//		uint8_t TmpChar = 0;
		unsigned char temp = 0;
//		s8 err;

		if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)				//空闲中断
		{	
			temp = USART2->SR;temp = USART2->DR; temp=temp;												//清USART_IT_IDLE标志 
			DMA_Cmd(DMA1_Stream5, DISABLE); 							  	//读取数据长度先关闭DMA 

			RS485_rec_counter = RS485_REC_BUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Stream5);//获取DMA接收的数据长度，
                        if(!RS485_REC_Flag)
                        {if(RS485DATA)memcpy(RS485DATA,RS485_buff,RS485_rec_counter);}
			DMA_SetCurrDataCounter(DMA1_Stream5,RS485_REC_BUFF_SIZE);		//设置传输数据长度    
			DMA_Cmd(DMA1_Stream5, ENABLE);
			RS485_REC_Flag = 1;												//DMA接收标志 
                        if(sem_RS485_rec_flag)xSemaphoreGiveFromISR(sem_RS485_rec_flag,0); 
		}
		if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)					//串口发送中断
		{
			USART_ClearITPendingBit(USART2, USART_IT_TC);
			RS485_dma_send_flag = 0;										//允许再次发送
		}
		
}  

/***********************************************************************
函数名称：RS232_Send_Data(unsigned char *send_buff,unsigned int length)
功    能：RS232发送字符串
输入参数：send_buff:待发送的数据指针；length：发送的数据长度（字符个数）
输出参数：
编写时间：2012.11.22
编 写 人：
注    意：
***********************************************************************/
void RS232_Send_Data(unsigned char *send_buff,unsigned int length)
{
 	unsigned int i = 0;
	for(i = 0;i < length;i ++)
	{			
		USART1->DR = send_buff[i];
		while((USART1->SR&0X40)==0);	
	}
}

///***********************************************************************
//函数名称：RS232_DMA_Send(unsigned char *send_buff,unsigned int length)
//功    能：RS232  DMA方式发送字符串
//输入参数：send_buff:待发送的数据指针；length：发送的数据长度（字符个数）
//输出参数：
//编写时间：2012.11.22
//编 写 人：
//注    意：
//***********************************************************************/
//void RS232_DMA_Send(unsigned char *send_buff,unsigned int length)
//{
//	while(RS232_dma_send_flag != 0)					//等待上次的DMA传输完成
//	{
//		;
//	}						 
//	RS232_dma_send_flag = 1;
//	DMA2_Stream7 -> M0AR = (u32)send_buff;
//	DMA_SetCurrDataCounter(DMA2_Stream7,length);  	//设置传输长度
//	DMA_Cmd(DMA2_Stream7,ENABLE);					//启动DMA传输
//}

/***********************************************************************
函数名称：RS485_Send_Data(unsigned char *send_buff,unsigned int length)
功    能：RS485发送字符串
输入参数：send_buff:待发送的数据指针；length：发送的数据长度（字符个数）
输出参数：
编写时间：2012.11.22
编 写 人：
注    意：
***********************************************************************/
void RS485_Send_Data(unsigned char *send_buff,unsigned int length)
{
 	unsigned int i = 0;
	RS_485_TX_EN;		//485发送使能
	RS485_Delay(200);		//稍作延时，注意延时的长短根据波特率来定，波特率越小，延时应该越长
	for(i = 0;i < length;i ++)
	{			
		USART2->DR = send_buff[i];
		while((USART2->SR&0X40)==0);	
	}
	RS485_Delay(50); 		//稍作延时，注意延时的长短根据波特率来定，波特率越小，延时应该越长
	RS_485_RX_EN;  		//485接收使能
}

/***********************************************************************
函数名称：void RS485_Delay(uint32_t nCount)
功    能：RS485收发延时函数
输入参数：
输出参数：
编写时间：2012.11.22
编 写 人：
注    意：
***********************************************************************/
static void RS485_Delay(uint32_t nCount)
{ 
  while(nCount > 0)
  { 
  	  nCount --;   
  }
}
