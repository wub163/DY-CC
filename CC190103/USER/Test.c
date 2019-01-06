#include "common.h"
//该文件用于测试单一的功能
#ifdef SYS_TEST
#define set485 GPIO_SetBits(GPIOA,  GPIO_Pin_4)
#define res485 GPIO_ResetBits(GPIOA,  GPIO_Pin_4)


unsigned char tmpBuf[20] = "hello";
unsigned char tmpBuf2[8] = {0};
unsigned char  EepromTest()
{
	unsigned char nRet = 0;
  int i = 0;
	
	OSTaskSuspend(MAIN_TASK_PRIO);
	OSTaskSuspend(COM1_TASK_PRIO);
	OSTaskSuspend(COM2_TASK_PRIO);
	OSTaskSuspend(COM3_TASK_PRIO);
	OSTaskSuspend(COM4_TASK_PRIO);
	OSTaskSuspend(COM5_TASK_PRIO);
	OSTaskSuspend(COM6_TASK_PRIO);

	delay_ms(200); 
						
	//保存
	AT24CXX_Write(0,&tmpBuf[0],1);
	delay_ms(50);
  //读取
	
	//查看是否保存成功

	AT24CXX_Read(0,&tmpBuf2[0],1);
	
	for(i = 0;i<6;i++)
	{
	   if(tmpBuf[i] != tmpBuf2[i])
		 { 
		   nRet = 1;
			 break;
		 }
			 
	}

	OSTaskResume(MAIN_TASK_PRIO);
	OSTaskResume(COM1_TASK_PRIO);
	OSTaskResume(COM2_TASK_PRIO);
	OSTaskResume(COM3_TASK_PRIO);
	OSTaskResume(COM4_TASK_PRIO);
	OSTaskResume(COM5_TASK_PRIO);
	OSTaskResume(COM6_TASK_PRIO);
	return nRet;   
}
unsigned  short CRC16(unsigned char *PSrc,unsigned char Count)     //CRC16????
{
	unsigned char i,Temp;
	unsigned short nCRC=0xFFFF;
	for(i=0;i<Count;i++)
	{
		nCRC=nCRC^*(PSrc++);
		for(Temp=0;Temp<8;Temp++)
		{
			if((nCRC&0x01)==1)
			{
				nCRC=(nCRC>>1)^0xA001;
			}
			else
			{
				nCRC=nCRC>>1;
			}
		}
	}
    return nCRC;
}

void RunWheel(unsigned char wheelID,unsigned short vel)
{
	 unsigned short nCRC = 0;
	 set485;
	
   tmpBuf[0] = wheelID;
	 tmpBuf[1] = 0x06;
	 tmpBuf[2] = 0;
	 tmpBuf[3] = 0x43;
	 tmpBuf[4] = vel>>8;
	 tmpBuf[5] = vel;//0xA0;//
	 nCRC = CRC16(&tmpBuf[0],6);
	 tmpBuf[6] = (unsigned char)nCRC;
	 tmpBuf[7] = nCRC>>8;
	 USART6_SendData(&tmpBuf[0],8);
   res485;
	 

}


void Test_Task(void *pdata)
{
	unsigned char flag = 0;
	unsigned char testCnt = 0;
	int i = 0;
	unsigned char testObj = 1;
	unsigned char showDis = 0;
	unsigned int cnt = 0;
	unsigned int cnt2 = 0;
	unsigned char headDis = 0;
	MP3_SET	mp3Set;
  pdata = pdata;
	LED_BLUE_OFF;
	LED_RED_OFF;
	LED_GREEN_OFF;
	LED_YELLOW_OFF;
	 while(1)
   {	
		 cnt++;
    //测试代码逻辑
		 if(testCnt == 0)
		 {
		     testCnt = 1;
		 }
		 //(1)电源指示灯
		 if(cnt%20 == 0)
		 {
				((flag = flag^1) == 0)?LED_BLUE_ON:LED_BLUE_OFF;
		 }
		 //(2)磁条传感器测试
     //(3)	
    //RunWheel(2,0x0fA0);
			
			//(3)mp3测试
			if(testObj == 1)
			{
				  testObj  = 2;
					mp3Set.Folder_Num = 1;
					mp3Set.para = 2;
					mp3Set.Type = MPC_SELDOC_SETMUS;
					OSMboxPost(g_COM5Mbox,&mp3Set);
				  
			}
			else if(testObj  == 2)
			{
			    testObj  =  3;
					
			}
      //触摸开关测试
			if(GPIO_ReadInputDataBit( GPIOD, GPIO_Pin_6) == 0  || GPIO_ReadInputDataBit( GPIOD, GPIO_Pin_7) == 0)
			{
			     LED_GREEN_ON; 
			}
			else
			{
			      LED_GREEN_OFF;
			}
			//超声波测试
			 if(cnt%10 == 0)
			 {
				if(OSSemAccept(g_COM3Sem) > 0)
				{
						LED_YELLOW_ON;
				}
				else
				{
					 LED_YELLOW_OFF;
				}
		  }
			 //面部表情测试
			 if(cnt%100 == 0)
			 {
				  switch(cnt2++%3)
					{
						case 0:
							showDis = 12;
			        OSMboxPost(g_headDisMbox,&showDis);
							break;
						case 1:
								showDis = 13;
			        OSMboxPost(g_headDisMbox,&showDis);
						SendHeadDis(showDis);
							break;
						case 2:
								showDis = 14;
			        OSMboxPost(g_headDisMbox,&showDis);
			
							break;
						default:
							break;
					}
			 
			 }

		  OSTimeDlyHMSM(0,0,0,10);
		  //RunWheel(3,0x0fA0);      //因为测试时轮子转动，这里注释掉
 		//OSTimeDlyHMSM(0,0,0,200);				 
	}		

}


#endif