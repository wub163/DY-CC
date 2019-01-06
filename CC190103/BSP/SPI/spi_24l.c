/**
  ******************************************************************************
  * @file    SPI/SPI_FLASH/spi_24L.c
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   This file provides a set of functions needed to manage the SPI M25Pxxx
  *          FLASH memory. 
  *            
  *            
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+---------------+-------------+
  *          |  STM32 SPI Pins             |     s24L      |    Pin      |
  *          +-----------------------------+---------------+-------------+
  *          |                             |   GND         |    1 (0 V)  |
  *          |                             |   VCC         |    2 (3.3 V)|
  *          | PG6                         |   NRF_MOD     |    3        |PC5
  *          | PG7                         |   NRF_CS      |    4        |PF10
  *          | PB13  SPI2          / SCK   |   Clock(C)    |    5        |PB13
  *          | PC3   SPI2          / MOSI  |   DataIn(D)   |    6        |PB15
  *          | PC2   SPI2          / MISO  |   DataOut(D)  |    7        |PB14
  *          | PG8                         |   NRF_Irq     |    8        |PG15
  *          +-----------------------------+---------------+-------------+  
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "spi_24l.h"

/* High layer functions  */
void s24L_DeInit(void);
void s24L_Init(void);
void s24L_WriteBuffer(u8 cmd,u8* pBuffer,  u8 NumByteToWrite);
void s24L_ReadBuffer(u8 cmd,u8* pBuffer,  u8 NumByteToWrite);

/* Low layer functions */
uint8_t s24L_SendByte(uint8_t byte);


void s24L_LowLevel_DeInit(void);
void s24L_LowLevel_Init(void); 
u8 addr[6];
u8 s24L_Send[32],s24L_Receive[32];
u8 resend=0;
u8 buf[5]={0X58,0X58,0X58,0X58,0X58};
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  DeInitializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void s24L_DeInit(void)
{
  s24L_LowLevel_DeInit();
}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void s24L_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  s24L_LowLevel_Init();
 s24L_CS=0;
  s24L_MOD=0;
    s24L_CS=1;
  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;

  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;       //频率42m/8=6m

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(s24L_SPI, &SPI_InitStructure);

  /*!< Enable the s24L_SPI  */
  SPI_Cmd(s24L_SPI, ENABLE);
  

}

/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle 
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH, must be equal
  *         or less than "s24L_PAGESIZE" value.
  * @retval None
  */
