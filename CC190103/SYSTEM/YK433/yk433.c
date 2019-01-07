
//ʹ�ö�ʱ��pwm������ʽ��ic1ͨ����������IC2ͨ����������DMA˫���巽ʽ�����

#include "../YK433/yk433.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "string.h"

u16 *YK433_BUFFER[2]=NULL;
 SemaphoreHandle_t sYK_DATAok;
QueueHandle_t xYKKEY;
/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_GPIO_Config(void) 
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (YK433_ICPWM_GPIO_CLK, ENABLE); 	

	/* ��ʱ���������� */
	GPIO_PinAFConfig(YK433_ICPWM_GPIO_PORT,YK433_ICPWM_PINSOURCE,YK433_ICPWM_AF); 
	
	
	/* �߼����ƶ�ʱ��PWM���벶������ */
	GPIO_InitStructure.GPIO_Pin = YK433_ICPWM_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(YK433_ICPWM_GPIO_PORT, &GPIO_InitStructure);
}

 /**
  * @brief  �߼����ƶ�ʱ�� TIMx,x[1,8]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = YK433_TIM_IRQn; 	
		// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	 
	  // ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


static void TIM_PWMINPUT_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	// ����TIMx_CLK,x[1,8] 
  YK433_TIMER_RCC_PERIPHERAL(YK433_TIM_CLK, ENABLE); 

  TIM_TimeBaseStructure.TIM_Period = 0xFFFF-1; 	
	// �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK=180MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=1000KHz
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / (1000000)) - 1;	
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
  TIM_TimeBaseInit(YK433_TIM, &TIM_TimeBaseStructure);
	
	/* IC1���������ش��� TI1FP1 */
  TIM_ICInitStructure.TIM_Channel = YK433_IC1PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(YK433_TIM, &TIM_ICInitStructure);              //�˺����Զ�����IC1 �� IC2.
	
	
	/* ѡ��ʱ�����봥��: TI1FP1 */
  TIM_SelectInputTrigger(YK433_TIM, TIM_TS_TI1FP1);		//��������λ��

  /* ѡ���ģʽ: ��λģʽ */
  TIM_SelectSlaveMode(YK433_TIM, TIM_SlaveMode_Reset);
  TIM_SelectMasterSlaveMode(YK433_TIM,TIM_MasterSlaveMode_Enable);

