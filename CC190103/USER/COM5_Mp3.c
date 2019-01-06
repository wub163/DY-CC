#include "common.h"

static char mp3Buf[20] = {0x7E,0x04,0x41,0x00,0x01,0xEF};
OS_EVENT *g_COM5Mbox; //接收信号，修改播放的音乐
char recvBuf2[4];

MP3_SET	g_MP3_Set;

void SendMP3Cmd(char *p,int len)
{
	int i = 0;	
	for(;i<len;i++)
	{
			USART_SendData(UART5,*(p+i));
			while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)
			{
			}
	}
}
//播放
void MP3_Play(void)
{
    mp3Buf[0] = 0x7E;
	  mp3Buf[1] = 0x02;
	  mp3Buf[2] = 0x01;
	  mp3Buf[3] = 0xEF;
	  SendMP3Cmd(mp3Buf,4);
}
//暂停
void MP3_Stop(void)
{
    mp3Buf[0] = 0x7E;
	  mp3Buf[1] = 0x02;
	  mp3Buf[2] = 0x02;
	  mp3Buf[3] = 0xEF;
	  SendMP3Cmd(mp3Buf,4);
}
//选择音乐号播放
void MP3_SelectDocMusic(char CMD,char docIndex,char musIndex)
{
    mp3Buf[0] = 0x7E;
	  mp3Buf[1] = 0x04;
	  mp3Buf[2] = 0x44;//CMD;
	  mp3Buf[3] = 0x01;//docIndex;
		mp3Buf[4] = 0x01;//musIndex;
	  mp3Buf[5] = 0xEF;
	  SendMP3Cmd(mp3Buf,6);
}
//设置音量大小
void MP3_SetVlolume(short num)
{
	  mp3Buf[0] = 0x7E;
	  mp3Buf[1] = 0x03;
	  mp3Buf[2] = 0x31;
//	  if(num > 25)
//			num = 25;
//		if(num < 15)
//			num = 15;
    mp3Buf[3] = num&0xFF;
	  mp3Buf[4] = 0xEF;
	  SendMP3Cmd(mp3Buf,5);
}

void UART5_IRQHandler(void)
{
//	OSIntEnter();
	if(USART_GetFlagStatus(UART5,USART_FLAG_ORE ) == SET)
	{
			USART_ClearFlag(UART5,USART_FLAG_ORE); //读SR其实就是清除标志
	}
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
  {
		USART_ClearITPendingBit(UART5,USART_IT_RXNE);	// 清除中断标志位
    //res = USART_ReceiveData(UART5);
		//recvBuf2[0] = 
	}
	if(USART_GetITStatus(UART5, USART_IT_TC) != RESET)
	{// 发送完成中断
		USART_ClearITPendingBit(UART5,USART_IT_TC);	// 清除中断标志位	
	}
//	OSIntExit();  		 	
}


void COM5_Task(void *pdata)
{
	unsigned char err=0;
	MP3_SET	*MP3Task_Set;
	pdata = pdata;
	g_COM5Mbox	= OSMboxCreate((void *)0);
	/**********************设置音量***********************/
	MP3_SetVlolume(40);   //设置MP3音量
 // MP3_SelectDocMusic(0x42,0x01,0x0b);
	OSTimeDlyHMSM(0,0,0,200);
	/*****************************************************/
  while(1)
	{
			MP3Task_Set = (MP3_SET *)OSMboxPend(g_COM5Mbox,0,&err);
			if(err == OS_ERR_NONE)
			{
				switch((char)MP3Task_Set->Type)
				{
					case MP3_SETVOL:
						MP3_SetVlolume(g_MP3_Set.para);
						break;
					case MP3_SELMUSIC:
					case MPC_SELDOC_SETMUS:
					 MP3_SelectDocMusic(g_MP3_Set.Type,g_MP3_Set.Folder_Num,g_MP3_Set.para);
					 OSTimeDlyHMSM(0,0,0,20);
					 break;
					default:break;
				 }
			
			}
		  OSTimeDlyHMSM(0,0,0,200);
   }//while(1)
}

