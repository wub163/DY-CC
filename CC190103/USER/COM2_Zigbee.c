
#include "common.h"

u8 g_RxUpBuf[3][50] = {0};  //ͨ�Ž�����
u8 ddd[33] = {0};
u8 testt[260] = {0};
static u8 tmpBuf[40] = {0};

volatile vu8 AGVERR = 1; //�������������ϱ�־
volatile vu8 IDERR = 1;  //���������ϱ�־
volatile vu8 CSBERR = 1; //

void USART2_IRQHandler(void)
{static u8 in;
	u8 data;
//	OSIntEnter();
 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		data=USART_ReceiveData(USART2);
		        ddd[in] = data;
		        in++;
		    }
		
	
	else
	{
	    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
			{
                           data = USART_ReceiveData(USART2);
                           if((in>=32)&&(pccmd[32]!=0x58)){
                                memcpy(pccmd,ddd,32);pccmd[32]=0x58;}
                          in=0;
                       }
	}
//	OSIntExit();
}
void mkjc(u8 da)
{
	tmpBuf[0] = 0xFC;
	tmpBuf[1] = 0x00;
	tmpBuf[2] = 0x99;
	tmpBuf[3] = da;
  USART2_SendData(tmpBuf,4);
}
//CSMע�ͣ�δʹ��
void zigbee(void)
{
	while(1)
	{
		gInnerData->mkjs=1;
		mkjc(0x11);                    //��ѯ�Զ����ַ
		while(gInnerData->mkjs);
		if(g_RxUpBuf[0][1]==7)
		{
			gInnerData->mkjs=2;
				mkjc(0x16);                //��ѯ�Զ����ַ
			while(gInnerData->mkjs);
			gInnerData->ID=g_RxUpBuf[0][1];
			return;
		}
		//mtime=0; //why...
		//while(mtime<200);
  }
}
void Status_UpLoad(void)        
{
	int i = 0;
	char crc = 0;
	static u8 num = 0;
	static char DZ = 0;
	num++;
	CSBERR++;
	//(1)�������������ϼ��
	if (AGVERR == 1)
  {
	  gUpData->FbErr   |=0X40; 
		
	}
	else if(AGVERR == 0)
	{
		gUpData->FbErr   &=~0x40; 
	}
	AGVERR = 1;//���ô�������������
  //(2)���������ϼ��
	if (IDERR == 1) 
  {
	  gUpData->FbErr   |=0X20;
	}
	else 	if (IDERR == 0) 
	{
		gUpData->FbErr   &=~0x20;
	}
	IDERR = 1;//����Ϊ�����й���
	
	//(3)������ģ����ϼ��,�����Χ���ܵ���200ms
	if (CSBERR == 1)
  {
	  gUpData->FbErr   |=0X80;
	}
	else
	{		
		gUpData->FbErr   &=~0x80;
	}
	CSBERR = 1;
	

	gUpData->FbErr &=0xF0;
	gUpData->FbErr |=((moto_data[52]&=0x03)|((moto_data[112]&=0x03)<<2));
	if (moto_data[0]==0xff)
	{
		gUpData->FbErr |= 0x0F;
	}
	if (moto_data[60]==0xff)
	{
		gUpData->FbErr |= 0xF0;
	}

	if((num % 20 == 0)||((DZ==0)&&(gUpData->DZ!=0))) 
	{
	gUpData->Fbposx  =(s32)gComData->FbPosx ;
	gUpData->Fbposy  =(s32)gComData->FbPosy ;

	gUpData->Fbposth =gComData->FbPosthe;
	gUpData->ID      =gHalData->Station;   

	gUpData->FbVx    =gComData->CmdVx/10;		//?????????x??;
	gUpData->FbVthe  =gComData->CmdVthe;		//?????????theta??;
	gUpData->FbStatus=(gComData->State<<4)|(gInnerData->State&0x0f);                  //????????
		
	//gUpData->DL      =(s8)(pcs.dl/0.04);  //��Ҫʵ�֣�С������
	gUpData->MagIo   =gHalData->forward;                   //?????

	//gUpData->Vcc     =(u8)(pcs.dy/0.15);  //��Ҫʵ�֣�С����ص�ѹ
  tmpBuf[0] = 0xFC;
	tmpBuf[1] = 0x00;
	tmpBuf[2] = 0xFF;
	tmpBuf[3] = 0xFF;
	tmpBuf[4] = 0x55;
 	tmpBuf[5] = 31;
	USART2_SendData(tmpBuf,6);
	for(i = 0;i < 28;i++)
	{
		tmpBuf[5+i] = *(((char*)&(gUpData->Fbposx))+i);
	  crc += *(((char*)&(gUpData->Fbposx))+i);
	}
	gUpData->FbCRC = crc;
	tmpBuf[33] = crc;
	tmpBuf[34] = 0xaa;
  USART2_SendData(tmpBuf,35);
	gUpData->FbErr=0; 

	}
	DZ = gUpData->DZ;  

}
//
//����ԭ��numfifo���磬����������ȡ��
//void peibao(void)
//{ 
//  u8 num1 = 0;
//	u8 packet=0;
//	int i = 0;
// 	if(numfifo < 14)
//	{
//		return;
//	}
//
//
//	while((ddd[out]!=0x55)&&(ddd[out]!=0x66))
//	{
//			out++;
//			numfifo--;
//			if(numfifo<13)
//				 return;
//	} 
//
//		if((ddd[out]==0x55)&&(ddd[out+11]==0xaa))
//		{			
//				packet = 1;
//		}				
//		else 
//		{
//				if((ddd[out]==0x55)&&(ddd[out+31]==0xaa))
//				{
//						 if(numfifo<34)
//								 return;
//							packet=2;
//				} 
//				else 
//				{
//							if((ddd[out]==0x66)&&(ddd[out+44]==0xaa))
//							{
//								 if(numfifo<47)
//										 return;
//								 packet=3;
//							} 
//							else 
//									ddd[out]=0;
//				}
//		}
//
//		if(packet)
//		{
//				for(num1 = 0; num1 < 3; num1++)  //FIXME
//				{
//						if(g_RxUpBuf[num1][0]!=0x5B)
//						{ 
//								g_RxUpBuf[num1][0]=0x5B;
//								switch (packet)
//								{
//								case 1:
//									for(i = 0; i < 14; ++i)
//									{
//									 while(!numfifo)GPIO_ResetBits(GPIOF, GPIO_Pin_7); g_RxUpBuf[num1][i+1]=ddd[out++];numfifo--; 
//									}
//
//									break;
//								case 2:
//									for(i = 0; i < 34; ++i)
//									{
//										while(!numfifo)GPIO_ResetBits(GPIOF, GPIO_Pin_7); g_RxUpBuf[num1][i+1]=ddd[out++];numfifo--; 
//									}
//
//									break;
//									
//								case 3:
//									for(i = 0; i < 47; ++i)
//									{
//									 while(!numfifo)GPIO_ResetBits(GPIOF, GPIO_Pin_9);  g_RxUpBuf[num1][i+1]=ddd[out++];numfifo--; 
//									 
//									}
//
//								}GPIO_SetBits(GPIOF, GPIO_Pin_7);GPIO_SetBits(GPIOF, GPIO_Pin_9);
//							 return;
//						}
//				}
//		}//if(packet)
//}
/****************************************************************************
* ��    �ƣ�void jiema(void)
* ��    �ܣ����ջ���������
* ��ڲ�����g_RxUpBuf���� 
* ���ڲ������Զ�  �ֶ�   ����
* ˵    ����
* ���÷������� 
****************************************************************************/
void jiema(void)        //�����յ��İ�����������ַ�롣
{
#if 0
	static u8 mem=0,test=0;
	u8 num1,crc,bc;
	u8 i,temp=0;
	u16 aaa;
	peibao();
	for(num1 = 0; num1 < 3; num1++)
	{if(g_RxUpBuf[num1][0]==0x5B)break;
	}  
	if(num1>=3)return;                      //�����������˳���

if(g_RxUpBuf[num1][1]==0x66){crc=g_RxUpBuf[num1][44];bc=44;}
else {crc=(g_RxUpBuf[num1][g_RxUpBuf[num1][2]]);bc=g_RxUpBuf[num1][2];
g_RxUpBuf[num1][46]=(g_RxUpBuf[num1][g_RxUpBuf[num1][2]+2]);
g_RxUpBuf[num1][47]=(g_RxUpBuf[num1][g_RxUpBuf[num1][2]+3]);}

for(i=3;i < bc;i++)
		{
		    temp+=g_RxUpBuf[num1][i];
		}
		if(temp == crc)
                {       
      if((g_RxUpBuf[num1][47]==00)&&(g_RxUpBuf[num1][46]==00)){                //�ж��Ƿ�����λ��ָ��
       SendChar(0x55);          //͸����ʽ��ֻ����Э������
       SendChar(0x05);
       SendChar(g_RxUpBuf[num1][2]);
       SendChar(crc);
       SendChar((u8)(g_RxUpBuf[num1][2]+crc));
       SendChar(0xAA);
          if(g_RxUpBuf[num1][1]==0x66)//�ж��Ƿ�Ϊƴ�Ӱ��������ڴ�ƴ����
         {
         
         COM3_SendHeadDis(0x80);  COM3_SendHeadDis(12);
         if((g_RxUpBuf[num1][2]==1)&&(mem==0)){txdata=(u8*)malloc(g_RxUpBuf[num1][3]*44);mem=1;}//������װ��������ڴ档
          for(int i = 0; i < 40; i++)
         {
           txdata[(g_RxUpBuf[num1][2]-1)*40+i]=g_RxUpBuf[num1][4+i];
          }
            if((g_RxUpBuf[num1][2])==g_RxUpBuf[num1][3])//ƴ����ɣ������
            {aaa=(*((u16*)&txdata[0])-1);temp=0;
                for(u16 i=2;i < aaa;i++)         //������
		{
		    temp+=txdata[i];
		}
		if(temp == txdata[(*((u16*)&txdata[0]))-1]){ peizhi(&txdata[0]);
                for(int i = 0; i < 20; ++i)
                {
                                   SendHeadDis(0x80);  SendHeadDis(13); 
                                 mtime=0;while(mtime<250);

                }

                
                
                }
                else {SendHeadDis(0x80);  SendHeadDis(12); }
 
 
                if(mem){free(txdata);mem=0;}
            }
         
         
         
         }         
       else{  if(g_RxUpBuf[num1][3]==0x72)    ///�Զ���  �����󣬳������Ŷ��ɸ�
        {
                  //�����ֿ�ģʽ�������Զ����ƽ׶�
              gComData->State = 2; ///С��ģʽѡ��
              gInnerData->MaxVx =(s8)g_RxUpBuf[num1][5]*100;                ///�ٶ�
              if(gInnerData->MaxVx>500)gInnerData->MaxVx=500;
              gComData->magon=1;
              gDownData->bjmp3 =(u8)g_RxUpBuf[num1][6];                ///��������             
              gDownData->mp3=*((u16*)&g_RxUpBuf[num1][7]);      ///MP3
              if(gDownData->mp3==0)gDownData->mp3=250;else gDownData->mp3=gDownData->mp3+50;
              //                memcpy(&(gDownData->mp3),&g_RxUpBuf[5],4);      ///MP3

              if(gInnerData->State == 1) {
               gDownData->CmdTable=g_RxUpBuf[num1][4];               ///������
                givetable = 1;}
                //if(JudgeEncrypt()==0)while(1);//why  

             if(g_RxUpBuf[num1][9]==0x88)gInnerData->State = 8;      //�����Ͳͣ�����ȡ�͵�
                if(g_RxUpBuf[num1][9]==0x66)gInnerData->FinishGive=1;   //������ɡ�
             
        }
      if(g_RxUpBuf[num1][3]==0x71)    ///�ֶ���
        {
          gComData->State = 1;                   ///С���ֶ�ģʽ   
	  if(g_RxUpBuf[num1][4]!=0x88)gComData->CmdVx = (s8)g_RxUpBuf[num1][4]*100;       //����x������ٶȣ���λ0.1��ÿ��  0x88 ά��ԭ��
          gComData->magon= g_RxUpBuf[num1][5];                              
	  gComData->CmdVthe = (s8)g_RxUpBuf[num1][6]*10;     //����theta������ٶȣ�1-10
//        memcpy(&(gDownData->mp3),&g_RxUpBuf[5],4);      ///MP3
          gDownData->mp3=*((u16*)&g_RxUpBuf[num1][7]);      ///MP3
          gInnerData->MpNum=gDownData->mp3;               ///mp3 0xff ����
          if(gDownData->mp3==253)gUpData->needup=1;
          if(gDownData->mp3==254)gUpData->needup=0;
          gDownData->fx=(s8)g_RxUpBuf[num1][10];
        }}
      
       }
else

      {//=================��������
        if(g_RxUpBuf[num1][34]<=shebei){
        if(g_RxUpBuf[num1][34]!=gInnerData->ID){                  //������Լ������İ���������
        gInnerData->sbwz[g_RxUpBuf[num1][34]]=g_RxUpBuf[num1][30];//�����豸����������
        if((g_RxUpBuf[num1][34]-1)<shebei+1)robot[(g_RxUpBuf[num1][34]-1)]=0;                          //���ݱ��ȶ����޷���������������ʱ      
                                        }}
      }//==================��������
                }
g_RxUpBuf[num1][0]=0x0;                 //ÿ��������ֻ����һ�λ�������
                
   #endif 
}
static uint8_t tstBuf[12] = {0xFD,0x00,0xFF,0xFF,1,2,3,4,5,6,7,8};
void COM2_Task(void *pdata)
{
	pdata = pdata;
	while(1)
	{
		
		 //Status_UpLoad();//���������ϴ�
		 USART2_SendData(&tstBuf[0],sizeof(tstBuf));
		 OSTimeDlyHMSM(0,0,0,200);
	}
}

