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
// 局部变量，用于保存转换计算后的电压值 	 
u16 ADC_ConvertedValueLocal[4]; 
 __IO uint8_t chargon;

 
__IO uint16_t ADC_ConvertedValue[RHEOSTAT_NOFCHANEL]={0};

/**
  * @brief  配置ADC的GPIO
  * @param  无
  * @retval 无
  */
static void Rheostat_ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 使能 GPIO 时钟
	RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK, ENABLE);
		
	// 配置 IO
  GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC1_CH3|RHEOSTAT_ADC1_CH4|RHEOSTAT_ADC1_CH5|RHEOSTAT_ADC1_CH6;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	    
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; //不上拉不下拉
	GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);			
}

/**
  * @brief  配置ADC，DMA传输
  * @param  无
  * @retval 无
  */
static void Rheostat_ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
  // 开启ADC时钟
	RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK , ENABLE);
	// 开启DMA时钟
  RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_DMA_CLK, ENABLE);
	
	// ------------------DMA Init 结构体参数 初始化--------------------------
	// 选择 DMA 通道，通道存在于数据流中
  DMA_InitStructure.DMA_Channel = RHEOSTAT_ADC_DMA_CHANNEL;
	
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = RHEOSTAT_ADC_DR_ADDR;	
  // 存储器地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC_ConvertedValue;  
	
  // 数据传输方向为外设到存储器	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
	// 缓冲区大小，指一次传输的数据项
	DMA_InitStructure.DMA_BufferSize = RHEOSTAT_NOFCHANEL;	
	
	// 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  // 存储器地址递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	
  // // 外设数据大小为半字，即两个字节 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
  //	存储器数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  // DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
  // 禁止DMA FIFO	，使用直连模式
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  
  // FIFO 阈值大小，FIFO模式禁止时，这个不用配置	
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	
	// 存储器突发单次传输
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	// 外设突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
  //初始化DMA数据流，流相当于一个大的管道，管道里面有很多通道
	DMA_Init(RHEOSTAT_ADC_DMA_STREAM, &DMA_InitStructure);
	
	// 使能DMA数据流
  DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);

  // -------------------ADC Common 结构体 参数 初始化------------------------
	// 独立ADC模式
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // 时钟为fpclk x分频	
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  // 禁止DMA直接访问模式	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // 采样时间间隔	
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;  
  ADC_CommonInit(&ADC_CommonInitStructure);
	
  // -------------------ADC Init 结构体 参数 初始化--------------------------
  // ADC 分辨率
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  // 开启扫描模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
  // 连续转换	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  //禁止外部边沿触发
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  //使用软件触发，外部触发不用配置，注释掉即可
  //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  //数据右对齐	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  //转换通道 x个
  ADC_InitStructure.ADC_NbrOfConversion = RHEOSTAT_NOFCHANEL;                                    
  ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
  //---------------------------------------------------------------------------
	
  // 配置 ADC 通道转换顺序和采样时间周期
  ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL3, 1, ADC_SampleTime_84Cycles);
  ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL4, 2, ADC_SampleTime_84Cycles); 
  ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL5, 3, ADC_SampleTime_84Cycles); 
	ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL6, 4, ADC_SampleTime_84Cycles); 
	
  // 使能DMA请求 after last transfer (Single-ADC mode)
  ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC, ENABLE);
  // 使能ADC DMA
  ADC_DMACmd(RHEOSTAT_ADC, ENABLE);
  // 使能ADC
  ADC_Cmd(RHEOSTAT_ADC, ENABLE);  
  //开始adc转换，软件触发
  ADC_SoftwareStartConv(RHEOSTAT_ADC);
}

/**
  * @brief  ADC1初始化
  * @param  无
  * @retval 无
  */
void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
}
 
void AutoCharg_GPIO_Config(void)
{
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启LED相关的GPIO外设时钟*/
		RCC_AHB1PeriphClockCmd ( SWITCH_CHARG_GPIO_CLK|RELAY1_GPIO_CLK, ENABLE); 



		/*设置引脚模式为输出模式*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*设置引脚为上拉模式*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		/*设置引脚速率为100MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	
	
		/*选择要控制的GPIO引脚*/															   
		GPIO_InitStructure.GPIO_Pin = SWITCH_CHARG_PIN;	
		GPIO_Init(SWITCH_CHARG_GPIO_PORT, &GPIO_InitStructure);	
    
    /*选择要控制的GPIO引脚*/															   
		GPIO_InitStructure.GPIO_Pin = RELAY1_PIN;	
    GPIO_Init(RELAY1_GPIO_PORT, &GPIO_InitStructure);	
    
		
		RELAY1(OFF);
		SWITCH_CHARG(OFF);
}

//红外遥控初始化
//设置IO以及TIM2_CH1的输入捕获
void Remote_Init(void)    			  
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef  TIM3_ICInitStructure;

	AutoCharg_GPIO_Config();
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOA时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//TIM1时钟使能 

  //GPIOA8  复用功能,上拉
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3); //GPIOA8复用为TIM1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM3); //GPIOA8复用为TIM1
	
	
	TIM_TimeBaseStructure.TIM_Prescaler=90-1;  ////预分频器,1M的计数频率,1us加1.	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=10000;   //设定计数器自动重装值 最大10ms溢出  
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure); 

 	//初始化TIM1输入捕获参数
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	选择输入端 IC1映射到TI1上
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM3_ICInitStructure.TIM_ICFilter = 0x03;//IC1F=0003 8个定时器时钟周期滤波
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);//初始化定时器1输入捕获通道

 	//初始化TIM1输入捕获参数
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	选择输入端 IC1映射到TI1上
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM3_ICInitStructure.TIM_ICFilter = 0x03;//IC1F=0003 8个定时器时钟周期滤波
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);//初始化定时器1输入捕获通道
 
	

	TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC3,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	
  TIM_Cmd(TIM3,ENABLE ); 	 	//使能定时器1
 
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//初始化NVIC寄存器
	
}

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM2_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化TIM2
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM2,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
u8 	RmtSta=0;	  	  
u16 Dval1;		//下降沿时计数器的值
u16 Dval4;		//下降沿时计数器的值
u16 Dval;		//下降沿时计数器的值
u32 RmtDataTemp=0;	//红外接收到的数据	   		    
u8  RmtData=0;	//红外接收到的数据	
u8  LEDCnt=0;	//按键按下的次数	 

