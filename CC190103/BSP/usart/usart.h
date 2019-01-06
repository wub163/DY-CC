#ifndef __USART_H
#define __USART_H 
#include <stdio.h>
#include "sys.h"
#include "delay.h" 
extern void USART1_SendData(uint8_t *data,uint8_t length);
extern void USART2_SendData(uint8_t *data,uint8_t length);
extern void USART3_SendData(uint8_t *data,uint8_t length);
extern void USART4_SendData(uint8_t *data,uint8_t length);
extern void USART6_SendData(uint8_t *data,uint8_t length);
extern void InitCOM(void);
#endif	   
















