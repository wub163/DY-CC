
#include "can.h"
#include "CCST.h"
#include "main.h"
void NVIC_CAN (void);

//
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ. @ref CAN_synchronisation_jump_width   ��Χ: ; CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   @ref CAN_time_quantum_in_bit_segment_2 ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   @refCAN_time_quantum_in_bit_segment_1  ��Χ: ;	  CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;(ʵ��Ҫ��1,Ҳ����1~1024) tq=(brp)*tpclk1
//������=Fpclk1/((tsjw+tbs1+tbs2+3)*brp);
//mode: @ref CAN_operating_mode ��Χ��CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��;
unsigned char gMag = 0;
u8 CAN1_Mode_Init(u16 BAUD)   //CAN1 PB8��9  CAN2 PB12,13
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    //ʹ�����ʱ��
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);	
	
    //��ʼ��GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;     //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //����
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	  //���Ÿ���ӳ������
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOB8����ΪCAN1
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOB9����ΪCAN1
	  CAN_DeInit(CAN1);
  	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM = DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM = DISABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM = DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART = DISABLE;//ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM = DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP = DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //ģʽ���� 
//        CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		   //BTR-SJW ����ͬ����Ծ��� 1��ʱ�䵥Ԫ
//        CAN_InitStructure.CAN_BS1=CAN_BS1_6tq;		   //BTR-TS1 ʱ���1 ռ����2��ʱ�䵥Ԫ
//        CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;		   //BTR-TS1 ʱ���2 ռ����3��ʱ�䵥Ԫ
//  	CAN_InitStructure.CAN_Prescaler = 6000/BAUD;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
//  	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 

        
        //  	CAN_InitStructure.CAN_Mode= CAN_Mode_LoopBack;	 //ģʽ���� 
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;		   //BTR-SJW ����ͬ����Ծ��� 2��ʱ�䵥Ԫ
	 
	/* ss=1 bs1=5 bs2=3 λʱ����Ϊ(1+5+3) �����ʼ�Ϊʱ������tq*(1+3+5)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;		   //BTR-TS1 ʱ���1 ռ����5��ʱ�䵥Ԫ
	CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;		   //BTR-TS1 ʱ���2 ռ����3��ʱ�䵥Ԫ	
	
	/* CAN Baudrate = 1 MBps (1MBps��Ϊstm32��CAN�������) (CAN ʱ��Ƶ��Ϊ APB 1 = 45 MHz) */     
	CAN_InitStructure.CAN_Prescaler =5000/BAUD;		  //BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 42/(1+4+2)/6=1 Mbps
	CAN_Init(CAN1, &CAN_InitStructure);

        
		//���ù�����
	  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //���������1
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
        
        NVIC_CAN();
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	  return 0;
}   



u8 CAN2_Mode_Init(u16 BAUD)   //CAN1 PB8��9  CAN2 PB12,13
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    //ʹ�����ʱ��
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1|RCC_APB1Periph_CAN2, ENABLE);//������CAN2ʱ����Ч�����ʹ���CAN1Ҳ����򿪡�	
	
    //��ʼ��GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;     //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //����
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	  //���Ÿ���ӳ������
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2); //GPIOB8����ΪCAN1
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2); //GPIOB9����ΪCAN1
	  CAN_DeInit(CAN2);
  	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM = DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM = DISABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM = DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART = DISABLE;//ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM = DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP = DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //ģʽ���� 
//        CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		   //BTR-SJW ����ͬ����Ծ��� 1��ʱ�䵥Ԫ
//        CAN_InitStructure.CAN_BS1=CAN_BS1_6tq;		   //BTR-TS1 ʱ���1 ռ����2��ʱ�䵥Ԫ
//        CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;		   //BTR-TS1 ʱ���2 ռ����3��ʱ�䵥Ԫ
//  	CAN_InitStructure.CAN_Prescaler = 6000/BAUD;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
//  	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 

        
        //  	CAN_InitStructure.CAN_Mode= CAN_Mode_LoopBack;	 //ģʽ���� 
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		   //BTR-SJW ����ͬ����Ծ��� 2��ʱ�䵥Ԫ
	 
	/* ss=1 bs1=5 bs2=3 λʱ����Ϊ(1+5+3) �����ʼ�Ϊʱ������tq*(1+3+5)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_4tq;		   //BTR-TS1 ʱ���1 ռ����5��ʱ�䵥Ԫ
	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		   //BTR-TS1 ʱ���2 ռ����3��ʱ�䵥Ԫ	
	
	/* CAN Baudrate = 1 MBps (1MBps��Ϊstm32��CAN�������) (CAN ʱ��Ƶ��Ϊ APB 1 = 45 MHz) */     
	CAN_InitStructure.CAN_Prescaler =6000/BAUD;		  //BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 42/(1+4+2)/6=1 Mbps
	CAN_Init(CAN2, &CAN_InitStructure);

        
		//���ù�����         CAN1
//	CAN_FilterInitStructure.CAN_FilterNumber = 0;
//  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
//  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
//  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
//  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
//  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
//  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
//   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
//  	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //���������1
//  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��

		//���ù�����         CAN2  
        CAN_FilterInitStructure.CAN_FilterNumber=14;            //CAN2����ʼ��������
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

   
    switch(RxMessage.StdId & 0x11f)  //�������ʹ����������ĵ�ַ�ռ�ֻ�ܣ�1-127��
    {
              case 0x11B://�����������
          if(xCANRMDS)xQueueSendFromISR(xCANRMDS,&RxMessage,0);break;
              case 0x017://�οƵµ��
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
    case 0x10: //���
//      if(xCANID)xQueueSendFromISR(xCANPE,&RxMessage,0);break;
    default:      break;

    }
}


//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN1_Send_Msg(u16 id,u8* msg,u8 len)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=id;	 // ��׼��ʶ��Ϊ0
  TxMessage.ExtId=0;	 // ������չ��ʾ����29λ��
  TxMessage.IDE=0;		  // ʹ����չ��ʶ��
  TxMessage.RTR=0;		  // ��Ϣ����Ϊ����֡��һ֡8λ
  TxMessage.DLC=len;							 // ������֡��Ϣ
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          

  mbox=CAN_Transmit(CAN1, &TxMessage);
  if( mbox>3)
  {vTaskDelay(5/portTICK_PERIOD_MS);//��������������ʱ�ط���
 CAN_Transmit(CAN1, &TxMessage);}
return mbox;

}


u8 CAN2_Send_Msg(u16 id,u8* msg,u8 len)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=id;	 // ��׼��ʶ��Ϊ0
  TxMessage.ExtId=0;	 // ������չ��ʾ����29λ��
  TxMessage.IDE=0;		  // ʹ����չ��ʶ��
  TxMessage.RTR=0;		  // ��Ϣ����Ϊ����֡��һ֡8λ
  TxMessage.DLC=len;							 // ������֡��Ϣ
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          

  mbox=CAN_Transmit(CAN2, &TxMessage);
  if( mbox>3)
  {vTaskDelay(5/portTICK_PERIOD_MS);//��������������ʱ�ط���
 CAN_Transmit(CAN2, &TxMessage);}
return mbox;

}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
    for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}

u8 CAN2_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN2,CAN_FIFO0)==0)return 0;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);//��ȡ����	
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