void s24L_WriteBuffer(u8 cmd,u8* pBuffer,  u8 NumByteToWrite)
{

  /*!< Select the FLASH: Chip Select low */
  s24L_CS=0;
  /*!< Send "Write to Memory " instruction */


  s24L_SendByte(cmd);

  /*!< while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    s24L_SendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  s24L_CS=1;

 }


/**
  * @brief  Reads a block of data from the FLASH.
  * @param  pBuffer: pointer to the buffer that receives the data read from the FLASH.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the FLASH.
  * @retval None
  */
void s24L_ReadBuffer(u8 cmd,u8* pBuffer,  u8 NumByteToRead)
{
  /*!< Select the FLASH: Chip Select low */
  s24L_CS=0;

  /*!< Send "Read from Memory " instruction */
  s24L_SendByte(cmd);

  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = s24L_SendByte( L01_NOP);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  s24L_CS=1;
}



/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t s24L_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(s24L_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(s24L_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(s24L_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(s24L_SPI);
}



/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
//====================m3主板底层
//void s24L_LowLevel_Init(void)                                                         //m3/m4
//{
//  GPIO_InitTypeDef GPIO_InitStructure;
//  NVIC_InitTypeDef NVIC_InitStructure;
//  EXTI_InitTypeDef intt;
//
//  /*!< Enable the SPI clock */
//  s24L_SPI_CLK_INIT(s24L_SPI_CLK, ENABLE);
//
//  /*!< Enable GPIO clocks */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			   //复用功能使能
//  RCC_APB2PeriphClockCmd(s24L_SPI_SCK_GPIO_CLK | s24L_SPI_MISO_GPIO_CLK | 
//                         s24L_SPI_MOSI_GPIO_CLK | s24L_CS_GPIO_CLK|
//                           s24L_MOD_GPIO_CLK| s24L_INT_GPIO_CLK, ENABLE);
//  
//  /*!< SPI pins configuration *************************************************/
//
//  /*!< Connect SPI pins to AF5 */  
//
//
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//        
//  /*!< SPI SCK pin configuration */
//  GPIO_InitStructure.GPIO_Pin = s24L_SPI_SCK_PIN;
//  GPIO_Init(s24L_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
//
//  /*!< SPI MOSI pin configuration */
//  GPIO_InitStructure.GPIO_Pin =  s24L_SPI_MOSI_PIN;
//  GPIO_Init(s24L_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
//
//  /*!< SPI MISO pin configuration */
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_InitStructure.GPIO_Pin =  s24L_SPI_MISO_PIN;
//  GPIO_Init(s24L_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
//
//  /*!< Configure s24L Card CS pin in output pushpull mode ********************/
//  GPIO_InitStructure.GPIO_Pin = s24L_CS_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(s24L_CS_GPIO_PORT, &GPIO_InitStructure);
//
//  /*!< Configure s24L Card MOD pin in output pushpull mode ********************/
//  GPIO_InitStructure.GPIO_Pin = s24L_MOD_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(s24L_MOD_GPIO_PORT, &GPIO_InitStructure);
//
//  /*!< Configure s24L Card INT pin in output pushpull mode ********************/
//  GPIO_InitStructure.GPIO_Pin = s24L_INT_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(s24L_INT_GPIO_PORT, &GPIO_InitStructure);
//    
//  /*外部中断*/
//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource15);
//   intt.EXTI_Line=EXTI_Line15;
//    intt.EXTI_LineCmd=ENABLE;
//    intt.EXTI_Mode= EXTI_Mode_Interrupt;
//    intt.EXTI_Trigger=EXTI_Trigger_Falling;
//  EXTI_Init(&intt);
//
// /* Enable and set EXTI Line0 Interrupt to the lowest priority */
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//
//}


//====================m4主板底层
void s24L_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef intt;

  /*!< Enable the SPI clock */
  s24L_SPI_CLK_INIT(s24L_SPI_CLK, ENABLE);

  /*!< Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(s24L_SPI_SCK_GPIO_CLK | s24L_SPI_MISO_GPIO_CLK | 
                         s24L_SPI_MOSI_GPIO_CLK | s24L_CS_GPIO_CLK|
                           s24L_MOD_GPIO_CLK| s24L_INT_GPIO_CLK, ENABLE);
  
  /*!< SPI pins configuration *************************************************/

  GPIO_PinAFConfig(s24L_SPI_SCK_GPIO_PORT, s24L_SPI_SCK_SOURCE, s24L_SPI_SCK_AF);
  GPIO_PinAFConfig(s24L_SPI_MISO_GPIO_PORT, s24L_SPI_MISO_SOURCE, s24L_SPI_MISO_AF);
  GPIO_PinAFConfig(s24L_SPI_MOSI_GPIO_PORT, s24L_SPI_MOSI_SOURCE, s24L_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        
  /*!< SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = s24L_SPI_SCK_PIN;
  GPIO_Init(s24L_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  s24L_SPI_MOSI_PIN;
  GPIO_Init(s24L_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  s24L_SPI_MISO_PIN;
  GPIO_Init(s24L_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure s24L Card CS pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = s24L_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(s24L_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure s24L Card MOD pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = s24L_MOD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(s24L_MOD_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure s24L Card INT pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = s24L_INT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(s24L_INT_GPIO_PORT, &GPIO_InitStructure);

    
  /*外部中断*/
   intt.EXTI_Line=EXTI_Line8;
    intt.EXTI_LineCmd=ENABLE;
    intt.EXTI_Mode= EXTI_Mode_Interrupt;
    intt.EXTI_Trigger=EXTI_Trigger_Falling;
  EXTI_Init(&intt);
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG,EXTI_PinSource8);
Ex_NVIC_Config(EXTI_PortSourceGPIOG,EXTI_PinSource8,1);

 /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}


//==中断服务程序
void EXTI9_5_IRQHandler(void)
{
  u8 a,da;
LED0=0;
  if(!PGin(8)){                          //无线模块中断。
      s24L_MOD=0; 
  a=s24L_status();
  
     switch (a&0x70)
{
case  0x10:     //重发无应答。
s24L_CLRTX();   //无应答，必须清除发送。
  break;

case  0x20:     //发送完成
    s24L_ReadBuffer(R_REG+L01REG_RF_CH,&a,1);
    if((a==2)&&(addr[0]<0x80))s25L_SETCH(addr[5]);   //如果配置通道时，发送通道参数状态，则立刻恢复频道

      switch(resend){                                               //判断是否有待发送包
        case  4:s24L_DASEND(addr,buf, 4,0);break;
        case  5:s24L_DASEND(buf,addr, 5,0);break;
        case 32:s24L_DASEND(addr,pccmd,32,1);break;
        case 0:   
          da=0x0b;                                            //模块上电发送发送完成转接收。
          s24L_WriteBuffer(W_REG+L01REG_CONFIG,&da,1);}

     break;

case  0x40:     //接收中断。
  s24L_ReadBuffer(R_RX_PL_WID,&da,1);
  s24L_ReadBuffer(R_RX,s24L_Receive,da);
// s24L_CLRRX();
  switch((a>>1)&0x07)
  {
  case 0:       //0通道
//    s24L_ReadBuffer(R_REG+L01REG_RF_CH,&a,1);
//    if(a==2){   //只有在配置通道时，才可更改。
//    if(((s24L_Receive[0])>0x80)&&(gInnerData->ID==0xff))
//      gInnerData->ID=(s24L_Receive[0]-0x80);
//   else {       //通道配置
//     s25L_SETCH(s24L_Receive[0]);
//      s24L_Receive[0]=gInnerData->ID;
//      s24L_set_CH(1,s24L_Receive,1,32,1);
//      s24L_Receive[0]=0xff;
//      s24L_set_CH(2,s24L_Receive,1,32,0);
//        }}
       break;

  case 1:       //1通道
      memcpy(cmd,s24L_Receive,32);cmd[32]=0x58;             
    break;
    
  case 2:       //2通道

    
    for(int i = 0; i < 5; ++i)
    {
      
    if(stat[i][32]==0x58)continue;
    memcpy(stat[i],s24L_Receive,32);stat[i][32]=0x58;break;             
    }
    break;
  }
break;


}
s24L_CLRINT();
       s24L_MOD=1; 
   }
EXTI_ClearITPendingBit(EXTI_Line8);
LED0=1;
}


/**
  * @brief  DeInitializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void s24L_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Disable the s24L_SPI  ************************************************/
  SPI_Cmd(s24L_SPI, DISABLE);
  
  /*!< DeInitializes the s24L_SPI *******************************************/
  SPI_I2S_DeInit(s24L_SPI);
  
  /*!< s24L_SPI Periph clock disable ****************************************/
  s24L_SPI_CLK_INIT(s24L_SPI_CLK, DISABLE);
      
  /*!< Configure all pins used by the SPI as input floating *******************/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

  GPIO_InitStructure.GPIO_Pin = s24L_SPI_SCK_PIN;
  GPIO_Init(s24L_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = s24L_SPI_MISO_PIN;
  GPIO_Init(s24L_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = s24L_SPI_MOSI_PIN;
  GPIO_Init(s24L_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = s24L_CS_PIN;
  GPIO_Init(s24L_CS_GPIO_PORT, &GPIO_InitStructure);
}

/*=============
函数名称：u8 s24L_OK(void)
函数返回：1：存在，0：不存在。
函数功能：模块存在检测
===============*/
u8 s24L_OK(void)
{
u8 da;
da=s24L_status();
if((da>0x0)&&(da<0x80))return 1;return 0;
}
/*=============
函数名称：u8 s24L_ready(void)
函数返回：1：存在，0：不存在。
函数功能：模块存在检测
===============*/
u8 s24L_ready(void)
{
  u8 da;   
  while(!s24L_OK());
s24L_MOD=0;                                             //待机模式
da=0xfa;                                            //模块自动重发配置 ,4ms重发10次
s24L_WriteBuffer(W_REG+L01REG_RETR,&da,1);
//da=0x73;                                            //启动隐藏命令。
//s24L_WriteBuffer(ACTIVATE,&da,1);
da=0x0b;                                            //模块上电 ，接收
s24L_WriteBuffer(W_REG+L01REG_CONFIG,&da,1);
s25L_SETCH(2);                                          //设定配置通道频率
//模块接收：通道，地址，启闭，长度，应答。

s24L_set_CH(0,buf,1,5,0);                            //广播通道。接收ID和通道信息。
buf[0]=0;
s24L_set_CH(1,buf,0,32,0);
s24L_set_CH(2,buf,0,32,0);
s24L_set_CH(3,buf,0,5,0);
s24L_set_CH(4,buf,0,5,0);
s24L_set_CH(5,buf,0,5,0);
s24L_CLRTX();
s24L_CLRRX();
s24L_CLRINT();
s24L_MOD=1;                                             //初始化时模式脚必须为低。
return da;

}

/*=============
函数名称：u8 s25L_SETCH(u8 ch)
函数入口：ch 0-127 频段编号
函数返回：1：成功，0：不成功。
函数功能：模块频段更换
===============*/
u8 s25L_SETCH(u8 ch)
{
u8 da,buf[1];
//       s24L_CLRRX();       s24L_CLRTX();


s24L_CS=0;
do{s24L_SendByte(R_REG+L01REG_FIFO_STATUS);//读取缓冲区状态。
da=s24L_SendByte(0XFF);}
while(!(da&0x10));
s24L_CS=1;

s24L_MOD=0;                                             //待机模式
ch&=0x7f;
s24L_WriteBuffer(W_REG+L01REG_RF_CH,&ch,1);
s24L_ReadBuffer(R_REG+L01REG_RF_CH,buf,1);
s24L_MOD=1; 
if (buf[0]==ch)return 1;return 0;
                                            //待机模式
}

/*=============
函数名称：u8 s24L_SETADDR(u8* da ,u8 addr)
函数返回：1：成功，0：失败。
函数功能：模块接收地址设定，&da地址，addr通道号。
===============*/
u8 s24L_SETADDR(u8* da ,u8 addr)
{
  u8 len,dda[6];
  addr%=6;
  if(addr<2)len=5;else len=1;
  s24L_WriteBuffer(W_REG+L01REG_RX_ADDR_P0+addr,da,len);
s24L_ReadBuffer(R_REG+L01REG_RX_ADDR_P0+addr,dda,len);

if(len==1){if (da[0]==dda[0])return 1;return 0;}
if ((da[0]==dda[0])&&(da[1]==dda[1])&&(da[2]==dda[2])&&(da[3]==dda[3])&&(da[4]==dda[4]))
return 1;return 0;

}

/*=============
函数名称：u8 s24L_CLRTX(void)
函数返回：模块状态
函数功能：模块发送数据清空
===============*/
u8 s24L_CLRTX(void)
{
 u8 da;
 s24L_CS=0;
 da=s24L_SendByte(CLR_TX);
s24L_CS=1;
return da;
}
/*=============
函数名称：u8 s24L_CLRRX(void)
函数返回：模块状态
函数功能：模块接收数据清空
===============*/
u8 s24L_CLRRX(void)
{
 u8 da;
 s24L_CS=0;
 da=s24L_SendByte(CLR_RX);
s24L_CS=1;
return da;
}

/*=============
函数名称：u8 s24L_status(void)
函数返回：模块状态
函数功能：模块状态获取。
===============*/
u8 s24L_status(void)
{
 u8 da;
 s24L_CS=0;
 da=s24L_SendByte(L01_NOP);
s24L_CS=1;
return da;
}

/*=============
函数名称：u8 s24L_CLRINT(void)
函数返回：模块状态
函数功能：模块状态获取。
===============*/
u8 s24L_CLRINT(void)
{
s24L_CS=0;
s24L_SendByte(W_REG+L01REG_STATUS);
s24L_SendByte(0x70);
s24L_CS=1;
return s24L_status();
}

/*=============
函数名称：u8 s24L_CNTBUG(void)
函数返回：模块丢失包统计。
函数功能：模块包丢失统计(0--15)。
===============*/
u8 s24L_CNTBUG(void)
{
u8 da;
  s24L_CS=0;
s24L_SendByte(R_REG+L01REG_OBSERVE_TX);
da=s24L_SendByte(0XFF);
s24L_CS=1;
return (da>>4);
}
/*=============
函数名称：u8 s24L_CDTEST(void)
函数返回：模块无线载波1，有  0，无。
函数功能：模块无线载波
===============*/
u8 s24L_CDTEST(void)
{
u8 da;
  s24L_CS=0;
s24L_SendByte(R_REG+L01REG_RPD);
da=s24L_SendByte(0XFF);
s24L_CS=1;
return da;
}
/*=============
函数名称：u8 s24L_DASEND(u8* addr,u8* data, u8 len, u8 wack)
函数入口：addr：地址 data：数据 len：长度 wack：是否应答
函数返回：模块应答发送1，成功  0，失败。
函数功能：模块带应答请求发送
===============*/
u8 s24L_DASEND(u8* addr,u8* data, u8 len, u8 wack)                                      //m3/m4
{
u8 da;
  s24L_CS=0;
  s24L_SendByte(R_REG+L01REG_FIFO_STATUS);//读取缓冲区状态。
da=s24L_SendByte(0XFF);
s24L_CS=1;
if((da&0x10)&&(da!=0xff)){
s24L_MOD=0;                             //待机模式
resend=0;
da=0x0a;                                            //模块上电 ，发射
s24L_WriteBuffer(W_REG+L01REG_CONFIG,&da,1);
  s24L_CLRTX();
  if(wack){s24L_ACK_CH(0,1); s24L_WriteBuffer(W_REG+L01REG_RX_ADDR_P0, addr ,5);}
          
          else s24L_ACK_CH(0,0);
          
              s24L_WriteBuffer(W_TX, data ,len);
            s24L_WriteBuffer(W_REG+L01REG_TX_ADDR, addr ,5);
   
         s24L_MOD=1;
//          delay_us(10);   //实测M4板大于2就可以了
                     
          da=170;while (da--); //m3 168主频主板，延时大概12us（要求不低于10us）
          s24L_MOD=0;
          return 1;
          }resend =len;
 return 0;}



/*=============
函数名称：u8 s24L_ACK_CH(u8 CH, u8 CMD)
函数入口：CH：通道号（0-5） CMD：1应答0无应答
函数返回：模块完成状态。
函数功能：模块应答通道开启与关闭
===============*/
u8 s24L_ACK_CH(u8 CH, u8 CMD)
{
u8 da;
  s24L_CS=0;
s24L_SendByte(R_REG+L01REG_EN_AA);//读取缓冲区状态。
da=s24L_SendByte(0XFF);
s24L_CS=1;
if(CMD)da|=(1<<CH);else da&=~(1<<CH);
              s24L_WriteBuffer(W_REG+L01REG_EN_AA,&da,1);
              s24L_WriteBuffer(R_REG+L01REG_EN_AA,&da,1);

          return da;
 
}
/*=============
函数名称：u8 s24L_ON_CH(u8 CH, u8 CMD)
函数入口：CH：通道号（0-5） CMD：1开启0关闭
函数返回：模块完成状态。
函数功能：模块应答通道开启与关闭
===============*/
u8 s24L_ON_CH(u8 CH, u8 CMD)
{
u8 da;
  s24L_CS=0;
s24L_SendByte(R_REG+L01REG_EN_RXADDR);//读取缓冲区状态。
da=s24L_SendByte(0XFF);
s24L_CS=1;
if(CMD)da|=(1<<CH);else da&=~(1<<CH);
              s24L_WriteBuffer(W_REG+L01REG_EN_RXADDR,&da,1);
              s24L_WriteBuffer(R_REG+L01REG_EN_RXADDR,&da,1);

          return da;
 
}
/*=============
函数名称：u8 s24L_set_CH(u8 CH, u8* addr,u8 onoff,u8 len,u8 ACK)
函数入口：CH：通道号（0-5） addr:接收地址 onoff：是否开启 len：接收长度 cmd2：1应答0无应答
函数返回：模块完成状态。通道开启情况。
函数功能：模块通道设置
===============*/
u8 s24L_set_CH(u8 CH, u8* addr,u8 onoff,u8 len,u8 ACK)
{
  u8 aa;
  s24L_MOD=0; 
  s24L_SETADDR(addr,CH);                                  //设定通道地址        
  s24L_ACK_CH(CH,ACK);                                    //设定应答
  s24L_WriteBuffer(W_REG+L01REG_RX_PW_P0+CH,&len,1);      //设定接收长度
  aa=s24L_ON_CH(CH,onoff);                            //设定通道启闭，返回启闭状态
 s24L_MOD=1; 
return aa;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
