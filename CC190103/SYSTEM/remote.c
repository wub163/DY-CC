#include "remote.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "sys.h"
#include "GPIO.h"


#define delay_ms(x)		 vTaskDelay((x)/portTICK_PERIOD_MS)
QueueHandle_t xAUTO_CHANGE=NULL;
 s16 speed[2]={0,};

 
vu16 ADC_ConvertedValue[4];
vu16 ADC_LED[3];
uint8_t key=0xff;
__IO u8 key_s[3];
 __IO u8 key_success;
// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
u16 ADC_ConvertedValueLocal[4]; 
 __IO uint8_t chargon;

 
__IO uint16_t ADC_ConvertedValue[RHEOSTAT_NOFCHANEL]={0};

/**
  * @brief  ����ADC��GPIO
  * @param  ��
  * @retval ��
  */
static void Rheostat_ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// ʹ�� GPIO ʱ��
	RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK, ENABLE);
		
	// ���� IO
  GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC1_CH3|RHEOSTAT_ADC1_CH4|RHEOSTAT_ADC1_CH5|RHEOSTAT_ADC1_CH6;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	    
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; //������������
	GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);			
}

/**
  * @brief  ����ADC��DMA����
  * @param  ��
  * @retval ��
  */
static void Rheostat_ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
  // ����ADCʱ��
	RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK , ENABLE);
	// ����DMAʱ��
  RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_DMA_CLK, ENABLE);
	
	// ------------------DMA Init �ṹ����� ��ʼ��--------------------------
	// ѡ�� DMA ͨ����ͨ����������������
  DMA_InitStructure.DMA_Channel = RHEOSTAT_ADC_DMA_CHANNEL;
	
	// �����ַΪ��ADC ���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = RHEOSTAT_ADC_DR_ADDR;	
  // �洢����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC_ConvertedValue;  
	
  // ���ݴ��䷽��Ϊ���赽�洢��	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
	// ��������С��ָһ�δ����������
	DMA_InitStructure.DMA_BufferSize = RHEOSTAT_NOFCHANEL;	
	
	// ����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  // �洢����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	
  // // �������ݴ�СΪ���֣��������ֽ� 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
  //	�洢�����ݴ�СҲΪ���֣����������ݴ�С��ͬ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	
	// ѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  // DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
  // ��ֹDMA FIFO	��ʹ��ֱ��ģʽ
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  
  // FIFO ��ֵ��С��FIFOģʽ��ֹʱ�������������	
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	
	// �洢��ͻ�����δ���
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	// ����ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
  //��ʼ��DMA�����������൱��һ����Ĺܵ����ܵ������кܶ�ͨ��
	DMA_Init(RHEOSTAT_ADC_DMA_STREAM, &DMA_InitStructure);
	
	// ʹ��DMA������
  DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);

  // -------------------ADC Common �ṹ�� ���� ��ʼ��------------------------
	// ����ADCģʽ
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // ʱ��Ϊfpclk x��Ƶ	
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  // ��ֹDMAֱ�ӷ���ģʽ	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // ����ʱ����	
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;  
  ADC_CommonInit(&ADC_CommonInitStructure);
	
  // -------------------ADC Init �ṹ�� ���� ��ʼ��--------------------------
  // ADC �ֱ���
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  // ����ɨ��ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
  // ����ת��	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  //��ֹ�ⲿ���ش���
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  //ʹ������������ⲿ�����������ã�ע�͵�����
  //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  //�����Ҷ���	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  //ת��ͨ�� x��
  ADC_InitStructure.ADC_NbrOfConversion = RHEOSTAT_NOFCHANEL;                                    
  ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
  //---------------------------------------------------------------------------
	
  // ���� ADC ͨ��ת��˳��Ͳ���ʱ������
  ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL3, 1, ADC_SampleTime_84Cycles);
  ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL4, 2, ADC_SampleTime_84Cycles); 
  ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL5, 3, ADC_SampleTime_84Cycles); 
	ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL6, 4, ADC_SampleTime_84Cycles); 
	
  // ʹ��DMA���� after last transfer (Single-ADC mode)
  ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC, ENABLE);
  // ʹ��ADC DMA
  ADC_DMACmd(RHEOSTAT_ADC, ENABLE);
  // ʹ��ADC
  ADC_Cmd(RHEOSTAT_ADC, ENABLE);  
  //��ʼadcת�����������
  ADC_SoftwareStartConv(RHEOSTAT_ADC);
}

