/*   
* ========================================================= 
*    文件名:  	dcabmoto.h 
*    描述: 	有刷电机带AB编码器驱动头文件
*    版本号:  	1.1   
*    创建日期:  2016-12-22 15:52:03  
*    IDE环境:  	IAR FOR ARM 7.30.3.8062 
*    字符编码:  	UTF-8   
*    作	者:  	吴兵   
*    版权所有:  	哈尔滨工大机器人有限公司
*    版本修改历史:  （XXX:为操作者）
*       2016-12-22 吴兵:创建文件   
*  	2011-4-22 XXX:增加方法Sub、Add   
* ===========================================================  */     
//0电机AB编码
#define DCMOTO0_AB1_PIN             GPIO_Pin_5            
#define DCMOTO0_AB2_PIN             GPIO_Pin_3            
#define DCMOTO0_AB1_GPIO_PORT       GPIOA                      
#define DCMOTO0_AB2_GPIO_PORT       GPIOB                      
#define DCMOTO0_AB1_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define DCMOTO0_AB2_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define DCMOTO0_AB1_PINSOURCE	    GPIO_PinSource5
#define DCMOTO0_AB2_PINSOURCE	    GPIO_PinSource3
#define DCMOTO0_AB_AF		    GPIO_AF_TIM2
#define DCMOTO0_AB_TIME		    TIM2
#define DCMOTO0_AB1_CHANNEL         TIM_Channel_1
#define DCMOTO0_AB2_CHANNEL         TIM_Channel_2

//1电机AB编码
#define DCMOTO1_AB1_PIN             GPIO_Pin_0            
#define DCMOTO1_AB2_PIN             GPIO_Pin_1            
#define DCMOTO1_AB1_GPIO_PORT       GPIOA                      
#define DCMOTO1_AB2_GPIO_PORT       GPIOA                      
#define DCMOTO1_AB1_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define DCMOTO1_AB2_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define DCMOTO1_AB1_PINSOURCE	    GPIO_PinSource0
#define DCMOTO1_AB2_PINSOURCE	    GPIO_PinSource1
#define DCMOTO1_AB_AF		    GPIO_AF_TIM5
#define DCMOTO1_AB_TIME		    TIM5
#define DCMOTO1_AB1_CHANNEL         TIM_Channel_1
#define DCMOTO1_AB2_CHANNEL         TIM_Channel_2

//2电机AB编码
#define DCMOTO2_AB1_PIN             GPIO_Pin_6            
#define DCMOTO2_AB2_PIN             GPIO_Pin_7            
#define DCMOTO2_AB1_GPIO_PORT       GPIOC                      
#define DCMOTO2_AB2_GPIO_PORT       GPIOC                      
#define DCMOTO2_AB1_GPIO_CLK        RCC_AHB1Periph_GPIOC
#define DCMOTO2_AB2_GPIO_CLK        RCC_AHB1Periph_GPIOC
#define DCMOTO2_AB1_PINSOURCE	    GPIO_PinSource6
#define DCMOTO2_AB2_PINSOURCE	    GPIO_PinSource7
#define DCMOTO2_AB_AF		    GPIO_AF_TIM3
#define DCMOTO2_AB_TIME		    TIM3
#define DCMOTO2_AB1_CHANNEL         TIM_Channel_1
#define DCMOTO2_AB2_CHANNEL         TIM_Channel_2

//3电机AB编码
#define DCMOTO3_AB1_PIN             GPIO_Pin_6            
#define DCMOTO3_AB2_PIN             GPIO_Pin_7            
#define DCMOTO3_AB1_GPIO_PORT       GPIOB                      
#define DCMOTO3_AB2_GPIO_PORT       GPIOB                      
#define DCMOTO3_AB1_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define DCMOTO3_AB2_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define DCMOTO3_AB1_PINSOURCE	    GPIO_PinSource6
#define DCMOTO3_AB2_PINSOURCE	    GPIO_PinSource7
#define DCMOTO3_AB_AF		    GPIO_AF_TIM4
#define DCMOTO3_AB_TIME		    TIM4
#define DCMOTO3_AB1_CHANNEL         TIM_Channel_1
#define DCMOTO3_AB2_CHANNEL         TIM_Channel_2

 void moto_AB_INIT(void);
s32 dcmotowz(u8 moto);
void dcmotoset(TIM_TypeDef* motot, s16 Counter);
void dcmoto(void* pv);
void highreset(void);

u16 highget (void);
