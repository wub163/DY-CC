#include "spi_dac.h"
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
u16 *DAC_OUT=NULL;
void SPI5_DAC_Config(void) 
{ 
	GPIO_InitTypeDef GPIO_InitStructure; 
	SPI_InitTypeDef  SPI_InitStructure; 

  /* 使能 DAC_SPI 及GPIO 时钟 */
  /*!< SPI_DAC_SPI_CS_GPIO, SPI_DAC_SPI_MOSI_GPIO, 
       SPI_DAC_SPI_MISO_GPIO,SPI_DAC_SPI_SCK_GPIO 时钟使能 */
  RCC_AHB1PeriphClockCmd (DAC_SPI_SCK_GPIO_CLK | DAC_SPI_MISO_GPIO_CLK|DAC_SPI_MOSI_GPIO_CLK|DAC_SPI_NSS_GPIO_CLK, ENABLE);

  /*!< SPI_DAC_SPI 时钟使能 */
  DAC_SPI_CLK_INIT(DAC_SPI_CLK, ENABLE);
 
  //设置引脚复用
  GPIO_PinAFConfig(DAC_SPI_SCK_GPIO_PORT,DAC_SPI_SCK_PINSOURCE,DAC_SPI_SCK_AF); 
	GPIO_PinAFConfig(DAC_SPI_MISO_GPIO_PORT,DAC_SPI_MISO_PINSOURCE,DAC_SPI_MISO_AF); 
	GPIO_PinAFConfig(DAC_SPI_MOSI_GPIO_PORT,DAC_SPI_MOSI_PINSOURCE,DAC_SPI_MOSI_AF); 
//  GPIO_PinAFConfig(DAC_SPI_NSS_GPIO_PORT,DAC_SPI_NSS_PINSOURCE,DAC_SPI_NSS_AF); 
  
  /*!< 配置 SPI_DAC_SPI 引脚: SCK */
  GPIO_InitStructure.GPIO_Pin = DAC_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  
  GPIO_Init(DAC_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< 配置 SPI_DAC_SPI 引脚: MISO */
  GPIO_InitStructure.GPIO_Pin = DAC_SPI_MISO_PIN;
  GPIO_Init(DAC_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< 配置 SPI_DAC_SPI 引脚: MOSI */
  GPIO_InitStructure.GPIO_Pin = DAC_SPI_MOSI_PIN;
  GPIO_Init(DAC_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/*!< 配置 SPI_DAC_SPI 引脚: NSS */
  GPIO_InitStructure.GPIO_Pin = DAC_SPI_NSS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(DAC_SPI_NSS_GPIO_PORT, &GPIO_InitStructure);  

//  /* 停止信号 FLASH: CS引脚高电平*/
//  SPI_DAC_CS_HIGH();

	//配置SPI5引脚模式 
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  //设置SPI单向或者双向的数据模式:SPI设置为单线发送 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	//设置SPI工作模式:设置为主SPI 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;	//设置SPI的数据大小:SPI发送接收16位帧结构 
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	//选择了串行时钟的稳态:时钟悬空高 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//数据捕获于第二个时钟沿 
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;	//定义波特率预分频的值:波特率预分频值为256 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始 
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式 
	SPI_Init(SPI5, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器 
//////	SPI5->CR2|=(1<<4); 	//TI MODE
//	SPI5->CR2|=(1<<2); 	//SSOE
//	SPI5->CR1|=(1<<8); 	//SSI
//	SPI_SSOutputCmd(SPI5, ENABLE);
//	SPI_TIModeCmd(SPI5, ENABLE);
	SPI_Cmd(SPI5, ENABLE); //使能SPI外设 
}


void SPI_DAC_SendData(SPI_TypeDef* SPIx, uint16_t Data)
{
	u16 	retry=0; 
	SPI_DAC_CS_LOW();
	SPI_I2S_SendData(SPIx, Data); 	//通过SPI2发送一个数据	   
	retry=0;
	while (SPI_I2S_GetFlagStatus(SPI5, SPI_I2S_FLAG_BSY) != RESET) //检测发送标志位 
	{
		retry++; 
		if(retry>500)
			break; 
	}
	SPI_DAC_CS_HIGH();
}

u8 DAC_SPI_SendData(u8 Ctrl,u16 Voltage) 
{
	u16 Voltagereg;
	Voltagereg=Ctrl;
	Voltagereg<<=12;
	Voltagereg|=(Voltage&0x0fff);


SPI_DAC_SendData(SPI5,Voltagereg);


	return 0;
}


u8 DAC_Channel(u8 Channel,u16 Voltage) 
{
	DAC_SPI_SendData( Channel,Voltage);
	return 0;
}


void DAC8 (void* pv)
{
  DAC_OUT=pvPortMalloc(2*8);
  memset(DAC_OUT,0,2*8);
SPI5_DAC_Config();
SPI_DAC_SendData(SPI5,DAC_128S_MODE_WTM);//写穿模式
while(1)
{
vTaskDelay(50/portTICK_PERIOD_MS);//50ms刷新
for(int i = 0; i < 8; ++i)
{
  DAC_Channel(i,DAC_OUT[i]&0X0FFF);
}
}

}