/**
  * @brief  ADC1��ʼ��
  * @param  ��
  * @retval ��
  */
void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
}
 
void AutoCharg_GPIO_Config(void)
{
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����LED��ص�GPIO����ʱ��*/
		RCC_AHB1PeriphClockCmd ( SWITCH_CHARG_GPIO_CLK|RELAY1_GPIO_CLK, ENABLE); 



		/*��������ģʽΪ���ģʽ*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*�������ŵ��������Ϊ�������*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*��������Ϊ����ģʽ*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		/*������������Ϊ100MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	
	
		/*ѡ��Ҫ���Ƶ�GPIO����*/															   
		GPIO_InitStructure.GPIO_Pin = SWITCH_CHARG_PIN;	
		GPIO_Init(SWITCH_CHARG_GPIO_PORT, &GPIO_InitStructure);	
    
    /*ѡ��Ҫ���Ƶ�GPIO����*/															   
		GPIO_InitStructure.GPIO_Pin = RELAY1_PIN;	
    GPIO_Init(RELAY1_GPIO_PORT, &GPIO_InitStructure);	
    
		
		RELAY1(OFF);
		SWITCH_CHARG(OFF);
}

//����ң�س�ʼ��
//����IO�Լ�TIM2_CH1�����벶��
void Remote_Init(void)    			  
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef  TIM3_ICInitStructure;

	AutoCharg_GPIO_Config();
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//TIM1ʱ��ʹ�� 

  //GPIOA8  ���ù���,����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3); //GPIOA8����ΪTIM1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM3); //GPIOA8����ΪTIM1
	
	
	TIM_TimeBaseStructure.TIM_Prescaler=90-1;  ////Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=10000;   //�趨�������Զ���װֵ ���10ms���  
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure); 

 	//��ʼ��TIM1���벶�����
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM3_ICInitStructure.TIM_ICFilter = 0x03;//IC1F=0003 8����ʱ��ʱ�������˲�
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);//��ʼ����ʱ��1���벶��ͨ��

 	//��ʼ��TIM1���벶�����
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM3_ICInitStructure.TIM_ICFilter = 0x03;//IC1F=0003 8����ʱ��ʱ�������˲�
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);//��ʼ����ʱ��1���벶��ͨ��
 
	

	TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC3,ENABLE);//��������ж� ,����CC1IE�����ж�	
	TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC4,ENABLE);//��������ж� ,����CC1IE�����ж�	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	
  TIM_Cmd(TIM3,ENABLE ); 	 	//ʹ�ܶ�ʱ��1
 
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//��ʼ��NVIC�Ĵ���
	
}

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM2_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//��ʼ��TIM2
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
u8 	RmtSta=0;	  	  
u16 Dval1;		//�½���ʱ��������ֵ
u16 Dval4;		//�½���ʱ��������ֵ
u16 Dval;		//�½���ʱ��������ֵ
u32 RmtDataTemp=0;	//������յ�������	   		    
u8  RmtData=0;	//������յ�������	
u8  LEDCnt=0;	//�������µĴ���	 

