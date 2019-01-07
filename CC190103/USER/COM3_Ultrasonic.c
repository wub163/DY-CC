#include "includes.h"		
#include "common.h"

static volatile unsigned short distance = 0xFFFF;

static volatile unsigned char recvFin = 0;

OS_EVENT *g_COM3Sem = 0;       //֪ͨ�����񣬻����˽��뵽Σ������
OS_EVENT *g_headDisMbox = 0;   //�����ź�
void USART3_IRQHandler(void)              
{
    static unsigned int  RecvCnt = 0;
    uint8_t TmpChar = 0;
	//  static uint16_t uValLen = 0;
//	  OSIntEnter();  
    if(USART_GetFlagStatus(USART3, USART_FLAG_ORE ) == SET)
    {
        USART_ClearFlag(USART3, USART_FLAG_ORE); 
    }
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */
        TmpChar = USART_ReceiveData(USART3);
		    if(RecvCnt == 0)
				{
				    distance = TmpChar;
					  RecvCnt++;
        }
				else if(RecvCnt == 1)
				{
				    distance = distance*256 + TmpChar;
					  RecvCnt = 0;
						recvFin = 1;
					  CSBERR = 0;   //��������ģ������Ϊ�޹���״̬
				}
		}
//		OSIntExit(); 
}
static void GetUltrasonicData(void)
{
	 USART_SendData(USART3,0x55);
	 while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){;}
}
unsigned char tmpc;
void COM3_Task(void *pdata)
{
  unsigned char showDis;
	unsigned char *headDis = 0;
	unsigned char cnt = 0;
	pdata = pdata;
	
	g_COM3Sem = OSSemCreate(0); 
	g_headDisMbox = OSMboxCreate((void*)0); 
	while(1)
	{
		  if(recvFin == 0)
			{
		     GetUltrasonicData();
			}
			else
			{
				recvFin = 0;
				if(distance < OBSTAC_DIS)   //��������Σ������
				{
            OSSemPost(g_COM3Sem);
				}
				else
				{
	          ;
				}
		  }

			*headDis = 0;
			headDis = OSMboxAccept(g_headDisMbox);
			if(*headDis != 0)
			{
				showDis = 0x80;
        SendHeadDis(showDis);
				SendHeadDis(*headDis);
			}
			OSTimeDlyHMSM(0,0,0,40);
	}   
}
