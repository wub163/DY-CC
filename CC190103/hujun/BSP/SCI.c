
#include "main.h"

/***********************************************************************
�ļ����ƣ�SCI.C
��    �ܣ���ɶ�usart1��usart2�Ĳ���
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺
***********************************************************************/

volatile unsigned char RS485_REC_Flag = 0;
u8* RS485_buff=NULL;//[RS485_REC_BUFF_SIZE];//���ڽ�������
volatile unsigned int RS485_rec_counter = 0;//����RS485���ռ���
unsigned short RS485_send_data_length = 0;

volatile unsigned char RS232_REC_Flag = 0;
u8* RS232_buff=NULL;//[RS232_REC_BUFF_SIZE];//���ڽ�������
volatile unsigned int RS232_rec_counter = 0;//����RS232���ռ���
unsigned short RS232_send_data_length = 0;

uint8_t Uart2Buf[20] ;  

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;  

	//while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
	//USART2->DR = (u8) ch;  	
	return ch;
}
/*
�������ܣ���ȡwifi������Ϣ
������   nByte---���յ����ֽ�
*/

//void Getwifi(uint8_t nByte)
//{
//    static uint8_t recvLen = 0;
//    static uint8_t  nCnt = 0;	
//
////	uint16_t nCRC;//CRCУ��ֵ
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
//						case 0x08://�����ּĴ���
//							recvLen = 10; //��������·���7���ֽ�
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
�������ƣ�void USART_232_Configuration(void) 
��    �ܣ�
���������
���������
��дʱ�䣺
�� д �ˣ�
ע    �⣺RS232�õ���USART1
***********************************************************************/
void USART_232_Configuration(void)
{ 
  
	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO_InitTypeDef���͵Ľṹ���ԱGPIO_InitStructure

	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	//ʹ����Ҫ�õ���GPIO�ܽ�ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD, ENABLE);
	//ʹ��USART1 ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	///��λ����1
	USART_DeInit(USART1);
	
	USART_StructInit(&USART_InitStructure);//����Ĭ��USART����
	USART_ClockStructInit(&USART_ClockInitStruct);//����Ĭ��USART����        
	//���ô���1�Ĺܽ� PA8 USART1_EN PA9 USART1_TX PA10 USART1_RX    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);        //�ܽ�PA9����ΪUSART1
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
	
// 	//����DMA��ʽ����  
 	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  
	//����DMA��ʽ����  
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); 
	
	
	USART_Cmd(USART1, ENABLE); 
	USART_ClearITPendingBit(USART1, USART_IT_TC);//����ж�TCλ	
RS232_buff=pvPortMalloc(RS232_REC_BUFF_SIZE);
}
/***********************************************************************
�������ƣ�void USART_485_Configuration(void) 
��    �ܣ�
���������
���������
��дʱ�䣺
�� д �ˣ�
ע    �⣺RS485�õ���USART2
***********************************************************************/
void USART_485_Configuration(void)
{ 	
//	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO_InitTypeDef���͵Ľṹ���ԱGPIO_InitStructure
//	USART_InitTypeDef USART_InitStructure;

//	//ʹ����Ҫ�õ���GPIO�ܽ�ʱ��
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
//      
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //����
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
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
//// 	//����DMA��ʽ����  
//// 	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);  
//	//����DMA��ʽ����  
//	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); 
//	
//	
//	USART_Cmd(USART2, ENABLE); 
//	USART_ClearITPendingBit(USART2, USART_IT_TC);//����ж�TCλ	
//    
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);    
	
	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO_InitTypeDef���͵Ľṹ���ԱGPIO_InitStructure
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;

	//ʹ����Ҫ�õ���GPIO�ܽ�ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
	USART_DeInit(USART2);
	USART_StructInit(&USART_InitStructure);//����Ĭ��USART����
	USART_ClockStructInit(&USART_ClockInitStruct);//����Ĭ��USART���� 
      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
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
	
