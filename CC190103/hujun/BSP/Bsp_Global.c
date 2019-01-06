/***********************************************************************
文件名称：
功    能：
编写时间：
编 写 人：
注    意：
***********************************************************************/
#include "main.h"

/*
 * 功能：
 *       板级初始化
 * 参数：
 *       无
 * 返回：
 *       无
 * 说明：
 *       可用与模块中初始化的尽量使用模块初始化
 */
void BSP_Init(void)
{
 WriteEncrypt();
	NVIC_Configuration();
	USART_485_Configuration();	
	USART_232_Configuration();	
	RS232_DMA_Init();
	RS485_DMA_Init();
	CAN1_Mode_Init(1000);
	Motor_Init();
}

