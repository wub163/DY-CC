#include "common.h"
void SendHeadDis(uint8_t ch)  
{
	 USART_SendData(USART3,ch);
	 while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){;}
}