u8 	SSta=0;
u16 	SScnt=0;
u8 aaas=0;
u8 bitcnt=0;
//��ʱ��3���벶���жϷ������	 
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_CC3)==SET) //������(CC1IE)�ж�
	{
		if(RDATA1)//�����ز���
		{
			TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
//			TIM_SetCounter(TIM3,0);	   	//��ն�ʱ��ֵ
			Dval1=TIM_GetCapture3(TIM3);//��ȡCCR1Ҳ������CC1IF��־λ
			RmtSta|=(1<<4);					//����������Ѿ�������
		
//			LED2=1;
		}
		else //�½��ز���
		{
			Dval=TIM_GetCapture3(TIM3);//��ȡCCR1Ҳ������CC1IF��־λ
			if(Dval<Dval1)
			{
				Dval1=10000-Dval1+Dval;
			}
			else
				Dval1=Dval-Dval1;
			TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Rising); //CC1P=0	����Ϊ�����ز���
//			LED2=0;
//			USART_SendData(UART5,Dval>>8);
//			while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
//			USART_SendData(UART5,Dval);
//			while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
			if(RmtSta&(1<<4))					//���һ�θߵ�ƽ���� 
			{
 				if(RmtSta&(1<<7))//���յ���������
				{
					if(Dval1>250&&Dval1<350)			//300Ϊ��׼ֵ,300us
					{
						RmtDataTemp<<=1;	//����һλ.
						RmtDataTemp|=0;	//���յ�0	   
						
						bitcnt++;
						if(bitcnt==2)//NO���յ���������
						{
							TIM_SetCounter(TIM2,0);	   	//��ն�ʱ��ֵ
							TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��2
						}
					}else if(Dval1>550&&Dval1<650)	//600Ϊ��׼ֵ,600us
					{
						RmtDataTemp<<=1;	//����һλ.
						RmtDataTemp|=1;	//���յ�1
						
						bitcnt++;
						if(bitcnt==2)//NO���յ���������
						{
							TIM_SetCounter(TIM2,0);	   	//��ն�ʱ��ֵ
							TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��2
						}
					}else if(Dval1>1150&&Dval1<1250)	//�õ�������ֵ���ӵ���Ϣ 1200Ϊ��׼ֵ1.2ms
					{
						key=RmtDataTemp;
						RmtData=RmtDataTemp; 		//
						RmtSta|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�

						bitcnt=0;
						RmtSta&=~(1<<7);
					}else
					{
						RmtSta&=~(1<<7);
						bitcnt=0;
					}
 				}
				else if(Dval1>850&&Dval1<950)		//900Ϊ��׼ֵ0.9ms
				{
					RmtSta|=1<<7;	//��ǳɹ����յ���������
					RmtData=0;		//���
					RmtDataTemp=0;
					bitcnt=0;
				}
			}
			RmtSta&=~(1<<4);
		}
		
		if(RmtSta&(1<<6))
		{
			RmtSta&=~(1<<6);
			switch(key)
			{
				case 0:
					key_s[0]=1;
					key_success=1;
					ADC_LED[0]=ADC_ConvertedValueLocal[1];
					break;
				case 1:
					key_s[1]=1;
					key_success=1;
					ADC_LED[1]=ADC_ConvertedValueLocal[1];
					break;
				case 2:
					key_s[2]=1;
					key_success=1;
					ADC_LED[2]=ADC_ConvertedValueLocal[1];
					break;
				default: 
					key=0xff;
					break;
			}
		}
	}

	if(TIM_GetITStatus(TIM3,TIM_IT_CC4)==SET) //������(CC1IE)�ж�
	{
		if(RDPWM1)//�����ز���
		{
			TIM_OC4PolarityConfig(TIM3,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
//			TIM_SetCounter(TIM1,0);	   	//��ն�ʱ��ֵ
			Dval4=TIM_GetCapture4(TIM3);//��ȡCCR1Ҳ������CC1IF��־λ
////			RmtSta|=(1<<4);					//����������Ѿ�������
////			LED2=1;
			
////////////////////////			TIM_SetCounter(TIM2,0);	   	//��ն�ʱ��ֵ
////////////////////////			TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��2
		}
		else //�½��ز���
		{
			Dval=TIM_GetCapture4(TIM3);//��ȡCCR1Ҳ������CC1IF��־λ
			if(Dval<Dval4)
			{
				Dval4=10000-Dval4+Dval;
			}
			else
				Dval4=Dval-Dval4;
			TIM_OC4PolarityConfig(TIM3,TIM_ICPolarity_Rising); //CC1P=0	����Ϊ�����ز���
			
			if(Dval4>480 && Dval4<520)
			{
				SSta=1;
				if(SScnt<1000)
				{
					SScnt++;
				}
				else
				{
					RELAY1(ON);
					SWITCH_CHARG(ON);
					chargon=1;
				}
			}
		}
	}
	
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		if(SSta==1)
			SSta=0;
		else
		{
//			RELAY1(OFF);
			SWITCH_CHARG(OFF);
			chargon=0;
			SScnt=0;
		}
	}	