u8 	SSta=0;
u16 	SScnt=0;
u8 aaas=0;
u8 bitcnt=0;
//定时器3输入捕获中断服务程序	 
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_CC3)==SET) //处理捕获(CC1IE)中断
	{
		if(RDATA1)//上升沿捕获
		{
			TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
//			TIM_SetCounter(TIM3,0);	   	//清空定时器值
			Dval1=TIM_GetCapture3(TIM3);//读取CCR1也可以清CC1IF标志位
			RmtSta|=(1<<4);					//标记上升沿已经被捕获
		
//			LED2=1;
		}
		else //下降沿捕获
		{
			Dval=TIM_GetCapture3(TIM3);//读取CCR1也可以清CC1IF标志位
			if(Dval<Dval1)
			{
				Dval1=10000-Dval1+Dval;
			}
			else
				Dval1=Dval-Dval1;
			TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Rising); //CC1P=0	设置为上升沿捕获
//			LED2=0;
//			USART_SendData(UART5,Dval>>8);
//			while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
//			USART_SendData(UART5,Dval);
//			while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
			if(RmtSta&(1<<4))					//完成一次高电平捕获 
			{
 				if(RmtSta&(1<<7))//接收到了引导码
				{
					if(Dval1>250&&Dval1<350)			//300为标准值,300us
					{
						RmtDataTemp<<=1;	//左移一位.
						RmtDataTemp|=0;	//接收到0	   
						
						bitcnt++;
						if(bitcnt==2)//NO接收到了引导码
						{
							TIM_SetCounter(TIM2,0);	   	//清空定时器值
							TIM_Cmd(TIM2,ENABLE); //使能定时器2
						}
					}else if(Dval1>550&&Dval1<650)	//600为标准值,600us
					{
						RmtDataTemp<<=1;	//左移一位.
						RmtDataTemp|=1;	//接收到1
						
						bitcnt++;
						if(bitcnt==2)//NO接收到了引导码
						{
							TIM_SetCounter(TIM2,0);	   	//清空定时器值
							TIM_Cmd(TIM2,ENABLE); //使能定时器2
						}
					}else if(Dval1>1150&&Dval1<1250)	//得到按键键值增加的信息 1200为标准值1.2ms
					{
						key=RmtDataTemp;
						RmtData=RmtDataTemp; 		//
						RmtSta|=1<<6;//标记已经完成一次按键的键值信息采集

						bitcnt=0;
						RmtSta&=~(1<<7);
					}else
					{
						RmtSta&=~(1<<7);
						bitcnt=0;
					}
 				}
				else if(Dval1>850&&Dval1<950)		//900为标准值0.9ms
				{
					RmtSta|=1<<7;	//标记成功接收到了引导码
					RmtData=0;		//清除
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

	if(TIM_GetITStatus(TIM3,TIM_IT_CC4)==SET) //处理捕获(CC1IE)中断
	{
		if(RDPWM1)//上升沿捕获
		{
			TIM_OC4PolarityConfig(TIM3,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
//			TIM_SetCounter(TIM1,0);	   	//清空定时器值
			Dval4=TIM_GetCapture4(TIM3);//读取CCR1也可以清CC1IF标志位
////			RmtSta|=(1<<4);					//标记上升沿已经被捕获
////			LED2=1;
			
////////////////////////			TIM_SetCounter(TIM2,0);	   	//清空定时器值
////////////////////////			TIM_Cmd(TIM2,ENABLE); //使能定时器2
		}
		else //下降沿捕获
		{
			Dval=TIM_GetCapture4(TIM3);//读取CCR1也可以清CC1IF标志位
			if(Dval<Dval4)
			{
				Dval4=10000-Dval4+Dval;
			}
			else
				Dval4=Dval-Dval4;
			TIM_OC4PolarityConfig(TIM3,TIM_ICPolarity_Rising); //CC1P=0	设置为上升沿捕获
			
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
	
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
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
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位 
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_CC3 | TIM_IT_CC4);  //清除中断标志位 
;
}


//定时器2中断服务函数

extern u8 ssadc;
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
    {
      TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
			TIM_Cmd(TIM2,DISABLE); //关闭定时器2
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


// ADC转换的电压值通过MDA方式传到SRAM
__IO uint16_t ADC_ConvertedValue[4];

// 局部变量，用于保存转换计算后的电压值 	 
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
        
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	
	TIM2_Int_Init(70-1,900-1);	//定时器时钟90M，分频系数9000，所以90M/9000=10Khz的计数频率，计数1次为20*10us=200us  
 	Remote_Init();				//红外接收初始化		 	
	Rheostat_Init(); 				//ADC通道初始化
 xAUTO_CHANGE  = xQueueCreate( 1, sizeof( int16_t )*2 );       //小车移动速度值

	
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