//  /* ʹ�ܸ߼����ƶ�ʱ�� */
//  TIM_Cmd(YK433_TIM, ENABLE);
//
//  /* ʹ�ܲ���/�Ƚ�2�ж����� */
//  TIM_ITConfig(YK433_TIM, TIM_IT_CC1, ENABLE);

    /* configure DMA */
    /* DMA clock enable */
    YK433_DMA_RCC_PERIPHERAL(YK433_DMA_PERIPHERAL, ENABLE);

    /* DMA1 Channel Config */
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(YK433_DMA_Stream );

    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&YK433_TIM->YK433_TIMER_CH;
    DMA_InitStructure.DMA_Memory0BaseAddr  = (uint32_t)YK433_BUFFER[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = YK433_DMA_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_Channel = YK433_DMA_CHANNEL;

    DMA_Init(YK433_DMA_Stream , &DMA_InitStructure);

    TIM_DMACmd(YK433_TIM, YK433_DMA_SOURCE, ENABLE);

    DMA_ITConfig(YK433_DMA_Stream , DMA_IT_TC, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = YK433_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //����˫������ģʽ
    DMA_DoubleBufferModeConfig(YK433_DMA_Stream, (uint32_t)YK433_BUFFER[1],(uint32_t)YK433_BUFFER[0]);//�趨���Ż�����
    DMA_DoubleBufferModeCmd(YK433_DMA_Stream, ENABLE);
    TIM_Cmd(YK433_TIM, ENABLE);
    DMA_Cmd(YK433_DMA_Stream, ENABLE);

}
void YK433_DMAHandler(void)
{
    if (DMA_GetFlagStatus(YK433_DMA_Stream,YK433_DMA_TC_FLAG)) {
        DMA_Cmd(YK433_DMA_Stream, DISABLE);
        DMA_ClearFlag(YK433_DMA_Stream,YK433_DMA_TC_FLAG);

        
        DMA_SetCurrDataCounter(YK433_DMA_Stream, YK433_DMA_BUFFER_SIZE);  // load number of bytes to be transferred
        DMA_Cmd(YK433_DMA_Stream, ENABLE);
        xSemaphoreGiveFromISR(sYK_DATAok,0);


    }

}



void  YK433_TIM_IRQHandler (void)
{
static u32 IC1Value,IC2Value;
static float DutyCycle,Frequency;
  /* �����ʱ������/�Ƚ�1�ж� */
  TIM_ClearITPendingBit(YK433_TIM, TIM_IT_CC1);

  /* ��ȡ���벶��ֵ */
  IC1Value = TIM_GetCapture1(YK433_TIM);
  IC2Value = TIM_GetCapture2(YK433_TIM);	
 // printf("IC1Value = %d  IC2Value = %d ",IC1Value,IC2Value);
	
  if (IC1Value != 0)
  {
    /* ռ�ձȼ��� */
    DutyCycle = (float)(IC2Value * 100) / IC1Value;

    /* Ƶ�ʼ��� */
    Frequency = 180000000/1800/(float)IC1Value;
//		printf("ռ�ձȣ�%0.2f%%   Ƶ�ʣ�%0.2fHz\n",DutyCycle,Frequency);
  }
  else
  {
    DutyCycle = 0;
    Frequency = 0;
  }
  
}


/**
  * @brief  ��ʼ���߼����ƶ�ʱ����ʱ��1ms����һ���ж�
  * @param  ��
  * @retval ��
  */
void YK433_Configuration(void)
{
	TIMx_GPIO_Config();
	
	TIMx_NVIC_Configuration();
        YK433_BUFFER[0]=pvPortMalloc(YK433_DMA_BUFFER_SIZE*sizeof(*YK433_BUFFER[0]));
        YK433_BUFFER[1]=pvPortMalloc(YK433_DMA_BUFFER_SIZE*sizeof(*YK433_BUFFER[1]));

	TIM_PWMINPUT_Config();
}




void YK433_task(void *pv)
{
  u32 datatmp[4][2];//�����ٴ档
u16* data=NULL;         //������ָ��
u16 da;                 //�м�����
u8 datalen=66;          //���ݳ���״̬��
u8 datallen=0;          //�����ظ��Լ�����
u32 datah=0,datal=0;
sYK_DATAok=xSemaphoreCreateBinary();
xYKKEY= xQueueCreate( 2, sizeof( u32 )*2 ); 
YK433_Configuration();
while(1)
{
    xSemaphoreTake(sYK_DATAok,portMAX_DELAY);//���ȴ�ʱ��
    if(DMA_GetCurrentMemoryTarget(YK433_DMA_Stream))
      data=YK433_BUFFER[0];
    else
      data=YK433_BUFFER[1];


    for(int i = 0; i < YK433_DMA_BUFFER_SIZE; ++i)
    {
da=*(data+i);

if((da>4900)&&(da<5200)){datalen=0;datah=0;datal=0;continue; }
if((da>650)||(da<100)){datalen=66; }
if(datalen<64){
  if(da>400){
  if(datalen<32) datal|=(1<<(31-datalen));
            else datah|=(1<<(63-datalen));
            }datalen++; 
              }
if(datalen==64)
{
datatmp[3][0]=datatmp[2][0];datatmp[3][1]=datatmp[2][1];
datatmp[2][0]=datatmp[1][0];datatmp[2][1]=datatmp[1][1];
datatmp[1][0]=datatmp[0][0];datatmp[1][1]=datatmp[0][1];
datatmp[0][0]=datah;datatmp[0][1]=datal;
datalen++;
for(int i = 0; i < 4; ++i)      
{
  if (datatmp[0][0]==datatmp[i][0])datallen++;
}
if(datallen>2)
{
     xQueueSend(xYKKEY,datatmp[0],100/portTICK_PERIOD_MS); 
memset(datatmp,0,sizeof(datatmp));
}              //���ݺϷ����š�
datallen=0;

}    
    } 
//uxTaskGetStackHighWaterMark( NULL );  //ʣ��67��
}
}