//	aaas%=2;
//	if(aaas++)
//		LED3(ON);
//	else
//		LED3(OFF);
 //                 PDout(11)=!PDout(11);
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ 
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_CC3 | TIM_IT_CC4);  //����жϱ�־λ 
;
}


//��ʱ��2�жϷ�����

extern u8 ssadc;
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //����ж�
    {
      TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
			TIM_Cmd(TIM2,DISABLE); //�رն�ʱ��2
//			ssadc=1;
			ADC_ConvertedValueLocal[1] = ADC_ConvertedValue[1];
			
    }
}


static void MoveWheelInSpeedMode(s16 a,s16 b)
{
speed[0]=(a*3)>>1;
speed[1]=-(b*3)>>1;


}


u8 ssadc=0;


// ADCת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
__IO uint16_t ADC_ConvertedValue[4];

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
u16 ADC_ConvertedValueLocal[4]={0}; 


__IO uint16_t ADC_LED[3]={0,0,0};

extern uint8_t key;
__IO uint8_t chargon=0;
__IO u8 key_success=0;
__IO u8 key_s[3]={0,0,0};

void change(void* pv)
{
	uint16_t speedtemp=0;
	u8 i=0;
	uint8_t aa=0x00;
	u32 t=0;
	__IO uint16_t k=0;
        
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	
	
	TIM2_Int_Init(70-1,900-1);	//��ʱ��ʱ��90M����Ƶϵ��9000������90M/9000=10Khz�ļ���Ƶ�ʣ�����1��Ϊ20*10us=200us  
 	Remote_Init();				//������ճ�ʼ��		 	
	Rheostat_Init(); 				//ADCͨ����ʼ��
 xAUTO_CHANGE  = xQueueCreate( 1, sizeof( int16_t )*2 );       //С���ƶ��ٶ�ֵ

	
	while(1)
	{
		delay_ms(50);
                LEDB=!LEDB;
		if(key_success==1 && chargon==0)
		{
			key_success=0;
			aa=0x00;
			aa|=key_s[0];
			aa<<=1;
			aa|=key_s[1];
			aa<<=1;
			aa|=key_s[2];
			
			key_s[0]=0;
			key_s[1]=0;
			key_s[2]=0;
			switch(aa)
			{
				case 2:        																															   //111
					if(ADC_LED[1]>3500)
					{
						MoveWheelInSpeedMode(500,-500);
					}
					break;		
				case 7:        																															   //111
					if(ADC_LED[0]>ADC_LED[2]+100)
					{
						speedtemp=(ADC_LED[0]-ADC_LED[2])/ADC_LED[0]*500;
						if(speedtemp>500)
							speedtemp=500;
						MoveWheelInSpeedMode(500+speedtemp,-(500-speedtemp));
					}
					else
						if(ADC_LED[2]>ADC_LED[0]+100)
						{
							speedtemp=(ADC_LED[2]-ADC_LED[0])/ADC_LED[2]*500;
							if(speedtemp>500)
								speedtemp=500;
							MoveWheelInSpeedMode((500-speedtemp),-(500+speedtemp));
						}
					break;
				case 3:        																															   //011
					MoveWheelInSpeedMode(0,-500);
					break;
				case 6:         																															 //111
					MoveWheelInSpeedMode(500,0);
					break;
				case 1:          																															 //001
					MoveWheelInSpeedMode(-500,-500);
					break;
				case 4:         																															 //100
					MoveWheelInSpeedMode(500,500);
					break;
				case 0:           																														 //000
					MoveWheelInSpeedMode(0,0);
					break;
				default :
					MoveWheelInSpeedMode(0,0);
					break;
			}
                        
                        
		}
		else if(chargon==1)
			MoveWheelInSpeedMode(0,0);
                
                        xQueueSend(xAUTO_CHANGE,speed,0);

	}
}

