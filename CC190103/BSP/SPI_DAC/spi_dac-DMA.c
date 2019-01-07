#include "spi_dac.h"
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "string.h"

u16* DAC_OUT=NULL;

    /* configure DMA */
    /* DMA clock enable */
void spi_dac_dma(void)
{
    DMA_InitTypeDef DMA_InitStructure;

spi_dac_DMA_RCC_PERIPHERAL(spi_dac_DMA_PERIPHERAL, ENABLE);

    /* DMA1 Channel Config */
    DMA_DeInit(spi_dac_DMA_Stream );

    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&spi_dac->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr  = (uint32_t)DAC_OUT;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = 8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;

    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_Channel = spi_dac_DMA_CHANNEL;

    DMA_Init(spi_dac_DMA_Stream , &DMA_InitStructure);

    SPI_I2S_DMACmd(spi_dac, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(spi_dac_DMA_Stream, ENABLE);

//    DMA_ITConfig(spi_dac_DMA_Stream , DMA_IT_TC, ENABLE);
}



void SPI5_DAC_Config(void) 
{ 
	GPIO_InitTypeDef GPIO_InitStructure; 
	SPI_InitTypeDef  SPI_InitStructure; 

  /* ʹ�� DAC_SPI ��GPIO ʱ�� */
  /*!< SPI_DAC_SPI_CS_GPIO, SPI_DAC_SPI_MOSI_GPIO, 
       SPI_DAC_SPI_MISO_GPIO,SPI_DAC_SPI_SCK_GPIO ʱ��ʹ�� */
  RCC_AHB1PeriphClockCmd (DAC_SPI_SCK_GPIO_CLK | DAC_SPI_MISO_GPIO_CLK|DAC_SPI_MOSI_GPIO_CLK|DAC_SPI_NSS_GPIO_CLK, ENABLE);

  /*!< SPI_DAC_SPI ʱ��ʹ�� */
  DAC_SPI_CLK_INIT(DAC_SPI_CLK, ENABLE);
 
  //�������Ÿ���
  GPIO_PinAFConfig(DAC_SPI_SCK_GPIO_PORT,DAC_SPI_SCK_PINSOURCE,DAC_SPI_SCK_AF); 
  GPIO_PinAFConfig(DAC_SPI_MISO_GPIO_PORT,DAC_SPI_MISO_PINSOURCE,DAC_SPI_MISO_AF); 
  GPIO_PinAFConfig(DAC_SPI_MOSI_GPIO_PORT,DAC_SPI_MOSI_PINSOURCE,DAC_SPI_MOSI_AF); 
  GPIO_PinAFConfig(DAC_SPI_NSS_GPIO_PORT,DAC_SPI_NSS_PINSOURCE,DAC_SPI_NSS_AF); 
  
  /*!< ���� SPI_DAC_SPI ����: SCK */
  GPIO_InitStructure.GPIO_Pin = DAC_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  
  GPIO_Init(DAC_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< ���� SPI_DAC_SPI ����: MISO */
  GPIO_InitStructure.GPIO_Pin = DAC_SPI_MISO_PIN;
  GPIO_Init(DAC_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< ���� SPI_DAC_SPI ����: MOSI */
  GPIO_InitStructure.GPIO_Pin = DAC_SPI_MOSI_PIN;
  GPIO_Init(DAC_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/*!< ���� SPI_DAC_SPI ����: NSS */
  GPIO_InitStructure.GPIO_Pin = DAC_SPI_NSS_PIN;
  GPIO_Init(DAC_SPI_NSS_GPIO_PORT, &GPIO_InitStructure);  

//  /* ֹͣ�ź� FLASH: CS���Ÿߵ�ƽ*/
//  SPI_DAC_CS_HIGH();

	//����SPI5����ģʽ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  //����SPI�������˫�������ģʽ:SPI����Ϊ���߷��� 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	//����SPI����ģʽ:����Ϊ��SPI 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;	//����SPI�����ݴ�С:SPI���ͽ���16λ֡�ṹ 
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո� 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//���ݲ����ڵڶ���ʱ���� 
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;	//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;	//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ 
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ 
	SPI_Init(SPI5, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ��� 
	SPI_SSOutputCmd(SPI5, ENABLE);
	SPI_TIModeCmd(SPI5, ENABLE);
	SPI_Cmd(SPI5, ENABLE); //ʹ��SPI���� 
        spi_dac_dma();
}



void SPI_DAC_SendData(SPI_TypeDef* SPIx, uint16_t Data)
{
	u16 	retry=0; 
//	SPI_DAC_CS_LOW();
	SPI_I2S_SendData(SPIx, Data); 	//ͨ��SPI2����һ������	   
	retry=0;
	while (SPI_I2S_GetFlagStatus(SPI5, SPI_I2S_FLAG_BSY) != RESET) //��ⷢ�ͱ�־λ 
	{
		retry++; 
		if(retry>500)
			break; 
	}
//	SPI_DAC_CS_HIGH();
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
memset(DAC_OUT,0,16);
SPI5_DAC_Config();
SPI_DAC_SendData(SPI5,DAC_128S_MODE_WTM);//д��ģʽ
while(1)
{
vTaskDelay(50/portTICK_PERIOD_MS);//50msˢ��
for(int i = 0; i < 8; ++i)
{
}
DMA_ClearFlag(spi_dac_DMA_Stream,spi_dac_DMA_TC_FLAG);
DMA_SetCurrDataCounter(spi_dac_DMA_Stream,8);  // load number of bytes to be transferred
    DMA_Cmd(spi_dac_DMA_Stream, ENABLE);

}

}