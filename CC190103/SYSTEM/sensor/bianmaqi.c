/****************************************
*****无线模块和红外避障所用引脚配置********
****************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bianmaqi.h"
//#include "delay.h"
#define CS1_H   PAout(4)=1   //片选高电平
#define CS1_L   PAout(4)=0   //片选低电平
#define CS2_H   PBout(13)=1   //片选高电平
#define CS2_L   PBout(13)=0   //片选低电平
#define CLK_H  PAout(5)=1  //时钟 高电平
#define CLK_L  PAout(5)=0  //时钟低电平
#define DATA   PAin (6)       //数据位
 
QueueHandle_t xBIANMA;
void delay_us(u8 us)
{
us*=10;
  while(us--);

}

 int SPI1_R()
   {
      int i=0;
      int data=0;
       CLK_L;	       
      CS1_L;            
      delay_us(2); 
 
      for(i=0; i<24; i++)       
      {
        data<<=1;
        data|=DATA;
        CLK_H;
        delay_us(1); 
        CLK_L;
         delay_us(1);  		
      }							
      CS1_H;            
//      return((data1&0xfffff)-146464);	
      return((data&0xffffff)>>6);	
  } 

 int SPI2_R()
   {
      int i=0;
      int data=0;
       CLK_L;	       
      CS2_L;            
      delay_us(2); 
 
      for(i=0; i<24; i++)       
      {
        data<<=1;
        data|=DATA;
        CLK_H;
        delay_us(1); 
        CLK_L;
         delay_us(1);  		
      }							
      CS2_H;            
//      return((data1&0xfffff)-146464);	
      return((data&0xffffff)>>6);	
  } 

 

       
  void  bianmaqi_Init() //模拟口线是一个周期22.65us    不能使用SPI模块，因为ssi        
{ 
    SPI_InitTypeDef  SPI_InitStructure;
  
  GPIO_InitTypeDef  GPIO_InitStructure;	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);

    
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;        //前轮编码器CS PA4   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;        //后轮编码器CS PB13   
	GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;        //前轮编码器CS PA4   
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
CS1_H;
CS2_H;

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Rx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;       //频率84m/2=42m

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(SPI1, ENABLE);

          
}




void SensorTask(void * pv)
{
bianmaqi a;
u8 i=0;
  bianmaqi_Init();
  xBIANMA= xQueueCreate( 2, sizeof( int )*3 );        //小车前后转向角度值（+—128度）



while(1)
{
vTaskDelay(100/portTICK_PERIOD_MS);
a.front=SPI1_R();
a.behind=SPI2_R();
if(i==2)i=0;
xQueueSend(xBIANMA,&a,0);
}


}





