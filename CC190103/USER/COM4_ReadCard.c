
#include "common.h"			
/**************************************************
*串口4接收中断
*函数功能：获取卡号
*
***************************************************/
#define CARD_RECV_BUF  20

OS_EVENT *g_COM4Mbox = 0; //通知已经获取到卡号

static uint8_t RFID_buf[7] = {0xFE,0x03,0x00,0x01,0x01,0x00,0xFD};
static char gCardBuf[CARD_RECV_BUF];   //读卡数据
static volatile char isCard = 0;

void UART4_IRQHandler(void)            
{
	  static unsigned int  RecvCnt = 0;
	  u8 res = 0;
//		OSIntEnter();
    if(USART_GetFlagStatus(UART4, USART_FLAG_ORE ) == SET)
    {
        USART_ClearFlag(UART4, USART_FLAG_ORE); 
    }
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
				USART_ClearITPendingBit(UART4,USART_IT_RXNE);	
			
        /* Read one byte from the receive data register */
				res = USART_ReceiveData(UART4);
				gCardBuf[RecvCnt] = res;
				if(RecvCnt == 0)     
				{
					RecvCnt = (0xFC != res)?0:RecvCnt+1;
				}
				else if(RecvCnt == 1)
				{
					RecvCnt = (0x03 != res)?0:RecvCnt+1;
				}
				else if(RecvCnt > 1) 
				{
					RecvCnt++;
					if(RecvCnt == 7+gCardBuf[2])
					{             
						if(RecvCnt == 11)
						{
							isCard = 1;
							IDERR = 0;      //卡故障设为0
						}
						RecvCnt = 0;
					}
				}
			 
    }
//		OSIntExit();
}
/**************************************************************
* 串口4 发送函数
* data:发送数据
* length:发送的字节长度
**************************************************************/
void UART4_SendData(uint8_t *data,uint8_t length)
{
    while(length--)
		{
			USART_SendData(UART4,*data);
			while(USART_GetFlagStatus(UART4,USART_FLAG_TXE)==RESET);
			data++;
		}
}

void COM4_Task(void *pdata)
{

	uint32_t nID = 0;       //当前读到的卡号
	pdata = pdata;
	g_COM4Mbox	= OSMboxCreate((void *)0);
	 while(1)
   {	
//		 USART4_SendData(&RFID_buf[0],7);//函数功能：请求卡的ID号
//		 if(isCard == 1)
//		 {
//			  isCard = 0;
//		    nID = gCardBuf[9];
//				if(nID !=0 )
//				{
//						nID = (nID<<8)+gCardBuf[8];
//						nID = (nID<<8)+gCardBuf[7];
//						nID = (nID<<8)+gCardBuf[6];
//					  gHalData->Station = nID;
//						//OSMboxPost(g_COM4Mbox,&nID);
//   			}
//		}	
		OSTimeDlyHMSM(0,0,0,30);				 
	}		
}	