// 	//����DMA��ʽ����  
// 	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  
	//����DMA��ʽ����  
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); 
	
	
	USART_Cmd(USART2, ENABLE); 
	USART_ClearITPendingBit(USART2, USART_IT_TC);//����ж�TCλ	
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);    
  
        RS485_buff=pvPortMalloc(RS485_REC_BUFF_SIZE);
}
/***********************************************************************
�������ƣ�void USART1_IRQHandler(void) 
��    �ܣ����SCI�����ݵĽ��գ�������ʶ
���������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺RS232�õ���USART1
***********************************************************************/
void USART1_IRQHandler(void)  
{

	unsigned char temp = 0;
//	s8 err;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{	
		temp = USART1->SR; temp = USART1->DR; temp=temp;												//��USART_IT_IDLE��־ 
		DMA_Cmd(DMA2_Stream5, DISABLE); 							  	//��ȡ���ݳ����ȹر�DMA 

		RS232_rec_counter = RS232_REC_BUFF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream5);//��ȡDMA���յ����ݳ��ȣ�
                  if(!RS232_REC_Flag)
                  {if(RS232DATA)memcpy(RS232DATA,RS232_buff,RS232_rec_counter);}

		DMA_SetCurrDataCounter(DMA2_Stream5,RS232_REC_BUFF_SIZE);		//���ô������ݳ���    
		DMA_Cmd(DMA2_Stream5, ENABLE);
		RS232_REC_Flag = 1;	   											//DMA���ձ�־
		if(sem_RS232_rec_flag)xSemaphoreGiveFromISR(sem_RS232_rec_flag,0);  //�׳�һ���ź�����ʾRS232�Ѿ��������һ֡����
	}
	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)					//���ڷ����ж�
	{
		USART_ClearITPendingBit(USART1, USART_IT_TC);
		USART_ITConfig(USART1, USART_IT_TC, DISABLE);// 
		RS232_dma_send_flag = 0;										//�����ٴη���
	}	
}
/***********************************************************************
�������ƣ�void USART1_IRQHandler(void) 
��    �ܣ����SCI�����ݵĽ��գ�������ʶ
���������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺RS232�õ���USART1
***********************************************************************/
void USART2_IRQHandler(void)                	//����2�жϷ������
{
//		uint8_t TmpChar = 0;
		unsigned char temp = 0;
//		s8 err;

		if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)				//�����ж�
		{	
			temp = USART2->SR;temp = USART2->DR; temp=temp;												//��USART_IT_IDLE��־ 
			DMA_Cmd(DMA1_Stream5, DISABLE); 							  	//��ȡ���ݳ����ȹر�DMA 

			RS485_rec_counter = RS485_REC_BUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Stream5);//��ȡDMA���յ����ݳ��ȣ�
                        if(!RS485_REC_Flag)
                        {if(RS485DATA)memcpy(RS485DATA,RS485_buff,RS485_rec_counter);}
			DMA_SetCurrDataCounter(DMA1_Stream5,RS485_REC_BUFF_SIZE);		//���ô������ݳ���    
			DMA_Cmd(DMA1_Stream5, ENABLE);
			RS485_REC_Flag = 1;												//DMA���ձ�־ 
                        if(sem_RS485_rec_flag)xSemaphoreGiveFromISR(sem_RS485_rec_flag,0); 
		}
		if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)					//���ڷ����ж�
		{
			USART_ClearITPendingBit(USART2, USART_IT_TC);
			RS485_dma_send_flag = 0;										//�����ٴη���
		}
		
}  

/***********************************************************************
�������ƣ�RS232_Send_Data(unsigned char *send_buff,unsigned int length)
��    �ܣ�RS232�����ַ���
���������send_buff:�����͵�����ָ�룻length�����͵����ݳ��ȣ��ַ�������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺
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
//�������ƣ�RS232_DMA_Send(unsigned char *send_buff,unsigned int length)
//��    �ܣ�RS232  DMA��ʽ�����ַ���
//���������send_buff:�����͵�����ָ�룻length�����͵����ݳ��ȣ��ַ�������
//���������
//��дʱ�䣺2012.11.22
//�� д �ˣ�
//ע    �⣺
//***********************************************************************/
//void RS232_DMA_Send(unsigned char *send_buff,unsigned int length)
//{
//	while(RS232_dma_send_flag != 0)					//�ȴ��ϴε�DMA�������
//	{
//		;
//	}						 
//	RS232_dma_send_flag = 1;
//	DMA2_Stream7 -> M0AR = (u32)send_buff;
//	DMA_SetCurrDataCounter(DMA2_Stream7,length);  	//���ô��䳤��
//	DMA_Cmd(DMA2_Stream7,ENABLE);					//����DMA����
//}

/***********************************************************************
�������ƣ�RS485_Send_Data(unsigned char *send_buff,unsigned int length)
��    �ܣ�RS485�����ַ���
���������send_buff:�����͵�����ָ�룻length�����͵����ݳ��ȣ��ַ�������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺
***********************************************************************/
void RS485_Send_Data(unsigned char *send_buff,unsigned int length)
{
 	unsigned int i = 0;
	RS_485_TX_EN;		//485����ʹ��
	RS485_Delay(200);		//������ʱ��ע����ʱ�ĳ��̸��ݲ�����������������ԽС����ʱӦ��Խ��
	for(i = 0;i < length;i ++)
	{			
		USART2->DR = send_buff[i];
		while((USART2->SR&0X40)==0);	
	}
	RS485_Delay(50); 		//������ʱ��ע����ʱ�ĳ��̸��ݲ�����������������ԽС����ʱӦ��Խ��
	RS_485_RX_EN;  		//485����ʹ��
}

/***********************************************************************
�������ƣ�void RS485_Delay(uint32_t nCount)
��    �ܣ�RS485�շ���ʱ����
���������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺
***********************************************************************/
static void RS485_Delay(uint32_t nCount)
{ 
  while(nCount > 0)
  { 
  	  nCount --;   
  }
}
